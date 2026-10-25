#pragma once

#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace sotc::test {

// Test-only decoder for opaque PNG fixtures. On success, rgba8 contains tightly packed,
// top-row-first RGBA8 pixels; every alpha byte is 255.
[[nodiscard]] bool decode_asset_png_rgba8(std::span<const std::uint8_t> png,
                                          std::uint32_t expected_width,
                                          std::uint32_t expected_height,
                                          std::vector<std::uint8_t>& rgba8,
                                          std::string& error);

}  // namespace sotc::test
