// The `sceSaveDataSetParam` block (`0x530` bytes), the native `USR-META` container and the shared
// SaveIcon payload (E604).
//
// The guest builds the block with `memset(0x530)` + three `strcpy` (0xa4b580..0xa4b5e4) and passes
// it to `SetParam(mount, 0, block, 0x530)` at 0xa4bcb1 without reading the result. The game reads
// only `+0x500` back (0xa48003); the texts are platform-dialog-only. The text bytes are caller data:
// the localization values are not in the ELF. The native encoder therefore length-checks before any
// I/O instead of copying unbounded strings.
//
// `USR-META` is a native wrapper (the guest never opens it): the block plus the FNV of the USR-DATA
// body it describes, so `save_enumerate` can report a metadata/data mismatch.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace sotc {

inline constexpr std::size_t kSaveParamBytes = 0x530;
inline constexpr std::size_t kSaveMetaTitleSlot = 0x080;
inline constexpr std::size_t kSaveMetaSubtitleSlot = 0x080;
inline constexpr std::size_t kSaveMetaDetailSlot = 0x400;
inline constexpr std::size_t kSaveMetaUserParamOffset = 0x500;
inline constexpr std::size_t kSaveMetaReservedBytes = 0x02C;  // +0x504..+0x52f, memset only
inline constexpr std::size_t kSaveIconMaxBytes = 1u << 20;   // NATIVE POLICY, not evidence

struct SaveMeta {
    // Stored without the NUL; encode adds it. A text may occupy at most slot-1 bytes, because the
    // guest writes len+1 bytes with strcpy into a slot of exactly `slot` bytes.
    std::string title;     // <= kSaveMetaTitleSlot - 1    (0x7f)
    std::string subtitle;  // <= kSaveMetaSubtitleSlot - 1 (0x7f)
    std::string detail;    // <= kSaveMetaDetailSlot - 1   (0x3ff)
    std::uint32_t user_param = 0;                             // -> +0x500
    std::array<std::byte, kSaveMetaReservedBytes> reserved{};  // -> +0x504
};

enum class SaveMetaError : std::uint8_t {
    none,
    text_too_long,
    text_has_nul,
    raw_size,
};

[[nodiscard]] const char* to_string(SaveMetaError) noexcept;

// `out` becomes exactly kSaveParamBytes. Rejects a text that would need limit+1 bytes; never
// truncates and never writes a partial block.
[[nodiscard]] SaveMetaError encode_save_meta(const SaveMeta& meta, std::vector<std::byte>& out);

// Strict decode: size == 0x530, all three texts NUL-terminated inside their slot, reserved copied.
[[nodiscard]] SaveMetaError decode_save_meta(std::span<const std::byte> bytes, SaveMeta& out);

// ---- USR-META container ---------------------------------------------------------------------
inline constexpr std::string_view kSaveMetaMagic = "SOTCMETA";
inline constexpr std::uint32_t kSaveMetaVersion = 1;
inline constexpr std::size_t kSaveMetaHeaderBytes = 0x20;
inline constexpr std::size_t kSaveMetaFileBytes = kSaveMetaHeaderBytes + kSaveParamBytes;

// {8 magic, u32 version, u32 descriptor_id, u64 data_hash, u64 param_bytes} + block.
// Returns an empty vector unless block.size() == kSaveParamBytes.
[[nodiscard]] std::vector<std::byte> encode_meta_file(std::span<const std::byte> block,
                                                      std::uint32_t data_hash,
                                                      std::uint32_t descriptor_id);

enum class SaveMetaFileError : std::uint8_t {
    none,
    missing,
    bad_magic,
    bad_version,
    bad_size,
    bad_param_bytes,
};

[[nodiscard]] const char* to_string(SaveMetaFileError) noexcept;

// Strict decode of the container. `data_hash`/`descriptor_id` are the stored values; `block` is
// exactly kSaveParamBytes on success and empty on failure.
[[nodiscard]] SaveMetaFileError decode_meta_file(std::span<const std::byte> bytes,
                                                 std::uint32_t& data_hash,
                                                 std::uint32_t& descriptor_id,
                                                 std::vector<std::byte>& block);

// ---- SaveIcon -------------------------------------------------------------------------------
struct SaveIcon {
    std::vector<std::byte> bytes;  // empty = no icon; the guest skips SaveIcon
    std::string resource_name;     // provenance only
};

[[nodiscard]] bool save_icon_ok(const SaveIcon& icon) noexcept;

}  // namespace sotc
