#include "ctor_slot_569_1cc80.hpp"

#include <atomic>

namespace sotc {

Slot569Result init_slot569_1cc80(Slot569Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot569Result out{};
    // 0x1ccbd: xchg qword [0x2dfc8a0], 0  (seq_cst store; the previous value is discarded)
    std::atomic_ref<std::uint64_t>(objects.cell).store(0, std::memory_order_seq_cst);
    out.cell_stored = true;

    // 0x1ccc4..0x1cdd2: 0xc580 + f590 + argfn destroy + final vptr + value/default + 0x20d60 + f590 + atexit 0x18a60
    const lane_m6::IntVarSiteSpec spec{kSlot569VarName, kSlot569VarNameLength, "", 0, kSlot569Min, kSlot569Max,
                                       1, kSlot569IntFnTag, 0, 0};
    out.var = lane_m6::init_int_var_site(objects.var, spec, concmd);
    if (!out.var.ok) return out;

    // 0x1cdd7..0x1ce3e: toggle(label "Memory", text, target = var, flags 0x30) ; f590 x2 ; atexit 0x272d0
    out.toggle = lane_m6::init_toggle_site_target(objects.item, ranged_int_c580_menu_target(objects.var),
                                                  kSlot569MenuLabel, sizeof(kSlot569MenuLabel) - 1, kSlot569MenuText,
                                                  sizeof(kSlot569MenuText) - 1, concmd, menu);
    out.ok = out.cell_stored && out.var.ok && out.toggle.ok;
    return out;
}

}  // namespace sotc
