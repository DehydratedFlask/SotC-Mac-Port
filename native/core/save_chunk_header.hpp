// Native, ordinary-C++ codec for one save tagged-chunk header, as decoded in
// docs/checkpoints/2026-10-20-root-sav01-serializer-complete.md:
//   little-endian u32 name_hash | u16 magic 0xBEEF | u16 tag | u32 payload_length   (12 bytes)
// The header shape and the name-hash rule are observations from the serializer bytes; the tag
// values' meaning is not known, so the tag is carried as an opaque u16.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>

namespace sotc {

inline constexpr std::uint32_t kSaveChunkMagic = 0xBEEF;
inline constexpr std::size_t kSaveChunkHeaderSize = 12;
inline constexpr std::uint32_t kFnv1a32OffsetBasis = 0x811c9dc5u;

struct SaveChunkHeader {
    std::uint32_t name_hash = 0;
    std::uint16_t tag = 0;
    std::uint32_t payload_length = 0;
};

// Small explicit error set. Deliberately absent: a "bad tag" error (the tag is opaque on the wire;
// the observed values 4/8/10/12/13 are inference, not a decoded enum) and a "payload truncated"
// error (this codec only reads the 12-byte header; the payload is the caller's business).
enum class ChunkParseError {
    none,
    truncated_header,   // fewer than 12 bytes
    bad_magic,          // bytes 4..5 are not 0xEF 0xBE
};

struct ChunkParseResult {
    ChunkParseError error = ChunkParseError::none;
    SaveChunkHeader header{};
};

// Reads exactly the first 12 bytes of `bytes`; trailing bytes are ignored.
[[nodiscard]] ChunkParseResult parse_save_chunk_header(std::span<const std::byte> bytes) noexcept;

// Emits the 12 wire bytes (little-endian; magic 0xBEEF appears as EF BE at offsets 4..5).
[[nodiscard]] std::array<std::byte, kSaveChunkHeaderSize> encode_save_chunk_header(
    const SaveChunkHeader& header) noexcept;

// FNV-1a-32 (seed 0x811c9dc5) over the field name after ASCII uppercase mapping ('a'..'z' only).
// NOT modelled: the original maps through the imported _Getptoupper, so its behaviour outside
// ASCII (locale-dependent or non-ASCII bytes) is out of scope here; non-ASCII bytes are hashed
// unchanged. This is a codec fixture, not a claim of real-save compatibility.
[[nodiscard]] std::uint32_t save_chunk_name_hash(std::string_view name) noexcept;

}  // namespace sotc
