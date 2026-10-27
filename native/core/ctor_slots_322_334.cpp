#include "ctor_slots_322_334.hpp"

#include <cstring>

namespace sotc {

Slot322Result init_slot322_849e30(Slot322Objects& o, ConCmdRegistry& concmd,
                                DebugMenuRegistry& menu) noexcept {
    Slot322Result out{};
    out.var = family_bool_var(o.var, kSlot322VarName, sizeof kSlot322VarName - 1, kSlot322VarFnTag,
                                    kSlot322VarValue, kSlot322VarDefault, concmd);
    out.item = family_named_item_26db0(o.item, kSlot322ItemParent, sizeof kSlot322ItemParent - 1, o.var.base,
                                            debug_menu_target_for(o.var), kSlot322ItemFlags, concmd, menu);
    out.ok = out.var.ok &&
             out.item.ok();
    return out;
}

Slot325Result init_slot325_839f00(Slot325Objects& o, ConCmdRegistry& concmd,
                                DebugMenuRegistry& menu) noexcept {
    Slot325Result out{};
    out.var = family_bool_var(o.var, kSlot325VarName, sizeof kSlot325VarName - 1, kSlot325VarFnTag,
                                    kSlot325VarValue, kSlot325VarDefault, concmd);
    out.item = family_toggle_item_26ea0(o.item, kSlot325ItemLabel, sizeof kSlot325ItemLabel - 1, kSlot325ItemText,
                                             sizeof kSlot325ItemText - 1, debug_menu_target_for(o.var),
                                             kSlot325ItemFlags, concmd, menu);
    out.ok = out.var.ok &&
             out.item.ok();
    return out;
}

Slot327Result init_slot327_836330(Slot327Objects& o, ConCmdRegistry& concmd,
                                DebugMenuRegistry& menu) noexcept {
    Slot327Result out{};
    out.var = family_bool_var(o.var, kSlot327VarName, sizeof kSlot327VarName - 1, kSlot327VarFnTag,
                                    kSlot327VarValue, kSlot327VarDefault, concmd);
    out.item = family_toggle_item_26ea0(o.item, kSlot327ItemLabel, sizeof kSlot327ItemLabel - 1, kSlot327ItemText,
                                             sizeof kSlot327ItemText - 1, debug_menu_target_for(o.var),
                                             kSlot327ItemFlags, concmd, menu);
    out.ok = out.var.ok &&
             out.item.ok();
    return out;
}

Slot328Result init_slot328_82e040(Slot328Objects& o, ConCmdRegistry& concmd) noexcept {
    Slot328Result out{};
    out.var_a = family_bool_var(o.var_a, kSlot328VarAName, sizeof kSlot328VarAName - 1, kSlot328VarAFnTag,
                                    kSlot328VarAValue, kSlot328VarADefault, concmd);
    out.var_b = family_bool_var(o.var_b, kSlot328VarBName, sizeof kSlot328VarBName - 1, kSlot328VarBFnTag,
                                    kSlot328VarBValue, kSlot328VarBDefault, concmd);
    out.ok = out.var_a.ok &&
             out.var_b.ok;
    return out;
}

Slot330Result init_slot330_810a50(Slot330Objects& o, ConCmdRegistry& concmd) noexcept {
    Slot330Result out{};
    out.var = family_bool_var(o.var, kSlot330VarName, sizeof kSlot330VarName - 1, kSlot330VarFnTag,
                                    kSlot330VarValue, kSlot330VarDefault, concmd);
    out.ok = out.var.ok;
    return out;
}

Slot331Result init_slot331_809640(Slot331Objects& o, ConCmdRegistry& concmd,
                                DebugMenuRegistry& menu) noexcept {
    Slot331Result out{};
    out.var = family_bool_var(o.var, kSlot331VarName, sizeof kSlot331VarName - 1, kSlot331VarFnTag,
                                    kSlot331VarValue, kSlot331VarDefault, concmd);
    out.group = family_group_item_26c10(o.group, kSlot331GroupParent, sizeof kSlot331GroupParent - 1, kSlot331GroupCaption,
                                            sizeof kSlot331GroupCaption - 1, kSlot331GroupFlags, concmd, menu);
    out.item = family_named_item_26db0(o.item, kSlot331ItemParent, sizeof kSlot331ItemParent - 1, o.var.base,
                                            debug_menu_target_for(o.var), kSlot331ItemFlags, concmd, menu);
    out.ok = out.var.ok &&
             out.group.ok() &&
             out.item.ok();
    return out;
}

Slot332Result init_slot332_805440(Slot332Objects& o, ConCmdRegistry& concmd,
                                DebugMenuRegistry& menu) noexcept {
    Slot332Result out{};
    out.var_a = family_bool_var(o.var_a, kSlot332VarAName, sizeof kSlot332VarAName - 1, kSlot332VarAFnTag,
                                    kSlot332VarAValue, kSlot332VarADefault, concmd);
    out.item_a = family_named_item_26db0(o.item_a, kSlot332ItemAParent, sizeof kSlot332ItemAParent - 1, o.var_a.base,
                                            debug_menu_target_for(o.var_a), kSlot332ItemAFlags, concmd, menu);
    out.var_b = family_bool_var(o.var_b, kSlot332VarBName, sizeof kSlot332VarBName - 1, kSlot332VarBFnTag,
                                    kSlot332VarBValue, kSlot332VarBDefault, concmd);
    out.item_b = family_named_item_26db0(o.item_b, kSlot332ItemBParent, sizeof kSlot332ItemBParent - 1, o.var_b.base,
                                            debug_menu_target_for(o.var_b), kSlot332ItemBFlags, concmd, menu);
    out.ok = out.var_a.ok &&
             out.item_a.ok() &&
             out.var_b.ok &&
             out.item_b.ok();
    return out;
}

// ---- slot 333 (0x7fd6d0) ---------------------------------------------------------------------------
// 0xf2f0(&name) / 0x20af0 / indirect 0x20d60 / 0xf590 x2 / atexit(0x4b0) twice, in that order for each
// command, exactly as the slot-306 sites; init_fconcmd_object models one such sequence.
Slot333Result init_slot333_7fd6d0(Slot333Objects& o, ConCmdRegistry& concmd) noexcept {
    Slot333Result out{};
    const FConCmdCommandSpec a{kSlot333CmdAName, kSlot333CmdADescription, sizeof kSlot333CmdADescription - 1,
                               kSlot333CmdAHandler};
    const FConCmdCommandSpec b{kSlot333CmdBName, kSlot333CmdBDescription, sizeof kSlot333CmdBDescription - 1,
                               kSlot333CmdBHandler};
    out.cmd_a = init_fconcmd_object(o.cmd_a, a, concmd);
    out.cmd_b = init_fconcmd_object(o.cmd_b, b, concmd);
    out.ok = out.cmd_a.ok && out.cmd_b.ok;
    return out;
}

// ---- slot 334 (0x7fc530) ---------------------------------------------------------------------------
bool unbounded_int_fn_table_lifted(std::uint64_t table) noexcept {
    return table == kUnboundedIntSeedFnTable || table == kUnboundedIntFnTable || table == kUnboundedIntFnTableB;
}

bool init_unbounded_int_var_worker_3a980(ConCmdRangedVar& var, const SharedStr& name, std::uint32_t flags,
                                         const RangedVarArgs& block, const SharedStr& description) noexcept {
    // The functor install clones the caller's functor through [block.tag + 8] after most stores.
    if (!unbounded_int_fn_table_lifted(block.fn_vptr)) return false;

    init_concmd_base_20af0(var.base, name, description, flags);   // call 0x20af0
    var.base.vptr = kUnboundedIntWorkerVptr;                      // [this] = 0x1594190 + 0x10
    var.value_ref = &var.value;                                   // [this+0x60] = rdx (this+0xd0)
    var.min_value[0] = 0;                                         // byte [this+0x6c] = 0
    var.min_engaged = 0;                                          // byte [this+0x70] = 0  (no bound)
    var.max_value[0] = 0;                                         // byte [this+0x74] = 0
    var.max_engaged = 0;                                          // byte [this+0x78] = 0  (no bound)
    std::memset(var.args, 0, sizeof var.args);                    // ymm [this+0x80] = 0
    var.fn_vptr = kUnboundedIntSeedFnTable;                       // [this+0xc0] = 0x1595660
    var.fn_payload = 0;                                           // [this+0xb0] = 0
    var.fn_aux = 0;                                               // [this+0xc8] = 0
    var.args[1] = block.q[1];                                     // xmm [this+0x88] = block+0x08
    var.args[2] = block.q[2];
    var.args[0] = block.q[0];                                     // [this+0x80] = block+0x00
    var.args[3] = block.q[3];                                     // [this+0x98] = block+0x18
    if (block.q[3] != 0) var.args[0] = reinterpret_cast<std::uint64_t>(&var.args[0]);

    // Functor install onto this+0xa0: clone(block.fn) through the block's table, copy to a temporary,
    // copy the seed functor, copy the temporary into the object, swap the aux words, destroy the
    // temporary (bare ret). Copy and clone bodies (0xc720 / 0xc740) both store {payload = src.payload,
    // tag = 0x15944a0}; the aux word is only moved, so the object ends with the block's aux.
    var.fn_payload = block.fn_payload;
    var.fn_vptr = kUnboundedIntFnTable;
    var.fn_aux = block.fn_aux;
    return true;
}

DebugMenuTarget unbounded_int_menu_target(const ConCmdRangedVar& var) noexcept {
    DebugMenuTarget out{};
    out.object = &var;
    if (var.base.vptr == kUnboundedIntFinalVptr) {
        out.slot3 = kUnboundedIntSlot3Result;
        out.slot6 = kUnboundedIntSlot6Result;
        out.known = true;
    }
    return out;
}

Slot334Result init_slot334_7fc530(Slot334Objects& o, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot334Result out{};
    const SharedStr name = family_view(kSlot334Name, sizeof kSlot334Name - 1);
    const SharedStr description = family_view("", 0);   // rodata NUL byte 0x128c2f4
    RangedVarArgs args{};                                // four zero qwords, payload 0, aux 0
    args.fn_vptr = kSlot334ArgFnTable;

    o.var.base.vptr = kUnboundedIntFinalVptr;            // 0x7fc5c7 (overwritten by the base ctor)
    out.worker_ok = init_unbounded_int_var_worker_3a980(o.var, name, kSlot334CtorFlags, args, description);
    if (!out.worker_ok) return out;                      // refused before any write
    out.description_inert = release_inert_view_f590(description);        // 0x7fc60e
    out.arg_functor_destroyed = unbounded_int_fn_table_lifted(args.fn_vptr);   // call [tag+0x18]: bare ret
    o.var.base.vptr = kUnboundedIntFinalVptr;            // 0x7fc624
    o.var.value = kSlot334Value;                         // dword [0x32883c8] = 0x10
    o.var.default_value = kSlot334Default;               // dword [0x3288360] = 0x10
    out.registration = register_concmd_20d60(o.var.base, concmd);        // call 0x20d60
    out.name_inert = release_inert_view_f590(name);                      // 0x7fc647
    out.var_atexit_recorded = concmd.atexit_log.record(kSlot334DtorRva, &o.var);   // 0x7fc660

    out.item = family_toggle_item_26ea0(o.item, kSlot334Label, sizeof kSlot334Label - 1, kSlot334Text,
                                        sizeof kSlot334Text - 1, unbounded_int_menu_target(o.var),
                                        kSlot334ItemFlags, concmd, menu);
    out.ok = out.worker_ok && out.description_inert && out.arg_functor_destroyed && out.registration.ok &&
             out.name_inert && out.var_atexit_recorded && out.item.ok();
    return out;
}

}  // namespace sotc
