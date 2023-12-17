#include "RadarDeviceUtils.hpp"
#include <algorithm>


namespace
{
    const std::array<uint16_t, 3>& get_firmware_min_version_according_board(const BoardType boardtype, bool& ok)
    {
        static std::array<uint16_t, 3> null_version = {};

        switch (boardtype)
        {
        case BoardType::RadarBaseboardAurix:
            ok = true;
            return min_firmware_version_aurix;
            break;

        case BoardType::RadarBaseboardMCU7:
            ok = true;
            return min_firmware_version_mcu7;
            break;

        case BoardType::V9:
            ok = true;
            return min_firmware_version_v9;
            break;

        default:
            // unknown board, so not supported
            ok = false;
            return null_version;
        };
    }
} // end of anonymous namespace


/*static*/
bool RadarDeviceUtils::is_firmware_version_supported(const BoardType boardtype, const uint16_t major, const uint8_t minor, const uint16_t build)
{
    bool supported;
    decltype(auto) min_fw_version = ::get_firmware_min_version_according_board(boardtype, supported);

    if(supported) {
        const std::array<uint16_t, 3> current_fw_version = {major, minor, build};

        supported = !std::lexicographical_compare(current_fw_version.begin(), current_fw_version.end(),
                                                  min_fw_version.begin(), min_fw_version.end());
    }

    return supported;
}
