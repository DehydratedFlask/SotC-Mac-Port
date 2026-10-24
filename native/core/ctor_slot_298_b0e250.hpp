// Native recovery of original constructor-table index 298 / RVA 0xb0e250 (ELF sha256
// d0723afb...5861d3). 331-byte static initialiser (sha256 39d29b11...32dddea) of the
// `shaderTypeToLoad` console variable, a ranged/enum variable object at original .bss 0x3305d20
// (value cell +0xd0, `EShaderTypeToLoad` strings sit next to the key in rodata):
//
//   name view {"shaderTypeToLoad" (0x12cfbe0), len 0x10, 0, 0}
//   description view {"" (0x12cfce7 -> NUL), len 0, 0, 0}
//   argument block at rsp+0x40: four zero qwords; functor {payload 0, vtable 0x15d5960, aux 0}
//   [obj] = 0x15d58f8
//   call 0xb0e060(obj, name, obj+0xd0, 1, &args, description)         ; ranged-var constructor
//   call 0xf590(description)                                            ; inert
//   call [0x15d5960 + 0x18] on the argument functor                     ; RESOLVED: bare `ret`
//   [obj] = 0x15d58f8; dword [obj+0xd0] = 0; dword [obj+0x68] = 0
//   call 0x20d60(obj)                                                   ; register (pending list)
//   call 0xf590(name)                                                   ; inert
//   __cxa_atexit(0xb0cb40, obj, 0x1604218)
//
// The previously unresolved indirect call (analysis R4) is the functor DESTROY slot of the
// four-entry table at 0x15d5960 (ph1 .data.rel.ro, RELATIVE relocations 0xb0e1f0 / 0xb0e210 /
// 0xb0e230 / 0xb0e240). Slot 3 is the single byte `c3`, so destroying the (payload-0, aux-0)
// argument functor has no effect, and the slot-0/1/2 bodies are the 20/20/9-byte copy stores that
// 0xb0e060 uses to install the functor. All four bodies are byte-pinned by the checker.
//
// Everything in this slot is native; the registered destructor 0xb0cb40 is pinned, not executed.
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_vars.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot298Rva = 0xb0e250;
inline constexpr std::size_t kSlot298BodySize = 331;
inline constexpr std::uint64_t kSlot298ObjectRva = 0x3305d20;
inline constexpr std::uint64_t kSlot298FinalVptr = 0x15d58f8;     // 0x15d58e8 + 0x10
inline constexpr std::uint64_t kSlot298DtorRva = 0xb0cb40;        // pinned, not executed
inline constexpr std::size_t kSlot298DtorSize = 0x36;
inline constexpr char kSlot298Name[] = "shaderTypeToLoad";        // 0x12cfbe0, length 16

using Slot298Object = ConCmdRangedVar;

struct Slot298Result {
    bool ok = false;
    bool ctor_ok = false;                 // 0xb0e060 completed
    bool functor_destroy_noop = false;    // the resolved indirect call is a bare `ret`
    ConCmdRegisterResult registration{};
    bool views_inert = false;
    bool atexit_recorded = false;
};

// Models 0xb0e250 against the shared ConCmd registry.
[[nodiscard]] Slot298Result init_slot298_b0e250(Slot298Object& obj, ConCmdRegistry& registry) noexcept;

}  // namespace sotc
