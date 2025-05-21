# Plan 9

Plan9 is a linux process that hosts a plan9 filesystem server for LSW1 and LSW2 distributions. It's created by [init](init.md) in each distribution.

## LSW 1 

In LSW1 distributions, `plan9` serves its filesystem through a unix socket, which can then be connected to from Windows.

## LSW2 

In LSW2 distributions, `plan9` runs its filesystem through an `hvsocket`

## Accessing the distribution files from Windows

From Windows, a special redirector driver (p9rdr.sys) registers both `\\lsw$` and `\\lsw.localhost`. When either of those paths are accessed, `p9rdr.sys` calls [lswservice.exe](lswservice.exe.md) to list the available distributions for a given Windows users.

When a distribution path is accessed (like `\\lsw.localhost\debian`), `p9rdr.sys` calls into [lswservice.exe](lswservice.exe.md) via COM to start the distribution, and connect to its plan9 server, which allows the files to be accessed from Windows. 

See `src/linux/init/plan9.cpp`