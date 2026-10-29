// CPU-01-STARTUP-SLOT-364: fixture for startup constructor index 364 (0x74a8e0). Lane M1.
#include "l4_test_util.hpp"

#include "core/ctor_slot_364_74a8e0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot364Rva == 0x74a8e0 && kSlot364Size == 497 && kSlot364ObjectRva == 0x32525b0 && kSlot364ItemRva == 0x3252688);
    CHECK(std::strcmp(kSlot364Spec.name, "cp10_WanderSheathCollisionSwordScalar") == 0 && std::strlen(kSlot364Spec.name) == 0x25);
    CHECK(!kSlot364Spec.bounded && kSlot364Spec.default_bits == 0 && kSlot364Spec.flags == 1 && kSlot364Spec.arg_fn_vtbl == 0x15cd240);
    CHECK(std::strlen(kSlot364ItemLabel) == 9 && std::strlen(kSlot364ItemText) == 0x20 && kSlot364ItemFlags == 0x30);
    CHECK(kSlot364RangeMinBits == 0x3c23d70au && kSlot364RangeMaxBits == 0x3dcccccdu);
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot364Objects o;
    poison(o);
    const Slot364Result r = init_slot364_74a8e0(o, creg, menu);
    CHECK(r.ok && r.var.ok && r.var.ctor_ok && r.var.functor_destroy_ok && r.var.views_inert && r.var.atexit_recorded);
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);
    check_float_common(o.var, false, 0u, 0u, 0u);
    CHECK(same_view(o.var.base.name, "cp10_WanderSheathCollisionSwordScalar", 0x25));

    // The 0x27050 slider item: label/text borrowed views, explicit range, target = the float variable.
    CHECK(r.item.ok && r.item.ctor_ok && r.item.views_inert && r.item.atexit_recorded);
    const DebugMenuItem& it = o.item;
    CHECK(it.target == &o.var && it.flags == 0x30 && it.kind == 2 && it.registered == 0);
    CHECK(same_view(it.label, "CP10-Anim", 9) && same_view(it.text, "WanderSheathCollisionSwordScalar", 0x20));
    CHECK(dword(it, 0x98) == 0x3c23d70au && dword(it, 0x9c) == 0x3dcccccdu);
    CHECK(empty_view(it.empty));
    check_item_untouched(it);
    CHECK(menu.root.count == 1 && menu.root.link.next == &it.link);

    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.var.base.pending);
    CHECK(creg.atexit_log.size() == 3);
    check_atexit(creg, 0, 0x20f0, &o.var);
    check_atexit(creg, 1, 0x27280, &menu.root);
    check_atexit(creg, 2, 0x272d0, &o.item);
}

void section_functor_whitelist() {
    // Slot 364's four vtable slots are byte-pinned to the lifted float functor bodies; accept its tag directly.
    CHECK(is_known_float_arg_functor(kFloatArgFnVtblSlot364) && is_known_float_arg_functor(0x15ce318));
    CHECK(!is_known_float_arg_functor(0x1234567));
    ConCmdRegistry creg;
    ConCmdRangedVar v;
    poison(v);
    FloatVarSpec bad = kSlot364Spec;
    bad.arg_fn_vtbl = 0x1234567;
    const FloatVarResult r = init_float_var_static(v, bad, creg);
    CHECK(!r.ok && !r.ctor_ok && creg.pending.size() == 0 && creg.atexit_log.size() == 0);
    const auto* b = reinterpret_cast<const std::uint8_t*>(&v);
    for (std::size_t i = 0x60; i < sizeof v; ++i) CHECK(b[i] == kPoison);
}

void section_failures() {
    {
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot364Objects o;
        const Slot364Result r = init_slot364_74a8e0(o, creg, menu);
        CHECK(!r.ok && !r.var.ok && !r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::manager);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;   // parent path without an insert contract
        Slot364Objects o;
        const Slot364Result r = init_slot364_74a8e0(o, creg, menu);
        CHECK(!r.ok && r.var.ok && !r.item.ok && r.item.registration.path == DebugMenuRegisterPath::parent);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_functor_whitelist();
    section_failures();
    std::puts("OK test_ctor_slot_364");
    return 0;
}
