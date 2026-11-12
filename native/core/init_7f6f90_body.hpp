// Native 0x7f6f90 body: load one staffroll text file and store one parsed token record.
//
// The 656 B original (0x7f6f90..0x7f7220, sha256
// 6b68dce8a6d7e449e5228659a617755f99f78e39813dd0e5ef26f68d4f66a2bd) first opens
// "/app0/CP3Shadow/%s" in "rb".  If fopen returns null it falls back to the resource-file
// family 0x56cdb0/0x56ceb0/0x56cec0/0x56ce60 with the unmodified string as the path.
//
// The parser is deliberately narrow.  It normalizes CR/LF to NUL, strips a UTF-8 BOM,
// finds the first non-space/control byte, then advances by strlen(remainder)+1.  The
// original therefore records at most the first token in each buffer; the bytes after the
// comma remain owned by the retained text buffer.  The record is {int table_index, u32
// reserved, const char* comma_plus_one}; table_index is -1 for an unknown token.
//
// Direct calls in the original, pinned by tools/lift/check_f531d60_post_base_7f7220.py:
//   0x57cb70 vsnprintf wrapper, 0xf902a0 fopen, 0xf90390 fseek, 0xf903a0 ftell,
//   0xf90210 malloc, 0xf903d0 fread, 0xf90270 fclose, 0x56cdb0 fallback load,
//   0x56ceb0 fallback size, 0x56cec0 fallback copy, 0x56ce60 fallback close,
//   0xf8f3b0 strlen, 0xf90220 ctype lookup (mask 0x144), 0xf902c0 strchr,
//   0xf8f500 strcasecmp, 0xf8f3a0 stack-check trap (pinned, never called).
// libc provides the first eight and the three string helpers; the four fallback calls are
// explicit host contracts because their resource manager is not modelled.

#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <memory>

#include "init_post_call_trace.hpp"

namespace sotc {

inline constexpr std::uint64_t kStaffroll7f6f90Rva = 0x7f6f90;
inline constexpr std::size_t kStaffroll7f6f90Size = 0x290;  // 656 B
inline constexpr std::uint64_t kStaffroll7f6f90EndRva = 0x7f7220;
inline constexpr std::uint64_t kStaffroll7f6f90PathFormatRva = 0x128b85f;
inline constexpr std::uint64_t kStaffroll7f6f90OpenModeRva = 0x128b872;
inline constexpr std::uint64_t kStaffroll7f6f90FallbackFormatRva = 0x128b875;
inline constexpr std::uint64_t kStaffroll7f6f90ShadowStringRva = 0x128b878;
inline constexpr std::uint64_t kStaffroll7f6f90OriginalStringRva = 0x128b892;
inline constexpr std::uint64_t kStaffroll7f6f90RemasterStringRva = 0x128b8ac;
inline constexpr std::uint64_t kStaffroll7f6f90TokenTableRva = 0x15ce750;
inline constexpr std::uint64_t kStaffroll7f6f90CountsRva = 0x326806c;
inline constexpr std::uint64_t kStaffroll7f6f90EntriesRva = 0x3268080;
inline constexpr std::size_t kStaffroll7f6f90TokenCount = 37;
inline constexpr std::size_t kStaffroll7f6f90ModeStride = 0x10000;
inline constexpr std::size_t kStaffroll7f6f90EntryBytes = 16;
inline constexpr std::size_t kStaffroll7f6f90MaxEntries = 0x1000;
inline constexpr std::size_t kStaffroll7f6f90PathBytes = 0x100;

struct Staffroll7f6f90Entry {
    std::int32_t index = -1;
    std::uint32_t reserved = 0;
    const char* rest = nullptr;
};
static_assert(sizeof(Staffroll7f6f90Entry) == kStaffroll7f6f90EntryBytes);

struct Staffroll7f6f90Hooks {
    // 0x56cdb0 family: explicit host resource-file contract, used only when fopen misses.
    // The host owns the slot and must keep load/copy/close consistent.
    InitPostCallOutcome (*load_56cdb0)(const char* path, std::int64_t arg, int* slot) = nullptr;
    InitPostCallOutcome (*size_56ceb0)(int slot, std::size_t* size) = nullptr;
    InitPostCallOutcome (*copy_56cec0)(int slot, char* destination, std::size_t size) = nullptr;
    InitPostCallOutcome (*close_56ce60)(int slot) = nullptr;

    // Optional override for fopen.  Null uses std::fopen.  The body still uses stdio for
    // fseek/ftell/fread/fclose, matching the original direct calls.
    InitPostCallOutcome (*fopen_f902a0)(const char* path, const char* mode,
                                        std::FILE** file) = nullptr;

    // Optional override for the ctype lookup at 0xf90220.  Null uses the host C locale
    // space/control predicate; underscores and other token punctuation are not skipped.
    InitPostCallOutcome (*is_skip_f90220)(unsigned char byte, bool* skip) = nullptr;

    std::int32_t* counts = nullptr;             // [2], remaster and original mode
    Staffroll7f6f90Entry* entries = nullptr;    // [2 * entry_capacity]
    std::size_t entry_capacity = 0;
};

struct Staffroll7f6f90Result {
    bool ok = false;
    const char* blocked = nullptr;
    int mode = -1;
    int entries = 0;
    std::size_t text_bytes = 0;
    bool used_fopen = false;
    // Retained parsed buffer.  Entry.rest points into this allocation; the runtime must
    // keep the result (or move this allocation to a longer-lived owner) while entries live.
    std::unique_ptr<char[]> text;
    InitPostCallLog call_log{};
};

// mode is the original's argument: 0 = remaster, 1 = original.  string is the basename
// passed to the "%s" fallback and to "/app0/CP3Shadow/%s".
[[nodiscard]] Staffroll7f6f90Result run_staffroll_7f6f90_body(
    int mode, const char* string, const Staffroll7f6f90Hooks& hooks) noexcept;

}  // namespace sotc
