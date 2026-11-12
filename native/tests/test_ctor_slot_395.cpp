// CPU-01-STARTUP-BATCH-9: fixture for startup constructor index 395 (0x65af60): one bool variable, three bounded float variables (tag 0x15cc5a8)
// and four 0x26db0 toggles (the first over float targets) under the menu parent `Shadow Player`.
#include "l4_test_util.hpp"

#include "core/ctor_slot_395_65af60.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot395Rva == 0x65af60 && kSlot395Size == 1525 && kSlot395BoolFnTag == 0x15cc588 && kSlot395FloatFnTag == 0x15cc5a8);
    CHECK(std::strlen(kSlot395BoolName) == kSlot395BoolNameLength && std::strlen(kSlot395Parent) == kSlot395ParentLength);
    for (std::size_t n = 0; n < kSlot395FloatCount; ++n) CHECK(std::strlen(kSlot395FloatNames[n]) == kSlot395FloatNameLengths[n]);
    CHECK(is_known_float_arg_functor(kSlot395FloatFnTag));
    CHECK(offsetof(Slot395Objects, fvar0) == 0x3010f08 - kSlot395Bool0Rva);
    CHECK(offsetof(Slot395Objects, fvar1) == 0x3010fe0 - kSlot395Bool0Rva);
    CHECK(offsetof(Slot395Objects, fvar2) == 0x30110b8 - kSlot395Bool0Rva);
    CHECK(offsetof(Slot395Objects, bitem) == 0x3011190 - kSlot395Bool0Rva);
    CHECK(offsetof(Slot395Objects, fitem0) == 0x3011238 - kSlot395Bool0Rva);
    CHECK(offsetof(Slot395Objects, fitem1) == 0x30112e0 - kSlot395Bool0Rva);
    CHECK(offsetof(Slot395Objects, fitem2) == 0x3011388 - kSlot395Bool0Rva);
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot395Objects o;
    poison(o);
    const Slot395Result r = init_slot395_65af60(o, creg, menu);
    CHECK(r.ok && r.bvar.ok && r.bvar.registration.ok && r.bitem.ok);
    check_bool_with_description(o.bvar, kSlot395BoolFnTag, kSlot395BoolValue, "", 0);
    CHECK(same_view(o.bvar.base.name, kSlot395BoolName, kSlot395BoolNameLength));
    CHECK(o.bitem.target == &o.bvar && o.bitem.flags == 0x30 && o.bitem.kind == 2 && o.bitem.range_min == 1.0f && o.bitem.range_max == 10.0f);
    CHECK(same_view(o.bitem.label, kSlot395Parent, kSlot395ParentLength) && same_view(o.bitem.text, kSlot395BoolName, kSlot395BoolNameLength));
    const ConCmdRangedVar* fvars[3] = {&o.fvar0, &o.fvar1, &o.fvar2};
    const DebugMenuItem* fitems[3] = {&o.fitem0, &o.fitem1, &o.fitem2};
    for (std::size_t n = 0; n < 3; ++n) {
        CHECK(r.fvar[n].ok && r.fvar[n].ctor_ok && r.fvar[n].atexit_recorded && r.fitem[n].ok && r.fitem[n].ctor_ok &&
              r.fitem[n].views_inert && r.fitem[n].atexit_recorded);
        const ConCmdRangedVar& v = *fvars[n];
        CHECK(v.base.flags == 1 && v.base.registered == 1 && same_view(v.base.name, kSlot395FloatNames[n], kSlot395FloatNameLengths[n]));
        CHECK(v.value == kSlot395FloatValueBits && v.default_value == kSlot395FloatValueBits && v.min_engaged == 1 && v.max_engaged == 1);
        std::uint32_t lo = 0;
        std::uint32_t hi = 0;
        std::memcpy(&lo, v.min_value, 4);
        std::memcpy(&hi, v.max_value, 4);
        CHECK(lo == kSlot395FloatLoBits && hi == kSlot395FloatHiBits[n]);
        const DebugMenuItem& it = *fitems[n];
        CHECK(it.target == fvars[n] && it.flags == 0x30 && it.kind == 2 && it.registered == 0);
        CHECK(same_view(it.label, kSlot395Parent, kSlot395ParentLength));
        CHECK(same_view(it.text, kSlot395FloatNames[n], kSlot395FloatNameLengths[n]));
        // The float class answers slot3 = 1, slot6 = 3: the only narrowing case of the shared 0x26db0 range rule.
        CHECK(it.range_min == 0.1f && it.range_max == 0.5f);
    }
    CHECK(menu.root.count == 4 && menu.root.link.next == &o.bitem.link);
    CHECK(creg.pending.size() == 4 && creg.pending.first() == &o.bvar.base.pending);
    // bvar, f0, f1, f2, [root destructor on the first item], bitem, fitem0, fitem1, fitem2.
    CHECK(creg.atexit_log.size() == 9);
    check_atexit(creg, 0, 0x20b0, &o.bvar);
    check_atexit(creg, 1, 0x20f0, &o.fvar0);
    check_atexit(creg, 2, 0x20f0, &o.fvar1);
    check_atexit(creg, 3, 0x20f0, &o.fvar2);
    check_atexit(creg, 4, 0x27280, &menu.root);
    check_atexit(creg, 5, 0x272d0, &o.bitem);
    check_atexit(creg, 6, 0x272d0, &o.fitem0);
    check_atexit(creg, 7, 0x272d0, &o.fitem1);
    check_atexit(creg, 8, 0x272d0, &o.fitem2);
}

void section_failures() {
    {
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot395Objects o;
        const Slot395Result r = init_slot395_65af60(o, creg, menu);
        CHECK(!r.ok && !r.bvar.ok && !r.fvar[0].ctor_ok && !r.bitem.ctor_ok);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot395Objects o;
        const Slot395Result r = init_slot395_65af60(o, creg, menu);
        CHECK(!r.ok && r.bvar.ok && r.fvar[2].ok && !r.bitem.ok && !r.fitem[0].ctor_ok);
    }
    {
        // A target that is not the lifted float class is refused with the item untouched.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        ConCmdRangedVar bad{};
        DebugMenuItem item{};
        const MenuSiteResult r = init_toggle_site_26db0_float(item, bad, kSlot395Parent, kSlot395ParentLength, creg, menu);
        CHECK(!r.ok && !r.ctor_ok && menu.root.count == 0);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_395");
    return 0;
}
