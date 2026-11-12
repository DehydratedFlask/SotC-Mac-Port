// CPU-01-AOT-CFG-SCALE-01-BATCH17: the post-constructor sequence of the two dword-ranged console-variable call sites of startup slot 451
// (0x49dbad -> 0x49ca60, 0x49df7f -> 0x49cc60), in the observed order. The constructors themselves are helper_49ca60_ranged_dword_var;
// the byte facts of both sites are pinned by tools/lift/check_helper_49ca60.py and tools/lift/check_ctor_slot_451.py.
//
//   0x49ca60/0x49cc60(obj, &name, &obj->value, &lo, &hi, flags, &args, &description) ; call [class + 0x18] (bare ret) ; [obj] = final vptr ;
//   dword [obj+0xd0] = value ; dword [obj+0x68] = default ; 0x20d60 (register) ; f590(description) ; f590(name) ; __cxa_atexit(0x498b20 / 0x498b60, obj)
//
// Nothing here executes or calls an original RVA; the vtable, the functor class and the destructor RVA are identity tags (the destructor is
// recorded in the atexit log and never run). FIXTURE_VERIFIED + STATIC_ANALYSIS_VERIFIED only.
#pragma once

#include <cstddef>
#include <cstdint>

#include "helper_49ca60_ranged_dword_var.hpp"

namespace sotc {

struct HelperDwordSiteSpec {
    RangedDwordVariant variant;
    const char* name;
    std::size_t name_length;
    const char* description;
    std::size_t description_length;
    std::uint32_t flags;
    std::uint64_t arg_fn_tag;       // the call site's REAL argument-functor tag (the constructor's own class A / B)
    std::int32_t min_value;         // dword *rcx
    std::int32_t max_value;         // dword *r8
    std::int32_t value;             // dword [var+0xd0]
    std::int32_t default_value;     // dword [var+0x68]
    std::uint64_t final_vptr;       // [var] after the constructor: a literal lea of the site (class table + 0x10)
    std::uint64_t dtor_rva;         // __cxa_atexit target (identity tag)
};

struct HelperDwordSiteResult {
    bool ok = false;
    HelperRefusal refusal = HelperRefusal::none;
    bool ctor_ok = false;
    bool views_inert = false;       // both 0xf590 calls were exact no-ops (borrowed views)
    ConCmdRegisterResult registration{};
    bool atexit_recorded = false;
};

[[nodiscard]] HelperDwordSiteResult helper_init_ranged_dword_var_site(ConCmdRangedVar& var, const HelperDwordSiteSpec& spec,
                                                                      ConCmdRegistry& registry) noexcept;

}  // namespace sotc
