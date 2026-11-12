// CPU-01-HELPER-CALLEE-MODELS-01: fixture for the family-admitting float constructor wrappers (0x6aa00 / 0xc300)
// and the float static-initialiser block. ELF pins: tools/lift/check_helper_6aa00.py. FIXTURE_VERIFIED.
#include <bit>

#include "core/helper_6aa00_unbounded_float.hpp"
#include "helper_test_util.hpp"

using namespace sotc;
using namespace helper_test;

namespace {

bool registered(const ConCmdRegistry& reg) { return reg.pending.size() != 0; }

}  // namespace

int main() {
    const SharedStr name = view("gTestFloat");
    const SharedStr desc = view("");
    std::uint32_t cell = 0;

    // ---- constructors: every family tag equals the lifted-tag object ---------------------------------------------
    ConCmdRangedVar ref_u;
    ConCmdRangedVar ref_b;
    poison(ref_u);
    poison(ref_b);
    CHECK(init_unbounded_float_var_6aa00(ref_u, name, &cell, 1, make_args(kRangedFloatFnVtbl), desc));
    CHECK(init_bounded_float_var_c300(ref_b, name, &cell, 0.5f, 4.0f, 1, make_args(kRangedFloatFnVtbl), desc));
    for (const std::uint64_t tag : kFloatFunctorTags) {
        ConCmdRangedVar u;
        ConCmdRangedVar b;
        poison(u);
        poison(b);
        const HelperCtorResult ru = helper_6aa00_unbounded_float(u, name, &cell, 1, make_args(tag), desc);
        const HelperCtorResult rb = helper_c300_bounded_float(b, name, &cell, 0.5f, 4.0f, 1, make_args(tag), desc);
        CHECK(ru.ok && ru.arg_destroy_ok && rb.ok && rb.arg_destroy_ok);
        CHECK(same_bytes(u, ref_u) && same_bytes(b, ref_b));
    }
    // exact stores for slot 504's tag
    {
        ConCmdRangedVar v;
        poison(v);
        CHECK(helper_6aa00_unbounded_float(v, name, &cell, 1, make_args(0x159ba68, 0x42, 0x43), desc).ok);
        CHECK(v.base.vptr == kUnboundedFloatMidVptr && v.value_ref == &cell);
        CHECK(v.min_engaged == 0 && v.max_engaged == 0);
        CHECK(v.fn_vptr == kRangedFloatFnVtbl && v.fn_payload == 0x42 && v.fn_aux == 0x43);
    }
    // ---- refusals write nothing ---------------------------------------------------------------------------------
    const std::uint64_t bad[] = {0, kIntFunctorClassTag, 0x15c8450, 0x159ba69, 0x159ba70, ~0ull};
    for (const std::uint64_t tag : bad) {
        ConCmdRangedVar v;
        poison(v);
        const HelperCtorResult r = helper_6aa00_unbounded_float(v, name, &cell, 1, make_args(tag), desc);
        CHECK(!r.ok && r.refusal == HelperRefusal::unadmitted_arg_functor && all_poison(v));
        const HelperCtorResult rb = helper_c300_bounded_float(v, name, &cell, 0, 1, 1, make_args(tag), desc);
        CHECK(!rb.ok && all_poison(v));
    }

    // ---- static-initialiser block: family tag == the older init_float_var_static on a lifted tag ---------------------
    const FloatVarSpec unb{"gSlot504Float", "", false, 0, 0, std::bit_cast<std::uint32_t>(2.5f), 0x159ba68, 1};
    const FloatVarSpec bnd{"gBoundedFloat", "", true, std::bit_cast<std::uint32_t>(0.0f),
                           std::bit_cast<std::uint32_t>(8.0f), std::bit_cast<std::uint32_t>(1.5f), 0x159ba68, 1};
    for (const FloatVarSpec& spec : {unb, bnd}) {
        FloatVarSpec lifted = spec;
        lifted.arg_fn_vtbl = kRangedFloatFnVtbl;
        ConCmdRangedVar a;
        ConCmdRangedVar b;
        ConCmdRegistry ra{};
        ConCmdRegistry rb{};
        poison(a);
        poison(b);
        const FloatVarResult fa = init_float_var_static_family(a, spec, ra);
        const FloatVarResult fb = init_float_var_static(b, lifted, rb);
        CHECK(fa.ok && fb.ok && fa.ctor_ok && fa.views_inert && fa.functor_destroy_ok && fa.atexit_recorded);
        CHECK(fa.registration.ok);
        CHECK(a.base.vptr == kRangedFloatFinalVptr && a.value == spec.default_bits && a.default_value == spec.default_bits);
        CHECK(ra.atexit_log.size() == rb.atexit_log.size() && ra.atexit_log.size() == 1);
        // value_ref points at the object's own cell in both, so compare the stored fields that do not hold addresses
        CHECK(a.fn_vptr == b.fn_vptr && a.fn_payload == b.fn_payload && a.fn_aux == b.fn_aux);
        CHECK(a.min_engaged == b.min_engaged && a.max_engaged == b.max_engaged);
        CHECK(std::memcmp(a.min_value, b.min_value, 4) == 0 && std::memcmp(a.max_value, b.max_value, 4) == 0);
    }
    // a refused tag registers and records nothing
    {
        FloatVarSpec spec = unb;
        spec.arg_fn_vtbl = 0x15c8450;   // an int tag
        ConCmdRangedVar v;
        ConCmdRegistry reg{};
        poison(v);
        const FloatVarResult f = init_float_var_static_family(v, spec, reg);
        CHECK(!f.ok && !f.ctor_ok && !f.atexit_recorded && reg.atexit_log.size() == 0);
        CHECK(!registered(reg));
    }
    std::puts("test_helper_6aa00: ok");
    return 0;
}
