// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 4): fixture for startup constructor index 368 (0x73df50): 3 BOOL console variable(s), 3 0x26ea0 toggle item(s), then 18 literal dword stores into static cells.
#include "l4_test_util.hpp"

#include "core/ctor_slot_368_73df50.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot368Rva == 0x73df50 && kSlot368Size == 1317);
    CHECK(kSlot368BoolFnTag == 0x15cd1a0);
    CHECK(std::strlen(kSlot368Var0Name) == kSlot368Var0NameLength && kSlot368Var0NameLength == 0x1c);
    CHECK(std::strlen(kSlot368Var1Name) == kSlot368Var1NameLength && kSlot368Var1NameLength == 0x28);
    CHECK(std::strlen(kSlot368Var2Name) == kSlot368Var2NameLength && kSlot368Var2NameLength == 0x1c);
    CHECK(std::strlen(kSlot368Item0Label) == kSlot368Item0LabelLength && kSlot368Item0LabelLength == 0x9);
    CHECK(std::strlen(kSlot368Item0Text) == kSlot368Item0TextLength && kSlot368Item0TextLength == 0x17);
    CHECK(std::strlen(kSlot368Item1Label) == kSlot368Item1LabelLength && kSlot368Item1LabelLength == 0x9);
    CHECK(std::strlen(kSlot368Item1Text) == kSlot368Item1TextLength && kSlot368Item1TextLength == 0x25);
    CHECK(std::strlen(kSlot368Item2Label) == kSlot368Item2LabelLength && kSlot368Item2LabelLength == 0x9);
    CHECK(std::strlen(kSlot368Item2Text) == kSlot368Item2TextLength && kSlot368Item2TextLength == 0x17);
    CHECK(kSlot368StaticStoreCount == 18);
    static constexpr std::uint64_t kRvas[] = {0x2dcc0ac, 0x2dcc0f4, 0x2dcc824, 0x2dccda8, 0x2dccdac, 0x2dccdb0, 0x2dcce80, 0x2dcce84, 0x2dcce88, 0x2dccc40, 0x2dccc44, 0x2dcd254, 0x2dcc184, 0x2dcc828, 0x2dccc48, 0x2dcd258, 0x2dcd620, 0x2dcd628};
    static constexpr std::uint32_t kBits[] = {0x00000000, 0x3f000000, 0xbfcccccd, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x3f800000, 0x3f800000, 0x40400000, 0x3ecccccd, 0x00000000, 0xc0833333, 0xbf800000, 0xc019999a, 0xbf800000, 0x3f666666};
    for (std::size_t i = 0; i < kSlot368StaticStoreCount; ++i) {
        CHECK(kSlot368StaticStores[i].rva == kRvas[i] && kSlot368StaticStores[i].bits == kBits[i]);
        for (std::size_t j = 0; j < i; ++j) CHECK(kRvas[j] != kRvas[i]);   // distinct cells
    }
    CHECK(sizeof(Slot368Objects) >= 3 * sizeof(ConCmdBoolVar) && sizeof(ConCmdBoolVar) == 0xc8);
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot368Objects o;
    poison(o);
    const Slot368Result r = init_slot368_73df50(o, creg, menu);
    CHECK(r.ok && r.var0.ok && r.var0.views_inert && r.var1.ok && r.var1.views_inert && r.var2.ok && r.var2.views_inert && r.item0.ok && r.item0.views_inert && r.item1.ok && r.item1.views_inert && r.item2.ok && r.item2.views_inert);
    CHECK(r.var0.registration.ok && r.var0.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var1.registration.ok && r.var1.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var2.registration.ok && r.var2.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_with_description(o.var0, kSlot368BoolFnTag, 1, "", 0);
    CHECK(same_view(o.var0.base.name, kSlot368Var0Name, kSlot368Var0NameLength));
    check_bool_with_description(o.var1, kSlot368BoolFnTag, 1, "", 0);
    CHECK(same_view(o.var1.base.name, kSlot368Var1Name, kSlot368Var1NameLength));
    check_bool_with_description(o.var2, kSlot368BoolFnTag, 0, "", 0);
    CHECK(same_view(o.var2.base.name, kSlot368Var2Name, kSlot368Var2NameLength));
    CHECK(o.item0.target == &o.var0 && o.item0.flags == 0x30 && o.item0.registered == 0);
    CHECK(same_view(o.item0.label, kSlot368Item0Label, kSlot368Item0LabelLength) && same_view(o.item0.text, kSlot368Item0Text, kSlot368Item0TextLength));
    CHECK(o.item0.range_min == 1.0f && o.item0.range_max == 10.0f);   // bool class slot 6 is 0: no narrowing
    CHECK(o.item1.target == &o.var1 && o.item1.flags == 0x30 && o.item1.registered == 0);
    CHECK(same_view(o.item1.label, kSlot368Item1Label, kSlot368Item1LabelLength) && same_view(o.item1.text, kSlot368Item1Text, kSlot368Item1TextLength));
    CHECK(o.item1.range_min == 1.0f && o.item1.range_max == 10.0f);   // bool class slot 6 is 0: no narrowing
    CHECK(o.item2.target == &o.var2 && o.item2.flags == 0x30 && o.item2.registered == 0);
    CHECK(same_view(o.item2.label, kSlot368Item2Label, kSlot368Item2LabelLength) && same_view(o.item2.text, kSlot368Item2Text, kSlot368Item2TextLength));
    CHECK(o.item2.range_min == 1.0f && o.item2.range_max == 10.0f);   // bool class slot 6 is 0: no narrowing
    CHECK(r.statics_written == kSlot368StaticStoreCount);
    for (std::size_t i = 0; i < kSlot368StaticStoreCount; ++i) CHECK(o.statics[i] == kSlot368StaticStores[i].bits);
    CHECK(creg.pending.size() == 3);
    CHECK(creg.atexit_log.size() == 7);
    check_atexit(creg, 0, 0x20b0, &o.var0);
    check_atexit(creg, 1, 0x20b0, &o.var1);
    check_atexit(creg, 2, 0x20b0, &o.var2);
    CHECK(creg.atexit_log.at(3).dtor_rva == 0x27280);   // the menu root destructor, recorded by the first menu item
    check_atexit(creg, 4, 0x272d0, &o.item0);
    check_atexit(creg, 5, 0x272d0, &o.item1);
    check_atexit(creg, 6, 0x272d0, &o.item2);
    CHECK(menu.root.count == 3);
}

void section_failures() {
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;
    DebugMenuRegistry menu;
    Slot368Objects o;
    const Slot368Result r = init_slot368_73df50(o, creg, menu);
    CHECK(!r.ok && !r.var0.ok && !r.var0.registration.ok && !r.var1.ok && !r.var1.registration.ok && !r.var2.ok && !r.var2.registration.ok && creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_368");
    return 0;
}
