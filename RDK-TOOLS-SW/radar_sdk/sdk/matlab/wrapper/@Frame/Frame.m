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

classdef Frame < handle
    %FRAME object that stores one frame of data from RadarDevice
    %   Consists of all samples from all chirps of a frame from all enabled antennas

    properties
        numRX;      % Number of RX antennas giving data
        oRXData;    % array of Matrix objects with data from each RX antenna
    end

    methods
        function obj = Frame(numRX,chirps,samples)
            %FRAME Construct an instance of Frame
            %   Instantiates the required number of Matrix objects
            %   based on number of RX antennas
            obj.numRX = numRX;
            obj.oRXData = cell([1 numRX]);
            for rx = 1:numRX
                obj.oRXData{rx} = Matrix(chirps,samples);
            end
        end

        function obj = set_data_all_antenna(obj,aRXData)
            %SET_DATA_ALL_ANTENNA Takes in a stream of raw data arriving
            %   from radar device and allocates it to the Matrix objects
            %   of the current Frame object
            %   aRXData is an array of doubles which is raw data coming straight
            %   from the Radar Device. it is assumed that this array
            %   contains one full frame (all samples from all chirps from all
            %   antennas). Otherwise the function may fail.
            dataSize = obj.oRXData{1}.chirps * obj.oRXData{1}.samples;
            for rx = 1:obj.numRX
                %deinterleaving
                aDataPerRX = aRXData( ((rx-1)*dataSize+1) : rx*dataSize );
                obj.oRXData{rx}.set_data(aDataPerRX);
            end
        end

        function oData = get_data_all_antenna(obj)
            %GET_DATA_ALL_ANTENNA returns the array of Matrix objects ..
            %   contains data from all antennas as array of Matrix objects
            oData = obj.oRXData;
        end

        function oData = get_data_per_antenna(obj,antenna)
            %GET_DATA_PER_ANTENNA returns one Matrix object
            %   contains data from the specified antenna
            oData = obj.oRXData{antenna};
        end
    end
end

