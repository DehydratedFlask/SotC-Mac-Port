// Fixture-level typed chunk codec on top of native/core/save_stream (the opaque tagged-chunk
// container). Every wire fact used here comes from
//   docs/checkpoints/2026-10-21-agentH-sav01-profile-chunks.md  (sections 3-7, labels below)
// and nothing was re-derived from game bytes in this task. This is NOT a claim of real-save
// compatibility; G2 stays NOT RUN.
//
// PROVED by agentH (OBSERVATION-VERIFIED unless stated):
//   * little-endian on disk; no byte swap in the readers; the writers' swap flag is never set;
//   * bool chunk = tag 13, ONE payload byte, no length prefix; a reader treats any nonzero byte as true;
//   * blob chunk = tag 13, payload = u64 LE length prefix + that many bytes; the prefix is INSIDE
//     payload_length (payload_length = 8 + blob size);
//   * 4-byte chunks use tag 8 or tag 4 (both 4 bytes LE); f32 chunks use tag 10; string chunks use
//     tag 12 = 7-bit varint byte count (LSB group first, bit 7 = continue) + bytes, no NUL;
//   * lookup by name hash is first match over top-level chunks; a missing or retagged chunk makes
//     the original keep the destination's prior value;
//   * the original has two hazards: an empty body (NULL dereference) and an overlong record
//     (out-of-bounds read). Here both are explicit errors, never reproduced;
//   * header dword 3 is a class descriptor id validated by the class loaders, not by the file
//     loader: profile 0xd, progress 4. The loader gate below is separate from the container.
//   * the 33 profile rows, 13 progress rows, 48 flag rows, 257 indexed hashes: tag + payload class
//     were re-derived by agentH's verifier (232 checks); enum VALUE SETS E1..E7 likewise.
//
// UNRESOLVED / INFERENCE / NOT modelled (do not read these as claims):
//   * tag 4 vs tag 8 is UNRESOLVED: both are 4 bytes LE and the codec exposes the raw 32-bit
//     pattern only. They are different PayloadKinds (Word32 vs Word32Tag4Unresolved) and a chunk
//     is rejected when its tag differs from the spec's tag. Signedness (i32 vs u32) is inference
//     and is left to the caller.
//   * name-hash case folding: the original maps through an imported toupper table. That it is
//     ASCII toupper is INFERENCE (no real save or libc was consulted). The fold is therefore an
//     explicit NameHashPolicy, default ascii_upper (same as save_chunk_name_hash); see below.
//   * enum chunks: the original writes the entry NAME and accepts decimal text or a case-
//     insensitive name/alias, and maps an unknown string to the table's first value. This codec
//     embeds no original names and accepts DECIMAL TEXT ONLY (digits, 1..10 chars, <= 0xFFFFFFFF,
//     leading zeros allowed, no sign/whitespace; the original _Stoul is more permissive). Importing
//     original saves that carry names is a policy decision the user has not taken.
//   * the flag-bit mapping (flag i = bit (i & 7) of byte (i >> 3) of the game-data blob) is
//     OBSERVATION-READING, not byte-verified.
//   * progress-object chunk semantics are not decoded; its table is tag/payload class only.
//   * "import mode" (ignore garbage tails, stop at first non-0xBEEF record) is not implemented;
//     the container's strict tiling policy applies. Duplicate chunks are accepted by the container
//     by default and find_first returns the first, like the original.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "core/save_stream.hpp"

namespace sotc {

// ---- wire tags actually used by the profile / progress writers --------------------------------
inline constexpr std::uint16_t kTagWord32Tag4 = 4;   // UNRESOLVED relative to tag 8
inline constexpr std::uint16_t kTagWord32Tag8 = 8;
inline constexpr std::uint16_t kTagF32 = 10;
inline constexpr std::uint16_t kTagString = 12;
inline constexpr std::uint16_t kTagBytes = 13;       // bool chunks (1 byte) and u64-prefixed blobs

inline constexpr std::uint32_t kProfileDescriptorId = 0xd;
inline constexpr std::uint32_t kProgressDescriptorId = 4;

enum class CodecError {
    none,
    // payload primitives
    short_read,                    // fewer bytes than the primitive needs
    trailing_bytes,                // strict mode: bytes left after the value
    varint_overlong,               // varint longer than 5 bytes or does not fit u32
    string_length_exceeds_payload, // varint byte count > remaining payload bytes
    blob_length_mismatch,          // u64 prefix != the caller-required length
    blob_length_exceeds_payload,   // u64 prefix > remaining payload bytes
    array_count_mismatch,          // u64 prefix != 4 * element count
    decimal_empty,                 // enum text is empty
    decimal_not_digit,             // enum text contains a non-digit byte (names are not accepted)
    decimal_overflow,              // enum text > 0xFFFFFFFF
    enum_value_not_in_set,         // value outside the spec's enum value set
    payload_too_large,             // writer: a length does not fit the wire field
    // chunk level
    tag_mismatch,                  // chunk tag != tag required by the decoder / spec
    chunk_missing,                 // no chunk with this name hash
    wrong_value_type,              // writer: ChunkValue alternative does not match the spec kind
    stream_rejected,               // writer: SaveStreamWriter refused (see CodecStatus::stream_error)
    // loader gate
    empty_body,                    // original: NULL dereference on the first record
    descriptor_mismatch,           // header dword 3 != expected class descriptor id
    container_rejected,            // parse_save_stream failed (see LoadGateResult::stream_error)
};

[[nodiscard]] const char* to_string(CodecError error) noexcept;

// ---- name hash policy ------------------------------------------------------------------------
// ascii_upper:           FNV-1a-32 over ASCII-uppercased bytes; identical to save_chunk_name_hash.
// ascii_upper_and_slash: additionally maps '\\' to '/' as agentH verified in the original hash
//                        loop (0x162020). Names in the verified tables contain no backslash.
// verbatim:              no folding at all (exists to show what the fold policy changes).
enum class NameHashPolicy { ascii_upper, ascii_upper_and_slash, verbatim };

[[nodiscard]] std::uint32_t chunk_name_hash(std::string_view name,
                                            NameHashPolicy policy = NameHashPolicy::ascii_upper) noexcept;

// ---- payload primitives ----------------------------------------------------------------------
class PayloadReader {
public:
    explicit PayloadReader(std::span<const std::byte> payload) noexcept : data_(payload) {}
    [[nodiscard]] std::size_t remaining() const noexcept { return data_.size() - pos_; }
    [[nodiscard]] std::size_t position() const noexcept { return pos_; }

    [[nodiscard]] CodecError read_u8(std::uint8_t& out) noexcept;
    [[nodiscard]] CodecError read_u32(std::uint32_t& out) noexcept;    // LE
    [[nodiscard]] CodecError read_u64(std::uint64_t& out) noexcept;    // LE
    [[nodiscard]] CodecError read_f32(float& out) noexcept;            // LE bit pattern, NaNs preserved
    [[nodiscard]] CodecError read_varint_u32(std::uint32_t& out) noexcept;
    // Borrow `count` bytes (no copy); the span aliases the payload.
    [[nodiscard]] CodecError read_bytes(std::size_t count, std::span<const std::byte>& out) noexcept;

private:
    std::span<const std::byte> data_;
    std::size_t pos_ = 0;
};

class PayloadWriter {
public:
    void put_u8(std::uint8_t value);
    void put_u32(std::uint32_t value);
    void put_u64(std::uint64_t value);
    void put_f32(float value);
    void put_varint_u32(std::uint32_t value);   // canonical (shortest) form
    void put_bytes(std::span<const std::byte> bytes);

    [[nodiscard]] const std::vector<std::byte>& bytes() const noexcept { return out_; }
    [[nodiscard]] std::vector<std::byte> take() noexcept { return std::move(out_); }

private:
    std::vector<std::byte> out_;
};

// ---- typed chunk payload encoders (return the payload only; header added by the container) ---
[[nodiscard]] std::vector<std::byte> encode_bool_payload(bool value);                 // 1 byte: 0 / 1
[[nodiscard]] std::vector<std::byte> encode_word32_payload(std::uint32_t bits);       // 4 bytes LE
[[nodiscard]] std::vector<std::byte> encode_f32_payload(float value);                 // 4 bytes LE
// varint byte count + bytes. Fails (payload_too_large) only if size > 0xFFFFFFFF.
[[nodiscard]] CodecError encode_string_payload(std::string_view text, std::vector<std::byte>& out);
// Canonical decimal text of `value` carried as a string payload.
[[nodiscard]] std::vector<std::byte> encode_enum_decimal_payload(std::uint32_t value);
// u64 LE length prefix (= blob size) + blob bytes.
[[nodiscard]] std::vector<std::byte> encode_blob_payload(std::span<const std::byte> blob);
// u64 LE length prefix (= 4 * N) + N raw LE u32.
[[nodiscard]] std::vector<std::byte> encode_u32_array_payload(std::span<const std::uint32_t> values);

// ---- typed chunk decoders --------------------------------------------------------------------
// strict:          the payload must be consumed exactly (native-written files).
// ignore_trailing: extra payload bytes after the value are ignored (the original reads a
//                  sub-stream and never looks at the rest; use for original-save import only).
enum class ReadMode { strict, ignore_trailing };

template <class T>
struct Decoded {
    CodecError error = CodecError::none;
    T value{};
    [[nodiscard]] bool ok() const noexcept { return error == CodecError::none; }
};

[[nodiscard]] Decoded<bool> decode_bool_chunk(const SaveStreamChunk& chunk,
                                              ReadMode mode = ReadMode::strict) noexcept;
// Raw 32-bit pattern; tag 8 and tag 4 are separate functions because the distinction is UNRESOLVED.
[[nodiscard]] Decoded<std::uint32_t> decode_word32_tag8_chunk(
    const SaveStreamChunk& chunk, ReadMode mode = ReadMode::strict) noexcept;
[[nodiscard]] Decoded<std::uint32_t> decode_word32_tag4_unresolved_chunk(
    const SaveStreamChunk& chunk, ReadMode mode = ReadMode::strict) noexcept;
[[nodiscard]] Decoded<float> decode_f32_chunk(const SaveStreamChunk& chunk,
                                              ReadMode mode = ReadMode::strict) noexcept;
[[nodiscard]] Decoded<std::string> decode_string_chunk(const SaveStreamChunk& chunk,
                                                       ReadMode mode = ReadMode::strict);

// Enum value sets E1..E7 from agentH section 4 (values only, no names).
enum class EnumSet : std::uint8_t { none = 0, e1, e2, e3, e4, e5, e6, e7 };
[[nodiscard]] bool enum_set_contains(EnumSet set, std::uint32_t value) noexcept;  // none -> always true

[[nodiscard]] Decoded<std::uint32_t> decode_enum_decimal_chunk(
    const SaveStreamChunk& chunk, EnumSet set, ReadMode mode = ReadMode::strict);

// Returns a span aliasing chunk.payload (valid only while the chunk lives: never pass a temporary). `required_length` of
// std::nullopt accepts any prefix; the original aborts when the profile param blob's prefix != 0x9c.
struct RequiredLength {
    bool has = false;
    std::uint64_t value = 0;
};
[[nodiscard]] Decoded<std::span<const std::byte>> decode_blob_chunk(
    const SaveStreamChunk& chunk, RequiredLength required = {}, ReadMode mode = ReadMode::strict) noexcept;
// u64 prefix must equal 4 * element_count (the count comes from another chunk).
[[nodiscard]] Decoded<std::vector<std::uint32_t>> decode_u32_array_chunk(
    const SaveStreamChunk& chunk, std::uint32_t element_count, ReadMode mode = ReadMode::strict);

// ---- lookup (original semantics: first match by name hash) ------------------------------------
enum class LookupStatus { found, missing, tag_mismatch };
struct ChunkLookup {
    LookupStatus status = LookupStatus::missing;
    const SaveStreamChunk* chunk = nullptr;   // first chunk with the hash, also for tag_mismatch
};
// The original does NOT continue to a later duplicate when the first match has another tag.
[[nodiscard]] ChunkLookup lookup_chunk(const ParsedSaveStream& stream, std::uint32_t name_hash,
                                       std::uint16_t expected_tag) noexcept;

// ---- constexpr spec tables --------------------------------------------------------------------
enum class PayloadKind : std::uint8_t {
    Bool8,                 // tag 13, 1 byte
    Word32,                // tag 8, 4 bytes LE (signedness inferred, not modelled)
    Word32Tag4Unresolved,  // tag 4, 4 bytes LE; tag 4 vs 8 is UNRESOLVED
    F32,                   // tag 10
    Str,                   // tag 12, varint count + bytes
    EnumStrDecimal,        // tag 12, decimal text of an enum value (see header comment)
    BlobU64,               // tag 13, u64 prefix + bytes
    ArrayU32U64,           // tag 13, u64 prefix (= 4N) + N raw LE u32
};

[[nodiscard]] constexpr bool kind_is_unresolved(PayloadKind kind) noexcept {
    return kind == PayloadKind::Word32Tag4Unresolved;
}

struct ChunkSpec {
    std::uint32_t name_hash = 0;
    std::uint16_t tag = 0;
    PayloadKind kind = PayloadKind::Bool8;
    EnumSet enum_set = EnumSet::none;   // EnumStrDecimal only
    std::uint64_t fixed_length = 0;     // BlobU64 only; 0 = any (profile param blob: 0x9c, game data: 0x3cd8)
};

// Profile object (descriptor 0xd): the 33 named chunks in the original writer order.
[[nodiscard]] std::span<const ChunkSpec> profile_named_chunk_specs() noexcept;
// Progress object (descriptor 4): the 13 named chunks. Tag/payload class only; semantics undecoded.
[[nodiscard]] std::span<const ChunkSpec> progress_named_chunk_specs() noexcept;

// Indexed block (both objects): 257 tag-12 enum chunks (set E7), names = 8-char prefix + decimal
// index. Hashes are reproduced from the FNV state after the (unpublished) prefix.
inline constexpr std::uint32_t kIndexedChunkCount = 257;
inline constexpr std::uint32_t kIndexPrefixState = 0x0b45d343u;
[[nodiscard]] std::uint32_t indexed_chunk_hash(std::uint32_t index) noexcept;   // index < 257
[[nodiscard]] ChunkSpec indexed_chunk_spec(std::uint32_t index) noexcept;       // index < 257

// 48 bool flag chunks (progress side): runs 0x78..0x87, 0xa5..0xb4, 0xd9..0xe8, tag 13, 1 byte.
struct FlagSpec {
    std::uint32_t flag_index;
    std::uint32_t name_hash;
};
inline constexpr std::size_t kFlagChunkCount = 48;
[[nodiscard]] std::span<const FlagSpec> flag_specs() noexcept;                  // original write order
[[nodiscard]] ChunkSpec flag_chunk_spec(const FlagSpec& flag) noexcept;
// Flag i is bit (i & 7) of byte (i >> 3) of the game-data blob (OBSERVATION-READING).
// Both return false when the byte index is outside the blob (no clamping, no wrap).
[[nodiscard]] bool get_flag_bit(std::span<const std::byte> blob, std::uint32_t flag_index, bool& out) noexcept;
[[nodiscard]] bool set_flag_bit(std::span<std::byte> blob, std::uint32_t flag_index, bool value) noexcept;

// ---- spec driven values ----------------------------------------------------------------------
// Alternatives by kind: Bool8 -> bool; Word32/Word32Tag4Unresolved/EnumStrDecimal -> uint32_t;
// F32 -> float; Str -> std::string; BlobU64 -> vector<byte>; ArrayU32U64 -> vector<uint32_t>.
// Beware: do not construct the string alternative from a string literal (it converts to bool).
using ChunkValue = std::variant<std::monostate, bool, std::uint32_t, float, std::string,
                                std::vector<std::byte>, std::vector<std::uint32_t>>;

struct CodecStatus {
    CodecError error = CodecError::none;
    SaveStreamError stream_error = SaveStreamError::none;   // set when error == stream_rejected
    [[nodiscard]] bool ok() const noexcept { return error == CodecError::none; }
};

// Encodes `value` by `spec.kind` and appends it with spec.name_hash / spec.tag. Validates the
// alternative (wrong_value_type), enum membership, BlobU64 fixed_length (blob_length_mismatch).
[[nodiscard]] CodecStatus append_chunk_by_spec(SaveStreamWriter& writer, const ChunkSpec& spec,
                                               const ChunkValue& value);

// Decodes one already-located chunk by spec (tag must equal spec.tag). `array_count` is only
// used for ArrayU32U64.
[[nodiscard]] Decoded<ChunkValue> decode_chunk_by_spec(const SaveStreamChunk& chunk,
                                                       const ChunkSpec& spec,
                                                       ReadMode mode = ReadMode::strict,
                                                       std::uint32_t array_count = 0);

// lookup_chunk + decode_chunk_by_spec. A missing chunk is chunk_missing and a retagged one is
// tag_mismatch; callers that want the original's "keep the default" simply ignore those two.
[[nodiscard]] Decoded<ChunkValue> read_chunk_by_spec(const ParsedSaveStream& stream,
                                                     const ChunkSpec& spec,
                                                     ReadMode mode = ReadMode::strict,
                                                     std::uint32_t array_count = 0);

// ---- loader-level descriptor gate (separate from the container) -------------------------------
// The container carries dword 3 but never checks it (the original file loader does not either).
[[nodiscard]] CodecError check_descriptor_id(const SaveStreamHeader& header,
                                             std::uint32_t expected_descriptor_id) noexcept;

struct LoadGateResult {
    CodecError error = CodecError::none;
    SaveStreamError stream_error = SaveStreamError::none;   // set when error == container_rejected
    ParsedSaveStream stream{};                               // valid only when error == none
};
// Order: container checks (parse_save_stream: version, size, hash, chunk tiling/overrun), then
// descriptor id (the original does this first and before reading any chunk), then empty body.
[[nodiscard]] LoadGateResult validate_for_loader(std::span<const std::byte> bytes,
                                                 std::uint32_t expected_descriptor_id);

}  // namespace sotc
