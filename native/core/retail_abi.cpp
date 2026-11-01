#include "retail_abi.hpp"

namespace sotc {

const RetailArgSpec* retail_arg_spec_for(std::uint64_t rva) noexcept {
    for (std::size_t index = 0; index < kRetailArgSpecCount; ++index) {
        if (kRetailArgSpecs[index].rva == rva) {
            return &kRetailArgSpecs[index];
        }
    }
    return nullptr;
}

bool retail_takes_at_least(std::uint64_t rva, std::uint8_t count) noexcept {
    const RetailArgSpec* spec = retail_arg_spec_for(rva);
    return spec != nullptr && spec->argument_count >= count;
}

}  // namespace sotc
