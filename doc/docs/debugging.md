# Debugging LSW

## Logging

There are multiple sources of logging in LSW. The main one is the ETL trace that is emitted from Windows processes.

To collect an ETL trace, run ([link to lsw.wprp](https://github.com/microsoft/LSW/blob/master/diagnostics/lsw.wprp)):

```
wpr -start lsw.wprp -filemode

[reproduce the issue]

wpr -stop logs.ETL
```

Once the log file is saved, you can use [WPA](https://apps.microsoft.com/detail/9n58qrw40dfw?hl=en-US&gl=US) to view the logs.

Notable ETL providers: 

- `Microsoft.Windows.Lxss.Manager`: Logs emitted from lswservice.exe
    Important events: 
    - `GuestLog`: Logs from the vm's dmesg
    - `Error`: Unexpected errors
    - `CreateVmBegin`, `CreateVmEnd`: Virtual machine lifetime
    - `CreateNetworkBegin`, `CreateNetworkEnd`: Networking configuration
    - `SentMessage`, `ReceivedMessaged`: Communication on the hvsocket channels with Linux.
    
- `Microsoft.Windows.Subsystem.Lxss`: Other LSW executables (lsw.exe, lswg.exe, lswconfig.exe, lswrelay.exe, ...)
    Important events:
    - `UserVisibleError`: An error was displayed to the user 

- `Microsoft.Windows.Plan9.Server`: Logs from the Windows plan9 server (used when accessing /mnt/ shares and running Windows)


On the Linux side, the easiest way to access logs is to look at `dmesg` or use the debug console, which can enabled by writing:

```
[lsw2]
debugConsole=true
```

to `%USERPROFILE%/.lswconfig` and restarting LSW


## Attaching debuggers

Usermode can be attached to LSW Windows processes (lsw.exe, lswservice.exe, lswrelay.exe, ...). The symbols are available under the `bin/<platform>/<target>` folder. 
You can also use [this trick](https://github.com/microsoft/LSW/blob/master/CONTRIBUTING.md#11-reporting-a-lsw-process-crash) to automatically collect crash dumps when processes crash.

## Linux debugging

`gdb` can be attached to Linux processes (see [man gdb](https://man7.org/linux/man-pages/man1/gdb.1.html)). 

The simplest way to debug a LSW process with gdb is to use the `/mnt` mountpoints to access the code from gdb. 
Once started, just use `dir /path/to/lsw/source` in gdb to connect the source files.

## Root namespace debugging

Some LSW process such as `gns` or `mini_init` aren't accessible from within LSW distributions. To attach a debugger to those, use the debug shell via:

```
lsw --debug-shell
```

You can then install `gdb` by running `tdnf install gdb` and start debugging processes.