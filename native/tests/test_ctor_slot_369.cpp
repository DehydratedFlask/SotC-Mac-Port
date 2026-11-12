// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 6): fixture for startup constructor index 369 (0x734190): bool `cp10_enableWanderBonePhysics` + its
// 0x26ea0 toggle, unbounded float `cp10_WanderBonePhysicsCollisionRadius` (0x6aa00 family wrapper, tag 0x15cd180) + its 0x27050 slider.
#include "l4_test_util.hpp"

#include "core/ctor_slot_369_734190.hpp"

namespace {
using namespace l4;

float bits_to_float(std::uint32_t b) {
    float f;
    std::memcpy(&f, &b, sizeof f);
    return f;
}

void section_constants() {
    CHECK(kSlot369Rva == 0x734190 && kSlot369Size == 898);
    CHECK(std::strlen(kSlot369Var0Name) == kSlot369Var0NameLength && kSlot369Var0NameLength == 0x1c);
    CHECK(std::strlen(kSlot369Parent) == kSlot369ParentLength && kSlot369ParentLength == 9);
    CHECK(std::strlen(kSlot369Item0Text) == kSlot369Item0TextLength && kSlot369Item0TextLength == 0x1a);
    CHECK(std::strlen(kSlot369Var1Name) == kSlot369Var1NameLength && kSlot369Var1NameLength == 0x25);
    CHECK(std::strlen(kSlot369Item1Text) == kSlot369Item1TextLength && kSlot369Item1TextLength == 0x24);
    CHECK(is_float_functor_tag(kSlot369FloatFnTag) && !is_int_functor_tag(kSlot369FloatFnTag));
    CHECK(sizeof(ConCmdBoolVar) == 0xc8 && sizeof(ConCmdRangedVar) == 0xd8 && sizeof(DebugMenuItem) == 0xa8);
    CHECK(offsetof(Slot369Objects, item0) == kSlot369Item0Rva - kSlot369Var0Rva);
    CHECK(offsetof(Slot369Objects, var1) == kSlot369Var1Rva - kSlot369Var0Rva);
    CHECK(offsetof(Slot369Objects, item1) == kSlot369Item1Rva - kSlot369Var0Rva);
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot369Objects o;
    poison(o);
    const Slot369Result r = init_slot369_734190(o, creg, menu);
    CHECK(r.ok && r.refusal == HelperRefusal::none);
    CHECK(r.var0.ok && r.var0.registration.ok && r.var0.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.item0.ctor_ok && r.item0.views_inert && r.item0.atexit_recorded && r.item0.ok);
    CHECK(r.var1.ok && r.var1.refusal == HelperRefusal::none && r.var1.ctor_ok && r.var1.functor_destroy_ok &&
          r.var1.views_inert && r.var1.atexit_recorded && r.var1.registration.ok);
    CHECK(r.item1.ctor_ok && r.item1.views_inert && r.item1.atexit_recorded && r.item1.ok);

    check_bool_with_description(o.var0, kSlot369BoolFnTag, 1, "", 0);
    CHECK(same_view(o.var0.base.name, kSlot369Var0Name, kSlot369Var0NameLength));
    check_float_common(o.var1, false, 0, 0, kSlot369Var1ValueBits);
    CHECK(same_view(o.var1.base.name, kSlot369Var1Name, kSlot369Var1NameLength));

    // 0x26ea0 toggle: bool class slot 6 is 0, so the {1.0, 10.0} default range stays
    CHECK(o.item0.target == &o.var0 && o.item0.flags == 0x30 && o.item0.kind == 2 && o.item0.registered == 0);
    CHECK(same_view(o.item0.label, kSlot369Parent, kSlot369ParentLength));
    CHECK(same_view(o.item0.text, kSlot369Item0Text, kSlot369Item0TextLength));
    CHECK(o.item0.range_min == 1.0f && o.item0.range_max == 10.0f);
    // 0x27050 slider: the explicit range, target = the float variable
    CHECK(o.item1.target == &o.var1 && o.item1.flags == 0x30 && o.item1.kind == 2 && o.item1.registered == 0);
    CHECK(same_view(o.item1.label, kSlot369Parent, kSlot369ParentLength));
    CHECK(same_view(o.item1.text, kSlot369Item1Text, kSlot369Item1TextLength));
    CHECK(o.item1.range_min == bits_to_float(kSlot369Item1MinBits) && o.item1.range_max == bits_to_float(kSlot369Item1MaxBits));
    CHECK(menu.root.count == 2 && menu.root.link.next == &o.item0.link && o.item0.link.next == &o.item1.link);

    CHECK(creg.pending.size() == 2 && creg.pending.first() == &o.var0.base.pending);
    CHECK(o.var0.base.pending.next == &o.var1.base.pending && o.var1.base.pending.next == &creg.pending.head);
    // var0, [root-list destructor on the first item], item0, var1, item1.
    CHECK(creg.atexit_log.size() == 5);
    check_atexit(creg, 0, 0x20b0, &o.var0);
    check_atexit(creg, 1, 0x27280, &menu.root);
    check_atexit(creg, 2, 0x272d0, &o.item0);
    check_atexit(creg, 3, 0x20f0, &o.var1);
    check_atexit(creg, 4, 0x272d0, &o.item1);
}

void section_refusal() {
    // A tag outside the float family is refused by the 0x6aa00 wrapper; nothing is written and the slider is never reached.
    ConCmdRegistry creg;
    ConCmdRangedVar v;
    poison(v);
    HelperFloatSiteSpec spec{kSlot369Var1Name, kSlot369Var1NameLength, "", 0, 1, 0x1234567, 0, true};
    const HelperFloatSiteResult bad = helper_init_unbounded_float_var_site(v, spec, creg);
    CHECK(!bad.ok && !bad.ctor_ok && bad.refusal == HelperRefusal::unadmitted_arg_functor);
    spec.arg_fn_tag = 0x15944a0;   // an int-family tag is not a float functor
    CHECK(!helper_init_unbounded_float_var_site(v, spec, creg).ok && creg.pending.size() == 0);
}

void section_failures() {
    {
        // First variable registration fails: nothing after it is reached.
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot369Objects o;
        const Slot369Result r = init_slot369_734190(o, creg, menu);
        CHECK(!r.ok && !r.var0.ok && !r.item0.ctor_ok && !r.var1.ctor_ok && !r.item1.ctor_ok);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent without an insert contract: the toggle registration fails visibly; the float variable is not reached.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot369Objects o;
        const Slot369Result r = init_slot369_734190(o, creg, menu);
        CHECK(!r.ok && r.var0.ok && !r.item0.ok && !r.var1.ctor_ok);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_refusal();
    section_failures();
    std::puts("OK test_ctor_slot_369");
    return 0;
}
