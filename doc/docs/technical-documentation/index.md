# LSW Overview

LSW is comprised of a set of executables, API's and protocols. This page offers an overview of the different components, and how they're connected.
Click on any component to get more details.


```mermaid
%%{ init: {
    'flowchart': { 'curve': 'stepBefore' },
    'theme': 'neutral'
    }
}%%
graph
  subgraph Windows["<b><p style="font-size:30px">Windows</p></b>"]
      C:\Windows\System32\lsw.exe["C:\Windows\System32\lsw.exe"]---|"CreateProcess()"|lsw.exe;
      lsw.exe[<a href="lsw.exe">lsw.exe</a>]---|COM|lswservice.exe;
      lswg.exe[<a href="lswg.exe">lswg.exe</a>]---|COM|lswservice.exe;
      lswconfig.exe[<a href="lswconfig.exe">lswconfig.exe</a>]---|COM|lswservice.exe;
      lswapi.dll[<a href="https://learn.microsoft.com/windows/win32/api/lswapi/">lswapi.dll</a>]---|COM|lswservice.exe;
      id[debian.exe, ubuntu.exe, ]---|"LoadLibrary()"|lswapi.dll;
      lswservice.exe[<a href="lswservice.exe">lswservice.exe</a>]---|"CreateProcessAsUser()"|lswrelay.exe[<a href="lswrelay.exe">lswrelay.exe</a>];
      lswservice.exe---|"CreateProcessAsUser()"|lswhost.exe[<a href="lswhost.exe">lswhost.exe</a>];
      fs["Windows filesystem (//lsw.localhost)"]
  end
  
  lswservice.exe -----|hvsocket| mini_init
  lswservice.exe -----|hvsocket| gns
  fs---|hvsocket|plan9

  lsw.exe---|hvsocket|relay
  
  subgraph Linux["<b><p style="font-size:30px">Linux</p></b>"]
      mini_init[<a href="mini_init">mini_init</a>]---|"exec()"|gns[<a href="gns">gns</a>]
      mini_init---|"exec()"|init[<a href="init">init</a>];
      mini_init---|"exec()"|localhost[<a href="localhost">localhost</a>];
      
      subgraph "Linux Distribution"["<b><p style="font-size:23px">Linux Distribution</p></b>"]

          init[<a href="init">init</a>]---|"exec()"|plan9[<a href="plan9">plan9</a>];
          init---|"exec()"|sid[session leader];
          sid[<a href="session-leader">session leader</a>]---|"exec()"|relay
          relay[<a href="relay">relay</a>]---|"exec()"|cid["User command (bash, curl)"]
      end

  end
```