# Localhost

`localhost` is a LSW2 linux process, created by [mini_init](mini_init.md). Its role is to forward network traffic between the LSW2 virtual machine, and Windows.


## NAT networking 

When `lsw2.networkingMode` is set to NAT, `localhost` will watch for bound TCP ports, and relay the network traffic to Windows via [lswrelay.exe](lswrelay.exe.md)

## Mirrored networking

In mirrored mode, `localhost` register a BPF program to intercept calls to `bind()`, and forward the calls to Windows via [lswservice.exe](lswservice.exe.md) so Windows can route the network traffic directly to the LSW2 virtual machine.

See `src/linux/localhost.cpp`.