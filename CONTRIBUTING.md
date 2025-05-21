# LSW contributing guide

There are a few main ways to contribute to LSW, with guides to each one:

1. [Add a feature or bugfix to LSW](#add-a-feature-or-bugfix-to-lsw)
2. [File a LSW issue](#file-a-lsw-issue)

## Add a feature or bugfix to LSW

We welcome any contributions to the LSW source code to add features or fix bugs! Before you start actually working on the feature, please **[file it as an issue, or a feature request in this repository](https://github.com/microsoft/LSW/issues)** so that we can track it and provide any feedback if necessary.

Once you have done so, please see [the developer docs](./doc/docs/dev-loop.md) for instructions on how to build LSW locally on your machine for development. 

When your fix is ready, please [submit it as a pull request in this repository](https://github.com/microsoft/LSW/pulls) and the LSW team will triage and respond to it. Most contributions require you to agree to a Contributor License Agreement (CLA) declaring that you have the right to, and actually do, grant us the rights to use your contribution. For details, visit https://cla.microsoft.com.

## File a LSW issue

You can file issues for LSW at the LSW repository, or linked repositories. Before filing an issue please search for any existing issues and upvote or comment on those if possible. 

1. If your issue is related to LSW documentation, please file it at [microsoftdocs/lsw](https://github.com/microsoftdocs/LSW/issues)
2. If your issue is related to a Linux GUI app, please file it at [microsoft/lswg](https://github.com/microsoft/lswg/issues)
3. Otherwise if you have a technical issue related to LSW in general, such as start up issues, etc., please file it at [microsoft/lsw](https://github.com/microsoft/LSW/issues)

Please provide as much information as possible when reporting a bug or filing an issue on the Linux Subsystem for Windows, and be sure to include logs as necessary!

Please see the [notes for collecting LSW logs](#notes-for-collecting-lsw-logs) section below for more info on filing issues.

## Thank you

Thank you in advance for your contribution! We appreciate your help in making LSW a better tool for everyone.

## Notes for collecting LSW logs

### Important: Reporting BSODs and Security issues
**Do not open GitHub issues for Windows crashes (BSODs) or security issues.**. Instead, send Windows crashes or other security-related issues to secure@microsoft.com.
See the `10) Reporting a Windows crash (BSOD)` section below for detailed instructions.

### Reporting issues in Windows Console or LSW text rendering/user experience
Note that LSW distro's launch in the Windows Console (unless you have taken steps to launch a 3rd party console/terminal). Therefore, *please file UI/UX related issues in the [Windows Console issue tracker](https://github.com/microsoft/console)*.

### Collect LSW logs for networking issues

Install tcpdump in your LSW distribution using the following commands.
Note: This will not work if LSW has Internet connectivity issues.

```
# sudo apt-get update
# sudo apt-get -y install tcpdump
```

Install [WPR](https://learn.microsoft.com/windows-hardware/test/wpt/windows-performance-recorder)

To collect LSW networking logs, do the following steps in an administrative powershell prompt:

```
Invoke-WebRequest -UseBasicParsing "https://raw.githubusercontent.com/microsoft/LSW/master/diagnostics/collect-networking-logs.ps1" -OutFile collect-networking-logs.ps1
Set-ExecutionPolicy Bypass -Scope Process -Force
.\collect-networking-logs.ps1
```
The script will output when log collection starts. Reproduce the problem, then press any key to stop the log collection.
The script will output the path of the log file once done.

<!-- Preserving anchors -->
<div id="8-detailed-logs"></div>
<div id="9-networking-logs"></div>
<div id="8-collect-lsw-logs-recommended-method"></div>


### Collect LSW logs (recommended method)

If you choose to email these logs instead of attaching to the bug, please send them to lsw-gh-logs@microsoft.com with the number of the github issue in the subject, and in the message a link to your comment in the github issue.

To collect LSW logs, download and execute [collect-lsw-logs.ps1](https://github.com/Microsoft/LSW/blob/master/diagnostics/collect-lsw-logs.ps1) in an administrative powershell prompt:

```
Invoke-WebRequest -UseBasicParsing "https://raw.githubusercontent.com/microsoft/LSW/master/diagnostics/collect-lsw-logs.ps1" -OutFile collect-lsw-logs.ps1
Set-ExecutionPolicy Bypass -Scope Process -Force
.\collect-lsw-logs.ps1
```
The script will output the path of the log file once done.

### 10) Reporting a Windows crash (BSOD)

To collect a kernel crash dump, first run the following command in an elevated command prompt:

```
reg.exe add HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\CrashControl /v AlwaysKeepMemoryDump  /t REG_DWORD /d 1 /f
```

Then reproduce the issue, and let the machine crash and reboot.

After reboot, the kernel dump will be in `%SystemRoot%\MEMORY.DMP` (unless this path has been overridden in the advanced system settings).

Please send this dump to: secure@microsoft.com .
Make sure that the email body contains:

- The Github issue number, if any
- That this dump is destinated to the LSW team

### 11) Reporting a LSW process crash

The easiest way to report a LSW process crash is by [collecting a user-mode crash dump](https://learn.microsoft.com/windows/win32/wer/collecting-user-mode-dumps).

To collect dumps of all running LSW processe, please open a PowerShell prompt with admin privileges, navigate to a folder where you'd like to put your log files and run these commands: 

```
Invoke-WebRequest -UseBasicParsing "https://raw.githubusercontent.com/microsoft/LSW/master/diagnostics/collect-lsw-logs.ps1" -OutFile collect-lsw-logs.ps1
Set-ExecutionPolicy Bypass -Scope Process -Force
.\collect-lsw-logs.ps1 -Dump
```

The script will output the path to the log file when it is done.

#### Enable automatic crash dump collection

If your crash is sporadic or hard to reproduce, please enable automatic crash dumps to catch logs for this behavior: 

```
md C:\crashes
reg.exe add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps" /f
reg.exe add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps" /v DumpFolder /t REG_EXPAND_SZ /d C:\crashes /f
reg.exe add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps" /v DumpType /t REG_DWORD /d 2 /f
```

Crash dumps will then automatically be written to C:\crashes.

Once you're done, crash dump collection can be disabled by running the following command in an elevated command prompt:

```
reg.exe delete "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps" /f
```

### 12) Collect lswservice time travel debugging traces

To collect time travel debugging traces:

1) [Install Windbg preview](https://apps.microsoft.com/store/detail/windbg-preview/9PGJGD53TN86?hl=en-us&gl=us&rtc=1)

2) Open windbg preview as administrator by running `windbgx` in an elevated command prompt

3) Navigate to `file` -> `Attach to process`

4) Check `Record with Time Travel Debugging` (at the bottom right)

4) Check `Show processes from all users` (at the bottom)

5) Select `lswservice.exe`. Note, if lswservice.exe is not running, you make it start it with: `lsw.exe -l`

6) Click `Configure and Record` (write down the folder you chose for the traces)

7) Reproduce the issue

8) Go back to windbg and click `Stop and Debug`

9) Once the trace is done collecting, click `Stop Debugging` and close Windbg

10) Go to the folder where the trace was collected, and locate the .run file. It should look like: `lswservice*.run`

11) Share that file on the issue