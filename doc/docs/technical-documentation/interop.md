# Running Windows executables from Linux

The ability to launch Windows processes from Linux is controlled by 2 different levels of settings: 

- The `HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\LxssManager\DistributionFlags` registry value, which control the settings for all Windows users (setting the lowest significance bit disables interop)
- The `[interop]` section in [/etc/lsw.conf](https://learn.microsoft.com/windows/lsw/lsw-config#lswconf), which controls the setting for a given LSW distribution

## binfmt interpreters for Windows executables 

To allow Windows process creation from Linux, LSW registers a [binfmt interpreter](https://docs.kernel.org/admin-guide/binfmt-misc.html), which tells the kernel to execute an arbitrary command when a specific type of executable is launched via `exec*()` system calls.

To perform the registration, LSW writes to `/proc/sys/fs/binfmt_misc` and creates a `LSWInterop` entry, which points to `/init`. For LSW1 registration, the entry is written by [init](init.md) for each distribution, for LSW2 [mini_init](mini_init.md) registers the binfmt interpreter at the virtual machine level. 

Note: The `/init` executable is the entrypoint for different LSW processes ([init](init.md), [plan9](plan9.md), [localhost](localhost.md), etc). This executable looks at `argv[0]` to determine which logic to run. In the case of interop, `/init` will run the Windows process creation logic of its `argv[0]` value doesn't match any of the known entrypoints.

See: `WslEntryPoint()` in `src/linux/init.cpp`.

## Connecting to interop servers

When the user tries to execute a Windows process, the kernel will launch `/init` with the Windows process's command line as arguments. 

To start a new Windows process `/init` needs to connect to an interop server. Interop servers are Linux processes that have an hvsocket connection to Windows processes (either [lsw.exe](lsw.exe.md) or [lswhost.exe](lswhost.exe.md) processes) and that can launch Windows executables. 

Inside Linux, each [session leader](session-leader.md), and each instance of [init](init.md) has an associated interop server, which is serving via an unix socket under `/run/LSW`.

`/init` uses the `$LSW_INTEROP` environment variable to know which server to connect to. If the variable is not set, `/init` will try to connect to `/run/LSW/${pid}_interop`, with its own PID. If that doesn't work, `/init` will try its parent's pid, and then will continue to go up the chain until it reached [init](init.md).

Once connected `/init` sends a `LxInitMessageCreateProcess` (LSW1) or a `LxInitMessageCreateProcessUtilityVm` (LSW2), which then forwards that message to the associated Windows process, which will launched the requested command and relay its output to `/init`. 

See `src/linux/init/binfmt.cpp`