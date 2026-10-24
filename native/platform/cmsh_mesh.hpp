#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <variant>
#include <vector>

namespace sotc {

struct CmshPosition {
    float x{};
    float y{};
    float z{};
};

struct CmshMesh {
    std::vector<CmshPosition> positions;
    std::vector<std::uint16_t> indices;
};

enum class CmshErrorCode : std::uint8_t {
    unsupported_compressed_layout,
    missing_position_descriptor,
    truncated_descriptor,
    unsupported_descriptor_tag,
    duplicate_descriptor,
    missing_sentinel,
    invalid_position_format,
    invalid_position_size,
    invalid_stream_offset,
    stream_out_of_bounds,
    overlapping_streams,
    truncated_index_tail,
    invalid_index_marker,
    index_count_not_triangle_aligned,
    index_out_of_range,
    non_finite_position,
};

struct CmshParseError {
    CmshErrorCode code{};
    std::size_t byte_offset{};
};

using CmshParseResult = std::variant<CmshMesh, CmshParseError>;

// Parses the admitted raw POS0 float3 / U16 triangle-list subset. NRM0, TAN0,
// and TEX0 descriptors are range-checked but their payloads and formats stay opaque.
[[nodiscard]] CmshParseResult parse_cmsh_mesh(std::span<const std::byte> bytes);

[[nodiscard]] const char* cmsh_error_name(CmshErrorCode code) noexcept;

}  // namespace sotc
