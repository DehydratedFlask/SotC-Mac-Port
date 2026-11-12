// CPU-01-STARTUP-SLOT-521: fixture for startup constructor index 521 (0x17bf50): 6 bool and 1 unbounded float console variables. Batch 8.
#include "l4_test_util.hpp"

#include "core/ctor_slot_521_17bf50.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot521Rva == 0x17bf50 && kSlot521Size == 1674);
    CHECK(kSlot521BoolFnTag == 0x1598810 && kSlot521FloatFnTag == 0x15987f0);
    CHECK(is_float_functor_tag(kSlot521FloatFnTag) && !is_int_functor_tag(kSlot521FloatFnTag));
    for (std::size_t n = 0; n < kSlot521BoolCount; ++n) {
        CHECK(std::strlen(kSlot521BoolNames[n]) == kSlot521BoolNameLengths[n]);
        CHECK(std::strlen(kSlot521BoolDescriptions[n]) == kSlot521BoolDescriptionLengths[n]);
    }
    for (std::size_t n = 0; n < kSlot521FloatCount; ++n) {
        CHECK(std::strlen(kSlot521FloatNames[n]) == kSlot521FloatNameLengths[n]);
        CHECK(std::strlen(kSlot521FloatDescriptions[n]) == kSlot521FloatDescriptionLengths[n]);
    }
    CHECK(offsetof(Slot521Objects, fvar0) == 0x2ee3e18 - kSlot521Obj0Rva);
    CHECK(offsetof(Slot521Objects, bvar0) == 0x2ee3ef0 - kSlot521Obj0Rva);
    CHECK(offsetof(Slot521Objects, bvar1) == 0x2ee3fb8 - kSlot521Obj0Rva);
    CHECK(offsetof(Slot521Objects, bvar2) == 0x2ee4080 - kSlot521Obj0Rva);
    CHECK(offsetof(Slot521Objects, bvar3) == 0x2ee4148 - kSlot521Obj0Rva);
    CHECK(offsetof(Slot521Objects, bvar4) == 0x2ee4240 - kSlot521Obj0Rva);
    CHECK(offsetof(Slot521Objects, bvar5) == 0x2ee4308 - kSlot521Obj0Rva);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot521Objects o;
    poison(o);
    const Slot521Result r = init_slot521_17bf50(o, creg);
    CHECK(r.ok && r.refusal == HelperRefusal::none);
    const ConCmdBoolVar* const bv[kSlot521BoolCount] = {&o.bvar0, &o.bvar1, &o.bvar2, &o.bvar3, &o.bvar4, &o.bvar5};
    for (std::size_t n = 0; n < kSlot521BoolCount; ++n) {
        CHECK(r.bvar[n].ok && r.bvar[n].registration.ok && r.bvar[n].views_inert && r.bvar[n].atexit_recorded);
        check_bool_with_description(*bv[n], kSlot521BoolFnTag, kSlot521BoolValues[n], kSlot521BoolDescriptions[n], kSlot521BoolDescriptionLengths[n]);
        CHECK(same_view(bv[n]->base.name, kSlot521BoolNames[n], kSlot521BoolNameLengths[n]));
    }
    const ConCmdRangedVar* const fv[kSlot521FloatCount] = {&o.fvar0};
    for (std::size_t n = 0; n < kSlot521FloatCount; ++n) {
        CHECK(r.fvar[n].ok && r.fvar[n].refusal == HelperRefusal::none && r.fvar[n].ctor_ok && r.fvar[n].functor_destroy_ok &&
              r.fvar[n].views_inert && r.fvar[n].atexit_recorded && r.fvar[n].registration.ok);
        CHECK(fv[n]->base.vptr == kRangedFloatFinalVptr && fv[n]->value_ref == &fv[n]->value && fv[n]->base.registered == 1 && fv[n]->base.flags == 1);
        CHECK(fv[n]->value == kSlot521FloatValueBits[n] && fv[n]->default_value == kSlot521FloatValueBits[n] && fv[n]->min_engaged == 0 && fv[n]->max_engaged == 0);
        CHECK(fv[n]->fn_payload == 0 && fv[n]->fn_aux == 0 && fv[n]->fn_vptr == kRangedFloatFnVtbl);
        CHECK(same_view(fv[n]->base.name, kSlot521FloatNames[n], kSlot521FloatNameLengths[n]));
        CHECK(same_view(fv[n]->base.description, kSlot521FloatDescriptions[n], kSlot521FloatDescriptionLengths[n]));
    }
    CHECK(creg.pending.size() == 7 && creg.pending.first() == &o.fvar0.base.pending);
    CHECK(o.fvar0.base.pending.next == &o.bvar0.base.pending);
    CHECK(o.bvar0.base.pending.next == &o.bvar1.base.pending);
    CHECK(o.bvar1.base.pending.next == &o.bvar2.base.pending);
    CHECK(o.bvar2.base.pending.next == &o.bvar3.base.pending);
    CHECK(o.bvar3.base.pending.next == &o.bvar4.base.pending);
    CHECK(o.bvar4.base.pending.next == &o.bvar5.base.pending);
    CHECK(o.bvar5.base.pending.next == &creg.pending.head);
    CHECK(creg.atexit_log.size() == 7);
    check_atexit(creg, 0, 0x20f0, &o.fvar0);
    check_atexit(creg, 1, 0x20b0, &o.bvar0);
    check_atexit(creg, 2, 0x20b0, &o.bvar1);
    check_atexit(creg, 3, 0x20b0, &o.bvar2);
    check_atexit(creg, 4, 0x20b0, &o.bvar3);
    check_atexit(creg, 5, 0x20b0, &o.bvar4);
    check_atexit(creg, 6, 0x20b0, &o.bvar5);
}

void section_failures() {
    {   // manager without an insert contract: every registration fails (the original has no early exit)
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot521Objects o;
        const Slot521Result r = init_slot521_17bf50(o, creg);
        CHECK(!r.ok && creg.pending.size() == 0);
    }
    {   // a tag outside the float family is refused by the wrapper and nothing is registered
        ConCmdRegistry creg;
        ConCmdRangedVar v;
        poison(v);
        HelperFloatSiteSpec spec{kSlot521FloatNames[0], kSlot521FloatNameLengths[0], "", 0, 1, 0x1234567, 0, true};
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
    std::puts("OK test_ctor_slot_521");
    return 0;
}
