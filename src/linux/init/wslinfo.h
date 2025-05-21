/*++

Copyright (c) Microsoft. All rights reserved.

Module Name:

    lswinfo.h

Abstract:

    This file contains lswinfo function declarations.

--*/

#pragma once

#define LSWINFO_NAME "lswinfo"

#define LSWINFO_MSAL_PROXY_PATH "--msal-proxy-path"
#define LSWINFO_NETWORKING_MODE "--networking-mode"
#define LSWINFO_LSW_VERSION "--version"
#define LSWINFO_LSW_VERSION_LEGACY "--lsw-version"
#define LSWINFO_LSW_HELP "--help"
#define LSWINFO_NO_NEWLINE 'n'

int WslInfoEntry(int Argc, char* Argv[]);
