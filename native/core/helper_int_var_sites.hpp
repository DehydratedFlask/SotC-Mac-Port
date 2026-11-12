// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 6): int-variable call sites that go through the family-admitting helper
// wrappers (helper_c580_ranged_int / helper_3a980_unbounded_int) instead of a private tag whitelist. Each function is
// the post-constructor sequence of one observed site, in the observed order, and reports the wrapper's HelperRefusal
// so a slot can prove that ITS tag was admitted (the per-slot checker pins the tag and the call site).
//
//   ranged (slot 430 order)    0xc580 ; call [argfn+0x18] ; [obj]=0x1594c08 ; value ; default ; 0x20d60 ;
//                              f590(description) ; f590(name) ; __cxa_atexit(0x18a60, obj)
//   unbounded (slot 555 order) 0x3a980 ; f590(description) ; call [argfn+0x18] ; [obj]=0x1594c08 ; value ; default ;
//                              0x20d60 ; f590(name) ; __cxa_atexit(0x18a60, obj)
//   unbounded, late order      0x3a980 ; call [argfn+0x18] ; [obj]=0x1594c08 ; value ; default ; 0x20d60 ;
//                              f590(description) ; f590(name) ; __cxa_atexit(0x18a60, obj)   (spec.early_description_release = false)
//
//   unbounded float (slots 397 / 442 / 450 / 460) 0x6aa00 through helper_6aa00_unbounded_float, then either
//     early order   f590(description) ; call [argfn+0x18] ; vptr ; value ; default ; 0x20d60 ; f590(name) ; atexit 0x20f0
//     late order    call [argfn+0x18] ; vptr ; value ; default ; 0x20d60 ; f590(description) ; f590(name) ; atexit 0x20f0
//
// Nothing here executes or calls an original RVA; vtables, functor tags and destructor RVAs are identity tags.
// FIXTURE_VERIFIED + STATIC_ANALYSIS_VERIFIED only.
#pragma once

#include <cstddef>
#include <cstdint>

#include "helper_3a980_unbounded_int.hpp"
#include "helper_6aa00_unbounded_float.hpp"
#include "helper_c580_ranged_int.hpp"

namespace sotc {

struct HelperIntSiteSpec {
    const char* name;
    std::size_t name_length;
    const char* description;
    std::size_t description_length;
    std::int32_t min_value;           // ranged only
    std::int32_t max_value;           // ranged only
    std::uint32_t flags;
    std::uint64_t arg_fn_tag;         // the call site's REAL argument-functor tag
    std::int32_t value;               // dword [var+0xd0]
    std::int32_t default_value;       // dword [var+0x68]
    bool early_description_release = true;   // unbounded only: false = late order (0x20d60 ; f590(description) ; f590(name)), slot 451
};

struct HelperIntSiteResult {
    bool ok = false;
    HelperRefusal refusal = HelperRefusal::none;
    bool ctor_ok = false;
    bool functor_destroy_ok = false;  // `call [argfn + 0x18]` is the bare-ret destroy
    bool views_inert = false;         // both 0xf590 calls were exact no-ops (borrowed views)
    ConCmdRegisterResult registration{};
    bool atexit_recorded = false;     // dtor 0x18a60 recorded, not run
};

[[nodiscard]] HelperIntSiteResult helper_init_ranged_int_var_site(ConCmdRangedVar& var, const HelperIntSiteSpec& spec,
                                                                  ConCmdRegistry& registry) noexcept;
[[nodiscard]] HelperIntSiteResult helper_init_unbounded_int_var_site(ConCmdRangedVar& var,
                                                                     const HelperIntSiteSpec& spec,
                                                                     ConCmdRegistry& registry) noexcept;

struct HelperFloatSiteSpec {
    const char* name;
    std::size_t name_length;
    const char* description;
    std::size_t description_length;
    std::uint32_t flags;
    std::uint64_t arg_fn_tag;         // the call site's REAL argument-functor tag (float family)
    std::uint32_t value_bits;         // IEEE bits stored at [var+0xd0] and [var+0x68]
    bool early_description_release;   // true: f590(description) right after 0x6aa00 (slots 397 / 450 / 460)
};

using HelperFloatSiteResult = HelperIntSiteResult;   // same fields; the atexit record is dtor 0x20f0

[[nodiscard]] HelperFloatSiteResult helper_init_unbounded_float_var_site(ConCmdRangedVar& var,
                                                                         const HelperFloatSiteSpec& spec,
                                                                         ConCmdRegistry& registry) noexcept;

}  // namespace sotc
