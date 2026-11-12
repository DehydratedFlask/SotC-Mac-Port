#include "ctor_slot_506_2340b0.hpp"

namespace sotc {

Slot506Result init_slot506_2340b0(Slot506Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot506Result out{};
    // 0x23410b..0x2341d0: 0x233ec0 (twin) ; f590(desc) ; argfn destroy ; [obj] = 0x159b618 ; value = default = 0 ; 0x20d60 ; f590(name) ;
    // __cxa_atexit(0x230d60, obj)
    const HelperIntSiteSpec spec0{kSlot506Var0Name, kSlot506Var0NameLength, "", 0, 0, 0, 1, kSlot506Var0FnTag, 0, 0};
    out.var0 = helper_init_unbounded_int_twin_site(objects.var0, spec0, concmd);
    out.refusal = out.var0.refusal;
    if (!out.var0.ok) return out;

    // 0x23427a..0x2342e1: 0x3a980 (family wrapper) ; f590(desc) ; argfn destroy ; [obj] = 0x1594c08 ; value = default = 0 ; 0x20d60 ; f590(name) ;
    // __cxa_atexit(0x18a60, obj)
    const HelperIntSiteSpec spec1{kSlot506Var1Name, kSlot506Var1NameLength, "", 0, 0, 0, 1, kSlot506Var1FnTag, 0, 0};
    out.var1 = helper_init_unbounded_int_var_site(objects.var1, spec1, concmd);
    if (out.refusal == HelperRefusal::none) out.refusal = out.var1.refusal;
    if (!out.var1.ok) return out;

    // 0x2342fc..0x234362: 0x26c10 group (empty parent, caption `Touch Manager`, flags 0x30), f590 x2, atexit 0x272d0
    out.group = family_group_item_26c10(objects.group, "", 0, kSlot506GroupCaption, kSlot506GroupCaptionLength, kSlot506ItemFlags,
                                        concmd, menu);
    if (!out.group.ok()) return out;

    // 0x234362..0x2343de: 0x26ea0 toggle `Draw` under `Touch Manager` -> variable (1)
    out.item = lane_m6::init_toggle_site_target(objects.item, unbounded_int_twin_menu_target(objects.var0), kSlot506GroupCaption,
                                                kSlot506GroupCaptionLength, kSlot506ItemText, kSlot506ItemTextLength, concmd, menu);
    if (!out.item.ok) return out;

    out.ok = out.var0.ok && out.var1.ok && out.group.ok() && out.item.ok;
    return out;
}

}  // namespace sotc
