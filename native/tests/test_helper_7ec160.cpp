// CPU-01-STARTUP-BATCH-7: fixture for the local enum-variable constructor 0x7ec160 / 0x7ec320 (startup slot 336). ELF pins:
// tools/lift/check_helper_7ec160.py. FIXTURE_VERIFIED. The object is the 0xc580 ranged-int object with the class pair 0x15ce4d0 / 0x15ce538.
#include "core/helper_7ec160_enum_var.hpp"
#include "helper_test_util.hpp"

using namespace sotc;
using namespace helper_test;

int main() {
    CHECK(kEnumVarWrapperRva == 0x7ec160 && kEnumVarWrapperSize == 0x1b3 && kEnumVarCtorRva == 0x7ec320 && kEnumVarCtorSize == 0x15f);
    CHECK(kEnumVarMidVptr == 0x15ce4d0 && kEnumVarFinalVptr == 0x15ce568 && kEnumVarFnVtbl == 0x15ce538 && kEnumVarDtorRva == 0x7ea160);
    CHECK(kEnumVarSlot3Result == 1 && kEnumVarSlot6Result == 1);
    CHECK(!is_int_functor_tag(kEnumVarFnVtbl) && !is_float_functor_tag(kEnumVarFnVtbl));   // a private table, not a family member

    const SharedStr name = view("gTestEnum");
    const SharedStr desc = view("which one");

    // ---- exact stores for the observed call (min 0, max 3, init 0) --------------------------------------------------
    {
        ConCmdRegistry creg;
        ConCmdRangedVar v;
        poison(v);
        const EnumVarCtorResult r = helper_7ec160_enum_var(v, name, 0, 0, 3, 1, make_args(kEnumVarFnVtbl, 0xABCD, 0x99), desc, creg);
        CHECK(r.ok && r.refusal == HelperRefusal::none && r.arg_destroy_ok && r.registration.ok);
        CHECK(v.base.vptr == kEnumVarFinalVptr && v.base.registered == 1 && v.base.flags == 1);
        CHECK(v.value_ref == &v.value);                     // 0x7ec2ac: rdx = this + 0xd0
        CHECK(v.value == 0 && v.default_value == 0);
        CHECK(v.min_engaged == 1 && v.max_engaged == 1);
        std::int32_t lo = -1, hi = -1;
        std::memcpy(&lo, v.min_value, 4);
        std::memcpy(&hi, v.max_value, 4);
        CHECK(lo == 0 && hi == 3);
        CHECK(v.fn_vptr == kEnumVarFnVtbl && v.fn_payload == 0xABCD && v.fn_aux == 0x99);
        CHECK(v.args[1] == 0x1111 && v.args[2] == 0x2222 && v.args[3] == 0);
        CHECK(creg.pending.size() == 1 && creg.pending.first() == &v.base.pending);
    }
    // ---- a non-zero initial value is stored into both cells; q[3] != 0 makes args[0] self-referential -------------------
    {
        ConCmdRegistry creg;
        ConCmdRangedVar v;
        poison(v);
        const EnumVarCtorResult r = helper_7ec160_enum_var(v, name, 2, -5, 9, 1, make_args(kEnumVarFnVtbl, 1, 2, 0, 0x55), desc, creg);
        CHECK(r.ok && v.value == 2 && v.default_value == 2 && v.args[0] == reinterpret_cast<std::uint64_t>(&v.args[0]));
        CHECK(enum_var_menu_target(v).known && enum_var_menu_target(v).slot3 == 1 && enum_var_menu_target(v).slot6 == 1);
        CHECK(enum_var_menu_target(v).object == &v);
    }
    // ---- the object equals the 0xc580 object outside the retagged cells --------------------------------------------------
    {
        ConCmdRegistry creg;
        ConCmdRangedVar a;
        ConCmdRangedVar b;
        poison(a);
        poison(b);
        CHECK(helper_7ec160_enum_var(a, name, 0, 0, 3, 1, make_args(kEnumVarFnVtbl), desc, creg).ok);
        std::uint32_t cell = 0;
        CHECK(init_ranged_int_var_c580(b, name, &cell, 0, 3, 1, make_args(kRangedIntFnVtbl), desc));
        CHECK(a.base.vptr == kEnumVarFinalVptr && b.base.vptr == kRangedIntMidVptr);
        CHECK(a.fn_vptr == kEnumVarFnVtbl && b.fn_vptr == kRangedIntFnVtbl);
        CHECK(std::memcmp(a.min_value, b.min_value, 4) == 0 && std::memcmp(a.max_value, b.max_value, 4) == 0);
        CHECK(a.fn_payload == b.fn_payload && a.fn_aux == b.fn_aux && std::memcmp(a.args, b.args, sizeof a.args) == 0);
    }
    // ---- an unlifted tag (including a family tag) is refused with nothing written -----------------------------------------
    for (const std::uint64_t tag : {std::uint64_t{0}, std::uint64_t{0x1234567}, kRangedIntFnVtbl, std::uint64_t{0x15ce5f0}}) {
        ConCmdRegistry creg;
        ConCmdRangedVar v;
        poison(v);
        const EnumVarCtorResult r = helper_7ec160_enum_var(v, name, 0, 0, 3, 1, make_args(tag), desc, creg);
        CHECK(!r.ok && r.refusal == HelperRefusal::unadmitted_arg_functor && !r.arg_destroy_ok);
        CHECK(all_poison(v) && creg.pending.size() == 0);
    }
    // ---- the whole call site -----------------------------------------------------------------------------------------------
    {
        ConCmdRegistry creg;
        ConCmdRangedVar v;
        poison(v);
        const HelperIntSiteSpec spec{"gTestEnum", 9, "which one", 9, 0, 3, 1, kEnumVarFnVtbl, 0, 0};
        const HelperIntSiteResult r = helper_init_enum_var_site(v, spec, creg);
        CHECK(r.ok && r.ctor_ok && r.functor_destroy_ok && r.views_inert && r.atexit_recorded && r.registration.ok);
        CHECK(creg.atexit_log.size() == 1 && creg.atexit_log.at(0).dtor_rva == 0x7ea160 && creg.atexit_log.at(0).object == &v);
        CHECK(v.base.vptr == kEnumVarFinalVptr && v.fn_vptr == kEnumVarFnVtbl);
        CHECK(v.base.name.size == 9 && v.base.description.size == 9 && v.base.description.ctl == nullptr);
    }
    {
        ConCmdRegistry creg;
        ConCmdRangedVar v;
        poison(v);
        const HelperIntSiteSpec bad{"gTestEnum", 9, "", 0, 0, 3, 1, 0x1234567, 0, 0};
        const HelperIntSiteResult r = helper_init_enum_var_site(v, bad, creg);
        CHECK(!r.ok && !r.ctor_ok && r.refusal == HelperRefusal::unadmitted_arg_functor);
        CHECK(creg.pending.size() == 0 && creg.atexit_log.size() == 0 && all_poison(v));
    }
    {
        // registration failure is visible and stops before the atexit record
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        ConCmdRangedVar v;
        poison(v);
        const HelperIntSiteSpec spec{"gTestEnum", 9, "", 0, 0, 3, 1, kEnumVarFnVtbl, 0, 0};
        const HelperIntSiteResult r = helper_init_enum_var_site(v, spec, creg);
        CHECK(!r.ok && !r.ctor_ok && !r.atexit_recorded && creg.atexit_log.size() == 0);
    }
    // ---- a variable that is not of the final class has no menu target ---------------------------------------------------------
    {
        ConCmdRangedVar v;
        poison(v);
        const DebugMenuTarget t = enum_var_menu_target(v);
        CHECK(!t.known && t.slot3 == 0 && t.slot6 == 0);
    }
    std::puts("OK test_helper_7ec160");
    return 0;
}
