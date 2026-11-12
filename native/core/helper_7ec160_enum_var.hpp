// CPU-01-STARTUP-BATCH-7: native model of the local ranged "enum" console-variable constructor 0x7ec160 (0x1b3 B, [0x7ec160, 0x7ec313)) and its
// inner constructor 0x7ec320 (0x15f B), used by startup constructor slot 336 (0x7ec4c0) only. Original ELF sha256 d0723afb...5861d3; every fact
// is byte-pinned by tools/lift/check_helper_7ec160.py. NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and
// destructor RVAs are IDENTITY TAGS, recorded and never run.
//
// 0x7ec320 is the ranged-int constructor 0xc580 with a different class pair: the 351 byte bodies are IDENTICAL except for six rel32 operands
// (the two stack-guard loads, the 0x20af0 call, the mid vtable lea, the functor class lea and the stack-fail call), which the checker pins one by
// one. So the layout is the 0xd8 byte ConCmdRangedVar of ranged_int_var_c580.hpp and the stores are init_ranged_int_var_c580's, with
//   mid vptr    [this] = 0x15ce4c0 + 0x10 = 0x15ce4d0   (0xc580: 0x15941a0)
//   fn class    [this+0xc0] = 0x15ce538                 (0xc580: 0x15944a0); the table's four slots 0x7ec100 / 0x7ec120 / 0x7ec140 / 0x7ec150 are the
//                                                          int bodies 0xc720..0xc770 with the class constant 0x15ce538 (bare-ret destroy).
// 0x7ec160 is the wrapper the call site uses: it pre-stores the FINAL vptr 0x15ce558 + 0x10 = 0x15ce568, passes `this + 0xd0` as the value
// reference (the variable's own value cell, unlike 0xc580 whose call sites pass an adjacent global), re-wraps the caller's functor through the
// class 0x15ce538 (clone / copy / copy / copy / swap aux / destroy; the net effect on the object is {payload, class 0x15ce538, aux} exactly as
// in 0xc580), stores the final vptr again, copies *init into [this+0xd0] and [this+0x68], and registers through 0x20d60. The wrapper reads the
// caller's tag only to select `[tag + 8]`; the only lifted tag is 0x15ce538 itself, any other tag is refused with nothing written.
//
// Final class 0x15ce568: dtor 0x7ea160 (slot 0), 0x20d60 (slot 2), slot 3 = 0x7eb570 (`mov eax,1; ret`), slot 6 = 0x7eb7f0 (`mov eax,1; ret`). So
// the debug-menu toggle 0x26ea0 reads slot3 == 1 and slot6 == 1: the `slot6 == 3` narrowing does not apply and the {1.0, 10.0} default stays.
//
// NOT established: what the enum value means at run time (INFERENCE only). The wrapper's own functor re-wrap is modelled by its net effect.
#pragma once

#include <cstddef>
#include <cstdint>

#include "debug_menu_item.hpp"
#include "helper_int_var_sites.hpp"

namespace sotc {

inline constexpr std::uint64_t kEnumVarWrapperRva = 0x7ec160;
inline constexpr std::size_t kEnumVarWrapperSize = 0x1b3;
inline constexpr std::uint64_t kEnumVarCtorRva = 0x7ec320;
inline constexpr std::size_t kEnumVarCtorSize = 0x15f;
inline constexpr std::uint64_t kEnumVarMidVptr = 0x15ce4d0;       // 0x15ce4c0 + 0x10
inline constexpr std::uint64_t kEnumVarFinalVptr = 0x15ce568;     // 0x15ce558 + 0x10 (stored by the wrapper)
inline constexpr std::uint64_t kEnumVarFnVtbl = 0x15ce538;        // functor class tag (also the only admitted caller tag)
inline constexpr std::uint64_t kEnumVarDtorRva = 0x7ea160;        // final class slot 0 (atexit target)
inline constexpr std::uint64_t kEnumVarSlot3Rva = 0x7eb570;       // mov eax,1; ret
inline constexpr std::uint64_t kEnumVarSlot6Rva = 0x7eb7f0;       // mov eax,1; ret
inline constexpr std::uint32_t kEnumVarSlot3Result = 1;
inline constexpr std::uint32_t kEnumVarSlot6Result = 1;

// 0x7ec160 up to and including the 0x20d60 registration. `refusal == unadmitted_arg_functor` (nothing written) for a tag other than 0x15ce538.
struct EnumVarCtorResult {
    bool ok = false;
    HelperRefusal refusal = HelperRefusal::none;
    bool arg_destroy_ok = false;      // `call [0x15ce538 + 0x18]` is the bare-ret 0x7ec150
    ConCmdRegisterResult registration{};
};

[[nodiscard]] EnumVarCtorResult helper_7ec160_enum_var(ConCmdRangedVar& var, const SharedStr& name, std::int32_t initial,
                                                       std::int32_t min_value, std::int32_t max_value, std::uint32_t flags,
                                                       const RangedVarArgs& args, const SharedStr& description,
                                                       ConCmdRegistry& registry) noexcept;

// The two virtual-slot results 0x26ea0 reads from a variable of class 0x15ce568; `known` only when the vptr is exactly the final class.
[[nodiscard]] DebugMenuTarget enum_var_menu_target(const ConCmdRangedVar& var) noexcept;

// The whole call site of slot 336's first variable, in the observed order:
//   0x7ec160 ; f590(description) ; call [argfn + 0x18] ; f590(name) ; __cxa_atexit(0x7ea160, obj).
// `spec.value` is the initial value, `spec.default_value` is ignored (the wrapper stores *init into both cells).
[[nodiscard]] HelperIntSiteResult helper_init_enum_var_site(ConCmdRangedVar& var, const HelperIntSiteSpec& spec,
                                                            ConCmdRegistry& registry) noexcept;

}  // namespace sotc
