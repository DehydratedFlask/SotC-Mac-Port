// CPU-01-HELPER-CALLEE-MODELS-01: fixture for the family-admitting unbounded-int constructor wrapper (0x3a980).
// ELF pins: tools/lift/check_helper_3a980.py. FIXTURE_VERIFIED.
#include "core/helper_3a980_unbounded_int.hpp"
#include "helper_test_util.hpp"

using namespace sotc;
using namespace helper_test;

int main() {
    const SharedStr name = view("gTestUnboundedInt");
    const SharedStr desc = view("");

    ConCmdRangedVar ref;
    poison(ref);
    CHECK(init_unbounded_int_var_worker_3a980(ref, name, 1, make_args(kUnboundedIntFnTable), desc));

    CHECK(ref.value_ref == &ref.value);
    ConCmdRangedVar ref_norm = ref;
    ref_norm.value_ref = nullptr;   // +0x60 points at each object's own +0xd0 cell, so compare it separately
    for (const std::uint64_t tag : kIntFunctorTags) {
        ConCmdRangedVar v;
        poison(v);
        const HelperCtorResult r = helper_3a980_unbounded_int(v, name, 1, make_args(tag), desc);
        CHECK(r.ok && r.refusal == HelperRefusal::none && r.arg_destroy_ok);
        CHECK(v.value_ref == &v.value);
        ConCmdRangedVar v_norm = v;
        v_norm.value_ref = nullptr;
        CHECK(same_bytes(v_norm, ref_norm));
    }

    // slot 555's tag, exact stores
    {
        ConCmdRangedVar v;
        poison(v);
        const HelperCtorResult r = helper_3a980_unbounded_int(v, name, 1, make_args(0x1595e60, 0x55, 0x66), desc);
        CHECK(r.ok);
        CHECK(v.base.vptr == kUnboundedIntWorkerVptr);
        CHECK(v.value_ref == &v.value);                       // the cell is this+0xd0
        CHECK(v.min_engaged == 0 && v.max_engaged == 0);      // unbounded
        CHECK(v.fn_vptr == kUnboundedIntFnTable);
        CHECK(v.fn_payload == 0x55 && v.fn_aux == 0x66);
        CHECK(unbounded_int_menu_target(v).known == false);   // the CALLER stores the final class afterwards
        v.base.vptr = kUnboundedIntFinalVptr;
        const DebugMenuTarget t = unbounded_int_menu_target(v);
        CHECK(t.known && t.slot3 == 1 && t.slot6 == 2);
    }

    const std::uint64_t bad[] = {0, 0x15942b0, kFloatFunctorClassTag, 0x159ba68, 0x1595e61, ~0ull};
    for (const std::uint64_t tag : bad) {
        ConCmdRangedVar v;
        poison(v);
        const HelperCtorResult r = helper_3a980_unbounded_int(v, name, 1, make_args(tag), desc);
        CHECK(!r.ok && r.refusal == HelperRefusal::unadmitted_arg_functor && !r.arg_destroy_ok);
        CHECK(all_poison(v));
    }
    std::puts("test_helper_3a980: ok");
    return 0;
}
