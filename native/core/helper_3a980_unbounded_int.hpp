// CPU-01-HELPER-CALLEE-MODELS-01: family-admitting wrapper for the unbounded-int constructor 0x3a980
// (0x142 B). The body is `init_unbounded_int_var_worker_3a980` (ctor_slots_322_334.hpp), which admits five
// hand-read tables; this layer admits the whole int functor family (helper_functor_tags.hpp). 0x3a980 is the
// unbounded twin of 0xc580: it stores the seed class 0x1595660 at +0xc0 and then runs the same four-step
// functor install through the caller's clone slot, so the final +0xc0 class is 0x15944a0 for every family tag.
//
// Unblocks (caller tag proven by tools/lift/check_helper_3a980.py): startup slot 555 (0x527f0), tag
// 0x1595e60. FIXTURE_VERIFIED + STATIC_ANALYSIS_VERIFIED; no original RVA is executed.
#pragma once

#include <cstdint>

#include "ctor_slots_322_334.hpp"
#include "helper_c580_ranged_int.hpp"

namespace sotc {

[[nodiscard]] HelperCtorResult helper_3a980_unbounded_int(ConCmdRangedVar& var, const SharedStr& name,
                                                          std::uint32_t flags, const RangedVarArgs& block,
                                                          const SharedStr& description) noexcept;

}  // namespace sotc
