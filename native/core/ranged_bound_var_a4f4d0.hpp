// CPU-01-RANGED-BOUND-VAR-A4F4D0-01: native recovery of the three console-variable constructors that
// startup slot 309 (0xa505e0, the "Save System" debug block) calls at 0xa50720 / 0xa50ad8 / 0xa50c57.
// Original ELF sha256 d0723afb...5861d3; body bytes, FDE extents and every identity tag are pinned by
// tools/lift/check_console_var_ctors.py. Offline analysis only: nothing here loads, relocates or
// executes an original RVA, and no native code dispatches through a recovered vtable.
//
// ======================= structure: one template, two stack frames =======================
// Each constructor is a WRAPPER that calls a LEAF; the six bodies are three instantiations of one
// shape. Disassembly facts (llvm-objdump over [0xa4f4d0, 0xa4fec0) / [0xa4fec0, 0xa50280) /
// [0xa50280, 0xa505a0)):
//
//   wrapper            leaf              caller
//   0xa4f4d0 (436 B)   0xa4f690 (352 B)  slot 309 @ 0xa50720   force_save_debug_condition  0..5
//   0xa4fec0 (436 B)   0xa50080 (352 B)  slot 309 @ 0xa50ad8   saveLoadStallState          0..18
//   0xa50280 (436 B)   0xa50440 (352 B)  slot 309 @ 0xa50c57   saveStoreStallState         0..20
//
// Each of the six bodies has EXACTLY ONE caller in the image (a full-image `callq` census): the
// wrappers are called only by slot 309, and each leaf only by its own wrapper. 99 / 90 instructions
// per body, 3 direct calls in a wrapper (leaf, 0x20d60, __stack_chk_fail) and 2 in a leaf (0x20af0,
// __stack_chk_fail), 2 conditional branches each (the `args.q[3] != 0` self-pointer test and the
// __stack_chk_guard compare), and 6 / 5 indirect `callq` sites travelling through the caller's
// functor class vtable.
//
// Wrapper prototype (SysV, decoded from the frame):
//   this=rdi, name=rsi, value_ref=rdx, &min=rcx, &max=r8, flags=r9d, args=[rbp+0x10],
//   description=[rbp+0x18]. Sequence:
//     1. guard = *[0x15fe518] (__stack_chk_guard) -> a frame slot;
//     2. [this] = final vptr (address point of the class, see RangedBoundVarClass::final_vptr);
//     3. build a local argument block at -0xd8(%rbp) from the caller's: q[0..3] copied with the
//        self-pointer rule (`if q[3] != 0 then q[0] = &this_block`), fn payload/aux zero, fn vtable
//        = the instantiation's own tag (NOT the caller's - they are equal at every instantiation);
//     4. functor dance: slot1 clone tmp1 <- args.fn; slot0 copy tmp2 <- tmp1; slot0 copy
//        local.fn <- tmp1; slot0 copy local.fn <- tmp2; swap(local.fn_aux, args.fn_aux);
//        slot3 destroy tmp1;
//     5. leaf(this, name, value_ref, &min, &max, flags, &local_block, description);
//     6. slot3 destroy the local block's functor (the class's destroy slot is a bare `ret`);
//     7. [this] = final vptr again;
//     8. dword [this+0xd0] = *value_ref; dword [this+0x68] = *value_ref;   (two separate loads)
//     9. 0x20d60(this) - the register-once guard of the console-command base;
//    10. canary compare; `jne` -> 0xf8f3a0 (__stack_chk_fail).
// No __cxa_atexit lives in these bodies: slot 309 registers the destructor itself.
//
// Leaf prototype: identical argument block ([rbp+0x10] = args, [rbp+0x18] = description). Sequence:
//     1. guard -> frame slot;
//     2. 0x20af0(this, name, description, flags);
//     3. [this] = mid vptr (the transient class, see RangedBoundVarClass::mid_vptr);
//     4. qword [this+0x60] = value_ref;
//        byte [this+0x6c] = 0, byte [this+0x70] = 1, dword [this+0x6c] = *min;
//        byte [this+0x74] = 0, byte [this+0x78] = 1, dword [this+0x74] = *max;
//        ymm [this+0x80] = 0; qword [this+0xc0] = the class functor tag; [this+0xb0] = 0;
//        [this+0xc8] = 0;
//     5. argument copy: xmm [this+0x88] <- args q[1..2]; [this+0x80] <- q[0]; [this+0x98] <- q[3];
//        if q[3] != 0 then [this+0x80] = this+0x80;
//     6. functor dance (clone, copy, tmp <- this.fn, this.fn <- tmp2, aux swap, bare-ret destroy);
//     7. canary compare / __stack_chk_fail.
// Byte offsets +0xa0..+0xaf (the functor's inline buffer) and +0xd4..+0xd7 (tail) are never written.
// This leaf IS the shape already native as 0xc300 (float) / 0xc580 (int) / 0xb0e060 (both bounds
// disengaged): same stores in the same order, parameterised by (mid vptr, class functor tag).
//
// ==================== what the model collapses, and why that is exact ====================
// Wrapper steps 3-4 build a fresh functor from `args` under the instantiation tag, and leaf step 6
// consumes that block with the same clone/copy/aux-swap dance. Composing them therefore leaves the
// object in the state a SINGLE pass produces: fn payload <- args.fn_payload, fn vptr <- class tag,
// fn aux <- args.fn_aux, args q[0..3] <- the caller's q with the self-pointer rule applied to the
// OBJECT (the wrapper's intermediate `q[0] = &local` is overwritten by the leaf's own fixup). The
// model performs that net state and reports how many indirect dispatches it collapsed
// (kRangedBoundFunctorSlotCalls) instead of pretending to run them.
//
// Only the three pinned functor classes are accepted. Their four slot bodies are, per instantiation:
//   0x15d1b78: 0xa4f470 copy / 0xa4f490 clone / 0xa4f4b0 move-assign / 0xa4f4c0 bare-`ret` destroy
//   0x15d1c30: 0xa4fe60      / 0xa4fe80       / 0xa4fea0            / 0xa4feb0
//   0x15d1cc8: 0xa50220      / 0xa50240       / 0xa50260            / 0xa50270
// The copy/clone bodies store {payload <- src+0x10, class tag -> dst+0x20}; the destroy slot is one
// `c3`. Any other tag would dispatch an unread body, so the model refuses with `ok == false` and
// writes NOTHING (a partially built object would be a fabrication).
//
// ============================== the class identities ==============================
// vtable address points, one row per instantiation (all pinned through .data.rel.ro RELATIVE
// relocations):
//   mid vptr   0x15d1a20 (0x15d1a10+0x10)  0x15d1a98  0x15d1b10   installed by the leaf
//   final vptr 0x15d1ba8 (0x15d1b98+0x10)  0x15d1c60  0x15d1cf8   installed by the wrapper
//   fn tag     0x15d1b78                   0x15d1c30  0x15d1cc8   [this+0xc0] and the caller's tag
//   dtor       0xa47d20                    0xa48240   0xa4ab50    final slot 0 (slot 309's atexit rdi)
//   non-deleting dtor 0xa4f7f0             0xa501e0   0xa505a0    final slot 1, adjacent to the leaf
// Final slots 3 / 6 of all three classes are `mov eax,1; ret` (0xa4d490/0xa4d760, 0xa4e190/0xa4e310,
// 0xa4e830/0xa4e9b0), so the debug-menu toggle constructor 0x26ea0 keeps its default range
// {1.0, 10.0} at all four slot-309 toggles (the narrowing needs slot3 == 1 AND slot6 == 3).
//
// ===================== one honest deviation the caller must know =====================
// Slot 309 builds each value cell as a STACK temporary (`movl $0,0x3c(%rsp)` / `0x38` / `0x34` and
// `leaq 0x3c(%rsp), %rdx`) and passes its address as `value_ref`. The object keeps that pointer at
// +0x60 after the constructor returns, i.e. the original leaves +0x60 pointing into a dead frame.
// The model reproduces the shape (it stores the pointer it is given and reads the cell twice-worth of
// state once) and does NOT silently repair it; nothing in the native path dereferences +0x60, and a
// future consumer of class slot 4 / slot 7 must treat it as dead.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_vars.hpp"

namespace sotc {

inline constexpr std::size_t kRangedBoundVarClassCount = 3;
inline constexpr std::uint32_t kRangedBoundVarFlags = 1;      // r9d at all three slot-309 sites
inline constexpr std::size_t kRangedBoundWrapperSize = 0x1b4;  // 436 B, FDE address_range
inline constexpr std::size_t kRangedBoundLeafSize = 0x160;     // 352 B, FDE address_range
inline constexpr std::size_t kRangedBoundWrapperInstructions = 99;
inline constexpr std::size_t kRangedBoundLeafInstructions = 90;
inline constexpr std::size_t kRangedBoundWrapperIndirectCalls = 6;   // 5 in the dance + 1 tear-down
inline constexpr std::size_t kRangedBoundLeafIndirectCalls = 5;
// Indirect functor dispatches a single original construction performs, wrapper + leaf. The model
// collapses all of them to the net state (see the header comment) and reports this count.
inline constexpr std::uint32_t kRangedBoundFunctorSlotCalls = 11;   // == wrapper + leaf
static_assert(kRangedBoundFunctorSlotCalls ==
                  kRangedBoundWrapperIndirectCalls + kRangedBoundLeafIndirectCalls,
              "the collapsed dispatch count is the wrapper and leaf site counts");
inline constexpr std::uint64_t kRangedBoundRegisterRva = 0x20d60;   // wrapper step 9
inline constexpr std::uint64_t kRangedBoundBaseCtorRva = 0x20af0;   // leaf step 2
inline constexpr std::uint64_t kRangedBoundStackChkFailRva = 0xf8f3a0;

// One instantiation. `wrapper_rva` / `leaf_rva` / `dtor_rva` / `non_deleting_dtor_rva` /
// `functor_destroy_rva` are identity only; `mid_vptr` / `final_vptr` / `fn_vtbl` are the three
// constants the two bodies store. A descriptor that is not exactly one of kRangedBoundVarClasses is
// refused, because its vtable slots would not be the pinned bodies.
struct RangedBoundVarClass {
    std::uint64_t wrapper_rva;
    std::uint64_t leaf_rva;
    std::uint64_t mid_vptr;
    std::uint64_t fn_vtbl;
    std::uint64_t final_vptr;
    std::uint64_t functor_destroy_rva;    // [[fn_vtbl] + 0x18], a bare `ret`
    std::uint64_t dtor_rva;               // final vtable slot 0 (the caller's __cxa_atexit rdi)
    std::uint64_t non_deleting_dtor_rva;  // final vtable slot 1, the body right after the leaf
};

inline constexpr std::array<RangedBoundVarClass, kRangedBoundVarClassCount> kRangedBoundVarClasses = {{
    {0xa4f4d0, 0xa4f690, 0x15d1a20, 0x15d1b78, 0x15d1ba8, 0xa4f4c0, 0xa47d20, 0xa4f7f0},
    {0xa4fec0, 0xa50080, 0x15d1a98, 0x15d1c30, 0x15d1c60, 0xa4feb0, 0xa48240, 0xa501e0},
    {0xa50280, 0xa50440, 0x15d1b10, 0x15d1cc8, 0x15d1cf8, 0xa50270, 0xa4ab50, 0xa505a0},
}};

// The pinned row whose wrapper_rva is `wrapper_rva`, or nullptr. Used by callers that only carry the
// original identity (they must not synthesise a descriptor).
[[nodiscard]] const RangedBoundVarClass* ranged_bound_var_class_for(std::uint64_t wrapper_rva) noexcept;

struct RangedBoundVarResult {
    bool ok = false;                        // every stage completed
    ConCmdRegisterResult registration{};     // the wrapper's 0x20d60 call
    std::uint32_t functor_slot_calls = 0;    // kRangedBoundFunctorSlotCalls once the dance is modelled
    bool value_from_cell = false;            // +0x68 / +0xd0 were copied from *value_ref
};

// One original construction (wrapper + leaf), natively. `min_cell` / `max_cell` are the RAW 4-byte
// cells the original copies verbatim (`movl (%rcx), %eax; movl %eax, 0x6c(%rbx)`); they are not
// interpreted as int or float here. `args` is the caller's argument block; its `fn_vptr` must equal
// `cls.fn_vtbl`. Refuses (writes nothing) on an unknown class row, a foreign functor tag or a null
// `value_ref` (the wrapper would fault on the second dereference).
[[nodiscard]] RangedBoundVarResult init_ranged_bound_var_a4f4d0(
    ConCmdRangedVar& var, const RangedBoundVarClass& cls, const SharedStr& name, void* value_ref,
    std::uint32_t min_cell, std::uint32_t max_cell, std::uint32_t flags, const RangedVarArgs& args,
    const SharedStr& description, ConCmdRegistry& registry) noexcept;

}  // namespace sotc
