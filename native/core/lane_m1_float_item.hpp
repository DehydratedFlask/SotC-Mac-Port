// CPU-01-STARTUP-SLOT-364 (lane M1): shared helpers for the unbounded-float-variable + 0x27050 slider-item site of
// startup constructor slot 364 (0x74a8e0). Namespace sotc::lane_m1. Every fact is byte-pinned by
// tools/lift/check_ctor_slot_364.py; NOTHING here executes, loads or calls an original RVA.
//
// Slot 364's 0x27050 slider-item site: 0x27050(item, label, text, target, 0x30, lo, hi); tail 0x268b0; f590(text);
//    f590(label) ; __cxa_atexit(0x272d0, item) -- the same site tail as init_toggle_site_26ea0, with 0x27050.
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc::lane_m1 {

// 0x27050 menu-item site (see above). `target` is the float variable.
[[nodiscard]] MenuSiteResult init_item_27050_site(DebugMenuItem& item, const void* target, const char* label,
                                                  const char* text, std::uint32_t flags, float range_min,
                                                  float range_max, ConCmdRegistry& registry,
                                                  DebugMenuRegistry& menu) noexcept;

}  // namespace sotc::lane_m1
