// CPU-01-HELPER-CALLEE-MODELS-02 (helper-c): native model of the "local ranged-int variable" wrapper family, whose
// members are the 435 byte (FDE 436) constructors 0x620e10 and 0x6211d0 called by startup slot 401 (0x62154f..) and 24 further twins.
// Original ELF sha256 d0723afb...5861d3; every fact is byte-pinned by tools/lift/check_helper_620e10.py. NOTHING here executes, loads or calls an
// original RVA: vtable pointers, functor class tags and destructor RVAs are IDENTITY TAGS, recorded and never run.
//
// The family: 26 wrappers of 0x1b3 B, each tail-composed with an inner constructor of 0x15f B. The checker proves the family claim by byte
// equality, not by a naming convention:
//   * every wrapper equals the hash-pinned wrapper 0x7ec160 (helper_7ec160_enum_var) outside seven rel32 operands (+0x1e stack-guard load,
//     +0x25 final vtable lea, +0x5d functor class lea, +0x158 inner-constructor call, +0x188 call 0x20d60, +0x18f guard reload, +0x1af
//     __stack_chk_fail) and each operand is resolved to the row below;
//   * every inner constructor equals the hash-pinned ranged-int constructor 0xc580 outside six rel32 operands (guard x2, the 0x20af0 call, the
//     mid vtable lea, the functor class lea, the stack-fail call), resolved to the row below;
//   * the four functor bodies of every class equal the int bodies 0xc720 / 0xc740 / 0xc760 / 0xc770 outside the 4 byte class operand (copy and
//     clone store their OWN class constant; move and destroy are identical);
//   * final-class slots 0 / 2 / 3 / 6 are (dtor, 0x20d60, `mov eax,1; ret`, `mov eax,1; ret`), so the debug-menu toggle 0x26ea0 / 0x26db0
//     reads slot3 == 1 and slot6 == 1 (the `slot6 == 3` float narrowing never applies; range stays {1.0, 10.0}).
// So each member is the 0xc580 ranged-int object with a private class pair; the model reuses init_ranged_int_var_c580 and re-tags the cells
// exactly as helper_7ec160_enum_var does:
//   wrapper: [this] = final vptr (pre-store) ; inner(this, name, this + 0xd0, &min, &max, flags, lifted args, description) ; [this] = mid vptr ;
//   [this+0xc0] = fn class ; destroy of the stack functor ; [this] = final vptr ; [this+0xd0] = [this+0x68] = *init ; 0x20d60(this).
// The caller tag is read only to select `[tag + 8]` (clone) and the final `call [tag + 0x18]` (destroy). Admitted tags: the row's OWN functor
// class (slot 401: tags 0x15cc318 and 0x15cc3b0, pinned by the checker) and a CALLER TWIN of that row's class (list below). A caller twin is a
// class-table ALIAS: a different .data.rel.ro table whose four slots point at exactly the same four bodies as the wrapper's own class (slots 398 /
// 399 pass 0x15cc508 / 0x15cc4a8, whose slots are 0x529100 / 0x529120 / 0x529140 / 0x529150, the slots of the wrapper 0x529160's own class
// 0x15c8268). Every virtual call through the twin therefore runs the same bodies as through the own class, so the net effect on the object is
// identical. check_helper_620e10.py proves the slot equality from the ELF. Any other tag (the shared int family, whose clone stores 0x15944a0, or a
// neighbouring wrapper's class) is refused with nothing written: the list is evidence-based, not a class-shape guess.
//
// Unblocks (call-site facts pinned by the checker): startup slot 401 (0x62154f), two variables `cp10_phaseBlendUse` (init 0, range [0,2], flags 1,
// object 0x300d0c0, wrapper 0x620e10, atexit 0x6112e0) and `cp10_phaseBlendTypeOverride` (init 0, range [1,4], flags 1, object 0x300d198, wrapper
// 0x6211d0, atexit 0x611320). The other 24 rows are table facts only; their callers are separate slots.
//
// NOT established: what the variables mean at run time (INFERENCE only). The wrapper's own functor re-wrap is modelled by its net effect
// (clone / copy / copy / copy / swap aux / destroy -> {payload, own class, aux}); that net effect is proven by the checker's body equalities,
// not by running anything. FIXTURE_VERIFIED + STATIC_ANALYSIS_VERIFIED; no boot, audio or save claim.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "debug_menu_item.hpp"
#include "helper_int_var_sites.hpp"

namespace sotc {

inline constexpr std::size_t kLocalIntVarWrapperSize = 0x1b3;
inline constexpr std::size_t kLocalIntVarInnerSize = 0x15f;
inline constexpr std::size_t kLocalIntVarClassCount = 26;
// Caller-twin class tables: {twin tag, the wrapper class whose four slot bodies it aliases} (sorted by tag).
struct LocalIntCallerTwin {
    std::uint64_t tag;
    std::uint64_t aliased_class;
};
inline constexpr std::array<LocalIntCallerTwin, 2> kLocalIntCallerTwins{{{0x15cc4a8, 0x15c8268}, {0x15cc508, 0x15c8268}}};   // slots 399, 398 -> wrapper 0x529160
// True when `tag` is a twin of the class `own_class`.
[[nodiscard]] bool is_local_int_caller_twin_tag(std::uint64_t tag, std::uint64_t own_class) noexcept;
inline constexpr std::uint32_t kLocalIntVarSlot3Result = 1;      // every row: slot 3 = `mov eax,1; ret`
inline constexpr std::uint32_t kLocalIntVarSlot6Result = 1;      // every row: slot 6 = `mov eax,1; ret`

// One member of the family. All fields are original RVAs / vtable addresses used as IDENTITY TAGS only.
struct LocalIntVarClass {
    std::uint64_t wrapper_rva;     // the 0x1b3 byte constructor the call site calls
    std::uint64_t inner_rva;       // the 0xc580 twin it tail-calls
    std::uint64_t mid_vptr;        // [this] while the inner constructor ran (table + 0x10)
    std::uint64_t fn_vtbl;         // functor class table: the stored [this+0xc0] and the own-class caller tag
    std::uint64_t final_vptr;      // [this] after the wrapper (table + 0x10)
    std::uint64_t dtor_rva;        // final class slot 0, the atexit target
    std::uint64_t slot3_rva;       // `mov eax,1; ret`
    std::uint64_t slot6_rva;       // `mov eax,1; ret`
};

inline constexpr std::array<LocalIntVarClass, kLocalIntVarClassCount> kLocalIntVarClasses{{
    {0x1cbe60, 0x1cc020, 0x1599058, 0x1599118, 0x1599148, 0x1c4f40, 0x1c8ac0, 0x1c8d40},
    {0x261b50, 0x261d10, 0x159c628, 0x159c7d8, 0x159c808, 0x253440, 0x25e3e0, 0x25e660},
    {0x261f10, 0x2620d0, 0x159c6a0, 0x159c870, 0x159c8a0, 0x253480, 0x25ef50, 0x25f220},
    {0x4c8f40, 0x4c9100, 0x15c1488, 0x15c1510, 0x15c1540, 0x4c45c0, 0x4c7f90, 0x4c82e0},
    {0x528da0, 0x528f60, 0x15c8148, 0x15c81b0, 0x15c81e0, 0x526040, 0x528320, 0x528370},
    {0x529160, 0x529320, 0x15c80d0, 0x15c8268, 0x15c8298, 0x526000, 0x5276f0, 0x527990},
    {0x5296d0, 0x529890, 0x15c8058, 0x15c8320, 0x15c8350, 0x525fc0, 0x526c70, 0x526cc0},
    {0x529a90, 0x529c50, 0x15c7fe0, 0x15c83b8, 0x15c83e8, 0x525f80, 0x526100, 0x526380},
    {0x56a9b0, 0x56ab70, 0x15ca690, 0x15ca828, 0x15ca858, 0x560460, 0x568660, 0x5688e0},
    {0x56ad70, 0x56af30, 0x15ca708, 0x15ca8c0, 0x15ca8f0, 0x5604a0, 0x5691d0, 0x569350},
    {0x56b150, 0x56b310, 0x15ca780, 0x15ca958, 0x15ca988, 0x560500, 0x569bc0, 0x569d40},
    {0x620e10, 0x620fd0, 0x15cc218, 0x15cc318, 0x15cc348, 0x6112e0, 0x61d820, 0x61d870},
    {0x6211d0, 0x621390, 0x15cc290, 0x15cc3b0, 0x15cc3e0, 0x611320, 0x61e2a0, 0x61e540},
    {0x720430, 0x7205f0, 0x15cce60, 0x15ccf60, 0x15ccf90, 0x70e150, 0x71c200, 0x71c250},
    {0x7207f0, 0x7209b0, 0x15cced8, 0x15cd038, 0x15cd068, 0x70e190, 0x71cc80, 0x71ccd0},
    {0x751190, 0x751350, 0x15cd270, 0x15cd3c8, 0x15cd3f8, 0x74aae0, 0x74e890, 0x74eb90},
    {0x751550, 0x751710, 0x15cd2e8, 0x15cd460, 0x15cd490, 0x74ab20, 0x74f670, 0x74f9f0},
    {0x751910, 0x751ad0, 0x15cd360, 0x15cd4f8, 0x15cd528, 0x74ab60, 0x750700, 0x750880},
    {0x7ec160, 0x7ec320, 0x15ce4d0, 0x15ce538, 0x15ce568, 0x7ea160, 0x7eb570, 0x7eb7f0},
    {0x829b30, 0x829cf0, 0x15ce9c8, 0x15ceae8, 0x15ceb18, 0x81be40, 0x825f40, 0x8261e0},
    {0xa324c0, 0xa32680, 0x15d1120, 0x15d1188, 0x15d11b8, 0xa2c850, 0xa31d90, 0xa31f10},
    {0xa4f4d0, 0xa4f690, 0x15d1a20, 0x15d1b78, 0x15d1ba8, 0xa47d20, 0xa4d490, 0xa4d760},
    {0xa4fec0, 0xa50080, 0x15d1a98, 0x15d1c30, 0x15d1c60, 0xa48240, 0xa4e190, 0xa4e310},
    {0xa50280, 0xa50440, 0x15d1b10, 0x15d1cc8, 0x15d1cf8, 0xa4ab50, 0xa4e830, 0xa4e9b0},
    {0xa624a0, 0xa62660, 0x15d1ea0, 0x15d1fa8, 0x15d1fd8, 0xa5bea0, 0xa60ea0, 0xa60ef0},
    {0xa62860, 0xa62a20, 0x15d1f18, 0x15d2080, 0x15d20b0, 0xa5bee0, 0xa61940, 0xa61990},
}};

inline constexpr std::uint64_t kLocalIntWrapperPhaseBlendUse = 0x620e10;
inline constexpr std::uint64_t kLocalIntWrapperPhaseBlendType = 0x6211d0;

// nullptr when `wrapper_rva` / `final_vptr` is not a family member.
[[nodiscard]] const LocalIntVarClass* find_local_int_var_class(std::uint64_t wrapper_rva) noexcept;
[[nodiscard]] const LocalIntVarClass* find_local_int_var_class_by_final(std::uint64_t final_vptr) noexcept;

// The wrapper up to and including the 0x20d60 registration. `unadmitted_arg_functor` (nothing written) for a tag that is neither the row's own class nor a caller twin,
// `wrong_helper` (nothing written) for an RVA that is not a family member.
struct LocalIntVarCtorResult {
    bool ok = false;
    HelperRefusal refusal = HelperRefusal::none;
    bool arg_destroy_ok = false;      // `call [own class + 0x18]` is the bare-ret functor destroy
    ConCmdRegisterResult registration{};
};

[[nodiscard]] LocalIntVarCtorResult helper_local_int_var_ctor(std::uint64_t wrapper_rva, ConCmdRangedVar& var,
                                                              const SharedStr& name, std::int32_t initial,
                                                              std::int32_t min_value, std::int32_t max_value,
                                                              std::uint32_t flags, const RangedVarArgs& args,
                                                              const SharedStr& description,
                                                              ConCmdRegistry& registry) noexcept;

// The two virtual-slot results the debug-menu toggles read; `known` only when the vptr is exactly the final class of some family member.
[[nodiscard]] DebugMenuTarget local_int_var_menu_target(const ConCmdRangedVar& var) noexcept;

// The 0x26db0 named menu item over a family variable. `unknown_target_class` (item untouched) when the variable is not of a family final class.
[[nodiscard]] HelperRefusal helper_26db0_named_item_local_int(DebugMenuItem& item, const SharedStr& parent,
                                                              const ConCmdRangedVar& target,
                                                              std::uint32_t flags) noexcept;

// A whole call site in the observed order of slot 401:
//   wrapper ; call [own class + 0x18] ; f590(name) ; __cxa_atexit(dtor, obj)
// The description is the caller's argument (slot 401 passes the static empty string 0xf730) and is NOT released here (slot 401 never releases it).
// With `release_description` the 7ec160 order is used instead: wrapper ; f590(description) ; destroy ; f590(name) ; atexit.
struct LocalIntSiteSpec {
    std::uint64_t wrapper_rva;
    const char* name;
    std::size_t name_length;
    std::int32_t value;               // *init: stored into [var+0xd0] and [var+0x68]
    std::int32_t min_value;
    std::int32_t max_value;
    std::uint32_t flags;
    std::uint64_t arg_fn_tag;         // the call site's REAL argument-functor tag
    bool release_description;
};

[[nodiscard]] HelperIntSiteResult helper_init_local_int_var_site(ConCmdRangedVar& var, const LocalIntSiteSpec& spec,
                                                                 const SharedStr& description,
                                                                 ConCmdRegistry& registry) noexcept;

// The two observed sites of startup slot 401 (names are the ELF strings at 0x126c259 / 0x126c26c).
inline constexpr LocalIntSiteSpec kSlot401PhaseBlendUseSite{kLocalIntWrapperPhaseBlendUse, "cp10_phaseBlendUse", 18, 0, 0, 2, 1, 0x15cc318, false};
inline constexpr LocalIntSiteSpec kSlot401PhaseBlendTypeSite{kLocalIntWrapperPhaseBlendType, "cp10_phaseBlendTypeOverride", 27, 0, 1, 4, 1, 0x15cc3b0, false};
inline constexpr std::uint64_t kSlot401PhaseBlendUseObject = 0x300d0c0;     // static object address (identity only)
inline constexpr std::uint64_t kSlot401PhaseBlendTypeObject = 0x300d198;

}  // namespace sotc
