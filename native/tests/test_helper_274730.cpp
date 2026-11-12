// CPU-01-HELPER-CALLEE-MODELS-04: fixture for the out-of-line bool console-variable constructor 0x274730 (startup slots 422 / 427 / 473 / 493).
// ELF pins: tools/lift/check_helper_274730.py. FIXTURE_VERIFIED. The object equals the inlined static bool constructor's object outside the
// argument-derived cells.
#include <cstring>

#include "core/helper_274730_bool_var.hpp"
#include "helper_test_util.hpp"

using namespace sotc;
using namespace helper_test;

namespace {

BoolVar274730SiteSpec site493_first() {
    BoolVar274730SiteSpec spec{};
    spec.name = "gTestBool";
    spec.name_length = 9;
    spec.initial = 1;
    spec.arg_fn_tag = 0x159ca60;
    spec.q[0] = 0;               // the original stores the block's own address; overwritten by the self-reference rule
    spec.q[1] = 0x274710;
    spec.q[2] = 0;
    spec.q[3] = 0x26fcf0;
    spec.fn_payload = 0;
    spec.fn_aux = 0;
    return spec;
}

}  // namespace

int main() {
    CHECK(kBoolVarCtorRva == 0x274730 && kBoolVarCtorSize == 661 && kBoolVarCtorStagingClass == 0x159ca60);
    CHECK(kBoolVarCtorFinalFnClass == 0x1594280 && kBoolVarCtorEmptyDescRva == 0xfc521a && kBoolVarCtorFlags == 1);
    CHECK(kBoolVarCtorPreVptr == kBoolVarFinalVptr && kBoolVarCtorMidVptr == kBoolVarMidVptr);
    CHECK(kBoolFunctorTagCount == 143 && kBoolFunctorTags.size() == 143);
    CHECK(kBoolFunctorCopyRva == 0xc2a0 && kBoolFunctorCloneRva == 0xc2c0 && kBoolFunctorMoveRva == 0xc2e0 && kBoolFunctorDestroyRva == 0xc2f0);
    CHECK(kBoolFunctorClassTag == 0x1594280 && kBoolFunctorTags.front() == 0x1594280 && kBoolFunctorTags.back() == 0x15d5800);
    CHECK(is_bool_functor_tag(0x1594280) && is_bool_functor_tag(0x159ca60) && is_bool_functor_tag(0x15d5800));
    CHECK(!is_bool_functor_tag(0) && !is_bool_functor_tag(0x159ca68) && !is_bool_functor_tag(0x15944a0) && !is_bool_functor_tag(0x1594318));
    CHECK(!is_int_functor_tag(0x159ca60) && !is_float_functor_tag(0x159ca60));   // the three families are disjoint here

    // ---- the exact stores for the slot 493 first-site shape ----------------------------------------------------------------
    {
        ConCmdRegistry creg;
        ConCmdBoolVar v;
        poison(v);
        const SharedStr name = view("gTestBool");
        const std::uint8_t initial = 1;
        RangedVarArgs args = make_args(0x159ca60, 0, 0, 0, 0x26fcf0);
        args.q[1] = 0x274710;
        args.q[2] = 0;
        const BoolVar274730Result r = helper_274730_bool_var(v, name, &initial, args, creg);
        CHECK(r.ok && r.refusal == HelperRefusal::none && r.desc_inert && r.arg_destroy_ok && r.registration.ok);
        CHECK(v.base.vptr == kBoolVarFinalVptr && v.base.registered == 1 && v.base.flags == 1);
        CHECK(v.base.name.data == name.data && v.base.name.size == 9);
        CHECK(v.base.description.size == 0 && v.base.description.ctl == nullptr);
        CHECK(v.value_ref == &v.value && v.value == 1 && v.default_value == 1);
        CHECK(v.zero69[0] == 0 && v.zero69[1] == 0 && v.zero69[2] == 0 && v.zero69[3] == 0);
        std::uint64_t q[4];
        std::memcpy(q, v.zero70, sizeof q);
        CHECK(q[0] == reinterpret_cast<std::uint64_t>(v.zero70));     // self reference into the OBJECT (+0x70)
        CHECK(q[1] == 0x274710 && q[2] == 0 && q[3] == 0x26fcf0);
        CHECK(v.fn_payload == 0 && v.fn_vptr == 0x1594280 && v.fn_aux == 0);   // the staging class 0x159ca60 does not survive
        CHECK(creg.pending.size() == 1 && creg.pending.first() == &v.base.pending);
        CHECK(creg.atexit_log.size() == 0);                              // the callee does not run __cxa_atexit
        // never-written gaps keep the poison
        for (std::size_t i = 0; i < 3; ++i) CHECK(v.gap6d[i] == kPoison);
        for (std::size_t i = 0; i < sizeof v.gap90; ++i) CHECK(v.gap90[i] == kPoison);
        for (std::size_t i = 0; i < sizeof v.gapa8; ++i) CHECK(v.gapa8[i] == kPoison);
    }
    // ---- q[3] == 0 : the four qwords are copied verbatim, no self reference; payload / aux propagate -----------------------
    {
        ConCmdRegistry creg;
        ConCmdBoolVar v;
        poison(v);
        const std::uint8_t initial = 0;
        RangedVarArgs args = make_args(0x15d5800, 0xABCDEF, 0x4242, 0x11, 0);
        const BoolVar274730Result r = helper_274730_bool_var(v, view("x"), &initial, args, creg);
        CHECK(r.ok && v.value == 0 && v.default_value == 0);
        std::uint64_t q[4];
        std::memcpy(q, v.zero70, sizeof q);
        CHECK(q[0] == 0x11 && q[1] == 0x1111 && q[2] == 0x2222 && q[3] == 0);
        CHECK(v.fn_payload == 0xABCDEF && v.fn_aux == 0x4242 && v.fn_vptr == 0x1594280);
    }
    // ---- any non-zero initial byte is stored verbatim into both cells ------------------------------------------------------
    {
        ConCmdRegistry creg;
        ConCmdBoolVar v;
        poison(v);
        const std::uint8_t initial = 0x7f;
        CHECK(helper_274730_bool_var(v, view("x"), &initial, make_args(0x1594280), creg).ok && v.value == 0x7f && v.default_value == 0x7f);
    }
    // ---- equality with the inlined static bool constructor outside the argument-derived cells ----------------------------
    {
        ConCmdRegistry creg_a, creg_b;
        ConCmdBoolVar a, b;
        poison(a);
        poison(b);
        const std::uint8_t initial = 1;
        const SharedStr name = view("same");
        const SharedStr desc = view("");
        CHECK(helper_274730_bool_var(a, name, &initial, make_args(0x159ca60, 0, 0), creg_a).ok);
        CHECK(init_bool_var_static(b, name, desc, kBoolFunctorClassTag, creg_b, 1, 1).ok);
        CHECK(a.base.vptr == b.base.vptr && a.base.flags == b.base.flags && a.base.registered == b.base.registered);
        CHECK(a.base.name.data == b.base.name.data && a.base.description.size == b.base.description.size);
        CHECK(a.value == b.value && a.default_value == b.default_value && a.fn_vptr == b.fn_vptr);
        CHECK(a.fn_payload == b.fn_payload && a.fn_aux == b.fn_aux);
        CHECK(std::memcmp(a.zero69, b.zero69, 4) == 0);
    }
    // ---- the full call site: ctor ; destroy ; f590(name) ; atexit(0x20b0) -------------------------------------------------
    {
        ConCmdRegistry creg;
        ConCmdBoolVar v;
        poison(v);
        const BoolVar274730SiteResult r = helper_init_bool_var_274730_site(v, site493_first(), creg);
        CHECK(r.ok && r.ctor_ok && r.functor_destroy_ok && r.views_inert && r.registration.ok && r.atexit_recorded);
        CHECK(creg.atexit_log.size() == 1 && creg.atexit_log.at(0).dtor_rva == 0x20b0 && creg.atexit_log.at(0).object == &v);
        CHECK(v.base.vptr == kBoolVarFinalVptr && v.value == 1);
    }
    // ---- every bool family tag is admitted; non-bool tags are refused with nothing written ----------------------------------
    {
        for (std::uint64_t tag : kBoolFunctorTags) {
            ConCmdRegistry creg;
            ConCmdBoolVar v;
            poison(v);
            BoolVar274730SiteSpec spec = site493_first();
            spec.arg_fn_tag = tag;
            CHECK(helper_init_bool_var_274730_site(v, spec, creg).ok);
        }
        for (std::uint64_t tag : {std::uint64_t{0}, std::uint64_t{0x15944a0}, std::uint64_t{0x1594318}, std::uint64_t{0x15ce538},
                                  std::uint64_t{0x159ca61}, std::uint64_t{0x15d5808}}) {
            ConCmdRegistry creg;
            ConCmdBoolVar v;
            poison(v);
            BoolVar274730SiteSpec spec = site493_first();
            spec.arg_fn_tag = tag;
            const BoolVar274730SiteResult r = helper_init_bool_var_274730_site(v, spec, creg);
            CHECK(!r.ok && r.refusal == HelperRefusal::unadmitted_arg_functor && !r.ctor_ok);
            CHECK(all_poison(v) && creg.pending.size() == 0 && creg.atexit_log.size() == 0);
        }
        ConCmdRegistry creg;
        ConCmdBoolVar v;
        poison(v);
        const BoolVar274730Result r = helper_274730_bool_var(v, view("x"), nullptr, make_args(0x1594280), creg);
        CHECK(!r.ok && r.refusal == HelperRefusal::wrong_helper && all_poison(v));
    }
    std::puts("OK test_helper_274730");
    return 0;
}
