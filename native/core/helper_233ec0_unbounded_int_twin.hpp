// CPU-01-STARTUP-BATCH-7: native model of the unbounded-int constructor twin 0x233ec0 (0x142 B, [0x233ec0, 0x234002); FDE 323 B), used by startup
// constructor slot 506 (0x2340b0) only. Original ELF sha256 d0723afb...5861d3; every fact is byte-pinned by
// tools/lift/check_helper_233ec0.py. NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs
// are IDENTITY TAGS, recorded and never run.
//
// 0x233ec0 is the unbounded-int constructor 0x3a980 with a different class pair: the two 322 byte bodies are IDENTICAL outside five rel32
// operands (stack-guard load, 0x20af0 call, mid vtable lea, seed functor table lea, __stack_chk_fail call), which the checker resolves on both
// bodies:
//   mid vptr        [this] = 0x159b590 + 0x10 = 0x159b5a0    (0x3a980: 0x1594190 + 0x10)
//   seed functor    [this+0xc0] = 0x159b680                  (0x3a980: 0x1595660)
// The seed class 0x159b680 is private to this object: slots {0x234050 copy, 0x234070 clone, 0x234090 move, 0x2340a0 bare ret}, whose copy and
// clone store {payload, class 0x159b680} (the int bodies 0xc720 / 0xc740 with the class constant 0x159b680 instead of 0x15944a0). The install
// sequence clones the caller functor into a temporary, copies it into a second temporary, copies this+0xa0 into the first temporary, copies the
// second temporary into the object through ITS OWN class's copy slot, swaps the aux word and destroys the first temporary through
// [0x159b680 + 0x18] = 0x2340a0 (a bare ret). The two temporaries are not observable, so the object ends with {payload, class C, aux} where C is
// the class constant the caller functor's clone stores: 0x159b680 for the private class (slot 506's own argument table is 0x159b680 itself, so
// the final +0xc0 class is 0x159b680), 0x15944a0 for every int-family table. The shared worker model is reused with the family-normalised tag
// and the +0xc0 class is then re-tagged; the mid vptr is re-tagged too.
//
// The slot's final class is 0x159b608 + 0x10 = 0x159b618 (dtor 0x230d60, slot 3 = 0x2334c0 `mov eax,1; ret`, slot 6 = 0x233510 `mov eax,1; ret`):
// the 0x26ea0 gate `slot3 == 1 && slot6 == 3` does not fire, so a toggle on this variable keeps the {1.0, 10.0} range.
//
// NOT established: what the variable means at run time (the name `drawTouch` is INFERENCE from the string only).
#pragma once

#include <cstddef>
#include <cstdint>

#include "debug_menu_item.hpp"
#include "helper_int_var_sites.hpp"

namespace sotc {

inline constexpr std::uint64_t kUnboundedIntTwinRva = 0x233ec0;
inline constexpr std::size_t kUnboundedIntTwinSize = 0x142;
inline constexpr std::uint64_t kUnboundedIntTwinMidVptr = 0x159b5a0;     // 0x159b590 + 0x10
inline constexpr std::uint64_t kUnboundedIntTwinSeedFnTable = 0x159b680; // private seed functor class (identity tag)
inline constexpr std::uint64_t kUnboundedIntTwinFinalVptr = 0x159b618;   // 0x159b608 + 0x10 (stored by the call site)
inline constexpr std::uint64_t kUnboundedIntTwinDtorRva = 0x230d60;      // final class slot 0 (atexit target)
inline constexpr std::uint64_t kUnboundedIntTwinSlot3Rva = 0x2334c0;     // mov eax,1; ret
inline constexpr std::uint64_t kUnboundedIntTwinSlot6Rva = 0x233510;     // mov eax,1; ret
inline constexpr std::uint32_t kUnboundedIntTwinSlot3Result = 1;
inline constexpr std::uint32_t kUnboundedIntTwinSlot6Result = 1;

// 0x233ec0 for the private class 0x159b680 or any int-family caller functor (anything else is refused with nothing written: the original would
// run an unread clone slot). Leaves the MID vptr in place.
[[nodiscard]] HelperCtorResult helper_233ec0_unbounded_int_twin(ConCmdRangedVar& var, const SharedStr& name,
                                                                std::uint32_t flags, const RangedVarArgs& block,
                                                                const SharedStr& description) noexcept;

// The two virtual-slot results 0x26ea0 reads from a variable of class 0x159b618; `known` only when the vptr is exactly the final class.
[[nodiscard]] DebugMenuTarget unbounded_int_twin_menu_target(const ConCmdRangedVar& var) noexcept;

// The whole call site of slot 506's first variable, in the observed order:
//   [obj] = final ; 0x233ec0 ; f590(description) ; call [argfn + 0x18] ; [obj] = final ; value ; default ; 0x20d60 ; f590(name) ;
//   __cxa_atexit(0x230d60, obj).
[[nodiscard]] HelperIntSiteResult helper_init_unbounded_int_twin_site(ConCmdRangedVar& var, const HelperIntSiteSpec& spec,
                                                                      ConCmdRegistry& registry) noexcept;

}  // namespace sotc
