// CPU-01-TINY-ATEXIT-SLOTS-01 (lane L1-TINY): native recovery of 19 original constructor-table slots whose
// body is "zero part of a static object, register its destructor with __cxa_atexit":
//
//   311 0xa46b70  515 0x1c00e0  522 0x16e960  523 0x16cd50  525 0x166ef0  527 0x157e60  528 0x156fb0
//   531 0x1442c0  533 0x143300  542 0x0fb3d0  546 0x0a25f0  547 0x09a970  549 0x096650  551 0x08d740
//   564 0x031220  565 0x02fcf0              one 8-byte cell each (qword 0), one atexit record
//   425 0x54b9c0                            two 8-byte cells, two atexit records (zero, atexit, zero, atexit)
//   432 0x51bc20                            24 zero bytes at +0x00 (vmovups + qword), one atexit record
//   471 0x3600f0                            24 zero bytes at +0x08 (vmovups + qword), one atexit record
//
// (ELF sha256 d0723afb...5861d3; offline analysis only.) The 15 thirty-three-byte members are the
// STATIC-CLEANUP-THUNKS set of tools/lift/verify_cleanup_thunks.py; 311 differs only in using a
// rip-relative store (37 B), 425/432/471 have the same effect with wider zero ranges.
//
// The shared table is data: `kTinyAtexitRows`. For every record the original does, in this order:
//   (zero the range [object + zero_offset, +zero_length)) then `__cxa_atexit(dtor, object, 0x1604218)`.
// Bytes of the object outside the zero range are NOT written (471 leaves object+0..7 alone). The
// destructor RVAs, object RVAs and the dso handle are IDENTITY TAGS ONLY: the destructor is recorded
// in the StartupAtexitLog and never run, nothing is executed through an original address. The native
// object is a caller-owned 0x20-byte buffer standing in for the original .bss cell; the log records
// that buffer's address as the atexit argument.
//
// Fail closed: a null log, an unknown slot or a log that cannot record reports ok == false with a named
// `blocked` stage. A null log is detected before the first write; a multi-record row (425) reports
// `records_done` for the records that did complete.
//
// Shared cells: 515 zeroes the app-info registry pointer `*(0x2ee68c8)` (core/app_info_registry) and
// 565 the debug-menu parent cell `[0x2dfd4b8]` (core/debug_menu_item). This module only owns standalone
// stand-ins for them; a host that wants one shared cell passes that cell's storage as the object
// instead of the state-owned one (see init_tiny_atexit_record).
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"

namespace sotc {

inline constexpr std::uint64_t kTinyAtexitDsoRva = 0x1604218;   // __dso_handle, identity tag
inline constexpr std::uint64_t kTinyAtexitPltRva = 0xf8f390;    // __cxa_atexit PLT thunk
inline constexpr std::size_t kTinyAtexitRowCount = 19;
inline constexpr std::size_t kTinyObjectBytes = 0x20;           // widest object (471: +0x08..+0x1f)

struct TinyAtexitRecord {
    std::uint64_t object_rva;   // original .bss address of the object (identity only)
    std::uint64_t dtor_rva;     // destructor registered with __cxa_atexit (identity only, never run)
    std::uint32_t zero_offset;  // first zeroed byte, relative to the object
    std::uint32_t zero_length;  // number of zeroed bytes
};

struct TinyAtexitRow {
    std::uint32_t slot;         // original constructor-table index
    std::uint64_t rva;          // original body address (identity only)
    std::uint32_t body_size;    // bytes of the original body
    std::uint32_t record_count; // 1 or 2
    std::array<TinyAtexitRecord, 2> rec;
};

// In original index order (one record per line group; machine-checked by check_ctor_tiny_atexit_slots.py).
inline constexpr std::array<TinyAtexitRow, kTinyAtexitRowCount> kTinyAtexitRows = {{
    {311, 0xa46b70, 37, 1, {{{0x32f7630, 0xa46620, 0, 8}, {0, 0, 0, 0}}}},
    {425, 0x54b9c0, 77, 2, {{{0x2eff448, 0x53fcb0, 0, 8}, {0x2eff450, 0x53fce0, 0, 8}}}},
    {432, 0x51bc20, 42, 1, {{{0x2efc930, 0x51b3a0, 0, 24}, {0, 0, 0, 0}}}},
    {471, 0x3600f0, 43, 1, {{{0x2ef54d0, 0x35efb0, 8, 24}, {0, 0, 0, 0}}}},
    {515, 0x1c00e0, 33, 1, {{{0x2ee68c8, 0x1bdae0, 0, 8}, {0, 0, 0, 0}}}},
    {522, 0x16e960, 33, 1, {{{0x2ee3e10, 0x16cda0, 0, 8}, {0, 0, 0, 0}}}},
    {523, 0x16cd50, 33, 1, {{{0x2ee3e08, 0x16b940, 0, 8}, {0, 0, 0, 0}}}},
    {525, 0x166ef0, 33, 1, {{{0x2ee3cd8, 0x163ad0, 0, 8}, {0, 0, 0, 0}}}},
    {527, 0x157e60, 33, 1, {{{0x2ee3c20, 0x156ff0, 0, 8}, {0, 0, 0, 0}}}},
    {528, 0x156fb0, 33, 1, {{{0x2ee3c18, 0x1561c0, 0, 8}, {0, 0, 0, 0}}}},
    {531, 0x1442c0, 33, 1, {{{0x2ee2cf8, 0x144290, 0, 8}, {0, 0, 0, 0}}}},
    {533, 0x143300, 33, 1, {{{0x2ee2a50, 0x143110, 0, 8}, {0, 0, 0, 0}}}},
    {542, 0x0fb3d0, 33, 1, {{{0x2edef60, 0x0faf80, 0, 8}, {0, 0, 0, 0}}}},
    {546, 0x0a25f0, 33, 1, {{{0x2ed37c0, 0x0a1da0, 0, 8}, {0, 0, 0, 0}}}},
    {547, 0x09a970, 33, 1, {{{0x2ed37b0, 0x0994c0, 0, 8}, {0, 0, 0, 0}}}},
    {549, 0x096650, 33, 1, {{{0x2ed3670, 0x095620, 0, 8}, {0, 0, 0, 0}}}},
    {551, 0x08d740, 33, 1, {{{0x2ed33f0, 0x08d620, 0, 8}, {0, 0, 0, 0}}}},
    {564, 0x031220, 33, 1, {{{0x2dfd678, 0x02fd20, 0, 8}, {0, 0, 0, 0}}}},
    {565, 0x02fcf0, 33, 1, {{{0x2dfd4b8, 0x026290, 0, 8}, {0, 0, 0, 0}}}},
}};

// Caller-owned stand-in for one original object (at most 0x20 bytes are ever touched).
struct TinyObject {
    alignas(8) std::array<std::uint8_t, kTinyObjectBytes> bytes{};
};

// One TinyObject per record, indexed [row][record]. Row order == kTinyAtexitRows order.
struct TinyAtexitState {
    std::array<std::array<TinyObject, 2>, kTinyAtexitRowCount> objects{};
};

struct TinyAtexitResult {
    bool ok = false;
    std::uint32_t slot = 0;
    std::uint32_t records_done = 0;     // records fully applied (zero + atexit)
    const char* blocked = nullptr;      // named stage when ok == false
};

// Row for an original slot index, or nullptr.
[[nodiscard]] const TinyAtexitRow* find_tiny_atexit_row(std::uint32_t slot) noexcept;
// Position of that row in kTinyAtexitRows (and in TinyAtexitState::objects), or kTinyAtexitRowCount.
[[nodiscard]] std::size_t tiny_atexit_row_index(std::uint32_t slot) noexcept;

// Applies ONE record: zero [object + zero_offset, +zero_length) inside `object` (a caller buffer of at
// least zero_offset + zero_length bytes), then record (dtor_rva, &object). False only when the log
// cannot record; the zero stores have then already happened, exactly like the original ordering.
[[nodiscard]] bool init_tiny_atexit_record(const TinyAtexitRecord& record, std::uint8_t* object,
                                           StartupAtexitLog& log) noexcept;

// Models the constructor of `slot`. Null `log` is rejected before any write.
[[nodiscard]] TinyAtexitResult init_tiny_atexit_slot(std::uint32_t slot, TinyAtexitState& state,
                                                     StartupAtexitLog* log) noexcept;

}  // namespace sotc
