// CPU-01-STARTUP-SLOTS-LANE-M6: shared construction helpers for the ten startup constructor slots 544, 557, 558,
// 559, 569, 448, 379, 518, 507 and 539. Original ELF sha256 d0723afb...5861d3. Namespace sotc::lane_m6.
//
// Everything here is a thin, honest composition of shapes that were already lifted by earlier lanes:
//   * bool variable          core/concmd_vars        init_bool_var_static
//   * bounded float variable core/ctor_shapes_343_356 init_float_var_static  (0xc300)
//   * ranged int variable    core/ranged_int_var_c580 init_ranged_int_var_c580 (0xc580)
//   * debug-menu toggle      core/debug_menu_item    init_debug_menu_item_26ea0 / register_debug_menu_item_268b0
// NOTHING here executes, loads or calls an original RVA. Vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS (the original addresses kept as numbers) and are recorded, never run.
//
// New facts, each byte-pinned by the slot checkers (tools/lift/check_ctor_slot_<idx>.py, with the helper
// tools/lift/lane_m6_lib.py):
//   * Three caller argument-functor tags that the earlier lanes' accept lists do not contain are PROVEN
//     EQUIVALENT by the checkers: the tag's four relocation slots are the same four bodies as an accepted tag
//       float 0x15cc898 (slot 379) and 0x1598a50 (slot 518): 0xc4a0 / 0xc4c0 / 0xc4e0 / 0xc4f0 (the float bodies)
//       int   0x1594c70 (slot 569): 0xc720 / 0xc740 / 0xc760 / 0xc770 (the int bodies)
//     The constructors read the caller tag only to select those four bodies and never store it (the object
//     receives the class tag from the copy/clone bodies), so the helpers below validate the REAL tag against the
//     list above and then hand the equivalent accepted tag to the shared shape. An unknown tag is refused and
//     nothing is written.
//   * The float variable class (final vptr 0x15942b0) answers the two virtual reads of the debug-menu toggle
//     0x26ea0 with slot 3 = 0xbba0 (`mov eax,1; ret`) and slot 6 = 0xbbf0 (`mov eax,3; ret`): slot3 == 1 &&
//     slot6 == 3, so the toggle narrows its range to {0.1, 0.5} (checker-pinned relocations and bodies).
#pragma once

#include <cstddef>
#include <cstdint>

#include "ctor_shapes_343_356.hpp"
#include "l2_slot_common.hpp"
#include "ranged_int_var_c580.hpp"

namespace sotc::lane_m6 {

// ---- caller argument-functor tags proven equivalent to an accepted tag ---------------------------------------
inline constexpr std::uint64_t kFloatSiteTag_379 = 0x15cc898;
inline constexpr std::uint64_t kFloatSiteTag_518 = 0x1598a50;
inline constexpr std::uint64_t kIntSiteTag_569 = 0x1594c70;

[[nodiscard]] bool is_lane_m6_float_site_tag(std::uint64_t tag) noexcept;
[[nodiscard]] bool is_lane_m6_int_site_tag(std::uint64_t tag) noexcept;

// ---- bool variable with explicit name / description lengths ---------------------------------------------------
// The original builds both strings as stack views {literal, immediate length, 0, 0} (control == nullptr), so the
// two 0xf590 releases are exact no-ops. `description` may be empty (length 0) as at slots 559 / 507 / 539.
[[nodiscard]] BoolVarResult init_bool_var_literal(ConCmdBoolVar& var, const char* name, std::size_t name_length,
                                                  const char* description, std::size_t description_length,
                                                  std::uint64_t fn_tag, ConCmdRegistry& registry,
                                                  std::uint8_t value, std::uint8_t default_value) noexcept;

// ---- bounded float variable whose caller functor tag is a lane-m6 tag -----------------------------------------
// `spec.arg_fn_vtbl` must be one of kFloatSiteTag_379 / kFloatSiteTag_518; otherwise nothing is written and
// ctor_ok == false. The rest is init_float_var_static unchanged.
[[nodiscard]] FloatVarResult init_float_var_site(ConCmdRangedVar& var, const FloatVarSpec& spec,
                                                 ConCmdRegistry& registry) noexcept;

// What 0x26ea0 reads from a variable of the float class 0x15942b0 (slot 3 -> 1, slot 6 -> 3).
inline constexpr std::uint64_t kRangedFloatSlot3Rva = 0xbba0;   // mov eax,1; ret
inline constexpr std::uint64_t kRangedFloatSlot6Rva = 0xbbf0;   // mov eax,3; ret
inline constexpr std::uint32_t kRangedFloatSlot3Result = 1;
inline constexpr std::uint32_t kRangedFloatSlot6Result = 3;
[[nodiscard]] DebugMenuTarget ranged_float_menu_target(const ConCmdRangedVar& var) noexcept;

// ---- ranged int variable (0xc580) static initialiser -----------------------------------------------------------
struct IntVarSiteSpec {
    const char* name;
    std::size_t name_length;
    const char* description;
    std::size_t description_length;
    std::int32_t min_value;
    std::int32_t max_value;
    std::uint32_t flags;
    std::uint64_t arg_fn_vtbl;        // the caller's REAL argument-functor tag (kIntSiteTag_569)
    std::int32_t value;               // dword [var+0xd0]
    std::int32_t default_value;       // dword [var+0x68]
};

struct IntVarResult {
    bool ok = false;
    bool ctor_ok = false;             // lane-m6 tag accepted, 0xc580 stores written
    bool views_inert = false;         // both 0xf590 calls were exact no-ops
    bool functor_destroy_ok = false;  // [argfn + 0x18] is the bare-ret destroy slot 0xc770
    ConCmdRegisterResult registration{};
    bool atexit_recorded = false;     // dtor 0x18a60 recorded, not run
};

// 0xc580 ; f590(description) ; argfn destroy slot ; [obj]=0x1594c08 ; value ; default ; 0x20d60 ;
// f590(name) ; __cxa_atexit(0x18a60, obj).
[[nodiscard]] IntVarResult init_int_var_site(ConCmdRangedVar& var, const IntVarSiteSpec& spec,
                                             ConCmdRegistry& registry) noexcept;

// ---- 0x26ea0 debug-menu toggle with an arbitrary lifted target ---------------------------------------------------
// ctor(label, text, target, 0x30) ; (tail) 0x268b0 ; f590(text) ; f590(label) ; __cxa_atexit(0x272d0, item).
[[nodiscard]] MenuSiteResult init_toggle_site_target(DebugMenuItem& item, const DebugMenuTarget& target,
                                                     const char* label, std::size_t label_length, const char* text,
                                                     std::size_t text_length, ConCmdRegistry& registry,
                                                     DebugMenuRegistry& menu) noexcept;

}  // namespace sotc::lane_m6
