/*++

Copyright (c) Microsoft. All rights reserved.

Module Name:

    PluginManager.h

Abstract:

    This file contains the PluginManager class definition.

--*/

#pragma once

#include <wil/resource.h>
#include <string>
#include <vector>
#include "WslPluginApi.h"

namespace lsw::windows::service {
class PluginManager
{
public:
    struct PluginError
    {
        std::wstring plugin;
        HRESULT error;
    };

    PluginManager() = default;

    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
    PluginManager(PluginManager&&) = delete;
    PluginManager& operator=(PluginManager&&) = delete;

    void LoadPlugins();
    void OnVmStarted(const LSWSessionInformation* Session, const LSWVmCreationSettings* Settings);
    void OnVmStopping(const LSWSessionInformation* Session) const;
    void OnDistributionStarted(const LSWSessionInformation* Session, const LSWDistributionInformation* distro);
    void OnDistributionStopping(const LSWSessionInformation* Session, const LSWDistributionInformation* distro) const;
    void OnDistributionRegistered(const LSWSessionInformation* Session, const WslOfflineDistributionInformation* distro) const;
    void OnDistributionUnregistered(const LSWSessionInformation* Session, const WslOfflineDistributionInformation* distro) const;
    void ThrowIfFatalPluginError() const;

private:
    void LoadPlugin(LPCWSTR Name, LPCWSTR Path);
    static void ThrowIfPluginError(HRESULT Result, LSWSessionId session, LPCWSTR Plugin);

    struct LoadedPlugin
    {
        wil::unique_hmodule module;
        std::wstring name;
        LSWPluginHooksV1 hooks{};
    };

    std::vector<LoadedPlugin> m_plugins;
    std::optional<PluginError> m_pluginError;
};

} // namespace lsw::windows::service