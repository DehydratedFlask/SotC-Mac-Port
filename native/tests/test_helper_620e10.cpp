// CPU-01-HELPER-CALLEE-MODELS-02: fixture for the local ranged-int variable wrapper family (0x620e10 / 0x6211d0 and 24 twins; startup slot 401).
// ELF pins: tools/lift/check_helper_620e10.py. FIXTURE_VERIFIED. Each object is the 0xc580 ranged-int object with a private class pair.
#include "core/hashed_string_14c3a0.hpp"
#include "core/helper_620e10_local_int_var.hpp"
#include "core/helper_7ec160_enum_var.hpp"
#include "core/menu_item_26db0.hpp"
#include "helper_test_util.hpp"

using namespace sotc;
using namespace helper_test;

int main() {
    // ---- the table ---------------------------------------------------------------------------------------------------------
    CHECK(kLocalIntVarWrapperSize == 0x1b3 && kLocalIntVarInnerSize == 0x15f && kLocalIntVarClassCount == 26);
    CHECK(kLocalIntVarSlot3Result == 1 && kLocalIntVarSlot6Result == 1);
    for (std::size_t i = 0; i < kLocalIntVarClasses.size(); ++i) {
        const LocalIntVarClass& row = kLocalIntVarClasses[i];
        CHECK(i == 0 || kLocalIntVarClasses[i - 1].wrapper_rva < row.wrapper_rva);            // strictly increasing, no duplicates
        CHECK(row.final_vptr == row.fn_vtbl + 0x30 && row.inner_rva == row.wrapper_rva + 0x1c0);   // 3 x 0x10 apart in .data.rel.ro
        CHECK(row.mid_vptr < row.fn_vtbl && row.mid_vptr != row.final_vptr);
        CHECK(!is_int_functor_tag(row.fn_vtbl) && !is_float_functor_tag(row.fn_vtbl));       // a private table, not a family member
        CHECK(find_local_int_var_class(row.wrapper_rva) == &row && find_local_int_var_class_by_final(row.final_vptr) == &row);
    }
    CHECK(find_local_int_var_class(0x7ec161) == nullptr && find_local_int_var_class_by_final(0x1594c08) == nullptr);
    // the 0x7ec160 row agrees with the separate enum module
    {
        const LocalIntVarClass* row = find_local_int_var_class(kEnumVarWrapperRva);
        CHECK(row != nullptr && row->inner_rva == kEnumVarCtorRva && row->mid_vptr == kEnumVarMidVptr && row->fn_vtbl == kEnumVarFnVtbl);
        CHECK(row->final_vptr == kEnumVarFinalVptr && row->dtor_rva == kEnumVarDtorRva);
        CHECK(row->slot3_rva == kEnumVarSlot3Rva && row->slot6_rva == kEnumVarSlot6Rva);
    }
    // the two slot-401 rows
    {
        const LocalIntVarClass* a = find_local_int_var_class(0x620e10);
        const LocalIntVarClass* b = find_local_int_var_class(0x6211d0);
        CHECK(a != nullptr && b != nullptr && a != b);
        CHECK(a->inner_rva == 0x620fd0 && a->mid_vptr == 0x15cc218 && a->fn_vtbl == 0x15cc318 && a->final_vptr == 0x15cc348 && a->dtor_rva == 0x6112e0);
        CHECK(b->inner_rva == 0x621390 && b->mid_vptr == 0x15cc290 && b->fn_vtbl == 0x15cc3b0 && b->final_vptr == 0x15cc3e0 && b->dtor_rva == 0x611320);
    }

    const SharedStr name = view("gLocalVar");
    const SharedStr desc = view("which one");

    // ---- exact stores for every family member (min 1, max 4, init 3) -------------------------------------------------------
    for (const LocalIntVarClass& row : kLocalIntVarClasses) {
        ConCmdRegistry creg;
        ConCmdRangedVar v;
        poison(v);
        const LocalIntVarCtorResult r = helper_local_int_var_ctor(row.wrapper_rva, v, name, 3, 1, 4, 1, make_args(row.fn_vtbl, 0xABCD, 0x99), desc, creg);
        CHECK(r.ok && r.refusal == HelperRefusal::none && r.arg_destroy_ok && r.registration.ok);
        CHECK(v.base.vptr == row.final_vptr && v.base.registered == 1 && v.base.flags == 1);
        CHECK(v.value_ref == &v.value && v.value == 3 && v.default_value == 3);
        CHECK(v.min_engaged == 1 && v.max_engaged == 1);
        std::int32_t lo = -1, hi = -1;
        std::memcpy(&lo, v.min_value, 4);
        std::memcpy(&hi, v.max_value, 4);
        CHECK(lo == 1 && hi == 4);
        CHECK(v.fn_vptr == row.fn_vtbl && v.fn_payload == 0xABCD && v.fn_aux == 0x99);
        CHECK(creg.pending.size() == 1 && creg.pending.first() == &v.base.pending);
        const DebugMenuTarget t = local_int_var_menu_target(v);
        CHECK(t.known && t.slot3 == 1 && t.slot6 == 1 && t.object == &v);
    }
    // ---- caller twins (slots 398 / 399 pass their own private int-body class to the shared wrapper 0x529160): admitted, the object keeps the WRAPPER's class ----
    {
        CHECK(kLocalIntCallerTwins.size() == 2 && is_local_int_caller_twin_tag(0x15cc4a8, 0x15c8268) && is_local_int_caller_twin_tag(0x15cc508, 0x15c8268));
        CHECK(!is_local_int_caller_twin_tag(0x15cc318, 0x15c8268) && !is_local_int_caller_twin_tag(0x15c8268, 0x15c8268) && !is_local_int_caller_twin_tag(kRangedIntFnVtbl, 0x15c8268) &&
              !is_local_int_caller_twin_tag(0, 0x15c8268) && !is_local_int_caller_twin_tag(0x15cc508, 0x15cc318));   // an alias of ANOTHER wrapper's class is not admitted
        const LocalIntVarClass* cls = find_local_int_var_class(0x529160);
        CHECK(cls != nullptr && cls->fn_vtbl == 0x15c8268 && cls->final_vptr == 0x15c8298 && cls->dtor_rva == 0x526000);
        for (const LocalIntCallerTwin& tw : kLocalIntCallerTwins) {
            const std::uint64_t twin = tw.tag;
            CHECK(!is_int_functor_tag(twin) && twin != cls->fn_vtbl && tw.aliased_class == cls->fn_vtbl);
            ConCmdRegistry creg;
            ConCmdRangedVar v;
            poison(v);
            const LocalIntVarCtorResult r = helper_local_int_var_ctor(0x529160, v, name, 0, 0, 4, 1, make_args(twin, 0x1234, 0x77), desc, creg);
            CHECK(r.ok && r.refusal == HelperRefusal::none && r.arg_destroy_ok && r.registration.ok);
            CHECK(v.base.vptr == cls->final_vptr && v.fn_vptr == cls->fn_vtbl && v.fn_payload == 0x1234 && v.fn_aux == 0x77);   // the twin's class is not left behind
            CHECK(v.value == 0 && v.default_value == 0 && creg.pending.size() == 1);
        }
    }
    {
        ConCmdRegistry creg;
        ConCmdRangedVar v;
        poison(v);
        const LocalIntVarCtorResult r = helper_local_int_var_ctor(0x620e10, v, name, 0, 0, 2, 1, make_args(0x15cc508), desc, creg);   // a twin of 0x529160's class, not of 0x620e10's
        CHECK(!r.ok && r.refusal == HelperRefusal::unadmitted_arg_functor && all_poison(v) && creg.pending.size() == 0);
    }
    // ---- the object equals the 0xc580 object outside the retagged cells ----------------------------------------------------
    {
        ConCmdRegistry creg;
        ConCmdRangedVar a;
        ConCmdRangedVar b;
        poison(a);
        poison(b);
        CHECK(helper_local_int_var_ctor(0x620e10, a, name, 0, 0, 2, 1, make_args(0x15cc318), desc, creg).ok);
        std::uint32_t cell = 0;
        CHECK(init_ranged_int_var_c580(b, name, &cell, 0, 2, 1, make_args(kRangedIntFnVtbl), desc));
        CHECK(a.base.vptr == 0x15cc348 && b.base.vptr == kRangedIntMidVptr && a.fn_vptr == 0x15cc318 && b.fn_vptr == kRangedIntFnVtbl);
        CHECK(std::memcmp(a.min_value, b.min_value, 4) == 0 && std::memcmp(a.max_value, b.max_value, 4) == 0);
        CHECK(a.fn_payload == b.fn_payload && a.fn_aux == b.fn_aux && std::memcmp(a.args, b.args, sizeof a.args) == 0);
    }
    // ---- q[3] != 0 makes args[0] self-referential (the inner constructor's argument copy) -----------------------------------
    {
        ConCmdRegistry creg;
        ConCmdRangedVar v;
        poison(v);
        CHECK(helper_local_int_var_ctor(0x6211d0, v, name, -2, -5, 9, 1, make_args(0x15cc3b0, 1, 2, 0, 0x55), desc, creg).ok);
        CHECK(v.value == static_cast<std::uint32_t>(-2) && v.args[0] == reinterpret_cast<std::uint64_t>(&v.args[0]));
    }
    // ---- a tag other than the row's own class (a family tag, a neighbour's class, garbage) is refused with nothing written -----
    for (const std::uint64_t tag : {std::uint64_t{0}, std::uint64_t{0x1234567}, kRangedIntFnVtbl, std::uint64_t{0x15cc3b0}, std::uint64_t{0x15ce538}}) {
        ConCmdRegistry creg;
        ConCmdRangedVar v;
        poison(v);
        const LocalIntVarCtorResult r = helper_local_int_var_ctor(0x620e10, v, name, 0, 0, 2, 1, make_args(tag), desc, creg);
        CHECK(!r.ok && r.refusal == HelperRefusal::unadmitted_arg_functor && !r.arg_destroy_ok);
        CHECK(all_poison(v) && creg.pending.size() == 0);
    }
    // ---- an RVA that is not a family member is refused with nothing written ------------------------------------------------------
    for (const std::uint64_t rva : {std::uint64_t{0}, std::uint64_t{0xc580}, std::uint64_t{0x620e11}, std::uint64_t{0x620fd0}}) {
        ConCmdRegistry creg;
        ConCmdRangedVar v;
        poison(v);
        const LocalIntVarCtorResult r = helper_local_int_var_ctor(rva, v, name, 0, 0, 2, 1, make_args(0x15cc318), desc, creg);
        CHECK(!r.ok && r.refusal == HelperRefusal::wrong_helper && all_poison(v) && creg.pending.size() == 0);
    }
    // ---- the two whole call sites of slot 401 (description = the static empty string, not released) ------------------------------
    {
        ConCmdRegistry creg;
        ConCmdRangedVar a;
        ConCmdRangedVar b;
        poison(a);
        poison(b);
        const HelperIntSiteResult ra = helper_init_local_int_var_site(a, kSlot401PhaseBlendUseSite, empty_string_f730(), creg);
        const HelperIntSiteResult rb = helper_init_local_int_var_site(b, kSlot401PhaseBlendTypeSite, empty_string_f730(), creg);
        CHECK(ra.ok && ra.ctor_ok && ra.functor_destroy_ok && ra.views_inert && ra.atexit_recorded && ra.registration.ok);
        CHECK(rb.ok && rb.ctor_ok && rb.functor_destroy_ok && rb.views_inert && rb.atexit_recorded && rb.registration.ok);
        CHECK(creg.atexit_log.size() == 2);
        CHECK(creg.atexit_log.at(0).dtor_rva == 0x6112e0 && creg.atexit_log.at(0).object == &a);
        CHECK(creg.atexit_log.at(1).dtor_rva == 0x611320 && creg.atexit_log.at(1).object == &b);
        CHECK(a.base.vptr == 0x15cc348 && b.base.vptr == 0x15cc3e0 && a.fn_vptr == 0x15cc318 && b.fn_vptr == 0x15cc3b0);
        CHECK(a.base.name.size == 18 && b.base.name.size == 27 && a.base.description.size == 0 && b.base.description.size == 0);
        std::int32_t lo = -1, hi = -1;
        std::memcpy(&lo, a.min_value, 4);
        std::memcpy(&hi, a.max_value, 4);
        CHECK(lo == 0 && hi == 2 && a.value == 0 && a.default_value == 0);
        std::memcpy(&lo, b.min_value, 4);
        std::memcpy(&hi, b.max_value, 4);
        CHECK(lo == 1 && hi == 4 && b.value == 0 && b.default_value == 0);
        CHECK(creg.pending.size() == 2);
    }
    // the 7ec160 order releases the description first; a borrowed view releases inertly
    {
        ConCmdRegistry creg;
        ConCmdRangedVar v;
        poison(v);
        LocalIntSiteSpec spec = kSlot401PhaseBlendUseSite;
        spec.release_description = true;
        const HelperIntSiteResult r = helper_init_local_int_var_site(v, spec, desc, creg);
        CHECK(r.ok && r.views_inert && creg.atexit_log.size() == 1);
    }
    // a bad site tag is visible and stops before the atexit record
    {
        ConCmdRegistry creg;
        ConCmdRangedVar v;
        poison(v);
        LocalIntSiteSpec bad = kSlot401PhaseBlendUseSite;
        bad.arg_fn_tag = 0x15cc3b0;
        const HelperIntSiteResult r = helper_init_local_int_var_site(v, bad, empty_string_f730(), creg);
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
        const HelperIntSiteResult r = helper_init_local_int_var_site(v, kSlot401PhaseBlendUseSite, empty_string_f730(), creg);
        CHECK(!r.ok && !r.ctor_ok && !r.atexit_recorded && creg.atexit_log.size() == 0);
    }
    // ---- the 0x26db0 menu item over a family variable keeps the {1.0, 10.0} range; an unrelated variable is refused ---------------
    {
        ConCmdRegistry creg;
        ConCmdRangedVar v;
        poison(v);
        CHECK(helper_init_local_int_var_site(v, kSlot401PhaseBlendUseSite, empty_string_f730(), creg).ok);
        DebugMenuItem item;
        poison(item);
        CHECK(helper_26db0_named_item_local_int(item, view("parent"), v, 0) == HelperRefusal::none);
        CHECK(item.range_min == 1.0f && item.range_max == 10.0f);
        ConCmdRangedVar stray;
        poison(stray);
        DebugMenuItem untouched;
        poison(untouched);
        CHECK(helper_26db0_named_item_local_int(untouched, view("parent"), stray, 0) == HelperRefusal::unknown_target_class);
        CHECK(all_poison(untouched));
        const DebugMenuTarget t = local_int_var_menu_target(stray);
        CHECK(!t.known && t.slot3 == 0 && t.slot6 == 0);
    }
    std::puts("OK test_helper_620e10");
    return 0;
}
