# LSW data & privacy

## Overview

LSW collects diagnostic data using Windows telemetry, just like other Windows components. You can disable this by opening Windows Settings, navigating to Privacy and Security -> Diagnostics & Feedback and disabling 'Diagnostic data'. You can also view all diagnostic data that you are sending in that menu using the 'View diagnostic data' option. 

For more information please read the [Microsoft privacy statement](https://www.microsoft.com/privacy/privacystatement).

## What does LSW collect?

1. Usage
   - Understanding what features and settings are most often used in LSW helps us make decisions on where to focus our time and energy.
2. Stability
   - Monitoring bugs and system crashes assists us in prioritizing the most urgent issues.
3. Performance
   - Assessing the performance of LSW gives us an understanding of what runtimes / components could be causing slow downs. This supports our commitment in providing you a speedy and effective LSW. 

You can search for LSW telemetry events by looking for calls to `LSW_LOG_TELEMETRY` in the source code of this repository.