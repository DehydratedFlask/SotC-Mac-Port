// CPU-01-HELPER-CALLEE-MODELS-04: native model of the two remaining unbounded-int constructor twins 0x140b00 (startup slot 534, 3 call sites) and
// 0x25ab0 (startup slot 566, 1 call site), each 0x142 B (FDE 323). Original ELF sha256 d0723afb...5861d3; every fact is byte-pinned by
// tools/lift/check_helper_140b00.py. NOTHING here executes, loads or calls an original RVA: vptr tables, functor classes and slot RVAs are IDENTITY TAGS.
//
// Both bodies are IDENTICAL to the already-native 0x3a980 (helper_3a980_unbounded_int) outside the same five rel32 operands the 0x233ec0 twin differs in
// (stack guard load, call 0x20af0, mid vptr table lea, seed functor class lea, call __stack_chk_fail); the checker compares the bytes outside those
// windows on all three bodies and resolves the operands. So the object, the engaged flags (both 0), value_ref and the argument copy are exactly
// those of 0x3a980; the twin changes only
//   mid vptr        [this] = mid table + 0x10        (0x3a980: 0x1594190 + 0x10)
//   seed class      [this+0xc0] = private seed table (0x3a980: 0x1595660)
//     0x140b00: mid table 0x1598240, seed class 0x15983c8 {0x140c90 copy, 0x140cb0 clone, 0x140cd0 move, 0x140ce0 bare ret}
//     0x25ab0 : mid table 0x1594fd0, seed class 0x1595100 {0x25c40 copy, 0x25c60 clone, 0x25c80 move, 0x25c90 bare ret}
// Every observed call site passes its OWN seed class as the argument functor (the clone/copy slots store the class constant), so the object keeps
// {payload, seed class, aux}. Any int-family tag (helper_functor_tags.hpp) is admitted too, exactly like the 0x233ec0 twin; anything else is refused with
// NOTHING written (the original would run an unread clone slot).
// The callees do not store the value, do not register and do not run __cxa_atexit: the slot's call sites do (not modelled here).
//
// NOT established: what the variables mean at run time; the final vptr classes of the two slots' objects (slot-side, pinned only as leas).
#pragma once

#include <cstddef>
#include <cstdint>

#include "helper_int_var_sites.hpp"

namespace sotc {

enum class UnboundedIntTwinVariant : std::uint8_t { a_140b00, b_25ab0 };

inline constexpr std::uint64_t kUnboundedIntTwinARva = 0x140b00;
inline constexpr std::uint64_t kUnboundedIntTwinBRva = 0x25ab0;
inline constexpr std::size_t kUnboundedIntTwinABSize = 0x142;
inline constexpr std::uint64_t kUnboundedIntTwinAMidTable = 0x1598240;   // [this] = table + 0x10
inline constexpr std::uint64_t kUnboundedIntTwinBMidTable = 0x1594fd0;
inline constexpr std::uint64_t kUnboundedIntTwinASeedClass = 0x15983c8;
inline constexpr std::uint64_t kUnboundedIntTwinBSeedClass = 0x1595100;
inline constexpr std::uint64_t kUnboundedIntTwinASeedCopyRva = 0x140c90;
inline constexpr std::uint64_t kUnboundedIntTwinASeedCloneRva = 0x140cb0;
inline constexpr std::uint64_t kUnboundedIntTwinASeedMoveRva = 0x140cd0;
inline constexpr std::uint64_t kUnboundedIntTwinASeedDestroyRva = 0x140ce0;
inline constexpr std::uint64_t kUnboundedIntTwinBSeedCopyRva = 0x25c40;
inline constexpr std::uint64_t kUnboundedIntTwinBSeedCloneRva = 0x25c60;
inline constexpr std::uint64_t kUnboundedIntTwinBSeedMoveRva = 0x25c80;
inline constexpr std::uint64_t kUnboundedIntTwinBSeedDestroyRva = 0x25c90;

[[nodiscard]] constexpr std::uint64_t unbounded_int_twin_mid_vptr(UnboundedIntTwinVariant v) noexcept {
    return (v == UnboundedIntTwinVariant::a_140b00 ? kUnboundedIntTwinAMidTable : kUnboundedIntTwinBMidTable) + 0x10;
}
[[nodiscard]] constexpr std::uint64_t unbounded_int_twin_seed_class(UnboundedIntTwinVariant v) noexcept {
    return v == UnboundedIntTwinVariant::a_140b00 ? kUnboundedIntTwinASeedClass : kUnboundedIntTwinBSeedClass;
}

// 0x140b00 / 0x25ab0 (this=rdi, name=rsi, flags=edx.., block, description): leaves the MID vptr in place. Refusals write nothing.
[[nodiscard]] HelperCtorResult helper_unbounded_int_twin_140b00(ConCmdRangedVar& var, UnboundedIntTwinVariant variant, const SharedStr& name,
                                                                std::uint32_t flags, const RangedVarArgs& block,
                                                                const SharedStr& description) noexcept;

}  // namespace sotc
