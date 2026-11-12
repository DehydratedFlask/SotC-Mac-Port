// CPU-01-HELPER-CALLEE-MODELS-01: family-admitting wrappers for the float console-variable constructors
// 0x6aa00 (unbounded, 0x142 B) and 0xc300 (bounded, 0x160 B), plus the whole float static-initialiser block
// (`init_float_var_static_family`). The bodies are `init_unbounded_float_var_6aa00` /
// `init_bounded_float_var_c300` (ctor_shapes_343_356.hpp), which admit eight hand-read functor tags; this layer
// admits the full ELF-derived float family (68 class tables whose slots are 0xc4a0/0xc4c0/0xc4e0/0xc4f0, all
// storing class 0x1594318 at the destination), so the stored class is independent of the caller tag.
//
// Unblocks (caller tag proven by tools/lift/check_helper_6aa00.py): startup slot 504 (0x23f4d0), tag 0x159ba68.
// FIXTURE_VERIFIED + STATIC_ANALYSIS_VERIFIED; no original RVA is executed.
#pragma once

#include <cstdint>

#include "ctor_shapes_343_356.hpp"
#include "helper_c580_ranged_int.hpp"

namespace sotc {

[[nodiscard]] HelperCtorResult helper_6aa00_unbounded_float(ConCmdRangedVar& var, const SharedStr& name,
                                                            void* value_ref, std::uint32_t flags,
                                                            const RangedVarArgs& args,
                                                            const SharedStr& description) noexcept;

[[nodiscard]] HelperCtorResult helper_c300_bounded_float(ConCmdRangedVar& var, const SharedStr& name,
                                                         void* value_ref, float min_value, float max_value,
                                                         std::uint32_t flags, const RangedVarArgs& args,
                                                         const SharedStr& description) noexcept;

// The static-initialiser block of one float variable (same sequence and result as `init_float_var_static`) with
// the family-wide functor set. `spec.arg_fn_vtbl` is the caller tag. Nothing is written for a tag outside the
// float family (result.ctor_ok == false).
[[nodiscard]] FloatVarResult init_float_var_static_family(ConCmdRangedVar& var, const FloatVarSpec& spec,
                                                          ConCmdRegistry& registry) noexcept;

}  // namespace sotc
