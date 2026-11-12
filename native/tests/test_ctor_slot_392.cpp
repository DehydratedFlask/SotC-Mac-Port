// CPU-01-STARTUP-BATCH-7: fixture for startup constructor index 392 (0x681690): four bounded float variables (0xc300, tag 0x15cc618) and
// four 0x27120 sliders whose text is each variable's own name view.
#include "l4_test_util.hpp"

#include "core/ctor_slot_392_681690.hpp"

namespace {
using namespace l4;

float bits_to_float(std::uint32_t b) {
    float f;
    std::memcpy(&f, &b, sizeof f);
    return f;
}

void section_constants() {
    CHECK(kSlot392Rva == 0x681690 && kSlot392Size == 1612 && kSlot392Count == 4);
    for (std::size_t n = 0; n < kSlot392Count; ++n) CHECK(std::strlen(kSlot392Names[n]) == kSlot392NameLengths[n]);
    CHECK(std::strlen(kSlot392Label) == kSlot392LabelLength);
    CHECK(is_known_float_arg_functor(kSlot392FloatFnTag) && !is_int_functor_tag(kSlot392FloatFnTag));
    CHECK(sizeof(ConCmdRangedVar) == 0xd8 && sizeof(DebugMenuItem) == 0xa8);
    CHECK(offsetof(Slot392Objects, item) == kSlot392Item0Rva - kSlot392Var0Rva);
    CHECK(kDebugMenuItemFloatFromVarCtorRva == 0x27120 && kDebugMenuItemFloatFromVarCtorSize == 0xd2);
}

void section_item_27120() {
    // The constructor alone: text is a COPY of the target's name view (the stores of 0x27050 otherwise).
    ConCmdRegistry creg;
    ConCmdRangedVar v;
    poison(v);
    const FloatVarSpec spec{kSlot392Names[0], "", true, 0, kSlot392VarHiBits, 0x3f99999a, kSlot392FloatFnTag, 1};
    CHECK(init_float_var_static(v, spec, creg).ok);
    DebugMenuItem item;
    poison(item);
    const SharedStr label = l2::measured_view("Shadow AI", 9);
    CHECK(init_debug_menu_item_27120(item, label, v.base, 0x30, 0.25f, 0.5f));
    CHECK(item.target == &v && item.kind == 2 && item.flags == 0x30 && item.registered == 0);
    CHECK(same_view(item.label, "Shadow AI", 9) && same_view(item.text, kSlot392Names[0], kSlot392NameLengths[0]));
    CHECK(item.range_min == 0.25f && item.range_max == 0.5f);
    CHECK(item.link.prev == &item.link && item.link.next == &item.link && item.link.owner == nullptr);
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot392Objects o;
    poison(o);
    const Slot392Result r = init_slot392_681690(o, creg, menu);
    CHECK(r.ok);
    for (std::size_t n = 0; n < kSlot392Count; ++n) {
        CHECK(r.var[n].ok && r.var[n].ctor_ok && r.var[n].functor_destroy_ok && r.var[n].views_inert &&
              r.var[n].atexit_recorded && r.var[n].registration.ok);
        CHECK(r.item[n].ok && r.item[n].ctor_ok && r.item[n].views_inert && r.item[n].atexit_recorded);
        const ConCmdRangedVar& v = o.var[n];
        CHECK(v.base.flags == 1 && v.base.registered == 1 && same_view(v.base.name, kSlot392Names[n], kSlot392NameLengths[n]));
        CHECK(v.value == kSlot392ValueBits[n] && v.default_value == kSlot392ValueBits[n] && v.value_ref == &v.value);
        CHECK(v.min_engaged == 1 && v.max_engaged == 1);
        const DebugMenuItem& it = o.item[n];
        CHECK(it.target == &o.var[n] && it.flags == 0x30 && it.kind == 2 && it.registered == 0);
        CHECK(same_view(it.label, kSlot392Label, kSlot392LabelLength));
        CHECK(same_view(it.text, kSlot392Names[n], kSlot392NameLengths[n]));
        CHECK(it.range_min == bits_to_float(kSlot392SliderLoBits) && it.range_max == bits_to_float(kSlot392SliderHiBits));
    }
    CHECK(menu.root.count == 4 && menu.root.link.next == &o.item[0].link && o.item[0].link.next == &o.item[1].link);
    CHECK(creg.pending.size() == 4 && creg.pending.first() == &o.var[0].base.pending);
    // var0..var3, [root-list destructor on the first item], item0..item3.
    CHECK(creg.atexit_log.size() == 9);
    for (std::size_t n = 0; n < 4; ++n) check_atexit(creg, n, 0x20f0, &o.var[n]);
    check_atexit(creg, 4, 0x27280, &menu.root);
    for (std::size_t n = 0; n < 4; ++n) check_atexit(creg, 5 + n, 0x272d0, &o.item[n]);
}

void section_failures() {
    {
        // First variable registration fails: nothing after it is reached.
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot392Objects o;
        const Slot392Result r = init_slot392_681690(o, creg, menu);
        CHECK(!r.ok && !r.var[0].ok && !r.var[1].ctor_ok && !r.item[0].ctor_ok);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent without an insert contract: the first slider registration fails visibly; later sliders are not reached.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot392Objects o;
        const Slot392Result r = init_slot392_681690(o, creg, menu);
        CHECK(!r.ok && r.var[3].ok && !r.item[0].ok && !r.item[1].ctor_ok);
    }
}

}  // namespace

int main() {
    section_constants();
    section_item_27120();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_392");
    return 0;
}
