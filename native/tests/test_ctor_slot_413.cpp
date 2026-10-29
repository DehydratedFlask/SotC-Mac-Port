// CPU-01-STARTUP-SLOT-413: fixture for startup constructor index 413 (0x57c810): the bounded float variable
// `shadow_deadZoneDefault` (min 0, max 1.0, default 0x3e780347, functor 0x15cb148). Lane M2.
#include "core/lane_m2_testkit.hpp"

#include "core/ctor_slot_413_57c810.hpp"

namespace {
using namespace sotc::lane_m2;

void section_constants() {
    CHECK(kSlot413Rva == 0x57c810 && kSlot413Size == 370 && kSlot413FnTag == 0x15cb148);
    CHECK(std::strlen(kSlot413VarName) == 0x16);
    CHECK(kSlot413Spec.bounded && kSlot413Spec.lo_bits == 0 && kSlot413Spec.hi_bits == 0x3f800000u &&
          kSlot413Spec.default_bits == 0x3e780347u && kSlot413Spec.arg_fn_vtbl == 0x15cb148 && kSlot413Spec.flags == 1);
    CHECK(is_m2_float_arg_functor(0x15cb148) && is_m2_float_arg_functor(0x15ce318) && !is_m2_float_arg_functor(0x1234));
}

void section_alone() {
    ConCmdRegistry creg;
    Slot413Objects o;
    poison(o);
    const Slot413Result r = init_slot413_57c810(o, creg);
    CHECK(r.ok && r.var.ok && r.var.ctor_ok && r.var.views_inert && r.var.functor_destroy_ok && r.var.atexit_recorded);
    check_float_common(o.var, true, 0u, 0x3f800000u, 0x3e780347u);
    CHECK(same_view(o.var.base.name, "shadow_deadZoneDefault", 0x16));
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.var.base.pending);
    CHECK(creg.atexit_log.size() == 1);
    check_atexit(creg, 0, 0x20f0, &o.var);
}

void section_unknown_functor_refused() {
    ConCmdRegistry creg;
    ConCmdRangedVar v;
    poison(v);
    FloatVarSpec bad = kSlot413Spec;
    bad.arg_fn_vtbl = 0x1234;
    const FloatVarResult r = init_float_var_site(v, bad, creg);
    CHECK(!r.ok && !r.ctor_ok && creg.pending.size() == 0 && creg.atexit_log.size() == 0);
    const auto* b = reinterpret_cast<const std::uint8_t*>(&v);
    for (std::size_t i = 0; i < sizeof v; ++i) CHECK(b[i] == kPoison);   // refused before any write
}

void section_failures() {
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;
    Slot413Objects o;
    const Slot413Result r = init_slot413_57c810(o, creg);
    CHECK(!r.ok && !r.var.registration.ok && creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_unknown_functor_refused();
    section_failures();
    std::puts("OK test_ctor_slot_413");
    return 0;
}
