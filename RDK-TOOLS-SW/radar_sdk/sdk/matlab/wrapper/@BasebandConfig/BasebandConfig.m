% ===========================================================================
% Copyright (C) 2021 Infineon Technologies AG
%
% Redistribution and use in source and binary forms, with or without
% modification, are permitted provided that the following conditions are met:
%
% 1. Redistributions of source code must retain the above copyright notice,
%    this list of conditions and the following disclaimer.
% 2. Redistributions in binary form must reproduce the above copyright
%    notice, this list of conditions and the following disclaimer in the
%    documentation and/or other materials provided with the distribution.
% 3. Neither the name of the copyright holder nor the names of its
%    contributors may be used to endorse or promote products derived from
%    this software without specific prior written permission.
%
% THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
% AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
% IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
% ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
% LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
% CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
% SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
% INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
% CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
% ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
% POSSIBILITY OF SUCH DAMAGE.
% ===========================================================================

classdef BasebandConfig
    % BASEBANDCONFIG Class definition for configuring a Constant Wave Control Baseband parameters
    % This matlab class creates an object with all parameters to configure the device,
    % similar to the configuration of the radar_sdk. The BasebandConfig object is required
    % as an input to set baseband paramsfor Constant Wave Control. This class also provides a helper
    % function mkBasebandConf() to create a BasebandConfig object and accepts param-value pairs in
    % any order. It assumes default values for any parameter that is not specified.

    properties
        % Gain setting of the Avian device integrated baseband amplifier (VGA)
        vga_gain,

        % Gain settings of the Avian device integrated baseband high pass filters
        hp_gain,

        % Cutoff frequency settings of the Avian device integrated baseband high pass filters
        hp_cutoff,

        % Cutoff frequency settings of the Avian device integrated baseband anti alias filteraaf_cutoff
        aaf_cutoff,
    end

    methods
        function obj = BasebandConfig(vga_gain, ...
                                    hp_gain, ...
                                    hp_cutoff, ...
                                    aaf_cutoff)
            % BASEBANDCONFIG Construct an instance of this class
            %   The function instantiates a Baseband Config object
            obj.vga_gain = vga_gain;
            obj.hp_gain = hp_gain;
            obj.hp_cutoff = hp_cutoff;
            obj.aaf_cutoff = aaf_cutoff;
        end
    end

    methods(Static)
        function obj = mkBasebandConf(varargin)
            % MKBASEBANDCONF Helper function for creating a Baseband Config object
            %   This function accepts the config params as param - value pairs
            %   in any order, and uses default values for any unspecified
            %   param, and returns a Baseband Config object.
            %   accepted parameters and their default values are as follows:
            %   param: vga_gain           default val = 0; % 0 dB
            %   param: hp_gain            default val = 0; % 18 dB
            %   param: hp_cutoff          default val = 0; % 20 kHz 
            %   param: aaf_cutoff         default val = 0; % 600 kHz

            p = inputParser;
            addParameter(p, 'vga_gain', DeviceConfigOptions.ifx_Device_Baseband_Vga_Gain.IFX_VGA_GAIN_0dB);
            addParameter(p, 'hp_gain', DeviceConfigOptions.ifx_Device_Baseband_Hp_Gain.IFX_HP_GAIN_18dB);
            addParameter(p, 'hp_cutoff', DeviceConfigOptions.ifx_Device_Baseband_Hp_Cutoff.IFX_HP_CUTOFF_20kHz);
            addParameter(p, 'aaf_cutoff', DeviceConfigOptions.ifx_Device_Baseband_Aaf_Cutoff.IFX_AAF_CUTOFF_600kHz);

            parse(p,varargin{:});
            params = p.Results;

            obj = BasebandConfig(params.vga_gain, ...
                               params.hp_gain, ...
                               params.hp_cutoff, ...
                               params.aaf_cutoff);
        end
    end
end

