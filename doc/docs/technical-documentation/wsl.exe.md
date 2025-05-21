# lsw.exe

lsw.exe is main command line entrypoint from LSW. Its job is to:

- Parse the command line arguments (See `src/windows/common/lswclient.cpp`)
- Call [lswservice.exe](lswservice.exe.md) via COM to launch LSW (see `src/windows/common/svccomm.cpp`)
- Relay stdin / stdout / stderr from and to the linux process

