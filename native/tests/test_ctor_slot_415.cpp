// CPU-01-STARTUP-SLOT-415: fixture for startup constructor index 415 (0x578b60): the unbounded int variable
// `isysLinkAllocMax` (flags 8, value/default 0, functor table 0x15cb128, worker 0x3a980, atexit 0x18a60). Lane M2.
#include "core/lane_m2_testkit.hpp"

#include "core/ctor_slot_415_578b60.hpp"

namespace {
using namespace sotc::lane_m2;

void section_constants() {
    CHECK(kSlot415Rva == 0x578b60 && kSlot415Size == 331 && kSlot415ArgFnTable == 0x15cb128 && kSlot415CtorFlags == 8);
    CHECK(std::strlen(kSlot415VarName) == 0x10);
    CHECK(kSlot415Spec.arg_fn_table == 0x15cb128 && kSlot415Spec.flags == 8 && kSlot415Spec.value == 0 &&
          kSlot415Spec.default_value == 0);
    CHECK(is_m2_int_arg_functor(0x15cb128) && is_m2_int_arg_functor(0x15ce8c0) && !is_m2_int_arg_functor(0x1234));
}

void section_alone() {
    ConCmdRegistry creg;
    Slot415Objects o;
    poison(o);
    const Slot415Result r = init_slot415_578b60(o, creg);
    CHECK(r.ok && r.var.ok && r.var.worker_ok && r.var.description_inert && r.var.functor_destroy_ok &&
          r.var.name_inert && r.var.atexit_recorded && r.var.registration.ok);
    const ConCmdRangedVar& v = o.var;
    CHECK(v.base.vptr == 0x1594c08 && v.base.registered == 1 && v.base.flags == 8);
    CHECK(same_view(v.base.name, "isysLinkAllocMax", 0x10) && empty_view(v.base.description));
    CHECK(v.value_ref == &v.value && v.default_value == 0 && v.value == 0);
    CHECK(v.min_value[0] == 0 && v.min_engaged == 0 && v.max_value[0] == 0 && v.max_engaged == 0);
    for (std::uint64_t q : v.args) CHECK(q == 0);
    CHECK(v.fn_payload == 0 && v.fn_vptr == 0x15944a0 && v.fn_aux == 0);
    check_untouched(v, {{0x00, 8}, {0x08, 0x10}, {0x18, 1}, {0x1c, 4}, {0x20, 0x40}, {0x60, 8}, {0x68, 4},
                        {0x6c, 1}, {0x70, 1}, {0x74, 1}, {0x78, 1}, {0x80, 0x20}, {0xb0, 8}, {0xc0, 8},
                        {0xc8, 8}, {0xd0, 4}});
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &v.base.pending);
    CHECK(creg.atexit_log.size() == 1);
    check_atexit(creg, 0, 0x18a60, &o.var);
}

void section_unknown_functor_refused() {
    ConCmdRegistry creg;
    ConCmdRangedVar v;
    poison(v);
    lane_m2::IntVarSpec bad = kSlot415Spec;
    bad.arg_fn_table = 0x1234;
    const lane_m2::IntVarResult r = lane_m2::init_unbounded_int_var_site(v, bad, creg);
    CHECK(!r.ok && !r.worker_ok && creg.pending.size() == 0 && creg.atexit_log.size() == 0);
    const auto* b = reinterpret_cast<const std::uint8_t*>(&v);
    for (std::size_t i = 0; i < sizeof v; ++i) CHECK(b[i] == kPoison);
}

void section_failures() {
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;
    Slot415Objects o;
    const Slot415Result r = init_slot415_578b60(o, creg);
    CHECK(!r.ok && !r.var.registration.ok && creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_unknown_functor_refused();
    section_failures();
    std::puts("OK test_ctor_slot_415");
    return 0;
}
