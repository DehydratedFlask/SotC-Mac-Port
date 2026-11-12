// CPU-01-STARTUP-SLOT-351: fixture for startup constructor index 351 (0x7ae220): two bool and seven bounded float console variables, two 0x27120 sliders
// (parent `Shadow Camera`). Batch 8.
#include "l4_test_util.hpp"

#include "core/ctor_slot_351_7ae220.hpp"

namespace {
using namespace l4;

float bits_to_float(std::uint32_t b) {
    float f;
    std::memcpy(&f, &b, sizeof f);
    return f;
}

void section_constants() {
    CHECK(kSlot351Rva == 0x7ae220 && kSlot351Size == 2706 && kSlot351BoolFnTag == 0x15ce220 && kSlot351FloatFnTag == 0x15ce240);
    for (std::size_t n = 0; n < kSlot351BoolCount; ++n) CHECK(std::strlen(kSlot351BoolNames[n]) == kSlot351BoolNameLengths[n]);
    for (std::size_t n = 0; n < kSlot351FloatCount; ++n) CHECK(std::strlen(kSlot351FloatNames[n]) == kSlot351FloatNameLengths[n]);
    CHECK(std::strlen(kSlot351Parent) == kSlot351ParentLength);
    CHECK(is_known_float_arg_functor(kSlot351FloatFnTag));
    CHECK(offsetof(Slot351Objects, bvar1) == 0x325ada8 - kSlot351Obj0Rva);
    CHECK(offsetof(Slot351Objects, fvar0) == 0x325ae70 - kSlot351Obj0Rva);
    CHECK(offsetof(Slot351Objects, fvar1) == 0x325af48 - kSlot351Obj0Rva);
    CHECK(offsetof(Slot351Objects, fvar2) == 0x325b020 - kSlot351Obj0Rva);
    CHECK(offsetof(Slot351Objects, fvar3) == 0x325b0f8 - kSlot351Obj0Rva);
    CHECK(offsetof(Slot351Objects, fvar4) == 0x325b1d0 - kSlot351Obj0Rva);
    CHECK(offsetof(Slot351Objects, fvar5) == 0x325b2a8 - kSlot351Obj0Rva);
    CHECK(offsetof(Slot351Objects, fvar6) == 0x325b380 - kSlot351Obj0Rva);
    CHECK(offsetof(Slot351Objects, sitem0) == 0x325b458 - kSlot351Obj0Rva);
    CHECK(offsetof(Slot351Objects, sitem1) == 0x325b500 - kSlot351Obj0Rva);
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot351Objects o;
    poison(o);
    const Slot351Result r = init_slot351_7ae220(o, creg, menu);
    CHECK(r.ok);
    const ConCmdBoolVar* const bv[2] = {&o.bvar0, &o.bvar1};
    for (std::size_t n = 0; n < 2; ++n) {
        CHECK(r.bvar[n].ok && r.bvar[n].registration.ok);
        check_bool_with_description(*bv[n], kSlot351BoolFnTag, kSlot351BoolValues[n], "", 0);
        CHECK(same_view(bv[n]->base.name, kSlot351BoolNames[n], kSlot351BoolNameLengths[n]));
    }
    const ConCmdRangedVar* const fv[7] = {&o.fvar0, &o.fvar1, &o.fvar2, &o.fvar3, &o.fvar4, &o.fvar5, &o.fvar6};
    for (std::size_t n = 0; n < 7; ++n) {
        CHECK(r.fvar[n].ok && r.fvar[n].ctor_ok && r.fvar[n].atexit_recorded);
        const ConCmdRangedVar& v = *fv[n];
        CHECK(v.base.flags == 1 && v.base.registered == 1 && same_view(v.base.name, kSlot351FloatNames[n], kSlot351FloatNameLengths[n]));
        CHECK(v.value == kSlot351FloatValueBits[n] && v.default_value == kSlot351FloatValueBits[n] && v.min_engaged == 1 && v.max_engaged == 1);
        CHECK(dword(v, 0x6c) == kSlot351FloatLoBits[n] && dword(v, 0x74) == kSlot351FloatHiBits[n]);
    }
    const DebugMenuItem* const si[2] = {&o.sitem0, &o.sitem1};
    const ConCmdRangedVar* const st[2] = {&o.fvar5, &o.fvar6};
    for (std::size_t n = 0; n < 2; ++n) {
        CHECK(r.sitem[n].ok && r.sitem[n].ctor_ok && r.sitem[n].views_inert && r.sitem[n].atexit_recorded);
        const DebugMenuItem& it = *si[n];
        CHECK(it.target == st[n] && it.flags == 0x30 && it.kind == 2 && it.registered == 0);
        CHECK(same_view(it.label, kSlot351Parent, kSlot351ParentLength));
        CHECK(same_view(it.text, kSlot351FloatNames[5 + n], kSlot351FloatNameLengths[5 + n]));
        CHECK(it.range_min == bits_to_float(kSlot351SliderLoBits) && it.range_max == bits_to_float(kSlot351SliderHiBits));
    }
    CHECK(menu.root.count == 2 && menu.root.link.next == &o.sitem0.link);
    CHECK(creg.pending.size() == 9 && creg.pending.first() == &o.bvar0.base.pending);
    // b0, b1, f0..f6, [root destructor on the first item], s0, s1.
    CHECK(creg.atexit_log.size() == 12);
    check_atexit(creg, 0, 0x20b0, &o.bvar0);
    check_atexit(creg, 1, 0x20b0, &o.bvar1);
    check_atexit(creg, 2, 0x20f0, &o.fvar0);
    check_atexit(creg, 3, 0x20f0, &o.fvar1);
    check_atexit(creg, 4, 0x20f0, &o.fvar2);
    check_atexit(creg, 5, 0x20f0, &o.fvar3);
    check_atexit(creg, 6, 0x20f0, &o.fvar4);
    check_atexit(creg, 7, 0x20f0, &o.fvar5);
    check_atexit(creg, 8, 0x20f0, &o.fvar6);
    check_atexit(creg, 9, 0x27280, &menu.root);
    check_atexit(creg, 10, 0x272d0, &o.sitem0);
    check_atexit(creg, 11, 0x272d0, &o.sitem1);
}

void section_failures() {
    {   // First variable registration fails: nothing after it is reached.
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot351Objects o;
        const Slot351Result r = init_slot351_7ae220(o, creg, menu);
        CHECK(!r.ok && !r.bvar[0].ok && !r.fvar[0].ctor_ok && !r.sitem[0].ctor_ok);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {   // Menu parent without an insert contract: the variables are registered, the first slider registration fails visibly.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot351Objects o;
        const Slot351Result r = init_slot351_7ae220(o, creg, menu);
        CHECK(!r.ok && r.bvar[1].ok && r.fvar[6].ok && !r.sitem[0].ok && !r.sitem[1].ctor_ok);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_351");
    return 0;
}
