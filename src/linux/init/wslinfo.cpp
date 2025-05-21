/*++

Copyright (c) Microsoft. All rights reserved.

Module Name:

    lswinfo.cpp

Abstract:

    This file lswpath function definitions.

--*/

#include "common.h"
#include <iostream>
#include <assert.h>
#include "getopt.h"
#include "util.h"
#include "lswpath.h"
#include "lswinfo.h"
#include "lxinitshared.h"
#include "defs.h"
#include "Localization.h"
#include "CommandLine.h"

enum class WslInfoMode
{
    GetNetworkingMode,
    MsalProxyPath,
    WslVersion
};

int WslInfoEntry(int Argc, char* Argv[])

/*++

Routine Description:

    This routine is the entrypoint for the lswinfo binary.

Arguments:

    Argc - Supplies the argument count.

    Argv - Supplies the command line arguments.

Return Value:

    0 on success, 1 on failure.

--*/

{
    using namespace lsw::shared;

    constexpr auto Usage = std::bind(Localization::MessageWslInfoUsage, Localization::Options::Default);

    std::optional<WslInfoMode> Mode;
    bool noNewLine = false;

    ArgumentParser parser(Argc, Argv);

    parser.AddArgument(UniqueSetValue<WslInfoMode, WslInfoMode::GetNetworkingMode>{Mode, Usage}, LSWINFO_NETWORKING_MODE);
    parser.AddArgument(UniqueSetValue<WslInfoMode, WslInfoMode::MsalProxyPath>{Mode, Usage}, LSWINFO_MSAL_PROXY_PATH);
    parser.AddArgument(UniqueSetValue<WslInfoMode, WslInfoMode::WslVersion>{Mode, Usage}, LSWINFO_LSW_VERSION);
    parser.AddArgument(UniqueSetValue<WslInfoMode, WslInfoMode::WslVersion>{Mode, Usage}, LSWINFO_LSW_VERSION_LEGACY);
    parser.AddArgument(NoOp{}, LSWINFO_LSW_HELP);
    parser.AddArgument(noNewLine, nullptr, LSWINFO_NO_NEWLINE);

    try
    {
        parser.Parse();
    }
    catch (const wil::ExceptionWithUserMessage& e)
    {
        std::cerr << e.what() << "\n";
        return 1;
    }

    if (!Mode.has_value())
    {
        std::cerr << Usage() << "\n";
        return 1;
    }
    else if (Mode.value() == WslInfoMode::GetNetworkingMode)
    {
        if (UtilIsUtilityVm())
        {
            auto NetworkingMode = UtilGetNetworkingMode();
            if (!NetworkingMode)
            {
                std::cerr << Localization::MessageFailedToQueryNetworkingMode() << "\n";
                return 1;
            }

            switch (NetworkingMode.value())
            {
            case LxMiniInitNetworkingModeNat:
                std::cout << "nat";
                break;

            case LxMiniInitNetworkingModeBridged:
                std::cout << "bridged";
                break;

            case LxMiniInitNetworkingModeMirrored:
                std::cout << "mirrored";
                break;

            case LxMiniInitNetworkingModeVirtioProxy:
                std::cout << "virtioproxy";
                break;

            default:
                std::cout << "none";
                break;
            }
        }
        else
        {
            std::cout << "lsw1";
        }
    }
    else if (Mode.value() == WslInfoMode::MsalProxyPath)
    {
        auto value = UtilGetEnvironmentVariable(LX_LSW2_INSTALL_PATH);
        if (value.empty())
        {
            std::cerr << Localization::MessageNoValueFound() << "\n";
            return 1;
        }

        auto translatedPath = WslPathTranslate(value.data(), TRANSLATE_FLAG_ABSOLUTE, TRANSLATE_MODE_UNIX);
        if (translatedPath.empty())
        {
            std::cerr << Localization::MessageFailedToTranslatePath() << "\n";
            return 1;
        }

        std::cout << translatedPath << "/msal.lsw.proxy.exe";
    }
    else if (Mode.value() == WslInfoMode::WslVersion)
    {
        std::cout << LSW_PACKAGE_VERSION;
    }

    if (!noNewLine)
    {
        std::cout << '\n';
    }

    return 0;
}
