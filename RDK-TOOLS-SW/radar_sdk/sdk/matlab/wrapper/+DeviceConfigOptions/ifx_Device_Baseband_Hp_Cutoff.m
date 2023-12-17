classdef ifx_Device_Baseband_Hp_Cutoff < int32
    %IFX_DEVICE_BASEBAND_HP_CUTOFF  This enumeration type lists the
    %available cutoff frequency settings of the Avian device's integrated
    %baseband high pass filters.
    enumeration
        IFX_HP_CUTOFF_20kHz   (0)  % The cutoff frequency of the high pass filter is 20kHz.
        IFX_HP_CUTOFF_45kHz   (1)  % The cutoff frequency of the high pass filter is 45kHz.
        IFX_HP_CUTOFF_70kHz   (2)  % The cutoff frequency of the high pass filter is 70kHz.
        IFX_HP_CUTOFF_80kHz   (3)  % The cutoff frequency of the high pass filter is 80kHz.
        IFX_HP_CUTOFF_160kHz  (4)  % The cutoff frequency of the high pass filter is 160kHz.
    end
end
