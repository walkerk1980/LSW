// Copyright (C) Microsoft Corporation. All rights reserved.

using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using System.Windows.Input;
using WslSettings.Contracts.Services;

namespace WslSettings.ViewModels.Settings;

public partial class OptionalFeaturesViewModel : WslConfigSettingViewModel
{
    private IWslConfigSetting? _memoryReclaimMode;
    private IWslConfigSetting? _gUIApplications;
    private IWslConfigSetting? _nestedVirtualization;
    private IWslConfigSetting? _safeMode;
    private IWslConfigSetting? _sparseVHD;
    private IWslConfigSetting? _vMIdleTimeout;
    private int _defaultVMIdleTimeout;

    public OptionalFeaturesViewModel()
    {
        InitializeConfigSettings();

        VMIdleTimeout_ResetEnabled = !Equals(_defaultVMIdleTimeout, _vMIdleTimeout!.Int32Value);
    }

    protected override void InitializeConfigSettings()
    {
        var lswConfigService = App.GetService<IWslConfigService>();
        _memoryReclaimMode = lswConfigService.GetWslConfigSetting(WslConfigEntry.AutoMemoryReclaim);
        _gUIApplications = lswConfigService.GetWslConfigSetting(WslConfigEntry.GUIApplicationsEnabled);
        _nestedVirtualization = lswConfigService.GetWslConfigSetting(WslConfigEntry.NestedVirtualizationEnabled);
        _safeMode = lswConfigService.GetWslConfigSetting(WslConfigEntry.SafeModeEnabled);
        _sparseVHD = lswConfigService.GetWslConfigSetting(WslConfigEntry.SparseVHDEnabled);
        _vMIdleTimeout = lswConfigService.GetWslConfigSetting(WslConfigEntry.VMIdleTimeout);

        _defaultVMIdleTimeout = lswConfigService.GetWslConfigSetting(WslConfigEntry.VMIdleTimeout, true).Int32Value;
    }

    public List<string> MemoryReclaimModes
    {
        get { return Enum.GetNames(typeof(MemoryReclaimMode)).ToList(); }
    }

    public int MemoryReclaimModeSelected
    {
        get { return (int)_memoryReclaimMode!.MemoryReclaimModeValue; }
        set { Set(ref _memoryReclaimMode!, value); }
    }

    public bool IsOnGUIApplications
    {
        get { return _gUIApplications!.BoolValue; }
        set { Set(ref _gUIApplications!, value); }
    }

    public bool IsOnNestedVirtualization
    {
        get { return _nestedVirtualization!.BoolValue; }
        set { Set(ref _nestedVirtualization!, value); }
    }

    public bool IsOnSafeMode
    {
        get { return _safeMode!.BoolValue; }
        set { Set(ref _safeMode!, value); }
    }

    public bool IsOnSparseVHD
    {
        get { return _sparseVHD!.BoolValue; }
        set { Set(ref _sparseVHD!, value); }
    }

    public string VMIdleTimeout
    {
        get
        {
            return _vMIdleTimeout!.Int32Value.ToString();
        }
        set
        {
            if (ValidateInput(value, Constants.IntegerRegex))
            {
                Set(ref _vMIdleTimeout!, Convert.ToInt32(value));
            }
        }
    }

    public void SetVMIdleTimeout_ResetEnabled(string? value)
    {
        if (Int32.TryParse(value, out Int32 parseResult))
        {
            VMIdleTimeout_ResetEnabled = !Equals(_defaultVMIdleTimeout, parseResult);
        }
        else
        {
            VMIdleTimeout_ResetEnabled = true;
        }
    }

    private bool _vMIdleTimeout_ResetEnabled;

    public bool VMIdleTimeout_ResetEnabled
    {
        get => _vMIdleTimeout_ResetEnabled;
        set => SetProperty(ref _vMIdleTimeout_ResetEnabled, value);
    }

    private void VMIdleTimeout_ResetExecuted(string? param)
    {
        VMIdleTimeout = _defaultVMIdleTimeout.ToString();
    }

    public ICommand VMIdleTimeout_ResetCommand => new RelayCommand<string>(VMIdleTimeout_ResetExecuted);
}