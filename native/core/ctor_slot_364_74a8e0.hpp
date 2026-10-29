// CPU-01-STARTUP-SLOT-364 (lane M1): native model of startup constructor slot 364 (RVA 0x74a8e0, 497 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 364 (checker-pinned). Every
// body fact below was read end to end from the pinned bytes and is byte-pinned by
// tools/lift/check_ctor_slot_364.py. NOTHING here executes, loads or calls an original RVA: vtable pointers,
// functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape, in order: (1) UNBOUNDED float console variable `cp10_WanderSheathCollisionSwordScalar` (object 0x32525b0) through 0x6aa00 (flags 1,
// empty description, value and default 0.0f, caller argument functor tag 0x15cd240); the site then runs the same
// tail as the other float vars: f590(description), functor destroy (bare ret), value = default, 0x20d60,
// f590(name), __cxa_atexit(0x20f0). (2) a debug-menu SLIDER item through 0x27050 (item 0x3252688): label
// (parent path) `CP10-Anim`, text `WanderSheathCollisionSwordScalar`, target = the variable, flags 0x30, range
// {0x3c23d70a (0.01f), 0x3dcccccd (0.1f)} loaded from rodata (0x127f9ec / 0x127f9f0); registered through
// the tail call 0x268b0, then f590(text), f590(label), atexit(0x272d0, item).
//
// The argument functor 0x15cd240 has its four vtable slots byte-pinned to the lifted float-functor bodies
// 0xc4a0/0xc4c0/0xc4e0/0xc4f0 and is accepted directly by the shared float-functor whitelist.
#pragma once

#include "ctor_shapes_343_356.hpp"
#include "lane_m1_float_item.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot364Rva = 0x74a8e0;
inline constexpr std::size_t kSlot364Size = 497;
inline constexpr FloatVarSpec kSlot364Spec{"cp10_WanderSheathCollisionSwordScalar", "", false, 0u, 0u, 0u, kFloatArgFnVtblSlot364, 1};
inline constexpr char kSlot364ItemLabel[] = "CP10-Anim";
inline constexpr char kSlot364ItemText[] = "WanderSheathCollisionSwordScalar";
inline constexpr std::uint32_t kSlot364ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot364RangeMinBits = 0x3c23d70au;   // 0.01f
inline constexpr std::uint32_t kSlot364RangeMaxBits = 0x3dcccccdu;   // 0.1f
inline constexpr std::uint64_t kSlot364ObjectRva = 0x32525b0;
inline constexpr std::uint64_t kSlot364ItemRva = 0x3252688;

struct Slot364Objects {
    ConCmdRangedVar var;     // 0x32525b0
    DebugMenuItem item;      // 0x3252688
};

struct Slot364Result {
    bool ok = false;
    FloatVarResult var{};
    MenuSiteResult item{};
};

[[nodiscard]] Slot364Result init_slot364_74a8e0(Slot364Objects& objects, ConCmdRegistry& concmd,
                                                    DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
