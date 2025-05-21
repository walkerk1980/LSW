# mini_init

mini_init is the first executable that's launched when the LSW2 virtual machine starts. See [LSW2 boot process](boot-process.md) for more details.

## Virtual machine setup

mini_init is started when the kernel is done booting, and calls `/init`, which is `mini_init`. Like other standard linux `init` executables, `mini_init` starts by mounting `/proc`, `/sys`, `/dev` and other standard mountpoints.

`mini_init` then performs various configuration such as enabling crash dump collection, configuring logging via `/dev/console` and tty configuration.

Once everything is ready, `mini_init` connects two hvsockets to [lswservice](lswservice.exe.md). 

One of them, called the "mini_init" channel is used for messages sent by `lswservice.exe`. See `src/shared/inc/lxinitshared.h` for a list of messages and responses. Common messages are:

- `LxMiniInitMessageLaunchInit`: Mount a virtual disk and start a new distribution. See [`init`](init.md) for more details
- `LxMiniInitMessageMount`: Mount a disk in `/mnt/lsw` (used for lsw --mount)
- `EJECT_VHD_MESSAGE`: Eject a disk
- `LxMiniInitMessageImport`: Import a distribution
- `LxMiniInitMessageExport`: Export a distribution

The other hvsocket channel is used to send notifications to [lswservice.exe](lswservice.exe.md). This is used mainly to report when linux processes exit (which lswservice uses to know when distributions are terminated).

## Networking configuration

As part of the boot process, `mini_init` also launches the [gns binary](gns.md) which managed networking configuration

## Other tasks

`mini_init` performs various other maintenance tasks such as:

- Reclaiming unused memory
- Launching the debug shell tty
- Synchronizing IO when the virtual machine terminates
- Resizing filesystem (for lsw --manage <distro> --resize)
- Formatting disks (used when installing new distributions)

