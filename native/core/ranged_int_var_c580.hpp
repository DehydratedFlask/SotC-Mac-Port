// CPU-01-RANGED-INT-C580-01: native recovery of 0xc580 (0x15f B, [0xc580, 0xc6df)), the ranged INT
// console-variable constructor with BOTH bounds engaged, used by startup slot 308 (0xa58f10). Original
// ELF sha256 d0723afb...5861d3; pinned by tools/lift/check_ctor_slot_308.py. Nothing here executes an
// original RVA: vtable pointers and functor tags are identity values.
//
// The body is the int twin of the float constructor 0xc300 (core/concmd_vars): the 70 instructions are
// byte-identical except for four rip-relative operands (the mid vtable, the functor class, and the two
// PLT-style stack-check cells that are the same in both). Differences from 0xc300:
//   mid vptr    [this] = 0x1594190 + 0x10 = 0x15941a0   (float: 0x1594118 + 0x10)
//   fn class    [this+0xc0] = 0x15944a0                 (float: 0x1594318); slots 0xc720 copy,
//               0xc740 clone, 0xc760 move, 0xc770 bare-ret destroy, each storing 0x15944a0 where the
//               float bodies store 0x1594318
// Prototype (SysV): this=rdi, name=rsi, value_ref=rdx, &min=rcx, &max=r8, flags=r9d, args=[rsp+8],
// description=[rsp+0x10]. Sequence: 0x20af0(this, name, description, flags); [this] = mid vptr;
// [+0x60] = value_ref; byte[+0x6c] = 0, byte[+0x70] = 1, dword[+0x6c] = *min; byte[+0x74] = 0,
// byte[+0x78] = 1, dword[+0x74] = *max; ymm[+0x80] = 0; [+0xc0] = class; [+0xb0] = 0; [+0xc8] = 0;
// then the argument copy at +0x80 and the four-step functor install (clone, copy, copy, copy, swap aux,
// bare-ret destroy), which the caller's functor tag selects. Only the lifted tags are accepted: the
// caller's tags 0x15d1d80 (slot 308) and the class tag 0x15944a0 itself, whose four slots are the same
// four bodies (0xc720..0xc770). Any other tag would run an unread clone slot: the model refuses with
// `false` and writes nothing.
//
// The final class (installed by the CALLER after this returns: `[this] = 0x1594bf8 + 0x10`) is
// 0x1594c08; its virtual slots 3 and 6 are `mov eax,1; ret` (0xbf70) and `mov eax,2; ret` (0xbfc0) and
// are the two virtual calls of the debug-menu toggle constructor 0x26ea0, so the `slot3 == 1 &&
// slot6 == 3` range narrowing does NOT apply (ranged_int_c580_menu_target reports {1, 2}).
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_vars.hpp"
#include "debug_menu_item.hpp"

namespace sotc {

inline constexpr std::uint64_t kRangedIntCtorRva = 0xc580;
inline constexpr std::size_t kRangedIntCtorSize = 0x15f;
inline constexpr std::uint64_t kRangedIntMidVptr = 0x15941a0;       // 0x1594190 + 0x10
inline constexpr std::uint64_t kRangedIntFinalVptr = 0x1594c08;     // 0x1594bf8 + 0x10 (stored by the caller)
inline constexpr std::uint64_t kRangedIntFnVtbl = 0x15944a0;        // functor class tag
inline constexpr std::uint64_t kRangedIntArgFnVtbl = 0x15d1d80; // slot 308's caller functor tag
inline constexpr std::uint64_t kRangedIntDtorRva = 0x18a60;         // class slot 0 (atexit target)
inline constexpr std::uint64_t kRangedIntSlot3Rva = 0xbf70;         // mov eax,1; ret
inline constexpr std::uint64_t kRangedIntSlot6Rva = 0xbfc0;         // mov eax,2; ret
inline constexpr std::uint32_t kRangedIntSlot3Result = 1;
inline constexpr std::uint32_t kRangedIntSlot6Result = 2;

// 0xc580. `false` (nothing written) for an unlifted argument-functor tag.
[[nodiscard]] bool init_ranged_int_var_c580(ConCmdRangedVar& var, const SharedStr& name, void* value_ref,
                                            std::int32_t min_value, std::int32_t max_value,
                                            std::uint32_t flags, const RangedVarArgs& args,
                                            const SharedStr& description) noexcept;

// The two virtual-slot results 0x26ea0 reads from a variable of class 0x1594c08. `known` is false
// unless the variable's vptr is exactly the final class.
[[nodiscard]] DebugMenuTarget ranged_int_c580_menu_target(const ConCmdRangedVar& var) noexcept;

}  // namespace sotc
