#include "save_chunk_header.hpp"

namespace sotc {
namespace {

void put_u16(std::byte* dst, std::uint16_t value) noexcept {
    dst[0] = static_cast<std::byte>(value & 0xffu);
    dst[1] = static_cast<std::byte>((value >> 8) & 0xffu);
}

void put_u32(std::byte* dst, std::uint32_t value) noexcept {
    for (int i = 0; i != 4; ++i) dst[i] = static_cast<std::byte>((value >> (8 * i)) & 0xffu);
}

std::uint16_t get_u16(const std::byte* src) noexcept {
    return static_cast<std::uint16_t>(std::to_integer<std::uint16_t>(src[0])
                                      | (std::to_integer<std::uint16_t>(src[1]) << 8));
}

std::uint32_t get_u32(const std::byte* src) noexcept {
    std::uint32_t value = 0;
    for (int i = 0; i != 4; ++i) {
        value |= std::to_integer<std::uint32_t>(src[i]) << (8 * i);
    }
    return value;
}

}  // namespace

ChunkParseResult parse_save_chunk_header(std::span<const std::byte> bytes) noexcept {
    if (bytes.size() < kSaveChunkHeaderSize) return {ChunkParseError::truncated_header, {}};
    if (get_u16(bytes.data() + 4) != kSaveChunkMagic) return {ChunkParseError::bad_magic, {}};
    SaveChunkHeader header;
    header.name_hash = get_u32(bytes.data());
    header.tag = get_u16(bytes.data() + 6);
    header.payload_length = get_u32(bytes.data() + 8);
    return {ChunkParseError::none, header};
}

std::array<std::byte, kSaveChunkHeaderSize> encode_save_chunk_header(
    const SaveChunkHeader& header) noexcept {
    std::array<std::byte, kSaveChunkHeaderSize> out{};
    put_u32(out.data(), header.name_hash);
    put_u16(out.data() + 4, static_cast<std::uint16_t>(kSaveChunkMagic));
    put_u16(out.data() + 6, header.tag);
    put_u32(out.data() + 8, header.payload_length);
    return out;
}

std::uint32_t save_chunk_name_hash(std::string_view name) noexcept {
    std::uint32_t hash = kFnv1a32OffsetBasis;
    for (const char raw : name) {
        unsigned char byte = static_cast<unsigned char>(raw);
        if (byte >= 'a' && byte <= 'z') byte = static_cast<unsigned char>(byte - 'a' + 'A');
        hash ^= byte;
        hash *= 0x01000193u;
    }
    return hash;
}

}  // namespace sotc
