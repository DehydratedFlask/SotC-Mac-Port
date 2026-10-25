#include "../platform/cmsh_mesh.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <span>
#include <string_view>
#include <utility>
#include <vector>

namespace {

using sotc::CmshErrorCode;
using sotc::CmshMesh;
using sotc::CmshParseError;
using sotc::CmshParseResult;

constexpr std::size_t kHeaderBytes = 0x4e;
constexpr std::size_t kDescriptorBytes = 24;
constexpr std::size_t kDescriptorStart = kHeaderBytes;
constexpr std::size_t kPositionFormatOffset = kDescriptorStart + 4;
constexpr std::size_t kPositionStreamOffsetField = kDescriptorStart + 8;
constexpr std::size_t kPositionSizeField = kDescriptorStart + 16;
constexpr std::size_t kNormalDescriptor = kDescriptorStart + kDescriptorBytes;
constexpr std::size_t kNormalFormatOffset = kNormalDescriptor + 4;
constexpr std::size_t kNormalStreamOffsetField = kNormalDescriptor + 8;
constexpr std::size_t kTexcoordDescriptor = kDescriptorStart + 3 * kDescriptorBytes;
constexpr std::size_t kTexcoordStreamOffsetField = kTexcoordDescriptor + 8;

struct Fixture {
    std::vector<std::byte> bytes;
    std::size_t sentinel_offset{};
    std::size_t data_start{};
    std::size_t base{};
    std::size_t position_start{};
    std::size_t index_tail{};
};

struct StreamSpec {
    std::array<char, 4> tag;
    std::uint16_t format;
    std::uint32_t size;
};

void put_u16(std::vector<std::byte>& bytes, std::size_t offset, std::uint16_t value) {
    bytes[offset] = static_cast<std::byte>(value & 0xff);
    bytes[offset + 1] = static_cast<std::byte>((value >> 8) & 0xff);
}

void put_u32(std::vector<std::byte>& bytes, std::size_t offset, std::uint32_t value) {
    bytes[offset] = static_cast<std::byte>(value & 0xff);
    bytes[offset + 1] = static_cast<std::byte>((value >> 8) & 0xff);
    bytes[offset + 2] = static_cast<std::byte>((value >> 16) & 0xff);
    bytes[offset + 3] = static_cast<std::byte>((value >> 24) & 0xff);
}

void append_u16(std::vector<std::byte>& bytes, std::uint16_t value) {
    bytes.push_back(static_cast<std::byte>(value & 0xff));
    bytes.push_back(static_cast<std::byte>((value >> 8) & 0xff));
}

void append_u32(std::vector<std::byte>& bytes, std::uint32_t value) {
    bytes.push_back(static_cast<std::byte>(value & 0xff));
    bytes.push_back(static_cast<std::byte>((value >> 8) & 0xff));
    bytes.push_back(static_cast<std::byte>((value >> 16) & 0xff));
    bytes.push_back(static_cast<std::byte>((value >> 24) & 0xff));
}

[[nodiscard]] std::size_t align16(std::size_t value) {
    return (value + 15) & ~std::size_t{15};
}

[[nodiscard]] Fixture make_fixture(std::span<const std::uint16_t> indices) {
    constexpr std::array<std::array<char, 4>, 4> tags = {{
        {{'0', 'S', 'O', 'P'}}, {{'0', 'M', 'R', 'N'}},
        {{'0', 'N', 'A', 'T'}}, {{'0', 'X', 'E', 'T'}},
    }};
    constexpr std::array<std::uint16_t, 4> formats = {2, 16, 16, 6};
    constexpr std::size_t kVertexCount = 4;
    const std::array<StreamSpec, 4> streams = {{
        {tags[0], formats[0], static_cast<std::uint32_t>(kVertexCount * 12)},
        {tags[1], formats[1], static_cast<std::uint32_t>(kVertexCount * 4)},
        {tags[2], formats[2], static_cast<std::uint32_t>(kVertexCount * 4)},
        {tags[3], formats[3], static_cast<std::uint32_t>(kVertexCount * 4)},
    }};

    Fixture fixture;
    fixture.bytes.resize(kHeaderBytes, std::byte{0});
    std::array<std::uint32_t, 4> stream_offsets{};
    std::uint32_t relative_offset = 0x70;
    for (std::size_t i = 0; i < streams.size(); ++i) {
        stream_offsets[i] = relative_offset;
        for (char c : streams[i].tag) fixture.bytes.push_back(static_cast<std::byte>(c));
        append_u16(fixture.bytes, streams[i].format);
        append_u16(fixture.bytes, 2);  // Opaque descriptor flags.
        append_u32(fixture.bytes, relative_offset);
        append_u32(fixture.bytes, 0);  // Opaque descriptor field.
        append_u32(fixture.bytes, streams[i].size);
        append_u32(fixture.bytes, 0);  // Opaque descriptor field.
        relative_offset = static_cast<std::uint32_t>(
            align16(static_cast<std::size_t>(relative_offset) + streams[i].size));
    }

    fixture.sentinel_offset = fixture.bytes.size();
    fixture.bytes.insert(fixture.bytes.end(), 8, std::byte{0xff});
    fixture.data_start = fixture.bytes.size();
    fixture.base = fixture.data_start - stream_offsets[0];

    std::size_t stream_end = 0;
    for (std::size_t i = 0; i < streams.size(); ++i) {
        stream_end = fixture.base + stream_offsets[i] + streams[i].size;
    }
    fixture.bytes.resize(stream_end, std::byte{0});
    fixture.position_start = fixture.base + stream_offsets[0];
    constexpr std::array<std::array<float, 3>, kVertexCount> positions = {{
        {{-1.0f, -1.0f, 0.0f}}, {{1.0f, -1.0f, 0.0f}},
        {{-1.0f, 1.0f, 0.0f}}, {{1.0f, 1.0f, 0.0f}},
    }};
    for (std::size_t i = 0; i < positions.size(); ++i) {
        for (std::size_t axis = 0; axis < 3; ++axis) {
            put_u32(fixture.bytes, fixture.position_start + i * 12 + axis * 4,
                    std::bit_cast<std::uint32_t>(positions[i][axis]));
        }
    }

    fixture.index_tail = stream_end;
    append_u32(fixture.bytes, 1);
    append_u32(fixture.bytes, static_cast<std::uint32_t>(indices.size()));
    for (std::uint16_t index : indices) append_u16(fixture.bytes, index);
    // Synthetic opaque footer bytes; the parser does not assign them semantics.
    fixture.bytes.insert(fixture.bytes.end(), {std::byte{'H'}, std::byte{'S'},
                                                std::byte{'E'}, std::byte{'M'}});
    fixture.bytes.insert(fixture.bytes.end(), 8, std::byte{0});
    return fixture;
}

[[nodiscard]] Fixture make_fixture() {
    constexpr std::array<std::uint16_t, 6> indices = {0, 1, 2, 2, 1, 3};
    return make_fixture(indices);
}

[[nodiscard]] bool has_error(const std::vector<std::byte>& bytes, CmshErrorCode expected) {
    const CmshParseResult result = sotc::parse_cmsh_mesh(bytes);
    const auto* parse_error = std::get_if<CmshParseError>(&result);
    return parse_error != nullptr && parse_error->code == expected;
}

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

void check_error(std::string_view name, std::vector<std::byte> bytes, CmshErrorCode expected) {
    check(name, has_error(bytes, expected));
}

}  // namespace

int main() {
    Fixture valid = make_fixture();
    CmshParseResult parsed = sotc::parse_cmsh_mesh(valid.bytes);
    const auto* mesh = std::get_if<CmshMesh>(&parsed);
    check("valid raw mesh accepted", mesh != nullptr);
    if (mesh != nullptr) {
        check("owned POS0 vertex count", mesh->positions.size() == 4);
        check("owned U16 triangle-list index count", mesh->indices.size() == 6);
        check("little-endian float3 decoded", mesh->positions[3].x == 1.0f &&
                                                   mesh->positions[3].y == 1.0f &&
                                                   mesh->positions[3].z == 0.0f);
        check("index values decoded", mesh->indices == std::vector<std::uint16_t>({0, 1, 2, 2, 1, 3}));
    }

    // Opaque stream formats are not interpreted by the POS0/index subset reader.
    Fixture opaque_formats = make_fixture();
    put_u16(opaque_formats.bytes, kNormalFormatOffset, 0xffff);
    check("opaque normal format retained as opaque",
          std::holds_alternative<CmshMesh>(sotc::parse_cmsh_mesh(opaque_formats.bytes)));

    std::vector<std::byte> compressed(8, std::byte{0});
    compressed[0] = std::byte{0x8c};
    check_error("compressed prefix rejected visibly", std::move(compressed),
                CmshErrorCode::unsupported_compressed_layout);

    check_error("missing POS0 rejected", std::vector<std::byte>(32, std::byte{0}),
                CmshErrorCode::missing_position_descriptor);

    std::vector<std::byte> short_descriptor(kHeaderBytes, std::byte{0});
    short_descriptor.insert(short_descriptor.end(),
                            {std::byte{'0'}, std::byte{'S'}, std::byte{'O'}, std::byte{'P'}});
    short_descriptor.insert(short_descriptor.end(), 8, std::byte{0});
    check_error("truncated descriptor rejected", std::move(short_descriptor),
                CmshErrorCode::truncated_descriptor);

    Fixture unsupported_tag = make_fixture();
    unsupported_tag.bytes[kNormalDescriptor] = std::byte{'X'};
    check_error("unsupported descriptor rejected", std::move(unsupported_tag.bytes),
                CmshErrorCode::unsupported_descriptor_tag);

    Fixture duplicate = make_fixture();
    duplicate.bytes[kNormalDescriptor] = std::byte{'0'};
    duplicate.bytes[kNormalDescriptor + 1] = std::byte{'S'};
    duplicate.bytes[kNormalDescriptor + 2] = std::byte{'O'};
    duplicate.bytes[kNormalDescriptor + 3] = std::byte{'P'};
    check_error("duplicate stream rejected", std::move(duplicate.bytes),
                CmshErrorCode::duplicate_descriptor);

    Fixture no_sentinel = make_fixture();
    no_sentinel.bytes[no_sentinel.sentinel_offset] = std::byte{0};
    check_error("malformed sentinel rejected", std::move(no_sentinel.bytes),
                CmshErrorCode::unsupported_descriptor_tag);

    Fixture truncated_sentinel = make_fixture();
    truncated_sentinel.bytes.resize(truncated_sentinel.sentinel_offset + 4);
    check_error("truncated sentinel rejected", std::move(truncated_sentinel.bytes),
                CmshErrorCode::missing_sentinel);

    Fixture bad_position_format = make_fixture();
    put_u16(bad_position_format.bytes, kPositionFormatOffset, 0xffff);
    check_error("unsupported POS0 format rejected", std::move(bad_position_format.bytes),
                CmshErrorCode::invalid_position_format);

    Fixture bad_position_size = make_fixture();
    put_u32(bad_position_size.bytes, kPositionSizeField, 49);
    check_error("non-float3 POS0 size rejected", std::move(bad_position_size.bytes),
                CmshErrorCode::invalid_position_size);

    Fixture offset_underflow = make_fixture();
    put_u32(offset_underflow.bytes, kPositionStreamOffsetField,
            std::numeric_limits<std::uint32_t>::max());
    check_error("base underflow rejected", std::move(offset_underflow.bytes),
                CmshErrorCode::invalid_stream_offset);

    Fixture out_of_bounds = make_fixture();
    put_u32(out_of_bounds.bytes, kTexcoordStreamOffsetField,
            std::numeric_limits<std::uint32_t>::max() - 3);
    check_error("out-of-bounds opaque stream rejected", std::move(out_of_bounds.bytes),
                CmshErrorCode::stream_out_of_bounds);

    Fixture overlap = make_fixture();
    put_u32(overlap.bytes, kNormalStreamOffsetField, 0x70 + 12);
    check_error("overlapping streams rejected", std::move(overlap.bytes),
                CmshErrorCode::overlapping_streams);

    Fixture short_tail = make_fixture();
    short_tail.bytes.resize(short_tail.index_tail + 4);
    check_error("short index header rejected", std::move(short_tail.bytes),
                CmshErrorCode::truncated_index_tail);

    Fixture bad_marker = make_fixture();
    put_u32(bad_marker.bytes, bad_marker.index_tail, 0);
    check_error("unsupported index marker rejected", std::move(bad_marker.bytes),
                CmshErrorCode::invalid_index_marker);

    constexpr std::array<std::uint16_t, 4> non_triangle_indices = {0, 1, 2, 3};
    Fixture bad_triangle_count = make_fixture(non_triangle_indices);
    check_error("non-triangle index count rejected", std::move(bad_triangle_count.bytes),
                CmshErrorCode::index_count_not_triangle_aligned);

    Fixture huge_index_count = make_fixture();
    put_u32(huge_index_count.bytes, huge_index_count.index_tail + 4,
            std::numeric_limits<std::uint32_t>::max());
    check_error("index payload overflow rejected", std::move(huge_index_count.bytes),
                CmshErrorCode::truncated_index_tail);

    constexpr std::array<std::uint16_t, 3> out_of_range_indices = {0, 1, 4};
    Fixture out_of_range_index = make_fixture(out_of_range_indices);
    check_error("out-of-range index rejected", std::move(out_of_range_index.bytes),
                CmshErrorCode::index_out_of_range);

    Fixture non_finite = make_fixture();
    put_u32(non_finite.bytes, non_finite.position_start, 0x7fc00000);
    check_error("non-finite position rejected", std::move(non_finite.bytes),
                CmshErrorCode::non_finite_position);

    check("error names are visible",
          std::string_view(sotc::cmsh_error_name(CmshErrorCode::unsupported_compressed_layout)) ==
              "unsupported compressed layout");

    if (failures != 0) {
        std::fprintf(stderr, "cmsh fixture checks: FAILED (%d)\n", failures);
        return 1;
    }
    std::puts("cmsh fixture checks: PASSED");
    return 0;
}
