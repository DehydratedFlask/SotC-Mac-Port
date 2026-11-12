// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 4): fixture for startup constructor index 550 (0x91290): 1 BOOL console variable(s), 2 0x26ea0 toggle item(s), 1 0x30-byte name holder(s) (dtor 0x8ef00).
#include "l4_test_util.hpp"

#include "core/ctor_slot_550_91290.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot550Rva == 0x91290 && kSlot550Size == 706);
    CHECK(kSlot550BoolFnTag == 0x1597680);
    CHECK(std::strlen(kSlot550Var0Name) == kSlot550Var0NameLength && kSlot550Var0NameLength == 0x1e);
    CHECK(std::strlen(kSlot550Item0Label) == kSlot550Item0LabelLength && kSlot550Item0LabelLength == 0xb);
    CHECK(std::strlen(kSlot550Item0Text) == kSlot550Item0TextLength && kSlot550Item0TextLength == 0x1a);
    CHECK(std::strlen(kSlot550Item1Label) == kSlot550Item1LabelLength && kSlot550Item1LabelLength == 0xa);
    CHECK(std::strlen(kSlot550Item1Text) == kSlot550Item1TextLength && kSlot550Item1TextLength == 0x1a);
    CHECK(std::strlen(kSlot550Holder0Name) == kSlot550Holder0NameLength && kSlot550Holder0NameLength == 0x19);
    CHECK(kSlot550HolderDtorRva == 0x8ef00 && kSlot550HolderVptr == 0x15976b0);
    CHECK(sizeof(Slot550Objects) >= 1 * sizeof(ConCmdBoolVar) && sizeof(ConCmdBoolVar) == 0xc8);
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot550Objects o;
    poison(o);
    const Slot550Result r = init_slot550_91290(o, creg, menu);
    CHECK(r.ok && r.var0.ok && r.var0.views_inert && r.item0.ok && r.item0.views_inert && r.item1.ok && r.item1.views_inert && r.holder0_ok);
    CHECK(r.var0.registration.ok && r.var0.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_with_description(o.var0, kSlot550BoolFnTag, 0, "", 0);
    CHECK(same_view(o.var0.base.name, kSlot550Var0Name, kSlot550Var0NameLength));
    CHECK(o.item0.target == &o.var0 && o.item0.flags == 0x30 && o.item0.registered == 0);
    CHECK(same_view(o.item0.label, kSlot550Item0Label, kSlot550Item0LabelLength) && same_view(o.item0.text, kSlot550Item0Text, kSlot550Item0TextLength));
    CHECK(o.item0.range_min == 1.0f && o.item0.range_max == 10.0f);   // bool class slot 6 is 0: no narrowing
    CHECK(o.item1.target == &o.var0 && o.item1.flags == 0x30 && o.item1.registered == 0);
    CHECK(same_view(o.item1.label, kSlot550Item1Label, kSlot550Item1LabelLength) && same_view(o.item1.text, kSlot550Item1Text, kSlot550Item1TextLength));
    CHECK(o.item1.range_min == 1.0f && o.item1.range_max == 10.0f);   // bool class slot 6 is 0: no narrowing
    CHECK(o.holder0.vptr == kSlot550HolderVptr && o.holder0.tail == 0 && same_view(o.holder0.name, kSlot550Holder0Name, kSlot550Holder0NameLength));
    CHECK(creg.pending.size() == 1);
    CHECK(creg.atexit_log.size() == 5);
    check_atexit(creg, 0, 0x20b0, &o.var0);
    CHECK(creg.atexit_log.at(1).dtor_rva == 0x27280);   // the menu root destructor, recorded by the first menu item
    check_atexit(creg, 2, 0x272d0, &o.item0);
    check_atexit(creg, 3, 0x272d0, &o.item1);
    check_atexit(creg, 4, 0x8ef00, &o.holder0);
    CHECK(menu.root.count == 2);
}

void section_failures() {
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;
    DebugMenuRegistry menu;
    Slot550Objects o;
    const Slot550Result r = init_slot550_91290(o, creg, menu);
    CHECK(!r.ok && !r.var0.ok && !r.var0.registration.ok && creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_550");
    return 0;
}
