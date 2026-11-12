#include "ctor_slot_369_734190.hpp"

#include <cstring>

namespace sotc {

Slot369Result init_slot369_734190(Slot369Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot369Result out{};
    // 0x7341a8..0x7342c5: bool variable (static shape), value = default = 1
    out.var0 = lane_m6::init_bool_var_literal(objects.var0, kSlot369Var0Name, kSlot369Var0NameLength, "", 0,
                                              kSlot369BoolFnTag, concmd, 1, 1);
    if (!out.var0.ok) return out;
    // 0x7342ca..0x73434e: 0x26ea0 toggle for it (parent, caption), flags 0x30, 0x268b0, f590 x2, atexit 0x272d0
    out.item0 = lane_m6::init_toggle_site_target(objects.item0, debug_menu_target_for(objects.var0), kSlot369Parent,
                                                 kSlot369ParentLength, kSlot369Item0Text, kSlot369Item0TextLength, concmd,
                                                 menu);
    if (!out.item0.ok) return out;
    // 0x73434e..0x734450: 0x6aa00 (family wrapper), early description release, value = default
    const HelperFloatSiteSpec spec{kSlot369Var1Name, kSlot369Var1NameLength, "", 0, 1, kSlot369FloatFnTag,
                                   kSlot369Var1ValueBits, true};
    out.var1 = helper_init_unbounded_float_var_site(objects.var1, spec, concmd);
    out.refusal = out.var1.refusal;
    if (!out.var1.ok) return out;
    // 0x734455..0x7344e4: 0x27050 slider (target = the float variable), 0x268b0, f590(text), f590(label), atexit 0x272d0
    const SharedStr label = l2::measured_view(kSlot369Parent, kSlot369ParentLength);
    const SharedStr text = l2::measured_view(kSlot369Item1Text, kSlot369Item1TextLength);
    float lo = 0.0f;
    float hi = 0.0f;
    const std::uint32_t lo_bits = kSlot369Item1MinBits;
    const std::uint32_t hi_bits = kSlot369Item1MaxBits;
    std::memcpy(&lo, &lo_bits, sizeof lo);
    std::memcpy(&hi, &hi_bits, sizeof hi);
    out.item1.ctor_ok = init_debug_menu_item_27050(objects.item1, label, text, &objects.var1, kSlot369ItemFlags, lo, hi);
    if (!out.item1.ctor_ok) return out;
    out.item1.registration = register_debug_menu_item_268b0(objects.item1, menu, concmd.atexit_log);
    out.item1.views_inert = release_inert_view_f590(text) && release_inert_view_f590(label);   // text first, then label
    out.item1.atexit_recorded = concmd.atexit_log.record(kDebugMenuItemDtorRva, &objects.item1);
    out.item1.ok = out.item1.ctor_ok && out.item1.registration.ok && out.item1.views_inert && out.item1.atexit_recorded;
    out.ok = out.var0.ok && out.item0.ok && out.var1.ok && out.item1.ok;
    return out;
}

}  // namespace sotc
