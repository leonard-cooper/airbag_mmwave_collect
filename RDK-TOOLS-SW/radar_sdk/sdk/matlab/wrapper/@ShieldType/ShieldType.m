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

classdef ShieldType

    properties
        value_as_uint,
    end

    methods
        function obj = ShieldType(uint_value)
            obj.value_as_uint = uint_value;
        end

        function setValue(obj, uint_value)
            obj.value_as_uint = uint_value;
        end

        function [out] = getValue(obj)
            if obj.value_as_uint == 0
                out = "DEVICE_MISSING";
            elseif obj.value_as_uint == 1
                out = "DEVICE_UNKNOWN";
            elseif obj.value_as_uint == 256
                out = "DEVICE_RBB_MCU7";
            elseif obj.value_as_uint == 512
                out = "DEVICE_BGT60TR13AIP_SHIELD";
            elseif obj.value_as_uint == 513
                out = "DEVICE_BGT60ATR24AIP_SHIELD";
            elseif obj.value_as_uint == 514
                out = "DEVICE_BGT60UTR11_SHIELD";
            elseif obj.value_as_uint == 515
                out = "DEVICE_BGT60UTR13D_SHIELD";
            elseif obj.value_as_uint == 768
                out = "DEVICE_BGT60LTR11_SHIELD";
            elseif obj.value_as_uint == 769
                out = "DEVICE_BGT60LTR11_MONOSTAT_SHIELD";
            elseif obj.value_as_uint == 770
                out = "DEVICE_BGT60LTR11_B11_SHIELD";
            elseif obj.value_as_uint == 1024
                out = "DEVICE_BGT24ATR22_ES_SHIELD";
            elseif obj.value_as_uint == 1025
                out = "DEVICE_BGT24ATR22_PROD_SHIELD";
            elseif obj.value_as_uint == 65535
                out = "DEVICE_ANY";
            else
                out = "DEVICE_UNRECOGNIZED";
            end
        end
    end

    methods (Static = true)

        function [out] = DEVICE_MISSING()
            out = "DEVICE_MISSING";
        end

        function [out] = DEVICE_UNKNOWN()
            out = "DEVICE_UNKNOWN";
        end

        function [out] = DEVICE_RBB_MCU7()
            out = "DEVICE_RBB_MCU7";
        end

        function [out] = DEVICE_BGT60TR13AIP_SHIELD()
            out = "DEVICE_BGT60TR13AIP_SHIELD";
        end

        function [out] = DEVICE_BGT60ATR24AIP_SHIELD()
            out = "DEVICE_BGT60ATR24AIP_SHIELD";
        end

        function [out] = DEVICE_BGT60UTR11_SHIELD()
            out = "DEVICE_BGT60UTR11_SHIELD";
        end

        function [out] = DEVICE_BGT60UTR13D_SHIELD()
            out = "DEVICE_BGT60UTR13D_SHIELD";
        end

        function [out] = DEVICE_BGT60LTR11_SHIELD()
            out = "DEVICE_BGT60LTR11_SHIELD";
        end

        function [out] = DEVICE_BGT60LTR11_MONOSTAT_SHIELD()
            out = "DEVICE_BGT60LTR11_MONOSTAT_SHIELD";
        end

        function [out] = DEVICE_BGT60LTR11_B11_SHIELD()
            out = "DEVICE_BGT60LTR11_B11_SHIELD";
        end

        function [out] = DEVICE_BGT24ATR22_ES_SHIELD()
            out = "DEVICE_BGT24ATR22_ES_SHIELD";
        end

        function [out] = DEVICE_BGT24ATR22_PROD_SHIELD()
            out = "DEVICE_BGT24ATR22_PROD_SHIELD";
        end

        function [out] = DEVICE_ANY()
            out = "DEVICE_ANY";
        end
    end
end
