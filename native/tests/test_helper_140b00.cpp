// CPU-01-HELPER-CALLEE-MODELS-04: fixture for the unbounded-int constructor twins 0x140b00 (slot 534) and 0x25ab0 (slot 566).
// ELF pins: tools/lift/check_helper_140b00.py. FIXTURE_VERIFIED. The object is the 0x3a980 object with the twin's mid vptr; the private seed class
// survives as +0xc0 only for its own tag.
#include "core/helper_140b00_unbounded_int_twin.hpp"
#include "helper_test_util.hpp"

using namespace sotc;
using namespace helper_test;

namespace {

void run_variant(UnboundedIntTwinVariant variant, std::uint64_t mid_table, std::uint64_t seed) {
    CHECK(unbounded_int_twin_mid_vptr(variant) == mid_table + 0x10 && unbounded_int_twin_seed_class(variant) == seed);
    CHECK(!is_int_functor_tag(seed));      // private table, admitted by the twin only
    const SharedStr name = view("gTestTwin");
    const SharedStr desc = view("");

    ConCmdRangedVar ref;
    poison(ref);
    CHECK(helper_3a980_unbounded_int(ref, name, 1, make_args(kIntFunctorClassTag, 0x55, 0x66), desc).ok);
    for (const std::uint64_t tag : kIntFunctorTags) {
        ConCmdRangedVar v;
        poison(v);
        const HelperCtorResult r = helper_unbounded_int_twin_140b00(v, variant, name, 1, make_args(tag, 0x55, 0x66), desc);
        CHECK(r.ok && r.refusal == HelperRefusal::none && r.arg_destroy_ok);
        CHECK(v.base.vptr == mid_table + 0x10 && v.value_ref == &v.value && v.min_engaged == 0 && v.max_engaged == 0);
        CHECK(v.fn_vptr == kIntFunctorClassTag && v.fn_payload == 0x55 && v.fn_aux == 0x66);
        ConCmdRangedVar a = v, b = ref;
        a.base.vptr = 0;
        b.base.vptr = 0;
        a.value_ref = nullptr;
        b.value_ref = nullptr;
        CHECK(same_bytes(a, b));
    }
    {   // the seed class (every observed site's argument table): the object keeps the seed class
        ConCmdRangedVar v;
        poison(v);
        const HelperCtorResult r = helper_unbounded_int_twin_140b00(v, variant, name, 1, make_args(seed, 0x55, 0x66), desc);
        CHECK(r.ok && r.arg_destroy_ok && v.base.vptr == mid_table + 0x10);
        CHECK(v.fn_vptr == seed && v.fn_payload == 0x55 && v.fn_aux == 0x66 && v.min_engaged == 0 && v.max_engaged == 0);
    }
    const UnboundedIntTwinVariant other = variant == UnboundedIntTwinVariant::a_140b00 ? UnboundedIntTwinVariant::b_25ab0
                                                                                        : UnboundedIntTwinVariant::a_140b00;
    for (const std::uint64_t tag : {std::uint64_t{0}, std::uint64_t{0x15942b0}, kFloatFunctorClassTag, unbounded_int_twin_seed_class(other), ~std::uint64_t{0}}) {
        ConCmdRangedVar v;
        poison(v);
        const HelperCtorResult r = helper_unbounded_int_twin_140b00(v, variant, name, 1, make_args(tag), desc);
        CHECK(!r.ok && r.refusal == HelperRefusal::unadmitted_arg_functor && !r.arg_destroy_ok && all_poison(v));
    }
}

}  // namespace

int main() {
    CHECK(kUnboundedIntTwinARva == 0x140b00 && kUnboundedIntTwinBRva == 0x25ab0 && kUnboundedIntTwinABSize == 0x142);
    CHECK(kUnboundedIntTwinAMidTable == 0x1598240 && kUnboundedIntTwinBMidTable == 0x1594fd0);
    CHECK(kUnboundedIntTwinASeedClass == 0x15983c8 && kUnboundedIntTwinBSeedClass == 0x1595100);
    CHECK(kUnboundedIntTwinASeedCopyRva == 0x140c90 && kUnboundedIntTwinASeedCloneRva == 0x140cb0 && kUnboundedIntTwinASeedMoveRva == 0x140cd0 &&
          kUnboundedIntTwinASeedDestroyRva == 0x140ce0);
    CHECK(kUnboundedIntTwinBSeedCopyRva == 0x25c40 && kUnboundedIntTwinBSeedCloneRva == 0x25c60 && kUnboundedIntTwinBSeedMoveRva == 0x25c80 &&
          kUnboundedIntTwinBSeedDestroyRva == 0x25c90);
    run_variant(UnboundedIntTwinVariant::a_140b00, kUnboundedIntTwinAMidTable, kUnboundedIntTwinASeedClass);
    run_variant(UnboundedIntTwinVariant::b_25ab0, kUnboundedIntTwinBMidTable, kUnboundedIntTwinBSeedClass);
    std::puts("OK test_helper_140b00");
    return 0;
}
