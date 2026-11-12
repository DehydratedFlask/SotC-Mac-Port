// CPU-01-STARTUP-SLOT-442: fixture for startup constructor index 442 (0x4e3760): bool + float + bool grip-fill test variables. Batch 6.
#include "l4_test_util.hpp"

#include "core/ctor_slot_442_4e3760.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot442Rva == 0x4e3760 && kSlot442Size == 779);
    CHECK(std::strlen(kSlot442V0Name) == kSlot442V0NameLength && kSlot442V0NameLength == 0x1f);
    CHECK(std::strlen(kSlot442V0Description) == kSlot442V0DescriptionLength && kSlot442V0DescriptionLength == 0x2e);
    CHECK(std::strlen(kSlot442V1Name) == kSlot442V1NameLength && kSlot442V1NameLength == 0x24);
    CHECK(std::strlen(kSlot442V1Description) == kSlot442V1DescriptionLength && kSlot442V1DescriptionLength == 0x39);
    CHECK(is_float_functor_tag(kSlot442V1FnTag) && !is_int_functor_tag(kSlot442V1FnTag));
    CHECK(kSlot442V1Rva - kSlot442V0Rva == offsetof(Slot442Objects, v1));
    CHECK(std::strlen(kSlot442V2Name) == kSlot442V2NameLength && kSlot442V2NameLength == 0x20);
    CHECK(std::strlen(kSlot442V2Description) == kSlot442V2DescriptionLength && kSlot442V2DescriptionLength == 0x34);
    CHECK(kSlot442V2Rva - kSlot442V0Rva == offsetof(Slot442Objects, v2));
}

void section_alone() {
    ConCmdRegistry creg;
    Slot442Objects o;
    poison(o);
    const Slot442Result r = init_slot442_4e3760(o, creg);
    CHECK(r.ok && r.refusal == HelperRefusal::none);
    CHECK(r.v0.ok && r.v0.registration.ok && r.v0.views_inert && r.v0.atexit_recorded);
    {
        check_bool_with_description(o.v0, kSlot442V0FnTag, 0, kSlot442V0Description, kSlot442V0DescriptionLength);
        CHECK(same_view(o.v0.base.name, kSlot442V0Name, kSlot442V0NameLength));
    }
    CHECK(r.v1.ok && r.v1.refusal == HelperRefusal::none && r.v1.ctor_ok && r.v1.functor_destroy_ok &&
          r.v1.views_inert && r.v1.atexit_recorded && r.v1.registration.ok);
    {
        const ConCmdRangedVar& f = o.v1;
        CHECK(f.base.vptr == kRangedFloatFinalVptr && f.base.flags == 1 && f.base.registered == 1);
        CHECK(f.value_ref == &f.value && f.value == kSlot442V1Value && f.default_value == kSlot442V1Value);
        CHECK(f.min_engaged == 0 && f.max_engaged == 0 && f.min_value[0] == 0 && f.max_value[0] == 0);
        for (std::uint64_t a : f.args) CHECK(a == 0);
        CHECK(f.fn_payload == 0 && f.fn_aux == 0 && f.fn_vptr == kRangedFloatFnVtbl);
        CHECK(same_view(f.base.description, kSlot442V1Description, kSlot442V1DescriptionLength));
        CHECK(same_view(f.base.name, kSlot442V1Name, kSlot442V1NameLength));
    }
    CHECK(r.v2.ok && r.v2.registration.ok && r.v2.views_inert && r.v2.atexit_recorded);
    {
        check_bool_with_description(o.v2, kSlot442V2FnTag, 0, kSlot442V2Description, kSlot442V2DescriptionLength);
        CHECK(same_view(o.v2.base.name, kSlot442V2Name, kSlot442V2NameLength));
    }
    CHECK(creg.pending.size() == 3 && creg.pending.first() == &o.v0.base.pending);
    CHECK(o.v0.base.pending.next == &o.v1.base.pending);
    CHECK(o.v1.base.pending.next == &o.v2.base.pending);
    CHECK(o.v2.base.pending.next == &creg.pending.head);
    CHECK(creg.atexit_log.size() == 3);
    check_atexit(creg, 0, 0x20b0, &o.v0);
    check_atexit(creg, 1, 0x20f0, &o.v1);
    check_atexit(creg, 2, 0x20b0, &o.v2);
}

void section_failures() {
    {   // manager without an insert contract: every registration fails (the original has no early exit)
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot442Objects o;
        const Slot442Result r = init_slot442_4e3760(o, creg);
        CHECK(!r.ok && creg.pending.size() == 0);
    }
    {   // a tag outside the float family is refused by the wrapper and nothing is registered
        ConCmdRegistry creg;
        ConCmdRangedVar v;
        poison(v);
        HelperFloatSiteSpec spec{kSlot442V1Name, kSlot442V1NameLength, "", 0, 1, 0x1234567, 0, true};
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
    std::puts("OK test_ctor_slot_442");
    return 0;
}
