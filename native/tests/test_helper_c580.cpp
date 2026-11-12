// CPU-01-HELPER-CALLEE-MODELS-01: fixture for the family-admitting ranged-int constructor wrapper (0xc580) and the
// shared functor-family tables. ELF pins: tools/lift/check_helper_c580.py. FIXTURE_VERIFIED.
#include <algorithm>

#include "core/helper_c580_ranged_int.hpp"
#include "helper_test_util.hpp"

using namespace sotc;
using namespace helper_test;

int main() {
    // ---- the tag tables --------------------------------------------------------------------------------------
    CHECK(kIntFunctorTags.size() == 37 && kFloatFunctorTags.size() == 68);
    CHECK(std::is_sorted(kIntFunctorTags.begin(), kIntFunctorTags.end()));
    CHECK(std::is_sorted(kFloatFunctorTags.begin(), kFloatFunctorTags.end()));
    CHECK(std::adjacent_find(kIntFunctorTags.begin(), kIntFunctorTags.end()) == kIntFunctorTags.end());
    CHECK(std::adjacent_find(kFloatFunctorTags.begin(), kFloatFunctorTags.end()) == kFloatFunctorTags.end());
    for (const std::uint64_t t : kIntFunctorTags) CHECK(is_int_functor_tag(t) && !is_float_functor_tag(t));
    for (const std::uint64_t t : kFloatFunctorTags) CHECK(is_float_functor_tag(t) && !is_int_functor_tag(t));
    CHECK(is_int_functor_tag(kIntFunctorClassTag) && is_float_functor_tag(kFloatFunctorClassTag));
    CHECK(is_int_functor_tag(0x15c8450));       // slot 430
    CHECK(is_int_functor_tag(0x15d1d80));       // slot 308 (the old whitelist)
    CHECK(!is_int_functor_tag(0) && !is_int_functor_tag(0x15c8451) && !is_int_functor_tag(0x15c8458));

    // ---- every int tag gives the same object as the lifted class tag --------------------------------------------
    const SharedStr name = view("gTestInt");
    const SharedStr desc = view("");
    ConCmdRangedVar ref;
    poison(ref);
    std::uint32_t cell = 0;
    const auto ref_args = make_args(kRangedIntFnVtbl);
    CHECK(init_ranged_int_var_c580(ref, name, &cell, -3, 9, 1, ref_args, desc));
    for (const std::uint64_t tag : kIntFunctorTags) {
        ConCmdRangedVar v;
        poison(v);
        const HelperCtorResult r = helper_c580_ranged_int(v, name, &cell, -3, 9, 1, make_args(tag), desc);
        CHECK(r.ok && r.refusal == HelperRefusal::none && r.arg_destroy_ok);
        CHECK(same_bytes(v, ref));
    }

    // ---- exact stores for one tag (the slot-430 tag) -------------------------------------------------------------
    {
        ConCmdRangedVar v;
        poison(v);
        const HelperCtorResult r = helper_c580_ranged_int(v, name, &cell, 2, 40, 1, make_args(0x15c8450, 0xABCD, 0x99),
                                                          desc);
        CHECK(r.ok);
        CHECK(v.base.vptr == kRangedIntMidVptr);
        CHECK(v.value_ref == &cell);
        CHECK(v.min_engaged == 1 && v.max_engaged == 1);
        std::int32_t lo = 0, hi = 0;
        std::memcpy(&lo, v.min_value, 4);
        std::memcpy(&hi, v.max_value, 4);
        CHECK(lo == 2 && hi == 40);
        CHECK(v.fn_vptr == kRangedIntFnVtbl);    // never the caller tag
        CHECK(v.fn_payload == 0xABCD && v.fn_aux == 0x99);
        CHECK(v.args[1] == 0x1111 && v.args[2] == 0x2222 && v.args[3] == 0);
    }
    // q[3] != 0 makes args[0] self-referential
    {
        ConCmdRangedVar v;
        poison(v);
        CHECK(helper_c580_ranged_int(v, name, &cell, 0, 1, 1, make_args(0x15944a0, 1, 2, 5, 7), desc).ok);
        CHECK(v.args[0] == reinterpret_cast<std::uint64_t>(&v.args[0]) && v.args[3] == 7);
    }

    // ---- refusals write nothing --------------------------------------------------------------------------------
    const std::uint64_t bad[] = {0, 1, kFloatFunctorClassTag, 0x1596878, 0x15c8458, 0x15c8450 + 1, 0x15d1d88,
                                 ~0ull};
    for (const std::uint64_t tag : bad) {
        ConCmdRangedVar v;
        poison(v);
        const HelperCtorResult r = helper_c580_ranged_int(v, name, &cell, 0, 1, 1, make_args(tag), desc);
        CHECK(!r.ok && r.refusal == HelperRefusal::unadmitted_arg_functor && !r.arg_destroy_ok);
        CHECK(all_poison(v));
    }
    std::puts("test_helper_c580: ok");
    return 0;
}
