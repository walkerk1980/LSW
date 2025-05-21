# Session leader

A session leader is a linux process, which is forked from [init](init.md) after receiving a `LxInitMessageCreateSession` message (see `src/linux/init.cpp`)

## Creating linux processes

Session leaders are used to create linux processes on behalf of the user. Each linux session leader is associated to a Windows console. 

To create a user process, [lswservice.exe](lswservice.exe.md) sends a `LxInitMessageCreateProcess` message (LSW1) or a `LxInitMessageCreateProcessUtilityVm` message (LSW2), which contains details about the process to create such as:

- Command line
- Current directory
- Environment variables 
- User name

### Creating a LSW1 process

When running in a LSW1 distribution, the session leader forks(), and uses the child process to `exec()` into the user linux process. Before calling `exec()`, child configures various settings such as:

- The user and group id
- The current directory
- The standard file descriptors (stdin, stdout, stderr)

## Creating a LSW2 process

When running in a LSW2 distribution, the session leaders forks() to create a [relay](relay.md) process, which is responsible for creating the user process and relaying its output back to [lsw.exe](lsw.exe.md)