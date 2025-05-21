// Copyright (C) Microsoft Corporation. All rights reserved.

using WslSettings.Contracts.Services;

namespace WslSettings.ViewModels.Settings;

public partial class DeveloperViewModel : WslConfigSettingViewModel
{
    private IWslConfigSetting? _debugConsole;
    private IWslConfigSetting? _hWPerfCounters;
    private IWslConfigSetting? _kernelPath;
    private IWslConfigSetting? _kernelModulesPath;
    private IWslConfigSetting? _systemDistroPath;

    public DeveloperViewModel()
    {
        InitializeConfigSettings();
    }

    override protected void InitializeConfigSettings()
    {
        var lswConfigService = App.GetService<IWslConfigService>();
        _debugConsole = lswConfigService.GetWslConfigSetting(WslConfigEntry.DebugConsoleEnabled);
        _hWPerfCounters = lswConfigService.GetWslConfigSetting(WslConfigEntry.HardwarePerformanceCountersEnabled);
        _kernelPath = lswConfigService.GetWslConfigSetting(WslConfigEntry.KernelPath);
        _kernelModulesPath = lswConfigService.GetWslConfigSetting(WslConfigEntry.KernelModulesPath);
        _systemDistroPath = lswConfigService.GetWslConfigSetting(WslConfigEntry.SystemDistroPath);
    }

    public bool IsOnDebugConsole
    {
        get { return _debugConsole!.BoolValue; }
        set { Set(ref _debugConsole!, value); }
    }

    public bool IsOnHWPerfCounters
    {
        get { return _hWPerfCounters!.BoolValue; }
        set { Set(ref _hWPerfCounters!, value); }
    }

    public string CustomKernelPath
    {
        get { return _kernelPath!.StringValue; }
        set { Set(ref _kernelPath!, value); }
    }

    public string CustomKernelModulesPath
    {
        get { return _kernelModulesPath!.StringValue; }
        set { Set(ref _kernelModulesPath!, value); }
    }

    public string CustomSystemDistroPath
    {
        get { return _systemDistroPath!.StringValue; }
        set { Set(ref _systemDistroPath!, value); }
    }
}