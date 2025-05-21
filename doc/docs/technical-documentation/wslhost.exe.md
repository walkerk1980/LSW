# Wslhost.exe 

`lswhost.exe` is a Windows executable that's used to display desktop notifications, and run Linux processes in the background.

## COM server

When running as COM server, `lswhost.exe` registers a [NotificationActivatorFactory](https://learn.microsoft.com/dotnet/api/microsoft.toolkit.uwp.notifications.notificationactivator?view=win-comm-toolkit-dotnet-7.1), which is then used to display desktop notifications to the user.

Notifications can be used to:

- Notify the user about a LSW update
- Warn the user about a configuration error
- Notify the user about a proxy change

See: `src/windows/common/notifications.cpp`

## Background processes 

When [lsw.exe](lsw.exe.md) terminates before the associated Linux processes terminates, `lswhost.exe` takes over the lifetime of the Linux process. 

This allows Linux processes to keep running Windows commands and access the terminal even after the associated `lsw.exe` terminates. 

See `src/windows/lswhost/main.cpp` and [interop](interop.md)