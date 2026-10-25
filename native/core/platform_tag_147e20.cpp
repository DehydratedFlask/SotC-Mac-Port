#include "platform_tag_147e20.hpp"

namespace sotc {

const PlatformTagRow kPlatformTagRows[kPlatformTagRowCount] = {
    {fourcc_of("crmp"), kPlatformTagCmn},
    {fourcc_of("csdr"), kPlatformTagCmn},
    {fourcc_of("cslt"), kPlatformTagCmn},
    {fourcc_of("cmat"), kPlatformTagCmn},
    {fourcc_of("cmsh"), kPlatformTagCmn},
    {fourcc_of("cmdl"), kPlatformTagCmn},
    {fourcc_of("cfon"), kPlatformTagPs4},
    {fourcc_of("csli"), kPlatformTagCmn},
    {fourcc_of("cpid"), kPlatformTagCmn},
    {fourcc_of("cvmg"), kPlatformTagCmn},
    {fourcc_of("cvgs"), kPlatformTagCmn},
    {fourcc_of("cskl"), kPlatformTagCmn},
    {fourcc_of("cani"), kPlatformTagCmn},
    {fourcc_of("cptx"), kPlatformTagPs4},
    {fourcc_of("cprt"), kPlatformTagCmn},
    {fourcc_of(".sxd"), kPlatformTagCmn},
    {fourcc_of("cgpr"), kPlatformTagCmn},
    {fourcc_of("csml"), kPlatformTagCmn},
    {fourcc_of("cctx"), kPlatformTagPs4},
    {fourcc_of("cxml"), kPlatformTagCmn},
    {fourcc_of("calt"), kPlatformTagCmn},
    {fourcc_of("cclm"), kPlatformTagCmn},
    {fourcc_of("capx"), kPlatformTagCmn},
    {fourcc_of("cchr"), kPlatformTagCmn},
    {kPlatformWildcardFourcc, kPlatformTagCmn},
};

std::uint32_t platform_tag_147e20_default(std::uint32_t fourcc) noexcept {
    return platform_tag_147e20(fourcc, PlatformRegistryState::kPopulated);
}

std::uint32_t platform_tag_147e20(std::uint32_t fourcc,
                                  PlatformRegistryState state) noexcept {
    if (state == PlatformRegistryState::kUninitialised) {
        return kPlatformTagPs4;
    }
    for (const PlatformTagRow& row : kPlatformTagRows) {
        if (row.fourcc == fourcc) {
            return row.tag;
        }
    }
    for (const PlatformTagRow& row : kPlatformTagRows) {
        if (row.fourcc == kPlatformWildcardFourcc) {
            return row.tag;
        }
    }
    return kPlatformTagPs4;
}

}  // namespace sotc
