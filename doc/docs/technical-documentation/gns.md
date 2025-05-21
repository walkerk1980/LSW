# GNS

`gns` is a process created by `mini_init`. Its jobs is to configure networking within the LSW2 virtual machine. 

## Networking configuration 

Networking settings are shared by all LSW2 distributions. While LSW2 is running, `gns` maintains an hvsocket channel to [lswservice.exe](lswservice.exe.md), which is used to send various networking related configurations such as:

- Interface IP configuration
- Routing table entries
- DNS configuration
- MTU size configuration

When DNS tunneling is enabled, `gns` is also responsible for replying to DNS requests.

See `src/linux/init/GnsEngine.cpp` and `src/windows/service/exe/GnsChannel.cpp`