// Native recovery of original constructor-table index 301 / RVA 0xac9550 (ELF sha256
// d0723afb...5861d3). 323-byte static initialiser (sha256 b4b9f2d5...a22ea04) of the `rumbleEnabled`
// console variable, a bool variable object at original .bss 0x33054a0:
//
//   name view {"rumbleEnabled" (0x12cdde0), len 0xd, 0, ctl 0}
//   description view {"" (0x12cdf29 -> a NUL byte), len 0, 0, ctl 0}
//   [obj] = 0x1594218; call 0x20af0(obj, name, description, 1)     ; ConCmd base constructor
//   [obj] = 0x15940b0; [obj+0x60] = obj+0xc0; dword [obj+0x69] = 0; zero [obj+0x70..0x8f];
//   qword [obj+0xa0] = 0; qword [obj+0xb0] = 0x15d5070; qword [obj+0xb8] = 0
//   call 0xf590(description)                                         ; inert, no control counter
//   [obj] = 0x1594218; byte [obj+0xc0] = 1; byte [obj+0x68] = 1
//   call 0x20d60(obj)                                                ; register (pending list)
//   call 0xf590(name)                                                ; inert
//   __cxa_atexit(0x20b0, obj, 0x1604218)
//
// The whole body is native: the shared shape lives in core/concmd_vars (init_bool_var_static) and
// is byte-pinned at this site and at the second block of slot 299's body. The registered
// destructor 0x20b0 is pinned, not executed. The declaring translation unit is the input
// generator (`CInputGenerator.cpp`, the rodata run that follows the key string), so this is a
// gameplay-adjacent setting, but no native consumer of the value exists yet.
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_vars.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot301Rva = 0xac9550;
inline constexpr std::size_t kSlot301BodySize = 323;
inline constexpr std::uint64_t kSlot301ObjectRva = 0x33054a0;
inline constexpr std::uint64_t kSlot301FnVtbl = 0x15d5070;      // functor vtable tag (identity only)
inline constexpr char kSlot301Name[] = "rumbleEnabled";          // 0x12cdde0, length 13

using Slot301Object = ConCmdBoolVar;

struct Slot301Result {
    bool ok = false;
    BoolVarResult var{};
};

// Models 0xac9550 against the shared ConCmd registry. `ok == false` is a visible failure
// (registration contract missing, a view carrying a control counter, or atexit not recordable).
[[nodiscard]] Slot301Result init_slot301_ac9550(Slot301Object& obj, ConCmdRegistry& registry) noexcept;

}  // namespace sotc
