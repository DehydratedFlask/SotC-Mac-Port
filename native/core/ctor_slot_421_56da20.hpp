// CPU-01-STARTUP-SLOTS-M2 (lane M2): native model of startup constructor slot 421 (RVA 0x56da20, 329 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 421. Every body fact below was
// read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_421.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: the bool variable `filesystem_allowAsync` (object 0x2f08fe8, constructor flags 8 -- every other observed
// bool variable passes 1 -- functor tag 0x15cade0, value and default 0), then a 128-byte zero block at 0x2f090d0
// (four 32-byte ymm stores, in the order +0x60, +0x40, +0x20, +0x00) without atexit. The block's consumer is NOT
// recovered here.
#pragma once

#include <array>

#include "lane_m2_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot421Rva = 0x56da20;
inline constexpr std::size_t kSlot421Size = 329;
inline constexpr char kSlot421VarName[] = "filesystem_allowAsync";
inline constexpr std::uint64_t kSlot421FnTag = 0x15cade0;
inline constexpr std::uint32_t kSlot421CtorFlags = 8;
inline constexpr std::size_t kSlot421BlockSize = 128;

struct Slot421Objects {
    ConCmdBoolVar var;                                  // 0x2f08fe8
    std::array<std::uint8_t, kSlot421BlockSize> block;  // 0x2f090d0
};

struct Slot421Result {
    bool ok = false;
    BoolVarResult var{};
};

[[nodiscard]] Slot421Result init_slot421_56da20(Slot421Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
