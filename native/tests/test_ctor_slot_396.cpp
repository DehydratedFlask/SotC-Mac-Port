// CPU-01-STARTUP-BATCH-7: fixture for startup constructor index 396 (0x650f20): six bool variables with 0x26db0 / 0x26ea0 toggles and two bounded
// float variables (tag 0x15cc568) with 0x27120 sliders, all under the menu parent `CP10-Anim`.
#include "l4_test_util.hpp"

#include "core/ctor_slot_396_650f20.hpp"

namespace {
using namespace l4;

float bits_to_float(std::uint32_t b) {
    float f;
    std::memcpy(&f, &b, sizeof f);
    return f;
}

void section_constants() {
    CHECK(kSlot396Rva == 0x650f20 && kSlot396Size == 2845 && kSlot396BoolFnTag == 0x15cc548 && kSlot396FloatFnTag == 0x15cc568);
    for (std::size_t n = 0; n < kSlot396BoolCount; ++n) CHECK(std::strlen(kSlot396BoolNames[n]) == kSlot396BoolNameLengths[n]);
    for (std::size_t n = 0; n < kSlot396FloatCount; ++n) CHECK(std::strlen(kSlot396FloatNames[n]) == kSlot396FloatNameLengths[n]);
    CHECK(std::strlen(kSlot396Parent) == kSlot396ParentLength && std::strlen(kSlot396ToggleText) == kSlot396ToggleTextLength);
    CHECK(is_known_float_arg_functor(kSlot396FloatFnTag));
    CHECK(offsetof(Slot396Objects, bitem0) == 0x3010348 - kSlot396Bool0Rva);
    CHECK(offsetof(Slot396Objects, bvar1) == 0x30103f0 - kSlot396Bool0Rva);
    CHECK(offsetof(Slot396Objects, bitem1) == 0x30104b8 - kSlot396Bool0Rva);
    CHECK(offsetof(Slot396Objects, bvar2) == 0x3010560 - kSlot396Bool0Rva);
    CHECK(offsetof(Slot396Objects, bitem2) == 0x3010628 - kSlot396Bool0Rva);
    CHECK(offsetof(Slot396Objects, bvar3) == 0x30106d0 - kSlot396Bool0Rva);
    CHECK(offsetof(Slot396Objects, bitem3) == 0x3010798 - kSlot396Bool0Rva);
    CHECK(offsetof(Slot396Objects, bvar4) == 0x3010840 - kSlot396Bool0Rva);
    CHECK(offsetof(Slot396Objects, bitem4) == 0x3010908 - kSlot396Bool0Rva);
    CHECK(offsetof(Slot396Objects, fvar0) == 0x30109b0 - kSlot396Bool0Rva);
    CHECK(offsetof(Slot396Objects, fitem0) == 0x3010a88 - kSlot396Bool0Rva);
    CHECK(offsetof(Slot396Objects, fvar1) == 0x3010b30 - kSlot396Bool0Rva);
    CHECK(offsetof(Slot396Objects, fitem1) == 0x3010c08 - kSlot396Bool0Rva);
    CHECK(offsetof(Slot396Objects, bvar5) == 0x3010cb0 - kSlot396Bool0Rva);
    CHECK(offsetof(Slot396Objects, bitem5) == 0x3010d78 - kSlot396Bool0Rva);
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot396Objects o;
    poison(o);
    const Slot396Result r = init_slot396_650f20(o, creg, menu);
    CHECK(r.ok);
    const ConCmdBoolVar* bvars[6] = {&o.bvar0, &o.bvar1, &o.bvar2, &o.bvar3, &o.bvar4, &o.bvar5};
    const DebugMenuItem* bitems[6] = {&o.bitem0, &o.bitem1, &o.bitem2, &o.bitem3, &o.bitem4, &o.bitem5};
    for (std::size_t n = 0; n < 6; ++n) {
        CHECK(r.bvar[n].ok && r.bvar[n].registration.ok && r.bitem[n].ok && r.bitem[n].ctor_ok && r.bitem[n].views_inert &&
              r.bitem[n].atexit_recorded);
        check_bool_with_description(*bvars[n], kSlot396BoolFnTag, kSlot396BoolValues[n], "", 0);
        CHECK(same_view(bvars[n]->base.name, kSlot396BoolNames[n], kSlot396BoolNameLengths[n]));
        const DebugMenuItem& it = *bitems[n];
        CHECK(it.target == bvars[n] && it.flags == 0x30 && it.kind == 2 && it.registered == 0);
        CHECK(same_view(it.label, kSlot396Parent, kSlot396ParentLength));
        CHECK(it.range_min == 1.0f && it.range_max == 10.0f);   // bool class: the gate never narrows
        if (n == 3) {
            CHECK(same_view(it.text, kSlot396ToggleText, kSlot396ToggleTextLength));   // the 0x26ea0 caption
        } else {
            CHECK(same_view(it.text, kSlot396BoolNames[n], kSlot396BoolNameLengths[n]));   // 0x26db0: the variable's own name
        }
    }
    const ConCmdRangedVar* fvars[2] = {&o.fvar0, &o.fvar1};
    const DebugMenuItem* fitems[2] = {&o.fitem0, &o.fitem1};
    for (std::size_t n = 0; n < 2; ++n) {
        CHECK(r.fvar[n].ok && r.fvar[n].ctor_ok && r.fvar[n].atexit_recorded && r.fitem[n].ok);
        const ConCmdRangedVar& v = *fvars[n];
        CHECK(v.base.flags == 1 && v.base.registered == 1 && same_view(v.base.name, kSlot396FloatNames[n], kSlot396FloatNameLengths[n]));
        CHECK(v.value == kSlot396FloatValueBits[n] && v.default_value == kSlot396FloatValueBits[n] && v.min_engaged == 1 && v.max_engaged == 1);
        const DebugMenuItem& it = *fitems[n];
        CHECK(it.target == fvars[n] && it.flags == 0x30 && it.kind == 2);
        CHECK(same_view(it.label, kSlot396Parent, kSlot396ParentLength));
        CHECK(same_view(it.text, kSlot396FloatNames[n], kSlot396FloatNameLengths[n]));
        CHECK(it.range_min == bits_to_float(kSlot396SliderLoBits) && it.range_max == bits_to_float(kSlot396SliderHiBits));
    }
    CHECK(menu.root.count == 8 && menu.root.link.next == &o.bitem0.link);
    CHECK(creg.pending.size() == 8 && creg.pending.first() == &o.bvar0.base.pending);
    // b0, [root destructor on the first item], i0, b1, i1, b2, i2, b3, i3, b4, i4, f0, fi0, f1, fi1, b5, i5.
    CHECK(creg.atexit_log.size() == 17);
    check_atexit(creg, 0, 0x20b0, &o.bvar0);
    check_atexit(creg, 1, 0x27280, &menu.root);
    check_atexit(creg, 2, 0x272d0, &o.bitem0);
    check_atexit(creg, 3, 0x20b0, &o.bvar1);
    check_atexit(creg, 4, 0x272d0, &o.bitem1);
    check_atexit(creg, 5, 0x20b0, &o.bvar2);
    check_atexit(creg, 6, 0x272d0, &o.bitem2);
    check_atexit(creg, 7, 0x20b0, &o.bvar3);
    check_atexit(creg, 8, 0x272d0, &o.bitem3);
    check_atexit(creg, 9, 0x20b0, &o.bvar4);
    check_atexit(creg, 10, 0x272d0, &o.bitem4);
    check_atexit(creg, 11, 0x20f0, &o.fvar0);
    check_atexit(creg, 12, 0x272d0, &o.fitem0);
    check_atexit(creg, 13, 0x20f0, &o.fvar1);
    check_atexit(creg, 14, 0x272d0, &o.fitem1);
    check_atexit(creg, 15, 0x20b0, &o.bvar5);
    check_atexit(creg, 16, 0x272d0, &o.bitem5);
}

void section_failures() {
    {
        // First variable registration fails: nothing after it is reached.
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot396Objects o;
        const Slot396Result r = init_slot396_650f20(o, creg, menu);
        CHECK(!r.ok && !r.bvar[0].ok && !r.bitem[0].ctor_ok && !r.fvar[0].ctor_ok);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent without an insert contract: the first toggle registration fails visibly; the later sites are not reached.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot396Objects o;
        const Slot396Result r = init_slot396_650f20(o, creg, menu);
        CHECK(!r.ok && r.bvar[0].ok && !r.bitem[0].ok && !r.bvar[1].ok && !r.fvar[0].ctor_ok);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_396");
    return 0;
}
