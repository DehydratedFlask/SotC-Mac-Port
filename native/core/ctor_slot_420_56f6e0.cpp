#include "ctor_slot_420_56f6e0.hpp"

#include <cstdlib>
#include <cstring>

namespace sotc {

namespace {

// Default host allocator hooks (documented stand-in for 0x12820 / 0x12af0).
void* host_allocate(std::size_t size, std::uint32_t, std::uint32_t, std::uint32_t) { return std::malloc(size); }
bool host_deallocate(void* ptr, std::uint32_t) {
    std::free(ptr);
    return true;
}

}  // namespace

Slot420Objects::~Slot420Objects() {
    // Only the separator text owns storage (0xf1a0: data tag 0, control tag 1).
    if (owned_alloc.deallocate == nullptr || sep.text.ctl == nullptr) return;
    (void)owned_alloc.deallocate(const_cast<std::uint8_t*>(sep.text.data), 0);
    (void)owned_alloc.deallocate(sep.text.ctl, 1);
    sep.text.ctl = nullptr;
}

Slot420Hooks default_slot420_hooks() noexcept {
    Slot420Hooks hooks{};
    hooks.separator_alloc.alloc.allocate = &host_allocate;
    hooks.separator_alloc.alloc.deallocate = &host_deallocate;
    return hooks;
}

Slot420Result init_slot420_56f6e0(Slot420Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu,
                                  const Slot420Hooks& hooks) noexcept {
    Slot420Result out{};
    // 0x56f771..0x56f7fe: bool variable (static shape), value = default = 0, atexit 0x20b0
    out.var0 = lane_m6::init_bool_var_literal(objects.var0, kSlot420Var0Name, kSlot420Var0NameLength, "", 0,
                                              kSlot420BoolFnTag, concmd, 0, 0);
    if (!out.var0.ok) return out;
    // 0x56f803..0x56f912: 0xc580 (family wrapper), f590(desc), argfn destroy, value = default = 0, atexit 0x18a60
    const HelperIntSiteSpec spec{kSlot420Var1Name, kSlot420Var1NameLength, "", 0, kSlot420Var1Min, kSlot420Var1Max, 1,
                                 kSlot420IntFnTag, 0, 0};
    out.var1 = helper_init_ranged_int_var_site(objects.var1, spec, concmd);
    out.refusal = out.var1.refusal;
    if (!out.var1.ok) return out;

    // 0x56f917..0x56f96c: 0x26b60 separator (parent `Sound`, flags 0x30), tail 0x268b0, f590(parent), atexit 0x272d0
    const SharedStr parent = l2::measured_view(kSlot420Parent, kSlot420ParentLength);
    out.sep.ctor_ok = init_menu_separator_26b60(objects.sep, parent, kSlot420ItemFlags, hooks.separator_alloc);
    if (!out.sep.ctor_ok) return out;
    objects.owned_alloc = hooks.separator_alloc.alloc;
    out.sep.registration_ok = register_menu_item_268b0(objects.sep, menu, concmd.atexit_log).ok;
    out.sep.views_inert = release_inert_view_f590(parent);
    out.sep.atexit_recorded = record_menu_item_dtor_272d0(concmd.atexit_log, objects.sep);
    out.sep.ok = out.sep.ctor_ok && out.sep.registration_ok && out.sep.views_inert && out.sep.atexit_recorded;
    if (!out.sep.ok) return out;

    // 0x56f971..0x56f9ec: 0x26ea0 toggle for the bool variable
    out.item1 = lane_m6::init_toggle_site_target(objects.item1, debug_menu_target_for(objects.var0), kSlot420Parent,
                                                 kSlot420ParentLength, kSlot420Item1Text, kSlot420Item1TextLength,
                                                 concmd, menu);
    if (!out.item1.ok) return out;

    // 0x56f9f1..0x56fa83: 0x27050 slider (target = the int variable), 0x268b0, f590(text), f590(label), atexit 0x272d0
    const SharedStr label = l2::measured_view(kSlot420Parent, kSlot420ParentLength);
    const SharedStr text = l2::measured_view(kSlot420Item2Text, kSlot420Item2TextLength);
    float lo = 0.0f;
    float hi = 0.0f;
    const std::uint32_t lo_bits = kSlot420Item2MinBits;
    const std::uint32_t hi_bits = kSlot420Item2MaxBits;
    std::memcpy(&lo, &lo_bits, sizeof lo);
    std::memcpy(&hi, &hi_bits, sizeof hi);
    out.item2.ctor_ok = init_debug_menu_item_27050(objects.item2, label, text, &objects.var1, kSlot420ItemFlags, lo, hi);
    if (!out.item2.ctor_ok) return out;
    out.item2.registration = register_debug_menu_item_268b0(objects.item2, menu, concmd.atexit_log);
    out.item2.views_inert = release_inert_view_f590(text) && release_inert_view_f590(label);   // text first, then label
    out.item2.atexit_recorded = concmd.atexit_log.record(kDebugMenuItemDtorRva, &objects.item2);
    out.item2.ok = out.item2.ctor_ok && out.item2.registration.ok && out.item2.views_inert && out.item2.atexit_recorded;
    if (!out.item2.ok) return out;

    // 0x56fa88..0x56faa0: 8 byte cell = 0, __cxa_atexit(0x56ef80, cell) (identity tag, not run)
    objects.holder = 0;
    out.holder_stored = true;
    out.holder_atexit_recorded = concmd.atexit_log.record(kSlot420HolderDtorRva, &objects.holder);

    out.ok = out.var0.ok && out.var1.ok && out.sep.ok && out.item1.ok && out.item2.ok && out.holder_stored &&
             out.holder_atexit_recorded;
    return out;
}

}  // namespace sotc
