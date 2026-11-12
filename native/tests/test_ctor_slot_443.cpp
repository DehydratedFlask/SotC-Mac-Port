// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 4): fixture for startup constructor index 443 (0x4de370): 2 BOOL console variable(s), 2 0x26ea0 toggle item(s), 1 0x26c10 group item(s).
#include "l4_test_util.hpp"

#include "core/ctor_slot_443_4de370.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot443Rva == 0x4de370 && kSlot443Size == 936);
    CHECK(kSlot443BoolFnTag == 0x15c2400);
    CHECK(std::strlen(kSlot443Var0Name) == kSlot443Var0NameLength && kSlot443Var0NameLength == 0x17);
    CHECK(std::strlen(kSlot443Var1Name) == kSlot443Var1NameLength && kSlot443Var1NameLength == 0x13);
    CHECK(std::strlen(kSlot443Item0Label) == kSlot443Item0LabelLength && kSlot443Item0LabelLength == 0xf);
    CHECK(std::strlen(kSlot443Item0Text) == kSlot443Item0TextLength && kSlot443Item0TextLength == 0x13);
    CHECK(std::strlen(kSlot443Item1Label) == kSlot443Item1LabelLength && kSlot443Item1LabelLength == 0xf);
    CHECK(std::strlen(kSlot443Item1Text) == kSlot443Item1TextLength && kSlot443Item1TextLength == 0xf);
    CHECK(std::strlen(kSlot443Group0Parent) == kSlot443Group0ParentLength && kSlot443Group0ParentLength == 0x4);
    CHECK(std::strlen(kSlot443Group0Caption) == kSlot443Group0CaptionLength && kSlot443Group0CaptionLength == 0xf);
    CHECK(kSlot443GroupFlags == 0x30);
    CHECK(sizeof(Slot443Objects) >= 2 * sizeof(ConCmdBoolVar) && sizeof(ConCmdBoolVar) == 0xc8);
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot443Objects o;
    poison(o);
    const Slot443Result r = init_slot443_4de370(o, creg, menu);
    CHECK(r.ok && r.var0.ok && r.var0.views_inert && r.var1.ok && r.var1.views_inert && r.group0.ok() && r.item0.ok && r.item0.views_inert && r.item1.ok && r.item1.views_inert);
    CHECK(r.var0.registration.ok && r.var0.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var1.registration.ok && r.var1.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_with_description(o.var0, kSlot443BoolFnTag, 0, "", 0);
    CHECK(same_view(o.var0.base.name, kSlot443Var0Name, kSlot443Var0NameLength));
    check_bool_with_description(o.var1, kSlot443BoolFnTag, 0, "", 0);
    CHECK(same_view(o.var1.base.name, kSlot443Var1Name, kSlot443Var1NameLength));
    CHECK(o.group0.kind == kMenuKindGroup && o.group0.target == nullptr && o.group0.flags == 0x30 && o.group0.registered == 0);
    CHECK(same_view(o.group0.label, kSlot443Group0Parent, kSlot443Group0ParentLength));   // the parent name
    CHECK(same_view(o.group0.text, kSlot443Group0Caption, kSlot443Group0CaptionLength) && same_view(o.group0.empty, kSlot443Group0Caption, kSlot443Group0CaptionLength));
    CHECK(o.item0.target == &o.var0 && o.item0.flags == 0x30 && o.item0.registered == 0);
    CHECK(same_view(o.item0.label, kSlot443Item0Label, kSlot443Item0LabelLength) && same_view(o.item0.text, kSlot443Item0Text, kSlot443Item0TextLength));
    CHECK(o.item0.range_min == 1.0f && o.item0.range_max == 10.0f);   // bool class slot 6 is 0: no narrowing
    CHECK(o.item1.target == &o.var1 && o.item1.flags == 0x30 && o.item1.registered == 0);
    CHECK(same_view(o.item1.label, kSlot443Item1Label, kSlot443Item1LabelLength) && same_view(o.item1.text, kSlot443Item1Text, kSlot443Item1TextLength));
    CHECK(o.item1.range_min == 1.0f && o.item1.range_max == 10.0f);   // bool class slot 6 is 0: no narrowing
    CHECK(creg.pending.size() == 2);
    CHECK(creg.atexit_log.size() == 6);
    check_atexit(creg, 0, 0x20b0, &o.var0);
    check_atexit(creg, 1, 0x20b0, &o.var1);
    CHECK(creg.atexit_log.at(2).dtor_rva == 0x27280);   // the menu root destructor, recorded by the first menu item
    check_atexit(creg, 3, 0x272d0, &o.group0);
    check_atexit(creg, 4, 0x272d0, &o.item0);
    check_atexit(creg, 5, 0x272d0, &o.item1);
    CHECK(menu.root.count == 3);
}

void section_failures() {
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;
    DebugMenuRegistry menu;
    Slot443Objects o;
    const Slot443Result r = init_slot443_4de370(o, creg, menu);
    CHECK(!r.ok && !r.var0.ok && !r.var0.registration.ok && !r.var1.ok && !r.var1.registration.ok && creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_443");
    return 0;
}
