/*++

Copyright (c) Microsoft. All rights reserved.

Module Name:

    PluginManager.cpp

Abstract:

    This file contains the PluginManager helper class implementation.

--*/

#include "precomp.h"
#include "PluginManager.h"
#include "WslPluginApi.h"
#include "LxssUserSessionFactory.h"

using lsw::windows::common::Context;
using lsw::windows::common::ExecutionContext;
using lsw::windows::service::PluginManager;

constexpr auto c_pluginPath = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Lxss\\Plugins";

constexpr LSWVersion Version = {lsw::shared::VersionMajor, lsw::shared::VersionMinor, lsw::shared::VersionRevision};

thread_local std::optional<std::wstring> g_pluginErrorMessage;

extern "C" {
HRESULT MountFolder(LSWSessionId Session, LPCWSTR WindowsPath, LPCWSTR LinuxPath, BOOL ReadOnly, LPCWSTR Name)
try
{
    const auto session = FindSessionByCookie(Session);
    RETURN_HR_IF(RPC_E_DISCONNECTED, !session);

    auto result = session->MountRootNamespaceFolder(WindowsPath, LinuxPath, ReadOnly, Name);

    LSW_LOG(
        "PluginMountFolderCall",
        TraceLoggingValue(WindowsPath, "WindowsPath"),
        TraceLoggingValue(LinuxPath, "LinuxPath"),
        TraceLoggingValue(ReadOnly, "ReadOnly"),
        TraceLoggingValue(Name, "Name"),
        TraceLoggingValue(result, "Result"));

    return result;
}
CATCH_RETURN();

HRESULT ExecuteBinary(LSWSessionId Session, LPCSTR Path, LPCSTR* Arguments, SOCKET* Socket)
try
{

    const auto session = FindSessionByCookie(Session);
    RETURN_HR_IF(RPC_E_DISCONNECTED, !session);

    auto result = session->CreateLinuxProcess(nullptr, Path, Arguments, Socket);

    LSW_LOG("PluginExecuteBinaryCall", TraceLoggingValue(Path, "Path"), TraceLoggingValue(result, "Result"));
    return result;
}
CATCH_RETURN();

HRESULT PluginError(LPCWSTR UserMessage)
try
{
    const auto* context = ExecutionContext::Current();
    THROW_HR_IF(E_INVALIDARG, UserMessage == nullptr);
    THROW_HR_IF_MSG(
        E_ILLEGAL_METHOD_CALL, context == nullptr || WI_IsFlagClear(context->CurrentContext(), Context::Plugin), "Message: %ls", UserMessage);

    // Logs when a LSW plugin hits an error and what that error message is
    LSW_LOG_TELEMETRY("PluginError", PDT_ProductAndServicePerformance, TraceLoggingValue(UserMessage, "Message"));

    THROW_HR_IF(E_ILLEGAL_STATE_CHANGE, g_pluginErrorMessage.has_value());

    g_pluginErrorMessage.emplace(UserMessage);

    return S_OK;
}
CATCH_RETURN();

HRESULT ExecuteBinaryInDistribution(LSWSessionId Session, const GUID* Distro, LPCSTR Path, LPCSTR* Arguments, SOCKET* Socket)
try
{
    THROW_HR_IF(E_INVALIDARG, Distro == nullptr);

    const auto session = FindSessionByCookie(Session);
    RETURN_HR_IF(RPC_E_DISCONNECTED, !session);

    auto result = session->CreateLinuxProcess(Distro, Path, Arguments, Socket);

    LSW_LOG("PluginExecuteBinaryInDistributionCall", TraceLoggingValue(Path, "Path"), TraceLoggingValue(result, "Result"));

    return result;
}
CATCH_RETURN();
}

static constexpr LSWPluginAPIV1 ApiV1 = {Version, &MountFolder, &ExecuteBinary, &PluginError, &ExecuteBinaryInDistribution};

void PluginManager::LoadPlugins()
{
    ExecutionContext context(Context::Plugin);

    const auto key = common::registry::CreateKey(HKEY_LOCAL_MACHINE, c_pluginPath, KEY_READ);
    const auto values = common::registry::EnumValues(key.get());

    std::set<std::wstring, lsw::shared::string::CaseInsensitiveCompare> loaded;
    for (const auto& e : values)
    {
        if (e.second != REG_SZ)
        {
            LOG_HR_MSG(E_UNEXPECTED, "Plugin value: '%ls' has incorrect type: %lu, skipping", e.first.c_str(), e.second);
            continue;
        }

        auto path = common::registry::ReadString(key.get(), nullptr, e.first.c_str());

        if (!loaded.insert(path).second)
        {
            LOG_HR_MSG(E_UNEXPECTED, "Module '%ls' has already been loaded, skipping plugin '%ls'", path.c_str(), e.first.c_str());
            continue;
        }

        auto loadResult = wil::ResultFromException(WI_DIAGNOSTICS_INFO, [&]() { LoadPlugin(e.first.c_str(), path.c_str()); });

        // Logs when a LSW plugin is loaded, used for evaluating plugin populations
        LSW_LOG_TELEMETRY(
            "PluginLoad",
            PDT_ProductAndServiceUsage,
            TraceLoggingValue(e.first.c_str(), "Name"),
            TraceLoggingValue(path.c_str(), "Path"),
            TraceLoggingValue(loadResult, "Result"));

        if (FAILED(loadResult))
        {
            // If this plugin reported an error, record it to display it to the user
            m_pluginError.emplace(PluginError{e.first, loadResult});
        }
    }
}

void PluginManager::LoadPlugin(LPCWSTR Name, LPCWSTR ModulePath)
{
    // Validate the plugin signature before loading it.
    // The handle to the module is kept open after validating the signature so the file can't be written to
    // after the signature check.
    wil::unique_hfile pluginHandle;
    if constexpr (lsw::shared::OfficialBuild)
    {
        pluginHandle = lsw::windows::common::lswutil::ValidateFileSignature(ModulePath);
        WI_ASSERT(pluginHandle.is_valid());
    }

    LoadedPlugin plugin{};
    plugin.name = Name;

    plugin.module.reset(LoadLibrary(ModulePath));
    THROW_LAST_ERROR_IF_NULL(plugin.module);

    const LSWPluginAPI_EntryPointV1 entryPoint =
        reinterpret_cast<LSWPluginAPI_EntryPointV1>(GetProcAddress(plugin.module.get(), GSL_STRINGIFY(LSWPLUGINAPI_ENTRYPOINTV1)));

    THROW_LAST_ERROR_IF_NULL(entryPoint);
    THROW_IF_FAILED_MSG(entryPoint(&ApiV1, &plugin.hooks), "Error returned by plugin: '%ls'", ModulePath);

    m_plugins.emplace_back(std::move(plugin));
}

void PluginManager::OnVmStarted(const LSWSessionInformation* Session, const LSWVmCreationSettings* Settings)
{
    ExecutionContext context(Context::Plugin);

    for (const auto& e : m_plugins)
    {
        if (e.hooks.OnVMStarted != nullptr)
        {
            LSW_LOG(
                "PluginOnVmStartedCall", TraceLoggingValue(e.name.c_str(), "Plugin"), TraceLoggingValue(Session->UserSid, "Sid"));

            ThrowIfPluginError(e.hooks.OnVMStarted(Session, Settings), Session->SessionId, e.name.c_str());
        }
    }
}

void PluginManager::OnVmStopping(const LSWSessionInformation* Session) const
{
    ExecutionContext context(Context::Plugin);

    for (const auto& e : m_plugins)
    {
        if (e.hooks.OnVMStopping != nullptr)
        {
            LSW_LOG(
                "PluginOnVmStoppingCall", TraceLoggingValue(e.name.c_str(), "Plugin"), TraceLoggingValue(Session->UserSid, "Sid"));

            const auto result = e.hooks.OnVMStopping(Session);
            LOG_IF_FAILED_MSG(result, "Error thrown from plugin: '%ls'", e.name.c_str());
        }
    }
}

void PluginManager::OnDistributionStarted(const LSWSessionInformation* Session, const LSWDistributionInformation* Distribution)
{
    ExecutionContext context(Context::Plugin);

    for (const auto& e : m_plugins)
    {
        if (e.hooks.OnDistributionStarted != nullptr)
        {
            LSW_LOG(
                "PluginOnDistroStartedCall",
                TraceLoggingValue(e.name.c_str(), "Plugin"),
                TraceLoggingValue(Session->UserSid, "Sid"),
                TraceLoggingValue(Distribution->Id, "DistributionId"));

            ThrowIfPluginError(e.hooks.OnDistributionStarted(Session, Distribution), Session->SessionId, e.name.c_str());
        }
    }
}

void PluginManager::OnDistributionStopping(const LSWSessionInformation* Session, const LSWDistributionInformation* Distribution) const
{
    ExecutionContext context(Context::Plugin);

    for (const auto& e : m_plugins)
    {
        if (e.hooks.OnDistributionStopping != nullptr)
        {
            LSW_LOG(
                "PluginOnDistroStoppingCall",
                TraceLoggingValue(e.name.c_str(), "Plugin"),
                TraceLoggingValue(Session->UserSid, "Sid"),
                TraceLoggingValue(Distribution->Id, "DistributionId"));

            const auto result = e.hooks.OnDistributionStopping(Session, Distribution);
            LOG_IF_FAILED_MSG(result, "Error thrown from plugin: '%ls'", e.name.c_str());
        }
    }
}

void PluginManager::OnDistributionRegistered(const LSWSessionInformation* Session, const WslOfflineDistributionInformation* Distribution) const
{
    ExecutionContext context(Context::Plugin);

    for (const auto& e : m_plugins)
    {
        if (e.hooks.OnDistributionRegistered != nullptr)
        {
            LSW_LOG(
                "PluginOnDistributionRegisteredCall",
                TraceLoggingValue(e.name.c_str(), "Plugin"),
                TraceLoggingValue(Session->UserSid, "Sid"),
                TraceLoggingValue(Distribution->Id, "DistributionId"));

            const auto result = e.hooks.OnDistributionRegistered(Session, Distribution);
            LOG_IF_FAILED_MSG(result, "Error thrown from plugin: '%ls'", e.name.c_str());
        }
    }
}

void PluginManager::OnDistributionUnregistered(const LSWSessionInformation* Session, const WslOfflineDistributionInformation* Distribution) const
{
    ExecutionContext context(Context::Plugin);

    for (const auto& e : m_plugins)
    {
        if (e.hooks.OnDistributionUnregistered != nullptr)
        {
            LSW_LOG(
                "PluginOnDistributionUnregisteredCall",
                TraceLoggingValue(e.name.c_str(), "Plugin"),
                TraceLoggingValue(Session->UserSid, "Sid"),
                TraceLoggingValue(Distribution->Id, "DistributionId"));

            const auto result = e.hooks.OnDistributionUnregistered(Session, Distribution);
            LOG_IF_FAILED_MSG(result, "Error thrown from plugin: '%ls'", e.name.c_str());
        }
    }
}

void PluginManager::ThrowIfPluginError(HRESULT Result, LSWSessionId Session, LPCWSTR Plugin)
{
    const auto message = std::move(g_pluginErrorMessage);
    g_pluginErrorMessage.reset(); // std::move() doesn't clear the previous std::optional

    if (FAILED(Result))
    {
        if (message.has_value())
        {
            THROW_HR_WITH_USER_ERROR(Result, lsw::shared::Localization::MessageFatalPluginErrorWithMessage(Plugin, message->c_str()));
        }
        else
        {
            THROW_HR_WITH_USER_ERROR(Result, lsw::shared::Localization::MessageFatalPluginError(Plugin));
        }
    }
    else if (message.has_value())
    {
        THROW_HR_MSG(E_ILLEGAL_STATE_CHANGE, "Plugin '%ls' emitted an error message but returned success", Plugin);
    }
}

void PluginManager::ThrowIfFatalPluginError() const
{
    ExecutionContext context(Context::Plugin);

    if (!m_pluginError.has_value())
    {
        return;
    }
    else if (m_pluginError->error == LSW_E_PLUGIN_REQUIRES_UPDATE)
    {
        THROW_HR_WITH_USER_ERROR(
            LSW_E_PLUGIN_REQUIRES_UPDATE, lsw::shared::Localization::MessagePluginRequiresUpdate(m_pluginError->plugin));
    }
    else
    {
        THROW_HR_WITH_USER_ERROR(m_pluginError->error, lsw::shared::Localization::MessageFatalPluginError(m_pluginError->plugin));
    }
}
