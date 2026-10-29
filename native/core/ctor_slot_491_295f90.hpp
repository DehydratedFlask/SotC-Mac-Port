// CPU-01-STARTUP-SLOTS-M5 (lane M5): native model of startup constructor slot 491 (RVA 0x295f90, 372 bytes).
// Original ELF sha256 d0723afb...5861d3; the constructor-table word for slot 491 is at 0x2dd70e0 - 491 * 8. Every
// body fact below was read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_491.py.
// NOTHING here executes, loads or calls an original RVA: the vptr, the cleanup RVAs and the zero helper are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape (the slot 302 shape plus a caption): NINE eight-byte handle cells at 0x2eef540 + 8 * k (k = 0..8). Per
// cell, in order: 0xaaf040(cell) (7 B `mov qword [rdi],0; ret`, native as `= nullptr`), then
// __cxa_atexit(0xaaf050, cell, &dso). 0xaaf050 is the 52 B handle cleanup (null path inert; the live path is not
// lifted and never reached here). Then, tail of the function:
//   caption SharedStr at 0x2eef5a0 = {"<TEXT NOT FOUND>" (0xfc6ac3), 16, 0, 0}   (data, size by two stores, aux/ctl
//   zeroed by one 16 B store), then the identity cell 0x2eef588 = 0x159e0e8 + 0x10 (stored AFTER the caption),
//   then the TAIL CALL `jmp __cxa_atexit(0xf7b0, 0x2eef5a0, &dso)` (0xf7b0 = SharedStr destructor).
// Bytes 0x2eef590..0x2eef59f are never written. OBSERVATION only: who reads the nine handles, the identity cell
// or the caption (a placeholder text for missing localisation, by its content: INFERENCE) is NOT modelled here.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot491Rva = 0x295f90;
inline constexpr std::size_t kSlot491Size = 372;
inline constexpr std::size_t kSlot491HandleCount = 9;
inline constexpr std::uint64_t kSlot491FirstHandleRva = 0x2eef540;
inline constexpr std::uint64_t kSlot491IdentityCellRva = 0x2eef588;
inline constexpr std::uint64_t kSlot491IdentityVptr = 0x159e0f8;      // 0x159e0e8 + 0x10
inline constexpr std::uint64_t kSlot491CaptionRva = 0x2eef5a0;
inline constexpr std::uint64_t kSlot491ZeroRva = 0xaaf040;
inline constexpr std::uint64_t kSlot491HandleDtorRva = 0xaaf050;
inline constexpr std::uint64_t kSlot491CaptionDtorRva = 0xf7b0;
inline constexpr char kSlot491CaptionText[] = "<TEXT NOT FOUND>";

struct Slot491Objects {
    std::array<void*, kSlot491HandleCount> handles;   // 0x2eef540 .. 0x2eef580
    std::uint64_t identity_vptr;                      // 0x2eef588
    std::uint8_t gap590[0x10];                        // 0x2eef590 never written
    SharedStr caption;                                // 0x2eef5a0
};
static_assert(offsetof(Slot491Objects, identity_vptr) == 0x48 && offsetof(Slot491Objects, caption) == 0x60 &&
              sizeof(Slot491Objects) == 0x80);

// False when the host atexit log could not record an entry; every source store still happens (as in 302).
[[nodiscard]] bool init_slot491_295f90(Slot491Objects& objects, StartupAtexitLog& cleanup) noexcept;

}  // namespace sotc
