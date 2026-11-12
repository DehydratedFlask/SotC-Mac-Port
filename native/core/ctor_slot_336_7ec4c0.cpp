#include "ctor_slot_336_7ec4c0.hpp"

#include <cstdlib>

#include "atomic_add_ref_14ccb0.hpp"
#include "store_value_clear_flag_158340.hpp"

namespace sotc {

namespace {

int host_rand() noexcept { return std::rand(); }

}  // namespace

Slot336Hooks default_slot336_hooks() noexcept {
    Slot336Hooks hooks{};
    hooks.rand_source = &host_rand;
    hooks.acquire_14d880 = &getter_14d880;
    hooks.retain_14ccb0 = static_cast<void (*)(void*)>(&add_ref_14ccb0);
    return hooks;
}

Slot336Result init_slot336_7ec4c0(Slot336Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu,
                                  const Slot336Hooks& hooks) noexcept {
    Slot336Result out{};
    // 0x7ec5a8..0x7ec5e3: 0x7ec160 enum variable (range 0..3, value = default = 0), f590(desc), argfn destroy, f590(name), atexit 0x7ea160
    const HelperIntSiteSpec spec0{kSlot336Var0Name, kSlot336Var0NameLength, kSlot336Var0Description,
                                  kSlot336Var0DescriptionLength, kSlot336Var0Min, kSlot336Var0Max, 1,
                                  kSlot336EnumFnTag, 0, 0};
    out.var0 = helper_init_enum_var_site(objects.var0, spec0, concmd);
    out.refusal = out.var0.refusal;
    if (!out.var0.ok) return out;

    // 0x7ec5f6..0x7ec6ec: bool variable (static shape), value = default = 1, atexit 0x20b0
    out.var1 = lane_m6::init_bool_var_literal(objects.var1, kSlot336Var1Name, kSlot336Var1NameLength, "", 0,
                                              kSlot336BoolFnTag, concmd, 1, 1);
    if (!out.var1.ok) return out;

    // 0x7ec6f1..0x7ec80e: 0xc580 (family wrapper), f590(desc), argfn destroy, value = default = 0, atexit 0x18a60
    const HelperIntSiteSpec spec2{kSlot336Var2Name, kSlot336Var2NameLength, "", 0, kSlot336Var2Min, kSlot336Var2Max, 1,
                                  kSlot336IntFnTag, 0, 0};
    out.var2 = helper_init_ranged_int_var_site(objects.var2, spec2, concmd);
    if (out.refusal == HelperRefusal::none) out.refusal = out.var2.refusal;
    if (!out.var2.ok) return out;

    // 0x7ec813..0x7ec821: rand() (host choice) -> 0x158340(0x3265228, value)
    out.rand_value = static_cast<std::uint32_t>((hooks.rand_source != nullptr ? hooks.rand_source : &host_rand)());
    store_value_clear_flag_158340(objects.rand_cell, out.rand_value);

    // 0x7ec826..0x7ec8a4: 0x26c10 group (parent `CP10`, caption `AttractMode`, flags 0x30), f590(caption), f590(parent), atexit 0x272d0
    out.group = family_group_item_26c10(objects.group, kSlot336GroupParent, kSlot336GroupParentLength, kSlot336GroupCaption,
                                        kSlot336GroupCaptionLength, kSlot336ItemFlags, concmd, menu);
    if (!out.group.ok()) return out;

    // 0x7ec8a9..0x7ec928: 0x26ea0 toggle `Type` -> the enum variable
    out.item0 = lane_m6::init_toggle_site_target(objects.item0, enum_var_menu_target(objects.var0), kSlot336GroupCaption,
                                                 kSlot336GroupCaptionLength, kSlot336Item0Text, kSlot336Item0TextLength,
                                                 concmd, menu);
    if (!out.item0.ok) return out;
    // 0x7ec92d..0x7ec9ac: 0x26ea0 toggle `Allow AttractMode to terminate` -> the bool variable
    out.item1 = lane_m6::init_toggle_site_target(objects.item1, debug_menu_target_for(objects.var1), kSlot336GroupCaption,
                                                 kSlot336GroupCaptionLength, kSlot336Item1Text, kSlot336Item1TextLength,
                                                 concmd, menu);
    if (!out.item1.ok) return out;
    // 0x7ec9b1..0x7eca33: 0x26ea0 toggle `AttractMode Spawn PlaceId (-1 = Random)` -> the ranged int variable
    out.item2 = lane_m6::init_toggle_site_target(objects.item2, ranged_int_c580_menu_target(objects.var2),
                                                 kSlot336GroupCaption, kSlot336GroupCaptionLength, kSlot336Item2Text,
                                                 kSlot336Item2TextLength, concmd, menu);
    if (!out.item2.ok) return out;

    // 0x7eca38..0x7ecab5: name holder {name view, tail 0}, vptr stored last; f590(view) inert; atexit 0x8ef00
    const SharedStr holder_view = l2::literal_view(kSlot336HolderName);
    objects.holder.name = holder_view;
    objects.holder.tail = 0;
    objects.holder.vptr = kSlot336HolderVptr;
    out.holder_ok = release_inert_view_f590(holder_view) && concmd.atexit_log.record(kSlot336HolderDtorRva, &objects.holder);
    if (!out.holder_ok) return out;

    // 0x7ecaba..0x7ecbbd: 0x6aa00 unbounded float (value = default = 5.0f), f590(desc) first, argfn destroy, f590(name), atexit 0x20f0
    const HelperFloatSiteSpec spec3{kSlot336Var3Name, kSlot336Var3NameLength, "", 0, 1, kSlot336FloatFnTag,
                                    kSlot336Var3ValueBits, true};
    out.var3 = helper_init_unbounded_float_var_site(objects.var3, spec3, concmd);
    if (out.refusal == HelperRefusal::none) out.refusal = out.var3.refusal;
    if (!out.var3.ok) return out;

    // 0x7ecbc2..0x7ecbea: cell.handle = 0x14d880(); cell.flag = 0; 0x14ccb0(handle); atexit(0x7ea1a0, cell)
    if (hooks.acquire_14d880 == nullptr || hooks.retain_14ccb0 == nullptr) return out;
    Object14c9a0* const handle = hooks.acquire_14d880();
    if (handle == nullptr) return out;
    objects.cell.handle = handle;
    objects.cell.flag = 0;
    out.cell_acquired = true;
    hooks.retain_14ccb0(handle);
    out.cell_retained = true;
    out.cell_atexit_recorded = concmd.atexit_log.record(kSlot336CellDtorRva, &objects.cell);

    out.ok = out.var0.ok && out.var1.ok && out.var2.ok && out.group.ok() && out.item0.ok && out.item1.ok && out.item2.ok &&
             out.holder_ok && out.var3.ok && out.cell_acquired && out.cell_retained && out.cell_atexit_recorded;
    return out;
}

}  // namespace sotc
