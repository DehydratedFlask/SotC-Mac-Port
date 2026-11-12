#include "ctor_slot_430_529df0.hpp"

#include <atomic>

namespace sotc {

Slot430Result init_slot430_529df0(Slot430Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot430Result out{};
    // 0x529e2f: mov qword [0x2efc9e0], 0 ; 0x529e36: __cxa_atexit(0x523ba0, cell, dso) (recorded, never run)
    std::atomic_ref<std::uint64_t>(objects.cell).store(0, std::memory_order_seq_cst);
    out.cell_stored = true;
    out.cell_atexit_recorded = concmd.atexit_log.record(kSlot430CellDtorRva, &objects.cell);

    // 0x529e3b..0x529f59: 0xc580 (family wrapper) ; argfn destroy ; vptr ; value/default ; 0x20d60 ; f590 x2 ; atexit 0x18a60
    const HelperIntSiteSpec spec{kSlot430VarName, kSlot430VarNameLength, kSlot430VarDescription,
                                 kSlot430VarDescriptionLength, kSlot430Min, kSlot430Max, 1, kSlot430IntFnTag,
                                 kSlot430Value, kSlot430Value};
    out.var = helper_init_ranged_int_var_site(objects.var, spec, concmd);
    if (!out.var.ok) return out;

    // 0x529f5e..0x529fdd: toggle(label, text, target = var, flags 0x30) ; f590 x2 ; atexit 0x272d0
    out.toggle = lane_m6::init_toggle_site_target(objects.item, ranged_int_c580_menu_target(objects.var),
                                                  kSlot430MenuLabel, sizeof(kSlot430MenuLabel) - 1, kSlot430MenuText,
                                                  sizeof(kSlot430MenuText) - 1, concmd, menu);
    out.ok = out.cell_stored && out.cell_atexit_recorded && out.var.ok && out.toggle.ok;
    return out;
}

}  // namespace sotc
