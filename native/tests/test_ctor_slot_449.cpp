// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 3): fixture for startup constructor index 449 (0x4b0200): 2 bool console variable(s) (tag 0x15c0798), 2 toggle item(s).
#include "l4_test_util.hpp"

#include "core/ctor_slot_449_4b0200.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot449Rva == 0x4b0200 && kSlot449Size == 776 && kSlot449BoolFnTag == 0x15c0798);
    CHECK(std::strlen(kSlot449Var0Name) == kSlot449Var0NameLength && kSlot449Var0NameLength == 0x13);
    CHECK(std::strlen(kSlot449Var1Name) == kSlot449Var1NameLength && kSlot449Var1NameLength == 0x12);
    CHECK(std::strlen(kSlot449Item0Label) == kSlot449Item0LabelLength && kSlot449Item0LabelLength == 0x8);
    CHECK(std::strlen(kSlot449Item0Text) == kSlot449Item0TextLength && kSlot449Item0TextLength == 0x15);
    CHECK(std::strlen(kSlot449Item1Label) == kSlot449Item1LabelLength && kSlot449Item1LabelLength == 0x8);
    CHECK(std::strlen(kSlot449Item1Text) == kSlot449Item1TextLength && kSlot449Item1TextLength == 0xb);
    CHECK(sizeof(Slot449Objects) >= 2 * sizeof(ConCmdBoolVar) && sizeof(ConCmdBoolVar) == 0xc8);
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot449Objects o;
    poison(o);
    const Slot449Result r = init_slot449_4b0200(o, creg, menu);
    CHECK(r.ok && r.var0.ok && r.var0.views_inert && r.var1.ok && r.var1.views_inert && r.item0.ok && r.item0.views_inert && r.item1.ok && r.item1.views_inert);
    CHECK(r.var0.registration.ok && r.var0.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var1.registration.ok && r.var1.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_with_description(o.var0, 0x15c0798, 0, "", 0);
    CHECK(same_view(o.var0.base.name, kSlot449Var0Name, 0x13));
    check_bool_with_description(o.var1, 0x15c0798, 0, "", 0);
    CHECK(same_view(o.var1.base.name, kSlot449Var1Name, 0x12));
    CHECK(o.item0.target == &o.var0 && o.item0.flags == 0x30 && o.item0.registered == 0);
    CHECK(same_view(o.item0.label, kSlot449Item0Label, kSlot449Item0LabelLength) && same_view(o.item0.text, kSlot449Item0Text, kSlot449Item0TextLength));
    CHECK(o.item0.range_min == 1.0f && o.item0.range_max == 10.0f);   // bool class slot 6 is 0: no narrowing
    CHECK(o.item1.target == &o.var1 && o.item1.flags == 0x30 && o.item1.registered == 0);
    CHECK(same_view(o.item1.label, kSlot449Item1Label, kSlot449Item1LabelLength) && same_view(o.item1.text, kSlot449Item1Text, kSlot449Item1TextLength));
    CHECK(o.item1.range_min == 1.0f && o.item1.range_max == 10.0f);   // bool class slot 6 is 0: no narrowing
    CHECK(creg.pending.size() == 2);
    CHECK(creg.atexit_log.size() == 5);
    check_atexit(creg, 0, 0x20b0, &o.var0);
    CHECK(creg.atexit_log.at(1).dtor_rva == 0x27280);   // the menu root destructor, recorded by the first item
    check_atexit(creg, 2, 0x272d0, &o.item0);
    check_atexit(creg, 3, 0x20b0, &o.var1);
    check_atexit(creg, 4, 0x272d0, &o.item1);
    CHECK(menu.root.count == 2);
}

void section_failures() {
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;
    DebugMenuRegistry menu;
    Slot449Objects o;
    const Slot449Result r = init_slot449_4b0200(o, creg, menu);
    CHECK(!r.ok && !r.var0.ok && !r.var0.registration.ok && !r.var1.ok && !r.var1.registration.ok && creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_449");
    return 0;
}
