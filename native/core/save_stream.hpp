// Fixture-level container for the save "tagged-chunk stream", built only from facts proved in
//   docs/checkpoints/2026-10-20-root-sav01-producer-complete.md   (outer 16-byte header, hash, limits)
//   docs/checkpoints/2026-10-20-root-sav01-serializer-complete.md (12-byte chunk header, name hash)
//   docs/checkpoints/2026-10-20-root-sav01-blob-flush-complete.md (stream is flushed verbatim)
//
// PROVED (static analysis, not exercised against a real save):
//   * Outer header = 4 dwords, written by fwrite(obj+8, 1, 0x10): dword0 = 1 (version), dword1 =
//     body hash, dword2 = body byte count, dword3 = descriptor id of the save object class.
//   * Body hash = FNV-1a-32, seed 0x811c9dc5, over the body bytes only (never the 16-byte header).
//   * Load requires: 16-byte header read, dword0 == 1, dword2 <= 0x6400000 (unsigned), body read of
//     exactly dword2 bytes, hash(body) == dword1. dword1 and dword3 are NOT checked at header time;
//     dword3 is not covered by the hash, so a flipped descriptor id is accepted by design here too.
//   * Body = concatenated chunks: 12-byte header (u32 name hash, u16 0xBEEF, u16 tag, u32 payload
//     length, see save_chunk_header.hpp) followed by the payload.
//
// NOT PROVED / deliberately NOT modelled (payloads and semantics stay opaque):
//   * Meaning of any tag value, any chunk name, or any payload byte. This container never
//     interprets a payload. The game-data blob (tag 0xd, u64 length prefix, 15596 bytes), the
//     48 bool chunks and the options blob are NOT produced or parsed here.
//   * Little-endian layout of the OUTER header is an inference from fwrite of x86-64 dwords (the
//     chunk header's endianness is likewise taken from save_chunk_header.hpp).
//   * That payload_length counts exactly the bytes after the 12-byte chunk header is the fixture's
//     interpretation of "back-patched u32 length"; whether the real blob chunk's u64 prefix is
//     inside that count is unknown (a caller simply puts it in the opaque payload).
//   * Duplicate chunk names: the real format may repeat names (loops are present in the serializer),
//     so the default policy is to ALLOW and preserve order; rejection is an explicit opt-in.
//   * Fixture policies that are NOT claims about the original loader: trailing bytes after the
//     declared body are rejected, chunks must tile the body exactly, and a body hash mismatch is
//     reported before chunk-level parsing.
//
// This is a fixture container, not a claim of game save compatibility.
#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace sotc {

inline constexpr std::uint32_t kSaveStreamVersion = 1;
inline constexpr std::size_t kSaveStreamHeaderSize = 16;
inline constexpr std::uint32_t kSaveStreamMaxBodySize = 0x6400000u;

enum class SaveStreamError {
    none,
    // writer side
    duplicate_name,       // DuplicatePolicy::reject and the name hash was already appended
    payload_too_large,    // payload does not fit the u32 payload_length field
    body_too_large,       // appending would exceed the writer's body size limit
    // reader side, in the order the reader checks them
    truncated_header,     // fewer than 16 bytes
    bad_version,          // dword0 != 1
    size_too_large,       // dword2 > 0x6400000 (checked before the body is looked at)
    truncated_body,       // fewer than dword2 body bytes available
    trailing_bytes,       // more bytes than header + dword2 (fixture policy)
    bad_hash,             // FNV-1a-32 of the body != dword1
    truncated_chunk_header,  // a chunk starts with < 12 bytes left in the body
    bad_chunk_magic,      // chunk header bytes 4..5 are not EF BE
    chunk_overrun,        // chunk payload_length exceeds the remaining body
    // reader side, only when DuplicatePolicy::reject is requested
    reader_duplicate_name,
    // file helpers
    file_not_found,       // open() failed with ENOENT (read only)
    file_too_large,       // file larger than header + max body (checked via fstat, nothing read)
    io_open,
    io_read,
    io_write,
    io_fsync,
    io_rename,
};

[[nodiscard]] const char* to_string(SaveStreamError error) noexcept;

enum class DuplicatePolicy {
    allow,   // default: keep every chunk in order, including repeated name hashes
    reject,  // fail on the second chunk carrying the same name hash
};

struct SaveStreamHeader {
    std::uint32_t version = kSaveStreamVersion;
    std::uint32_t body_hash = 0;       // dword1; recomputed by the writer, verified by the reader
    std::uint32_t body_size = 0;       // dword2
    std::uint32_t descriptor_id = 0;   // dword3; opaque, not covered by the hash
};

struct SaveStreamChunk {
    std::uint32_t name_hash = 0;
    std::uint16_t tag = 0;               // opaque
    std::vector<std::byte> payload;      // opaque
};

class SaveStreamWriter {
public:
    explicit SaveStreamWriter(std::uint32_t descriptor_id,
                              DuplicatePolicy policy = DuplicatePolicy::allow,
                              std::size_t max_body_size = kSaveStreamMaxBodySize) noexcept;

    // Name is hashed with save_chunk_name_hash (FNV-1a-32 over the ASCII-uppercased name).
    // On any error the writer is unchanged.
    [[nodiscard]] SaveStreamError append_chunk(std::string_view name, std::uint16_t tag,
                                               std::span<const std::byte> payload);
    // Same, with an already-hashed name (the original format only stores the hash).
    [[nodiscard]] SaveStreamError append_chunk_hashed(std::uint32_t name_hash, std::uint16_t tag,
                                                      std::span<const std::byte> payload);

    [[nodiscard]] std::size_t body_size() const noexcept { return body_.size(); }
    [[nodiscard]] std::size_t chunk_count() const noexcept { return hashes_.size(); }

    // 16-byte outer header + body. Never fails: limits are enforced at append time.
    [[nodiscard]] std::vector<std::byte> serialize() const;

private:
    std::uint32_t descriptor_id_;
    DuplicatePolicy policy_;
    std::size_t max_body_size_;
    std::vector<std::byte> body_;
    std::vector<std::uint32_t> hashes_;
};

struct ParsedSaveStream {
    SaveStreamHeader header{};
    std::vector<SaveStreamChunk> chunks;

    // First chunk with this name hash, or nullptr. Duplicates (if allowed) are reachable by index.
    [[nodiscard]] const SaveStreamChunk* find_first(std::uint32_t name_hash) const noexcept;
    [[nodiscard]] const SaveStreamChunk* find_first(std::string_view name) const noexcept;
};

struct SaveStreamReadResult {
    SaveStreamError error = SaveStreamError::none;
    int os_errno = 0;            // set for file_*/io_* errors
    ParsedSaveStream stream{};   // valid only when error == none
};

// Parses and validates a complete container image (exactly header + body, no trailing bytes).
[[nodiscard]] SaveStreamReadResult parse_save_stream(
    std::span<const std::byte> bytes, DuplicatePolicy policy = DuplicatePolicy::allow);

// Reads the whole file, then parse_save_stream. A file larger than header + max body is rejected
// from its size before any byte is read.
[[nodiscard]] SaveStreamReadResult read_save_stream_file(
    const std::string& path, DuplicatePolicy policy = DuplicatePolicy::allow);

struct SaveStreamIoResult {
    SaveStreamError error = SaveStreamError::none;
    int os_errno = 0;
};

// Atomic replace: exclusively create a unique `<path>.tmp.*` in the same directory with mode
// 0644 subject to umask, write and fsync it, rename over `path`, then best-effort fsync of the
// directory. On failure only this invocation's temp is removed and an existing `path` is left
// untouched. Pre-existing temp files and aliases are preserved. This gives same-volume atomic
// replacement, NOT a power-loss durability proof (see docs/VALIDATION.md G2 [B06]/[B07]); on
// macOS F_FULLFSYNC is attempted first. Concurrent-writer ordering is not specified.
[[nodiscard]] SaveStreamIoResult write_file_atomic(const std::string& path,
                                                   std::span<const std::byte> bytes);

}  // namespace sotc
