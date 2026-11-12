#include "ctor_slot_488_2ba5d0.hpp"

#include <cstring>

namespace sotc {

Slot488Result init_slot488_2ba5d0(Slot488Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot488Result out{};

    // 0x2ba5f6..0x2ba722: 0xc580 (family wrapper) ; f590(desc) ; argfn destroy ; vptr ; 0 / 0 ; 0x20d60 ; f590(name) ; atexit 0x18a60
    const HelperIntSiteSpec spec0{kSlot488Var0Name, kSlot488Var0NameLength, "", 0, kSlot488Var0Min, kSlot488Var0Max, 1,
                                  kSlot488IntFnTag, 0, 0};
    out.var0 = helper_init_ranged_int_var_site(objects.var0, spec0, concmd);
    if (!out.var0.ok) return out;

    // 0x2ba727..0x2ba7a4: holder = {0x15a0a48, view(ptr, 0x12), 0x18 zero bytes} ; f590(temp view) ; __cxa_atexit(0x2b5050, holder)
    const SharedStr text = lane_m4::borrowed_view_of(kSlot488HolderText, kSlot488HolderTextLength);
    objects.holder.vptr = kSlot488HolderVtbl;
    objects.holder.text = kSlot488HolderText;
    objects.holder.text_length = static_cast<std::int64_t>(kSlot488HolderTextLength);
    std::memset(objects.holder.zero, 0, sizeof objects.holder.zero);
    out.holder_stored = true;
    out.holder_views_inert = release_inert_view_f590(text);
    out.holder_atexit_recorded = concmd.atexit_log.record(kSlot488HolderDtorRva, &objects.holder);

    // 0x2ba7a9..0x2ba8a5 and 0x2ba8aa..0x2ba9a6: two 0x3a980 unbounded int variables
    const HelperIntSiteSpec spec1{kSlot488Var1Name, kSlot488Var1NameLength, "", 0, 0, 0, 1, kSlot488IntFnTag, 0, 0};
    out.var1 = helper_init_unbounded_int_var_site(objects.var1, spec1, concmd);
    if (!out.var1.ok) return out;
    const HelperIntSiteSpec spec2{kSlot488Var2Name, kSlot488Var2NameLength, "", 0, 0, 0, 1, kSlot488IntFnTag, 0, 0};
    out.var2 = helper_init_unbounded_int_var_site(objects.var2, spec2, concmd);

    out.ok = out.var0.ok && out.holder_stored && out.holder_views_inert && out.holder_atexit_recorded && out.var1.ok &&
             out.var2.ok;
    return out;
}

}  // namespace sotc
