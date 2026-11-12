// CPU-01-STARTUP-SLOT-450: fixture for startup constructor index 450 (0x4af4b0): three unbounded float console variables (UI logo / main-menu timings). Batch 6.
#include "l4_test_util.hpp"

#include "core/ctor_slot_450_4af4b0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot450Rva == 0x4af4b0 && kSlot450Size == 808);
    CHECK(std::strlen(kSlot450V0Name) == kSlot450V0NameLength && kSlot450V0NameLength == 0x19);
    CHECK(is_float_functor_tag(kSlot450V0FnTag) && !is_int_functor_tag(kSlot450V0FnTag));
    CHECK(std::strlen(kSlot450V1Name) == kSlot450V1NameLength && kSlot450V1NameLength == 0x11);
    CHECK(is_float_functor_tag(kSlot450V1FnTag) && !is_int_functor_tag(kSlot450V1FnTag));
    CHECK(kSlot450V1Rva - kSlot450V0Rva == offsetof(Slot450Objects, v1));
    CHECK(std::strlen(kSlot450V2Name) == kSlot450V2NameLength && kSlot450V2NameLength == 0x14);
    CHECK(is_float_functor_tag(kSlot450V2FnTag) && !is_int_functor_tag(kSlot450V2FnTag));
    CHECK(kSlot450V2Rva - kSlot450V0Rva == offsetof(Slot450Objects, v2));
}

void section_alone() {
    ConCmdRegistry creg;
    Slot450Objects o;
    poison(o);
    const Slot450Result r = init_slot450_4af4b0(o, creg);
    CHECK(r.ok && r.refusal == HelperRefusal::none);
    CHECK(r.v0.ok && r.v0.refusal == HelperRefusal::none && r.v0.ctor_ok && r.v0.functor_destroy_ok &&
          r.v0.views_inert && r.v0.atexit_recorded && r.v0.registration.ok);
    {
        const ConCmdRangedVar& f = o.v0;
        check_float_common(f, false, 0, 0, kSlot450V0Value);
        CHECK(same_view(f.base.name, kSlot450V0Name, kSlot450V0NameLength));
    }
    CHECK(r.v1.ok && r.v1.refusal == HelperRefusal::none && r.v1.ctor_ok && r.v1.functor_destroy_ok &&
          r.v1.views_inert && r.v1.atexit_recorded && r.v1.registration.ok);
    {
        const ConCmdRangedVar& f = o.v1;
        check_float_common(f, false, 0, 0, kSlot450V1Value);
        CHECK(same_view(f.base.name, kSlot450V1Name, kSlot450V1NameLength));
    }
    CHECK(r.v2.ok && r.v2.refusal == HelperRefusal::none && r.v2.ctor_ok && r.v2.functor_destroy_ok &&
          r.v2.views_inert && r.v2.atexit_recorded && r.v2.registration.ok);
    {
        const ConCmdRangedVar& f = o.v2;
        check_float_common(f, false, 0, 0, kSlot450V2Value);
        CHECK(same_view(f.base.name, kSlot450V2Name, kSlot450V2NameLength));
    }
    CHECK(creg.pending.size() == 3 && creg.pending.first() == &o.v0.base.pending);
    CHECK(o.v0.base.pending.next == &o.v1.base.pending);
    CHECK(o.v1.base.pending.next == &o.v2.base.pending);
    CHECK(o.v2.base.pending.next == &creg.pending.head);
    CHECK(creg.atexit_log.size() == 3);
    check_atexit(creg, 0, 0x20f0, &o.v0);
    check_atexit(creg, 1, 0x20f0, &o.v1);
    check_atexit(creg, 2, 0x20f0, &o.v2);
}

void section_failures() {
    {   // manager without an insert contract: every registration fails (the original has no early exit)
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot450Objects o;
        const Slot450Result r = init_slot450_4af4b0(o, creg);
        CHECK(!r.ok && creg.pending.size() == 0);
    }
    {   // a tag outside the float family is refused by the wrapper and nothing is registered
        ConCmdRegistry creg;
        ConCmdRangedVar v;
        poison(v);
        HelperFloatSiteSpec spec{kSlot450V0Name, kSlot450V0NameLength, "", 0, 1, 0x1234567, 0, true};
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
    std::puts("OK test_ctor_slot_450");
    return 0;
}
