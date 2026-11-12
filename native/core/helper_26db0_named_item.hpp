// CPU-01-HELPER-CALLEE-MODELS-01: class-aware front end for the debug-menu "named item" constructor 0x26db0
// (0xef B, [0x26db0, 0x26e9f)). The body is `init_debug_menu_item_26db0` (menu_item_26db0.hpp), which takes the
// target's two virtual-slot results as a `DebugMenuTarget`; the older `init_menu_toggle_from_var_26db0` only
// serves the bool class. 0x26db0 itself is class-agnostic: the caption is the target's own name view and the two
// `call [vptr+0x18/0x30]` select the range. This header names the targets the lifted classes provide:
//
//   bool variable   class 0x1594218 slot3 1, slot6 0 -> range {1.0, 10.0}
//   int variable    class 0x1594c08 slot3 1, slot6 2 -> {1.0, 10.0}
//   float variable  class 0x15942b0 slot3 1, slot6 3 -> {0.1, 0.5} (the one narrowing case)
//   string variable class 0x1594ef0 slot3 1, slot6 4 -> {1.0, 10.0}
//
// Targets of any other class are refused with the item untouched. Registration (0x268b0) stays separate, as in
// the original tail jump. Slots 366/411 (`0x26db0` x2, bool targets) need no new constructor logic; this adapter
// only lets a debug-page slot name the target class explicitly. FIXTURE_VERIFIED + STATIC_ANALYSIS_VERIFIED.
#pragma once

#include <cstdint>

#include "concmd_vars.hpp"
#include "helper_functor_tags.hpp"
#include "lane_m6_shapes.hpp"
#include "menu_item_26db0.hpp"
#include "string_var_257a0.hpp"

namespace sotc {

[[nodiscard]] HelperRefusal helper_26db0_named_item_bool(DebugMenuItem& item, const SharedStr& parent,
                                                         const ConCmdBoolVar& target, std::uint32_t flags) noexcept;
[[nodiscard]] HelperRefusal helper_26db0_named_item_int(DebugMenuItem& item, const SharedStr& parent,
                                                        const ConCmdRangedVar& target, std::uint32_t flags) noexcept;
[[nodiscard]] HelperRefusal helper_26db0_named_item_float(DebugMenuItem& item, const SharedStr& parent,
                                                          const ConCmdRangedVar& target,
                                                          std::uint32_t flags) noexcept;
[[nodiscard]] HelperRefusal helper_26db0_named_item_string(DebugMenuItem& item, const SharedStr& parent,
                                                           const ConCmdStringVar& target,
                                                           std::uint32_t flags) noexcept;

}  // namespace sotc
