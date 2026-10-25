// Progress save object (outer-header descriptor id 4) <-> USR-DATA chunk body.
//
// Built only from the private contract analysis in
//   local/re/evidence/savedata-contract/report.md
// and the already native, already tested pieces:
//   core/save_stream.*        outer 16-byte header, atomic file write
//   core/save_chunk_header.*  12-byte chunk header, name hash
//   core/save_checksum.*      FNV-1a-32
//   core/save_chunk_codec.*   typed payloads + the 13 progress / 48 flag / 257 indexed spec tables
//
// PROVED / REUSED (see those headers for their own labels):
//   * progress descriptor id 4; game-data blob is 0x3cd8 = 15 576 bytes, chunk payload is
//     u64 LE 0x3cd8 + that blob, so the GameData record is 15 596 bytes;
//   * 13 named progress chunks, 48 tag-13 one-byte flag chunks, 257 tag-12 indexed chunks
//     (decimal text, values 0..2); all little-endian; flag i is bit (i & 7) of byte (i >> 3).
//
// WHAT THIS FILE DOES NOT CLAIM:
//   * The meaning of any byte inside the 0x3cd8 blob. It is an opaque, caller-owned buffer.
//   * That a game-produced save was read or written. All fixtures are synthetic.
//   * The original writer's chunk ORDER (only "48 flags before GameData" is evidenced). The order
//     below is this codec's canonical writer order, chosen so offsets are reproducible.
//   * That the original loads our files. Import compatibility is a separate, unproven step.
//
// DELIBERATE DIVERGENCES from the original code (documented again in the savedata-transaction
// report):
//   * `ChunkWalkPolicy::strict` (used by the transaction) rejects trailing bytes, unknown chunks,
//     duplicates and missing required chunks. The original stops at the first non-0xBEEF record,
//     ignores unknown chunks and lets the first match win (`original` policy models that).
//   * The original dereferences NULL on an empty body and walks out of bounds on an overlong
//     record. Both are explicit errors here under either policy.
//   * The original does not enforce flag/blob agreement. `encode_progress_body` rewrites the blob
//     bits from `flags` (the writer's rule in the contract, C9/F24); `progress_check` reports the
//     conflict instead of hiding it.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

#include "core/save_chunk_codec.hpp"
#include "core/save_stream.hpp"

namespace sotc {

// New here; kProgressDescriptorId (4), kIndexedChunkCount (257) and kFlagChunkCount (48) are
// reused from core/save_chunk_codec.hpp.
inline constexpr std::size_t kProgressGameDataBytes = 0x3cd8;   // 15 576

// ---- canonical writer layout (bytes of a body produced by encode_progress_body) ---------------
// These are WIRE-body offsets, not recovered in-memory struct offsets. The original exposes the
// blob only through a base+size accessor (0x7e5bb0: lea rax,[0x3260bb8]; mov [rdi],0x3cd8), so no
// field offsets inside the blob are claimed here.
inline constexpr std::size_t kProgressFlagBlockBytes = kFlagChunkCount * 13;             // 624
inline constexpr std::size_t kProgressGameDataOffset = kProgressFlagBlockBytes;           // 624
inline constexpr std::size_t kProgressGameDataChunkBytes = 12 + 8 + kProgressGameDataBytes;  // 15 596
inline constexpr std::size_t kProgressWordChunks = 5;
inline constexpr std::size_t kProgressBoolChunks = 3;
inline constexpr std::size_t kProgressFloatChunks = 2;
inline constexpr std::size_t kProgressCountOffset = 16371;   // tag-4 element count chunk
inline constexpr std::size_t kProgressArrayOffset = 16387;   // tag-13 u64-prefixed id array
inline constexpr std::size_t kProgressIndexedOffset = 16407; // + 4 * id_count()
inline constexpr std::size_t kProgressIndexedChunkBytes = 14; // 12 + varint(1) + one digit

// 48 flags (13 B) + GameData (15 596 B) + 5 words (16 B) + 3 bools (13 B) + 2 floats (16 B)
// + count (16 B) + array (20 B + 4N) + 257 indexed (14 B each, values 0..2 -> one digit).
//
// NOTE: local/re/evidence/savedata-contract/report.md §5.2/§6.1 state `19748 + 4N`. That figure
// counts each indexed chunk as 12 + 1 bytes and drops the one-byte varint string length (257 B).
// With the wire form the report itself specifies ("tag 12 = varint count + bytes") the body is
// 20005 + 4N. The transaction report records this correction; the test pins both the term-by-term
// breakdown and the total.
[[nodiscard]] constexpr std::size_t progress_body_size(std::size_t id_count) noexcept {
    return kProgressFlagBlockBytes + kProgressGameDataChunkBytes +
           kProgressWordChunks * 16 + kProgressBoolChunks * 13 + kProgressFloatChunks * 16 +
           16 + (20 + 4 * id_count) + kIndexedChunkCount * kProgressIndexedChunkBytes;
}
static_assert(progress_body_size(0) == 20005);
static_assert(progress_body_size(2) == 20013);

// The game's own 1-based ESaveResult ordinals (report §4.3). The values are what the original
// stores at [task+0x18]; 2 (Canceled) is only ever produced by a dialog result type 1.
enum class SaveResult : std::uint8_t {
    success = 1,
    canceled = 2,
    failed_no_space = 3,
    failed_file_not_found = 4,
    failed_mount = 5,
    failed_corrupt = 6,
    failed_generic = 7,
};
[[nodiscard]] const char* to_string(SaveResult result) noexcept;

// Named-chunk slots in the order of progress_named_chunk_specs(). Field names are descriptive
// only: the wire carries hashes, and the meaning of `unlock` / `words` / `floats` / `ids` is not
// decoded. Sizes are exact; there is no padding and no hidden field.
struct ProgressV1 {
    std::vector<std::byte> game_data;                      // exactly kProgressGameDataBytes
    std::array<std::uint8_t, kIndexedChunkCount> unlock{};  // 257 entries, value set {0,1,2}
    std::array<bool, kFlagChunkCount> flags{};              // 48 entries, index = flag_specs() row
    std::array<std::uint32_t, 5> words{};                    // tag 8 / tag 4 rows
    std::array<bool, 3> booleans{};                          // tag 13 rows
    std::array<float, 2> floats{};                           // tag 10 rows
    std::vector<std::uint32_t> ids;                          // tag 13 array, count in the tag-4 row
};

// A default object with a correctly sized, zero-filled game_data buffer. A default-constructed
// ProgressV1 is deliberately invalid (empty blob) so a forgotten allocation fails loudly.
[[nodiscard]] ProgressV1 make_progress_v1();

enum class ProgressError : std::uint8_t {
    none,
    // container walk
    empty_body,               // no record at all (the original dereferences NULL here)
    truncated_chunk_header,   // strict: fewer than 12 bytes left where a record must start
    bad_chunk_magic,          // strict: bytes 4..5 are not EF BE
    chunk_overrun,            // payload_length past the end of the body (both policies)
    duplicate_chunk,          // strict: the same name hash appears twice
    unknown_chunk,            // strict: a name hash outside the progress/flag/indexed tables
    chunk_missing,            // strict: a required name hash is absent
    trailing_bytes,           // strict: leftover bytes shorter than a chunk header
    // payload / object rules
    game_data_size,           // blob is not exactly 0x3cd8
    count_mismatch,           // tag-4 count != ids.size() / u64 array prefix != 4*count
    flag_conflict,            // flags[i] != blob bit flag_specs()[i].flag_index
    unlock_out_of_range,      // unlock[i] outside {0,1,2}
    tag_mismatch,             // chunk tag differs from its spec (strict)
    payload_rejected,         // detail in ObjectStatus::codec
    stream_rejected,          // detail in ObjectStatus::stream
    layout_mismatch,          // encoder self-check failed (never expected)
};
[[nodiscard]] const char* to_string(ProgressError error) noexcept;

// strict:   the body must tile exactly and contain every required chunk once (native files).
// original: first match wins; a missing/retagged chunk keeps the default (for GameData the default
//           is the zero blob, for flags it is the blob bit); unknown chunks and trailing bytes are
//           ignored. Overrun, empty body and count/array disagreement stay errors.
enum class ChunkWalkPolicy : std::uint8_t { strict, original };
[[nodiscard]] const char* to_string(ChunkWalkPolicy policy) noexcept;

struct ObjectStatus {
    SaveResult result = SaveResult::success;
    ProgressError error = ProgressError::none;
    CodecError codec = CodecError::none;       // filled when error == payload_rejected
    SaveStreamError stream = SaveStreamError::none;   // filled when error == stream_rejected
    std::uint32_t name_hash = 0;               // first chunk that failed, 0 if none
    [[nodiscard]] bool ok() const noexcept {
        return result == SaveResult::success && error == ProgressError::none &&
               codec == CodecError::none && stream == SaveStreamError::none;
    }
};

// Explicit syntax / consistency check, independent of any wire image. Returns the game's 1-based
// ordinals: failed_corrupt (6) for a blob-size / count / unlock / flag-conflict violation,
// success (1) otherwise. This is the check a caller runs before offering a save in the UI.
[[nodiscard]] ObjectStatus progress_check(const ProgressV1& object) noexcept;

// Body bytes only (no 16-byte outer header). Emits, in this order:
//   48 flag chunks -> 13 named chunks (spec order) -> 257 indexed chunks.
// The GameData chunk contains a copy of game_data with the 48 flag bits rewritten from `flags`
// (the caller's object is not modified). Fails with failed_generic (7) when:
//   * game_data.size() != kProgressGameDataBytes,
//   * ids.size() > 0x3FFFFFFF (the u32 count / u64 byte count would not fit),
//   * the underlying writer refuses a chunk (stream_rejected).
// Flag/blob disagreement is NOT an encode failure: `flags` wins on the wire (contract C9/F24).
[[nodiscard]] ObjectStatus encode_progress_body(const ProgressV1& object,
                                                std::vector<std::byte>& body);

// Parses a body produced by encode_progress_body or (with ChunkWalkPolicy::original) by the game.
// `out` is only assigned on success; on failure it is left unchanged and the returned status
// carries the first offending name hash. All structural problems map to failed_corrupt (6),
// matching the class loader's local result 6.
[[nodiscard]] ObjectStatus decode_progress_body(std::span<const std::byte> body, ProgressV1& out,
                                                ChunkWalkPolicy policy = ChunkWalkPolicy::strict,
                                                ReadMode mode = ReadMode::strict);

}  // namespace sotc
