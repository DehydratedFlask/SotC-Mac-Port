// CPU-01-STARTUP-SLOT-397: fixture for startup constructor index 397 (0x64d760): four unbounded float console variables (player acceleration scalars). Batch 6.
#include "l4_test_util.hpp"

#include "core/ctor_slot_397_64d760.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot397Rva == 0x64d760 && kSlot397Size == 1036);
    CHECK(std::strlen(kSlot397V0Name) == kSlot397V0NameLength && kSlot397V0NameLength == 0x1c);
    CHECK(is_float_functor_tag(kSlot397V0FnTag) && !is_int_functor_tag(kSlot397V0FnTag));
    CHECK(std::strlen(kSlot397V1Name) == kSlot397V1NameLength && kSlot397V1NameLength == 0x24);
    CHECK(is_float_functor_tag(kSlot397V1FnTag) && !is_int_functor_tag(kSlot397V1FnTag));
    CHECK(kSlot397V1Rva - kSlot397V0Rva == offsetof(Slot397Objects, v1));
    CHECK(std::strlen(kSlot397V2Name) == kSlot397V2NameLength && kSlot397V2NameLength == 0x1c);
    CHECK(is_float_functor_tag(kSlot397V2FnTag) && !is_int_functor_tag(kSlot397V2FnTag));
    CHECK(kSlot397V2Rva - kSlot397V0Rva == offsetof(Slot397Objects, v2));
    CHECK(std::strlen(kSlot397V3Name) == kSlot397V3NameLength && kSlot397V3NameLength == 0x24);
    CHECK(is_float_functor_tag(kSlot397V3FnTag) && !is_int_functor_tag(kSlot397V3FnTag));
    CHECK(kSlot397V3Rva - kSlot397V0Rva == offsetof(Slot397Objects, v3));
}

void section_alone() {
    ConCmdRegistry creg;
    Slot397Objects o;
    poison(o);
    const Slot397Result r = init_slot397_64d760(o, creg);
    CHECK(r.ok && r.refusal == HelperRefusal::none);
    CHECK(r.v0.ok && r.v0.refusal == HelperRefusal::none && r.v0.ctor_ok && r.v0.functor_destroy_ok &&
          r.v0.views_inert && r.v0.atexit_recorded && r.v0.registration.ok);
    {
        const ConCmdRangedVar& f = o.v0;
        check_float_common(f, false, 0, 0, kSlot397V0Value);
        CHECK(same_view(f.base.name, kSlot397V0Name, kSlot397V0NameLength));
    }
    CHECK(r.v1.ok && r.v1.refusal == HelperRefusal::none && r.v1.ctor_ok && r.v1.functor_destroy_ok &&
          r.v1.views_inert && r.v1.atexit_recorded && r.v1.registration.ok);
    {
        const ConCmdRangedVar& f = o.v1;
        check_float_common(f, false, 0, 0, kSlot397V1Value);
        CHECK(same_view(f.base.name, kSlot397V1Name, kSlot397V1NameLength));
    }
    CHECK(r.v2.ok && r.v2.refusal == HelperRefusal::none && r.v2.ctor_ok && r.v2.functor_destroy_ok &&
          r.v2.views_inert && r.v2.atexit_recorded && r.v2.registration.ok);
    {
        const ConCmdRangedVar& f = o.v2;
        check_float_common(f, false, 0, 0, kSlot397V2Value);
        CHECK(same_view(f.base.name, kSlot397V2Name, kSlot397V2NameLength));
    }
    CHECK(r.v3.ok && r.v3.refusal == HelperRefusal::none && r.v3.ctor_ok && r.v3.functor_destroy_ok &&
          r.v3.views_inert && r.v3.atexit_recorded && r.v3.registration.ok);
    {
        const ConCmdRangedVar& f = o.v3;
        check_float_common(f, false, 0, 0, kSlot397V3Value);
        CHECK(same_view(f.base.name, kSlot397V3Name, kSlot397V3NameLength));
    }
    CHECK(creg.pending.size() == 4 && creg.pending.first() == &o.v0.base.pending);
    CHECK(o.v0.base.pending.next == &o.v1.base.pending);
    CHECK(o.v1.base.pending.next == &o.v2.base.pending);
    CHECK(o.v2.base.pending.next == &o.v3.base.pending);
    CHECK(o.v3.base.pending.next == &creg.pending.head);
    CHECK(creg.atexit_log.size() == 4);
    check_atexit(creg, 0, 0x20f0, &o.v0);
    check_atexit(creg, 1, 0x20f0, &o.v1);
    check_atexit(creg, 2, 0x20f0, &o.v2);
    check_atexit(creg, 3, 0x20f0, &o.v3);
}

void section_failures() {
    {   // manager without an insert contract: every registration fails (the original has no early exit)
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot397Objects o;
        const Slot397Result r = init_slot397_64d760(o, creg);
        CHECK(!r.ok && creg.pending.size() == 0);
    }
    {   // a tag outside the float family is refused by the wrapper and nothing is registered
        ConCmdRegistry creg;
        ConCmdRangedVar v;
        poison(v);
        HelperFloatSiteSpec spec{kSlot397V0Name, kSlot397V0NameLength, "", 0, 1, 0x1234567, 0, true};
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
    std::puts("OK test_ctor_slot_397");
    return 0;
}
