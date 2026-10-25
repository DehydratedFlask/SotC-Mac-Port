#include "cmsh_mesh.hpp"

#include <array>
#include <bit>
#include <cmath>
#include <limits>

namespace sotc {
namespace {

constexpr std::size_t kDescriptorSize = 24;
constexpr std::size_t kSentinelSize = 8;
constexpr std::uint16_t kPositionFormatFloat3 = 2;
constexpr std::uint32_t kIndexMarker = 1;

enum class StreamKind : std::uint8_t { position, normal, tangent, texcoord };

struct Descriptor {
    StreamKind kind{};
    std::uint16_t format{};
    std::uint32_t offset{};
    std::uint32_t size{};
    std::size_t tag_offset{};
    std::size_t start{};
    std::size_t end{};
};

[[nodiscard]] bool checked_add(std::size_t left, std::size_t right,
                               std::size_t& result) noexcept {
    if (right > std::numeric_limits<std::size_t>::max() - left) return false;
    result = left + right;
    return true;
}

[[nodiscard]] std::uint16_t read_u16(std::span<const std::byte> bytes,
                                     std::size_t offset) noexcept {
    const auto lo = std::to_integer<std::uint16_t>(bytes[offset]);
    const auto hi = std::to_integer<std::uint16_t>(bytes[offset + 1]);
    return static_cast<std::uint16_t>(lo | (hi << 8));
}

[[nodiscard]] std::uint32_t read_u32(std::span<const std::byte> bytes,
                                     std::size_t offset) noexcept {
    const auto b0 = std::to_integer<std::uint32_t>(bytes[offset]);
    const auto b1 = std::to_integer<std::uint32_t>(bytes[offset + 1]);
    const auto b2 = std::to_integer<std::uint32_t>(bytes[offset + 2]);
    const auto b3 = std::to_integer<std::uint32_t>(bytes[offset + 3]);
    return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
}

void write_kind_bytes(StreamKind kind, std::array<char, 4>& tag) noexcept {
    switch (kind) {
        case StreamKind::position: tag = {'0', 'S', 'O', 'P'}; break;
        case StreamKind::normal: tag = {'0', 'M', 'R', 'N'}; break;
        case StreamKind::tangent: tag = {'0', 'N', 'A', 'T'}; break;
        case StreamKind::texcoord: tag = {'0', 'X', 'E', 'T'}; break;
    }
}

[[nodiscard]] bool has_tag(std::span<const std::byte> bytes, std::size_t offset,
                           const std::array<char, 4>& tag) noexcept {
    if (offset > bytes.size() || bytes.size() - offset < tag.size()) return false;
    for (std::size_t i = 0; i < tag.size(); ++i) {
        if (std::to_integer<unsigned char>(bytes[offset + i]) !=
            static_cast<unsigned char>(tag[i])) {
            return false;
        }
    }
    return true;
}

[[nodiscard]] bool decode_tag(std::span<const std::byte> bytes, std::size_t offset,
                              StreamKind& kind) noexcept {
    constexpr std::array<StreamKind, 4> kinds = {
        StreamKind::position, StreamKind::normal, StreamKind::tangent, StreamKind::texcoord};
    for (const StreamKind candidate : kinds) {
        std::array<char, 4> tag{};
        write_kind_bytes(candidate, tag);
        if (has_tag(bytes, offset, tag)) {
            kind = candidate;
            return true;
        }
    }
    return false;
}

[[nodiscard]] std::size_t find_position_tag(std::span<const std::byte> bytes) noexcept {
    std::array<char, 4> tag{};
    write_kind_bytes(StreamKind::position, tag);
    if (bytes.size() < tag.size()) return bytes.size();
    for (std::size_t offset = 0; offset <= bytes.size() - tag.size(); ++offset) {
        if (has_tag(bytes, offset, tag)) return offset;
    }
    return bytes.size();
}

[[nodiscard]] bool is_sentinel(std::span<const std::byte> bytes,
                               std::size_t offset) noexcept {
    if (offset > bytes.size() || bytes.size() - offset < kSentinelSize) return false;
    for (std::size_t i = 0; i < kSentinelSize; ++i) {
        if (bytes[offset + i] != std::byte{0xff}) return false;
    }
    return true;
}

[[nodiscard]] bool is_truncated_sentinel(std::span<const std::byte> bytes,
                                         std::size_t offset) noexcept {
    if (offset > bytes.size()) return false;
    const std::size_t remaining = bytes.size() - offset;
    if (remaining == 0 || remaining >= kSentinelSize) return false;
    for (std::size_t i = 0; i < remaining; ++i) {
        if (bytes[offset + i] != std::byte{0xff}) return false;
    }
    return true;
}

[[nodiscard]] CmshParseError error(CmshErrorCode code, std::size_t offset) noexcept {
    return CmshParseError{code, offset};
}

}  // namespace

CmshParseResult parse_cmsh_mesh(std::span<const std::byte> bytes) {
    if (!bytes.empty() && std::to_integer<std::uint8_t>(bytes.front()) == 0x8c) {
        return error(CmshErrorCode::unsupported_compressed_layout, 0);
    }

    const std::size_t first_descriptor = find_position_tag(bytes);
    if (first_descriptor == bytes.size()) {
        return error(CmshErrorCode::missing_position_descriptor, 0);
    }

    std::vector<Descriptor> descriptors;
    std::array<bool, 4> seen_kinds{};
    std::size_t cursor = first_descriptor;
    bool found_sentinel = false;

    while (cursor <= bytes.size()) {
        if (cursor > bytes.size() || bytes.size() - cursor < 4) {
            return error(CmshErrorCode::missing_sentinel, cursor);
        }
        if (is_sentinel(bytes, cursor)) {
            cursor += kSentinelSize;
            found_sentinel = true;
            break;
        }
        if (is_truncated_sentinel(bytes, cursor)) {
            return error(CmshErrorCode::missing_sentinel, cursor);
        }

        StreamKind kind{};
        if (!decode_tag(bytes, cursor, kind)) {
            return error(CmshErrorCode::unsupported_descriptor_tag, cursor);
        }
        if (cursor > bytes.size() || bytes.size() - cursor < kDescriptorSize) {
            return error(CmshErrorCode::truncated_descriptor, cursor);
        }

        const auto kind_index = static_cast<std::size_t>(kind);
        if (seen_kinds[kind_index]) {
            return error(CmshErrorCode::duplicate_descriptor, cursor);
        }
        seen_kinds[kind_index] = true;

        const std::uint16_t format = read_u16(bytes, cursor + 4);
        const std::uint32_t stream_offset = read_u32(bytes, cursor + 8);
        const std::uint32_t stream_size = read_u32(bytes, cursor + 16);
        descriptors.push_back(Descriptor{kind, format, stream_offset, stream_size, cursor, 0, 0});
        cursor += kDescriptorSize;
    }

    if (!found_sentinel) {
        return error(CmshErrorCode::missing_sentinel, cursor);
    }

    if (descriptors.empty() || descriptors.front().kind != StreamKind::position) {
        return error(CmshErrorCode::missing_position_descriptor, first_descriptor);
    }

    const Descriptor& position = descriptors.front();
    if (position.format != kPositionFormatFloat3) {
        return error(CmshErrorCode::invalid_position_format, position.tag_offset + 4);
    }
    if (position.size % 12 != 0) {
        return error(CmshErrorCode::invalid_position_size, position.tag_offset + 16);
    }

    if (position.offset > cursor) {
        return error(CmshErrorCode::invalid_stream_offset, position.tag_offset + 8);
    }
    const std::size_t data_start = cursor;
    const std::size_t base = data_start - static_cast<std::size_t>(position.offset);

    std::size_t tail = 0;
    for (Descriptor& descriptor : descriptors) {
        if (descriptor.offset < position.offset) {
            return error(CmshErrorCode::invalid_stream_offset, descriptor.tag_offset + 8);
        }

        std::size_t relative_end = 0;
        if (!checked_add(static_cast<std::size_t>(descriptor.offset),
                         static_cast<std::size_t>(descriptor.size), relative_end) ||
            !checked_add(base, static_cast<std::size_t>(descriptor.offset), descriptor.start) ||
            !checked_add(base, relative_end, descriptor.end)) {
            return error(CmshErrorCode::invalid_stream_offset, descriptor.tag_offset + 8);
        }
        if (descriptor.start < data_start || descriptor.end > bytes.size()) {
            return error(CmshErrorCode::stream_out_of_bounds, descriptor.tag_offset + 8);
        }
        if (descriptor.end > tail) tail = descriptor.end;
    }

    for (std::size_t i = 0; i < descriptors.size(); ++i) {
        for (std::size_t j = i + 1; j < descriptors.size(); ++j) {
            const Descriptor& left = descriptors[i];
            const Descriptor& right = descriptors[j];
            if (left.start < right.end && right.start < left.end) {
                return error(CmshErrorCode::overlapping_streams, right.tag_offset + 8);
            }
        }
    }

    if (tail > bytes.size() || bytes.size() - tail < 8) {
        return error(CmshErrorCode::truncated_index_tail, tail);
    }
    const std::uint32_t marker = read_u32(bytes, tail);
    if (marker != kIndexMarker) {
        return error(CmshErrorCode::invalid_index_marker, tail);
    }
    const std::uint32_t index_count_u32 = read_u32(bytes, tail + 4);
    const std::size_t index_count = static_cast<std::size_t>(index_count_u32);
    if (index_count % 3 != 0) {
        return error(CmshErrorCode::index_count_not_triangle_aligned, tail + 4);
    }
    if (index_count > std::numeric_limits<std::size_t>::max() / sizeof(std::uint16_t)) {
        return error(CmshErrorCode::truncated_index_tail, tail + 4);
    }
    const std::size_t index_bytes = index_count * sizeof(std::uint16_t);
    const std::size_t index_start = tail + 8;
    if (index_bytes > bytes.size() - index_start) {
        return error(CmshErrorCode::truncated_index_tail, tail + 4);
    }

    const std::size_t position_count = static_cast<std::size_t>(position.size / 12);
    CmshMesh mesh;
    mesh.positions.reserve(position_count);
    for (std::size_t i = 0; i < position_count; ++i) {
        const std::size_t point_offset = position.start + i * 12;
        const CmshPosition point{
            std::bit_cast<float>(read_u32(bytes, point_offset)),
            std::bit_cast<float>(read_u32(bytes, point_offset + 4)),
            std::bit_cast<float>(read_u32(bytes, point_offset + 8)),
        };
        if (!std::isfinite(point.x) || !std::isfinite(point.y) || !std::isfinite(point.z)) {
            return error(CmshErrorCode::non_finite_position, point_offset);
        }
        mesh.positions.push_back(point);
    }

    mesh.indices.reserve(index_count);
    for (std::size_t i = 0; i < index_count; ++i) {
        const std::size_t index_offset = index_start + i * sizeof(std::uint16_t);
        const std::uint16_t index = read_u16(bytes, index_offset);
        if (static_cast<std::size_t>(index) >= position_count) {
            return error(CmshErrorCode::index_out_of_range, index_offset);
        }
        mesh.indices.push_back(index);
    }

    return mesh;
}

const char* cmsh_error_name(CmshErrorCode code) noexcept {
    switch (code) {
        case CmshErrorCode::unsupported_compressed_layout: return "unsupported compressed layout";
        case CmshErrorCode::missing_position_descriptor: return "missing POS0 descriptor";
        case CmshErrorCode::truncated_descriptor: return "truncated stream descriptor";
        case CmshErrorCode::unsupported_descriptor_tag: return "unsupported stream descriptor tag";
        case CmshErrorCode::duplicate_descriptor: return "duplicate stream descriptor";
        case CmshErrorCode::missing_sentinel: return "missing descriptor sentinel";
        case CmshErrorCode::invalid_position_format: return "unsupported POS0 format";
        case CmshErrorCode::invalid_position_size: return "POS0 size is not a float3 span";
        case CmshErrorCode::invalid_stream_offset: return "invalid stream offset arithmetic";
        case CmshErrorCode::stream_out_of_bounds: return "stream span is outside the input";
        case CmshErrorCode::overlapping_streams: return "stream spans overlap";
        case CmshErrorCode::truncated_index_tail: return "truncated index tail";
        case CmshErrorCode::invalid_index_marker: return "unsupported index marker";
        case CmshErrorCode::index_count_not_triangle_aligned: return "index count is not divisible by three";
        case CmshErrorCode::index_out_of_range: return "index refers past POS0 vertex count";
        case CmshErrorCode::non_finite_position: return "POS0 contains a non-finite position";
    }
    return "unknown cmsh parse error";
}

}  // namespace sotc
