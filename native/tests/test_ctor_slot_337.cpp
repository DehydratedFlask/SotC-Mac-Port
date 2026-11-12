// CPU-01-STARTUP-SLOT-337: fixture for startup constructor index 337 (0x7e81d0): two bool and five unbounded float console variables
// (the "wormhole" fade page) plus one 0x30 byte name holder. Batch 8.
#include "l4_test_util.hpp"

#include "core/ctor_slot_337_7e81d0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot337Rva == 0x7e81d0 && kSlot337Size == 1849);
    CHECK(kSlot337BoolFnTag == 0x15ce480 && kSlot337FloatFnTag == 0x15ce4a0);
    CHECK(is_float_functor_tag(kSlot337FloatFnTag) && !is_int_functor_tag(kSlot337FloatFnTag));
    for (std::size_t n = 0; n < kSlot337BoolCount; ++n) CHECK(std::strlen(kSlot337BoolNames[n]) == kSlot337BoolNameLengths[n]);
    for (std::size_t n = 0; n < kSlot337FloatCount; ++n) CHECK(std::strlen(kSlot337FloatNames[n]) == kSlot337FloatNameLengths[n]);
    CHECK(std::strlen(kSlot337HolderName) == kSlot337HolderNameLength && kSlot337HolderNameLength == 0xe);
    CHECK(kSlot337HolderDtorRva == 0x8ef00 && kSlot337HolderVptr == 0x15976b0);
    CHECK(kSlot337Float0Rva - kSlot337Bool0Rva == offsetof(Slot337Objects, fvar0) && kSlot337HolderRva - kSlot337Bool0Rva == offsetof(Slot337Objects, holder));
}

void section_alone() {
    ConCmdRegistry creg;
    Slot337Objects o;
    poison(o);
    const Slot337Result r = init_slot337_7e81d0(o, creg);
    CHECK(r.ok && r.refusal == HelperRefusal::none && r.holder_ok);
    const ConCmdBoolVar* const bv[kSlot337BoolCount] = {&o.bvar0, &o.bvar1};
    for (std::size_t n = 0; n < kSlot337BoolCount; ++n) {
        CHECK(r.bvar[n].ok && r.bvar[n].registration.ok && r.bvar[n].views_inert && r.bvar[n].atexit_recorded);
        check_bool_with_description(*bv[n], kSlot337BoolFnTag, kSlot337BoolValues[n], "", 0);
        CHECK(same_view(bv[n]->base.name, kSlot337BoolNames[n], kSlot337BoolNameLengths[n]));
    }
    const ConCmdRangedVar* const fv[kSlot337FloatCount] = {&o.fvar0, &o.fvar1, &o.fvar2, &o.fvar3, &o.fvar4};
    for (std::size_t n = 0; n < kSlot337FloatCount; ++n) {
        CHECK(r.fvar[n].ok && r.fvar[n].refusal == HelperRefusal::none && r.fvar[n].ctor_ok && r.fvar[n].functor_destroy_ok &&
              r.fvar[n].views_inert && r.fvar[n].atexit_recorded && r.fvar[n].registration.ok);
        check_float_common(*fv[n], false, 0, 0, kSlot337FloatValueBits[n]);
        CHECK(same_view(fv[n]->base.name, kSlot337FloatNames[n], kSlot337FloatNameLengths[n]));
    }
    CHECK(o.holder.vptr == kSlot337HolderVptr && o.holder.tail == 0 && same_view(o.holder.name, kSlot337HolderName, kSlot337HolderNameLength));
    CHECK(creg.pending.size() == 7 && creg.pending.first() == &o.bvar0.base.pending);
    CHECK(o.bvar0.base.pending.next == &o.bvar1.base.pending && o.bvar1.base.pending.next == &o.fvar0.base.pending);
    CHECK(o.fvar0.base.pending.next == &o.fvar1.base.pending && o.fvar1.base.pending.next == &o.fvar2.base.pending);
    CHECK(o.fvar2.base.pending.next == &o.fvar3.base.pending && o.fvar3.base.pending.next == &o.fvar4.base.pending);
    CHECK(o.fvar4.base.pending.next == &creg.pending.head);
    CHECK(creg.atexit_log.size() == 8);
    check_atexit(creg, 0, 0x20b0, &o.bvar0);
    check_atexit(creg, 1, 0x20b0, &o.bvar1);
    check_atexit(creg, 2, 0x20f0, &o.fvar0);
    check_atexit(creg, 3, 0x20f0, &o.fvar1);
    check_atexit(creg, 4, 0x20f0, &o.fvar2);
    check_atexit(creg, 5, 0x20f0, &o.fvar3);
    check_atexit(creg, 6, 0x20f0, &o.fvar4);
    check_atexit(creg, 7, 0x8ef00, &o.holder);
}

void section_failures() {
    {   // manager without an insert contract: every registration fails (the original has no early exit)
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot337Objects o;
        const Slot337Result r = init_slot337_7e81d0(o, creg);
        CHECK(!r.ok && creg.pending.size() == 0);
    }
    {   // a tag outside the float family is refused by the wrapper and nothing is registered
        ConCmdRegistry creg;
        ConCmdRangedVar v;
        poison(v);
        HelperFloatSiteSpec spec{kSlot337FloatNames[0], kSlot337FloatNameLengths[0], "", 0, 1, 0x1234567, 0, true};
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
    std::puts("OK test_ctor_slot_337");
    return 0;
}
