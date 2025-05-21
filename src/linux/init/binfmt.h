/*++

Copyright (c) Microsoft. All rights reserved.

Module Name:

    binfmt.h

Abstract:

    This file contains declarations for the Windows interop binfmt interpreter.

--*/

#pragma once

//
// Name of the LSW binfmt_misc intrepreter.
//

#define LX_INIT_BINFMT_NAME "LSWInterop"

//
// Name of the LSW 'late' binfmt_misc intrepreter.
// This name is used by the lsw-binfmt systemd unit which
// registers the interpreter a second time after systemd-binfmt to make sure
// that lsw's interpreter is always registered last.
//

#define LX_INIT_BINFMT_NAME_LATE "LSWInterop-late"
#define BINFMT_MISC_MOUNT_TARGET "/proc/sys/fs/binfmt_misc"
#define BINFMT_MISC_REGISTER_FILE BINFMT_MISC_MOUNT_TARGET "/register"
#define BINFMT_INTEROP_REGISTRATION_STRING(Name) ":" Name ":M::MZ::" LX_INIT_PATH ":P"

int CreateNtProcess(int Argc, char* Argv[]);
