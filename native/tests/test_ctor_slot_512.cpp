// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 4): fixture for startup constructor index 512 (0x1d5e80): 2 BOOL console variable(s), 2 0x26ea0 toggle item(s), 1 0x26c10 group item(s).
#include "l4_test_util.hpp"

#include "core/ctor_slot_512_1d5e80.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot512Rva == 0x1d5e80 && kSlot512Size == 902);
    CHECK(kSlot512BoolFnTag == 0x1599288);
    CHECK(std::strlen(kSlot512Var0Name) == kSlot512Var0NameLength && kSlot512Var0NameLength == 0x20);
    CHECK(std::strlen(kSlot512Var1Name) == kSlot512Var1NameLength && kSlot512Var1NameLength == 0x1c);
    CHECK(std::strlen(kSlot512Item0Label) == kSlot512Item0LabelLength && kSlot512Item0LabelLength == 0x14);
    CHECK(std::strlen(kSlot512Item0Text) == kSlot512Item0TextLength && kSlot512Item0TextLength == 0x1e);
    CHECK(std::strlen(kSlot512Item1Label) == kSlot512Item1LabelLength && kSlot512Item1LabelLength == 0x14);
    CHECK(std::strlen(kSlot512Item1Text) == kSlot512Item1TextLength && kSlot512Item1TextLength == 0x16);
    CHECK(std::strlen(kSlot512Group0Parent) == kSlot512Group0ParentLength && kSlot512Group0ParentLength == 0x0);
    CHECK(std::strlen(kSlot512Group0Caption) == kSlot512Group0CaptionLength && kSlot512Group0CaptionLength == 0x14);
    CHECK(kSlot512GroupFlags == 0x30);
    CHECK(sizeof(Slot512Objects) >= 2 * sizeof(ConCmdBoolVar) && sizeof(ConCmdBoolVar) == 0xc8);
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot512Objects o;
    poison(o);
    const Slot512Result r = init_slot512_1d5e80(o, creg, menu);
    CHECK(r.ok && r.var0.ok && r.var0.views_inert && r.var1.ok && r.var1.views_inert && r.group0.ok() && r.item0.ok && r.item0.views_inert && r.item1.ok && r.item1.views_inert);
    CHECK(r.var0.registration.ok && r.var0.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var1.registration.ok && r.var1.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_with_description(o.var0, kSlot512BoolFnTag, 1, "", 0);
    CHECK(same_view(o.var0.base.name, kSlot512Var0Name, kSlot512Var0NameLength));
    check_bool_with_description(o.var1, kSlot512BoolFnTag, 0, "", 0);
    CHECK(same_view(o.var1.base.name, kSlot512Var1Name, kSlot512Var1NameLength));
    CHECK(o.group0.kind == kMenuKindGroup && o.group0.target == nullptr && o.group0.flags == 0x30 && o.group0.registered == 0);
    CHECK(same_view(o.group0.label, kSlot512Group0Parent, kSlot512Group0ParentLength));   // the parent name
    CHECK(same_view(o.group0.text, kSlot512Group0Caption, kSlot512Group0CaptionLength) && same_view(o.group0.empty, kSlot512Group0Caption, kSlot512Group0CaptionLength));
    CHECK(o.item0.target == &o.var0 && o.item0.flags == 0x30 && o.item0.registered == 0);
    CHECK(same_view(o.item0.label, kSlot512Item0Label, kSlot512Item0LabelLength) && same_view(o.item0.text, kSlot512Item0Text, kSlot512Item0TextLength));
    CHECK(o.item0.range_min == 1.0f && o.item0.range_max == 10.0f);   // bool class slot 6 is 0: no narrowing
    CHECK(o.item1.target == &o.var1 && o.item1.flags == 0x30 && o.item1.registered == 0);
    CHECK(same_view(o.item1.label, kSlot512Item1Label, kSlot512Item1LabelLength) && same_view(o.item1.text, kSlot512Item1Text, kSlot512Item1TextLength));
    CHECK(o.item1.range_min == 1.0f && o.item1.range_max == 10.0f);   // bool class slot 6 is 0: no narrowing
    CHECK(creg.pending.size() == 2);
    CHECK(creg.atexit_log.size() == 6);
    check_atexit(creg, 0, 0x20b0, &o.var0);
    CHECK(creg.atexit_log.at(1).dtor_rva == 0x27280);   // the menu root destructor, recorded by the first menu item
    check_atexit(creg, 2, 0x272d0, &o.group0);
    check_atexit(creg, 3, 0x272d0, &o.item0);
    check_atexit(creg, 4, 0x20b0, &o.var1);
    check_atexit(creg, 5, 0x272d0, &o.item1);
    CHECK(menu.root.count == 3);
}

void section_failures() {
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;
    DebugMenuRegistry menu;
    Slot512Objects o;
    const Slot512Result r = init_slot512_1d5e80(o, creg, menu);
    CHECK(!r.ok && !r.var0.ok && !r.var0.registration.ok && !r.var1.ok && !r.var1.registration.ok && creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_512");
    return 0;
}
