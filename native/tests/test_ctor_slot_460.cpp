// CPU-01-STARTUP-SLOT-460: fixture for startup constructor index 460 (0x41c340): bool drawWaypointNode + two unbounded floats (waypoint debug draw). Batch 6.
#include "l4_test_util.hpp"

#include "core/ctor_slot_460_41c340.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot460Rva == 0x41c340 && kSlot460Size == 857);
    CHECK(std::strlen(kSlot460V0Name) == kSlot460V0NameLength && kSlot460V0NameLength == 0x10);
    CHECK(std::strlen(kSlot460V1Name) == kSlot460V1NameLength && kSlot460V1NameLength == 0x13);
    CHECK(is_float_functor_tag(kSlot460V1FnTag) && !is_int_functor_tag(kSlot460V1FnTag));
    CHECK(kSlot460V1Rva - kSlot460V0Rva == offsetof(Slot460Objects, v1));
    CHECK(std::strlen(kSlot460V2Name) == kSlot460V2NameLength && kSlot460V2NameLength == 0x12);
    CHECK(is_float_functor_tag(kSlot460V2FnTag) && !is_int_functor_tag(kSlot460V2FnTag));
    CHECK(kSlot460V2Rva - kSlot460V0Rva == offsetof(Slot460Objects, v2));
}

void section_alone() {
    ConCmdRegistry creg;
    Slot460Objects o;
    poison(o);
    const Slot460Result r = init_slot460_41c340(o, creg);
    CHECK(r.ok && r.refusal == HelperRefusal::none);
    CHECK(r.v0.ok && r.v0.registration.ok && r.v0.views_inert && r.v0.atexit_recorded);
    {
        check_bool_common(o.v0, kSlot460V0FnTag, 0);
        CHECK(same_view(o.v0.base.name, kSlot460V0Name, kSlot460V0NameLength));
    }
    CHECK(r.v1.ok && r.v1.refusal == HelperRefusal::none && r.v1.ctor_ok && r.v1.functor_destroy_ok &&
          r.v1.views_inert && r.v1.atexit_recorded && r.v1.registration.ok);
    {
        const ConCmdRangedVar& f = o.v1;
        check_float_common(f, false, 0, 0, kSlot460V1Value);
        CHECK(same_view(f.base.name, kSlot460V1Name, kSlot460V1NameLength));
    }
    CHECK(r.v2.ok && r.v2.refusal == HelperRefusal::none && r.v2.ctor_ok && r.v2.functor_destroy_ok &&
          r.v2.views_inert && r.v2.atexit_recorded && r.v2.registration.ok);
    {
        const ConCmdRangedVar& f = o.v2;
        check_float_common(f, false, 0, 0, kSlot460V2Value);
        CHECK(same_view(f.base.name, kSlot460V2Name, kSlot460V2NameLength));
    }
    CHECK(creg.pending.size() == 3 && creg.pending.first() == &o.v0.base.pending);
    CHECK(o.v0.base.pending.next == &o.v1.base.pending);
    CHECK(o.v1.base.pending.next == &o.v2.base.pending);
    CHECK(o.v2.base.pending.next == &creg.pending.head);
    CHECK(creg.atexit_log.size() == 3);
    check_atexit(creg, 0, 0x20b0, &o.v0);
    check_atexit(creg, 1, 0x20f0, &o.v1);
    check_atexit(creg, 2, 0x20f0, &o.v2);
}

void section_failures() {
    {   // manager without an insert contract: every registration fails (the original has no early exit)
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot460Objects o;
        const Slot460Result r = init_slot460_41c340(o, creg);
        CHECK(!r.ok && creg.pending.size() == 0);
    }
    {   // a tag outside the float family is refused by the wrapper and nothing is registered
        ConCmdRegistry creg;
        ConCmdRangedVar v;
        poison(v);
        HelperFloatSiteSpec spec{kSlot460V1Name, kSlot460V1NameLength, "", 0, 1, 0x1234567, 0, true};
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
    std::puts("OK test_ctor_slot_460");
    return 0;
}
