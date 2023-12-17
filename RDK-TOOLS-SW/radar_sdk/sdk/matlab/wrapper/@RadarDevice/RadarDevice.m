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

classdef RadarDevice < handle
    %RADARDEVICE Class for a radar device.
    %   An object of this class helps to
    %   - configure a radar device and connect to it
    %   - retrieve radar data from it
    %   - disconnect from it.
    %   This object may be used as a Radar SDK wrapper in matlab,
    %   as it performs the above tasks similar to Radar SDK


    % TODO: When 'connect' failed, the hardware device needs to be reset. But the
    % 'dev_handle' is invalid

    properties
        oDevConf;   % DeviceConfig object
        cw_control_handle; % Reference to Constant Wave Control Object handler. It protects from destroying CW when the device handle is no longer available.
    end

    properties (Access = private)
        dll_ver;            % sdk dll version
        device_handle;      % device handle given by radar sdk
    end

    properties (Constant)
        min_major = 1;
        min_minor = 1;
        min_patch = 1;

        error_api_base = hex2dec('00010000');
        error_dev_base = hex2dec('00011000');
        IFX_OK = 0;
    end

    methods
        function obj = RadarDevice(oDevConf)
            %RADARDEVICE Constructor for an instance of RadarDevice
            obj.oDevConf = oDevConf;
            obj.device_handle = 0;
            obj.cw_control_handle = 0;
            disp('******************************************************************');
        end

        function connect(obj)
            %CONNECT connect to a Radar Device attached via USB
            %   This method connects to the Radar device, configures it with the specified params
            %   provided with DeviceConfig object during construction and triggers transmission
            %   After this method is called, the radar device is active
            try
                [ec, ver] = DeviceControlM('get_version');
                RadarDevice.check_error_code(ec);
                obj.dll_ver = ver;
                %obj.check_version();
                [ec, handle] = DeviceControlM('create');
                RadarDevice.check_error_code(ec);
                obj.device_handle = handle;
                ec = DeviceControlM('set_config', obj.device_handle, obj.oDevConf);
                RadarDevice.check_error_code(ec);
                disp('Radar SDK Matlab MEX Wrapper: Radar Device created and connected.');
            catch ME
                rethrow(ME);
            end
        end

        function disconnect(obj)
            %DISCONNECT disconnect from a Radar Device attached via USB
            %   This method disconnects from the port where the radar device is connected
            if obj.device_handle ~= 0
                if(obj.cw_control_handle ~= 0)
                    obj.cw_control_handle.delete();
                end
                ec = DeviceControlM('destroy', obj.device_handle);
                RadarDevice.check_error_code(ec);
                obj.device_handle = 0;
                disp('Radar SDK Matlab MEX Wrapper: Radar Device disconnected and destroyed.');
            end
        end

        function constant_wave_control = create_cw_control(obj)
            %CREATE_CW_CONTROL Creates and initializes the constant wave mode controller instance for a connected device
            
            obj.cw_control_handle = ConstantWaveControl(obj.device_handle);
            constant_wave_control = obj.cw_control_handle;
        end

        function reg_list_string = get_register_list_string(obj, trigger)
            %GET_REG_LIST_STRING get register contents from a Radar Device attached via USB
            %   This method returns the device register list in hexadecimal string format
            try
                [err_code, reg_list_string] = DeviceControlM('get_register_list_string',obj.device_handle, trigger);
                obj.check_error_code(err_code);
            catch ME
                rethrow(ME);
            end
        end

        function RxFrame = get_next_frame_data(obj, timeout_ms_opt)
            %GET_NEXT_FRAME_DATA method to fetch radar data from radar device
            %   This method fetches data from the radar device in terms of one
            %   frame (all samples of all chirps from all enabled antennas)
            %   and returns a Frame object

            Total_samples = [];
            RxFrame = [];
            if exist('timeout_ms_opt','var')
                [ec, num_rx, num_samples_per_chirp, num_chirpts_per_frame, Total_samples] = DeviceControlM('get_next_frame_timeout', obj.device_handle, timeout_ms_opt);
            else
                [ec, num_rx, num_samples_per_chirp, num_chirpts_per_frame, Total_samples] = DeviceControlM('get_next_frame', obj.device_handle);
            end

            RadarDevice.check_error_code(ec);

            RxFrame = Frame(num_rx, num_chirpts_per_frame, num_samples_per_chirp);
            RxFrame.set_data_all_antenna(Total_samples);
        end

        function Started = start_acquisition(obj)
            %START_ACQUISITION starts the acquisition of raw data
            %   This method starts the acquisition of raw data when the radar device is connected
            if obj.device_handle ~= 0
                [ec, Started] = DeviceControlM('start_acquisition', obj.device_handle);
                RadarDevice.check_error_code(ec);
                if(Started)
                  disp('Radar SDK Matlab MEX Wrapper: acquisition of raw data has started.');
                end
            end
        end

        function Stopped = stop_acquisition(obj)
            %STOP_ACQUISITION stops the acquisition of raw data
            %   This method stops the acquisition of raw data when the radar device is connected
            if obj.device_handle ~= 0
                [ec, Stopped] = DeviceControlM('stop_acquisition', obj.device_handle);
                RadarDevice.check_error_code(ec);
                if(Stopped)
                  disp('Radar SDK Matlab MEX Wrapper: acquisition of raw data has stopped.');
                end
            end
        end

        function dev_info = get_device_information(obj)
            %GET_DEVICE_INFORMATION Gets information about the connected device
            try
                oDevInfo = DeviceInfo();
                [err_code, dev_info] = DeviceControlM('get_device_information', obj.device_handle, oDevInfo);
                obj.check_error_code(err_code);
            catch ME
                rethrow(ME);
            end
        end

        function shield_type = get_device_shield_type(obj)
            %GET_DEVICE_SHIELD_TYPE Gets the shield type from the connected device
            try
                obj.oDevInfo = DeviceInfo();
                [err_code, obj.oDevInfo] = DeviceControlM('get_device_information', obj.device_handle, obj.oDevInfo);
                obj.check_error_code(err_code);
                shield_type_as_enum = ShieldType(obj.oDevInfo.shield_type);
                shield_type = shield_type_as_enum.getValue();
            catch ME
                rethrow(ME);
            end
        end

        function fw_info = get_firmware_information(obj)
            %GET_FIRMWARE_INFORMATION Gets information about the firmware of a connected device
            try
                oFwInfo = FirmwareInfo();
                [err_code, fw_info] = DeviceControlM('get_firmware_information', obj.device_handle, oFwInfo);
                obj.check_error_code(err_code);
            catch ME
                rethrow(ME);
            end
        end

        function delete(obj)
            %RADARDEVICE destroyer for the RadarDevice object
            obj.disconnect();
        end
    end

    methods(Static)
        function version = getVersionFull();
            % Returns the full SDL version string including
            % the git hash from which this release was build
            [ec, version] = DeviceControlM('get_version_full');
        end

        function version = getVersion();
            % Returns the short SDL version string excluding
            % the git hash from which this release was build
            [ec, version] = DeviceControlM('get_version');
        end
    end

    %% Private methods
    methods (Access = private)
        function check_version(obj)
            v = strsplit(obj.dll_ver, '.');
            major = str2num(v{1});
            minor = str2num(v{2});
            patch = str2num(v{3});

            passed = true;
            if major < obj.min_major
                passed = false;
            elseif major == obj.min_major
                if minor < obj.min_minor
                    passed = false;
                elseif minor == obj.min_minor
                    if patch < obj.min_patch
                        passed = false;
                    end
                end
            end

            if passed == false
                msg = sprintf('Error: sdk dll version: %d.%d.%d, but expected at least: %d.%d.%d', major, minor, patch, obj.min_major, obj.min_minor, obj.min_patch);
                error(msg);
            end
        end
    end

    methods (Static)
        function ret = check_error_code(err_code)
            ret = (err_code ~= 0);
            if(ret)
                description = DeviceControlM(':describe_error', err_code);
                ME = MException(['RadarDevice:error' num2str(err_code)], description);
                throw(ME);
            end
        end

        function shield_type = get_device_shield_type_from_information(information)
            shield_type_as_enum = ShieldType(information.shield_type);
            shield_type = shield_type_as_enum.getValue();
        end

    end

end

