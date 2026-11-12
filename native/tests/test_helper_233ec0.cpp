// CPU-01-STARTUP-BATCH-7: fixture for the unbounded-int constructor twin 0x233ec0 (startup slot 506). ELF pins: tools/lift/check_helper_233ec0.py.
// FIXTURE_VERIFIED. The object is the 0x3a980 object with the mid class 0x159b5a0; the private class 0x159b680 survives as +0xc0 only for its own tag.
#include "core/helper_233ec0_unbounded_int_twin.hpp"
#include "helper_test_util.hpp"

using namespace sotc;
using namespace helper_test;

int main() {
    CHECK(kUnboundedIntTwinRva == 0x233ec0 && kUnboundedIntTwinSize == 0x142);
    CHECK(kUnboundedIntTwinMidVptr == 0x159b5a0 && kUnboundedIntTwinFinalVptr == 0x159b618 && kUnboundedIntTwinSeedFnTable == 0x159b680);
    CHECK(kUnboundedIntTwinDtorRva == 0x230d60 && kUnboundedIntTwinSlot3Result == 1 && kUnboundedIntTwinSlot6Result == 1);
    CHECK(!is_int_functor_tag(kUnboundedIntTwinSeedFnTable));   // private table, not a family member (admitted by the twin only)

    const SharedStr name = view("gTestTwin");
    const SharedStr desc = view("");

    // ---- the twin equals the 3a980 object outside the mid vptr, for every admitted tag ----------------------------------------
    {
        ConCmdRangedVar ref;
        poison(ref);
        CHECK(helper_3a980_unbounded_int(ref, name, 1, make_args(0x159b6a0, 0x55, 0x66), desc).ok);
        for (const std::uint64_t tag : kIntFunctorTags) {
            ConCmdRangedVar v;
            poison(v);
            const HelperCtorResult r = helper_233ec0_unbounded_int_twin(v, name, 1, make_args(tag, 0x55, 0x66), desc);
            CHECK(r.ok && r.refusal == HelperRefusal::none && r.arg_destroy_ok);
            CHECK(v.base.vptr == kUnboundedIntTwinMidVptr);
            CHECK(v.value_ref == &v.value && v.min_engaged == 0 && v.max_engaged == 0);
            CHECK(v.fn_vptr == kIntFunctorClassTag && v.fn_payload == 0x55 && v.fn_aux == 0x66);
            ConCmdRangedVar a = v, b = ref;
            a.base.vptr = 0;
            b.base.vptr = 0;
            a.value_ref = nullptr;
            b.value_ref = nullptr;
            CHECK(same_bytes(a, b));
        }
    }
    // ---- the private class (slot 506's argument table): the object keeps class 0x159b680 -------------------------------------------
    {
        ConCmdRangedVar v;
        poison(v);
        const HelperCtorResult r = helper_233ec0_unbounded_int_twin(v, name, 1, make_args(kUnboundedIntTwinSeedFnTable, 0x55, 0x66), desc);
        CHECK(r.ok && r.refusal == HelperRefusal::none && r.arg_destroy_ok);
        CHECK(v.base.vptr == kUnboundedIntTwinMidVptr && v.value_ref == &v.value && v.min_engaged == 0 && v.max_engaged == 0);
        CHECK(v.fn_vptr == kUnboundedIntTwinSeedFnTable && v.fn_payload == 0x55 && v.fn_aux == 0x66);
    }
    // ---- refusal writes nothing --------------------------------------------------------------------------------------------
    for (const std::uint64_t tag : {std::uint64_t{0}, std::uint64_t{0x15942b0}, kFloatFunctorClassTag, kUnboundedIntTwinFinalVptr, ~std::uint64_t{0}}) {
        ConCmdRangedVar v;
        poison(v);
        const HelperCtorResult r = helper_233ec0_unbounded_int_twin(v, name, 1, make_args(tag), desc);
        CHECK(!r.ok && r.refusal == HelperRefusal::unadmitted_arg_functor && !r.arg_destroy_ok);
        CHECK(all_poison(v));
    }
    // ---- menu target: known only for the final class -----------------------------------------------------------------------
    {
        ConCmdRangedVar v;
        poison(v);
        CHECK(helper_233ec0_unbounded_int_twin(v, name, 1, make_args(0x159b6a0), desc).ok);
        CHECK(!unbounded_int_twin_menu_target(v).known);        // mid class
        v.base.vptr = kUnboundedIntTwinFinalVptr;
        const DebugMenuTarget t = unbounded_int_twin_menu_target(v);
        CHECK(t.known && t.slot3 == 1 && t.slot6 == 1 && t.object == &v);
    }
    // ---- the whole site: order and stores ----------------------------------------------------------------------------------
    {
        ConCmdRegistry creg;
        ConCmdRangedVar v;
        poison(v);
        const HelperIntSiteSpec spec{"drawTouch", 9, "", 0, 0, 0, 1, 0x159b680, 0, 0};
        const HelperIntSiteResult r = helper_init_unbounded_int_twin_site(v, spec, creg);
        CHECK(r.ok && r.refusal == HelperRefusal::none && r.ctor_ok && r.functor_destroy_ok && r.views_inert && r.atexit_recorded && r.registration.ok);
        CHECK(v.base.vptr == kUnboundedIntTwinFinalVptr && v.value == 0 && v.default_value == 0 && v.base.registered == 1);
        CHECK(v.fn_vptr == kUnboundedIntTwinSeedFnTable);
        CHECK(creg.pending.size() == 1 && creg.pending.first() == &v.base.pending);
        CHECK(creg.atexit_log.size() == 1 && creg.atexit_log.at(0).dtor_rva == 0x230d60 && creg.atexit_log.at(0).object == &v);
        ConCmdRangedVar w;
        poison(w);
        const HelperIntSiteSpec bad{"x", 1, "", 0, 0, 0, 1, 0x159b618, 0, 0};
        ConCmdRegistry creg2;
        const HelperIntSiteResult rb = helper_init_unbounded_int_twin_site(w, bad, creg2);
        CHECK(!rb.ok && rb.refusal == HelperRefusal::unadmitted_arg_functor && creg2.atexit_log.size() == 0 && creg2.pending.size() == 0);
    }
    std::puts("test_helper_233ec0: ok");
    return 0;
}
