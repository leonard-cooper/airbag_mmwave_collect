classdef ifx_Device_Baseband_Aaf_Cutoff < int32
    %IFX_DEVICE_BASEBAND_AAF_CUTOFF  This enumeration type lists the
    %available cutoff frequency settings of the Avian device's integrated
    %baseband anti alias filter.
    enumeration
        IFX_AAF_CUTOFF_600kHz  (0) % The cutoff frequency of the anti alias filter is 600kHz
        IFX_AAF_CUTOFF_1MHz    (1) % The cutoff frequency of the anti alias filter is 1MHz
    end
end
