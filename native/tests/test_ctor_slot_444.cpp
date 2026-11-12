// CPU-01-STARTUP-SLOT-444: fixture for startup constructor index 444 (0x4d3f10): 1 bool and 4 unbounded float console variables. Batch 8.
#include "l4_test_util.hpp"

#include "core/ctor_slot_444_4d3f10.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot444Rva == 0x4d3f10 && kSlot444Size == 1276);
    CHECK(kSlot444BoolFnTag == 0x15c1be8 && kSlot444FloatFnTag == 0x15c1bc8);
    CHECK(is_float_functor_tag(kSlot444FloatFnTag) && !is_int_functor_tag(kSlot444FloatFnTag));
    for (std::size_t n = 0; n < kSlot444BoolCount; ++n) {
        CHECK(std::strlen(kSlot444BoolNames[n]) == kSlot444BoolNameLengths[n]);
        CHECK(std::strlen(kSlot444BoolDescriptions[n]) == kSlot444BoolDescriptionLengths[n]);
    }
    for (std::size_t n = 0; n < kSlot444FloatCount; ++n) {
        CHECK(std::strlen(kSlot444FloatNames[n]) == kSlot444FloatNameLengths[n]);
        CHECK(std::strlen(kSlot444FloatDescriptions[n]) == kSlot444FloatDescriptionLengths[n]);
    }
    CHECK(offsetof(Slot444Objects, fvar0) == 0x2efb348 - kSlot444Obj0Rva);
    CHECK(offsetof(Slot444Objects, fvar1) == 0x2efb420 - kSlot444Obj0Rva);
    CHECK(offsetof(Slot444Objects, fvar2) == 0x2efb4f8 - kSlot444Obj0Rva);
    CHECK(offsetof(Slot444Objects, fvar3) == 0x2efb5d0 - kSlot444Obj0Rva);
    CHECK(offsetof(Slot444Objects, bvar0) == 0x2efb6a8 - kSlot444Obj0Rva);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot444Objects o;
    poison(o);
    const Slot444Result r = init_slot444_4d3f10(o, creg);
    CHECK(r.ok && r.refusal == HelperRefusal::none);
    const ConCmdBoolVar* const bv[kSlot444BoolCount] = {&o.bvar0};
    for (std::size_t n = 0; n < kSlot444BoolCount; ++n) {
        CHECK(r.bvar[n].ok && r.bvar[n].registration.ok && r.bvar[n].views_inert && r.bvar[n].atexit_recorded);
        check_bool_with_description(*bv[n], kSlot444BoolFnTag, kSlot444BoolValues[n], kSlot444BoolDescriptions[n], kSlot444BoolDescriptionLengths[n]);
        CHECK(same_view(bv[n]->base.name, kSlot444BoolNames[n], kSlot444BoolNameLengths[n]));
    }
    const ConCmdRangedVar* const fv[kSlot444FloatCount] = {&o.fvar0, &o.fvar1, &o.fvar2, &o.fvar3};
    for (std::size_t n = 0; n < kSlot444FloatCount; ++n) {
        CHECK(r.fvar[n].ok && r.fvar[n].refusal == HelperRefusal::none && r.fvar[n].ctor_ok && r.fvar[n].functor_destroy_ok &&
              r.fvar[n].views_inert && r.fvar[n].atexit_recorded && r.fvar[n].registration.ok);
        check_float_common(*fv[n], false, 0, 0, kSlot444FloatValueBits[n]);
        CHECK(same_view(fv[n]->base.name, kSlot444FloatNames[n], kSlot444FloatNameLengths[n]));
        CHECK(same_view(fv[n]->base.description, kSlot444FloatDescriptions[n], kSlot444FloatDescriptionLengths[n]));
    }
    CHECK(creg.pending.size() == 5 && creg.pending.first() == &o.fvar0.base.pending);
    CHECK(o.fvar0.base.pending.next == &o.fvar1.base.pending);
    CHECK(o.fvar1.base.pending.next == &o.fvar2.base.pending);
    CHECK(o.fvar2.base.pending.next == &o.fvar3.base.pending);
    CHECK(o.fvar3.base.pending.next == &o.bvar0.base.pending);
    CHECK(o.bvar0.base.pending.next == &creg.pending.head);
    CHECK(creg.atexit_log.size() == 5);
    check_atexit(creg, 0, 0x20f0, &o.fvar0);
    check_atexit(creg, 1, 0x20f0, &o.fvar1);
    check_atexit(creg, 2, 0x20f0, &o.fvar2);
    check_atexit(creg, 3, 0x20f0, &o.fvar3);
    check_atexit(creg, 4, 0x20b0, &o.bvar0);
}

void section_failures() {
    {   // manager without an insert contract: every registration fails (the original has no early exit)
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot444Objects o;
        const Slot444Result r = init_slot444_4d3f10(o, creg);
        CHECK(!r.ok && creg.pending.size() == 0);
    }
    {   // a tag outside the float family is refused by the wrapper and nothing is registered
        ConCmdRegistry creg;
        ConCmdRangedVar v;
        poison(v);
        HelperFloatSiteSpec spec{kSlot444FloatNames[0], kSlot444FloatNameLengths[0], "", 0, 1, 0x1234567, 0, true};
        const HelperFloatSiteResult bad = helper_init_unbounded_float_var_site(v, spec, creg);
        CHECK(!bad.ok && !bad.ctor_ok && bad.refusal == HelperRefusal::unadmitted_arg_functor);
        spec.arg_fn_tag = 0x15944a0;   // an int-family tag is not a float functor
        CHECK(!helper_init_unbounded_float_var_site(v, spec, creg).ok && creg.pending.size() == 0);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_444");
    return 0;
}
