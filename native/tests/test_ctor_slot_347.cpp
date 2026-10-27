// CPU-01-STARTUP-SLOT-347: fixture for startup constructor index 347 (0x7c93a0): the bounded float variable
// `nico_AltDiveAttackHoldTime` (0xc300, min 0, max 10, default 0x3e19999a, functor 0x15ce318). Lane L4.
#include "l4_test_util.hpp"

#include "core/ctor_slot_347_7c93a0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot347Rva == 0x7c93a0 && kSlot347Size == 370);
    CHECK(std::strcmp(kSlot347Spec.name, "nico_AltDiveAttackHoldTime") == 0 && std::strlen(kSlot347Spec.name) == 0x1a);
    CHECK(kSlot347Spec.bounded && kSlot347Spec.lo_bits == 0 && kSlot347Spec.hi_bits == 0x41200000u);
    CHECK(kSlot347Spec.default_bits == 0x3e19999au && kSlot347Spec.arg_fn_vtbl == 0x15ce318 && kSlot347Spec.flags == 1);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot347Objects o;
    poison(o);
    const Slot347Result r = init_slot347_7c93a0(o, creg);
    CHECK(r.ok && r.var.ok && r.var.ctor_ok && r.var.functor_destroy_ok && r.var.views_inert && r.var.atexit_recorded);
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);
    check_float_common(o.var, true, 0u, 0x41200000u, 0x3e19999au);
    CHECK(same_view(o.var.base.name, "nico_AltDiveAttackHoldTime", 0x1a));
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.var.base.pending);
    CHECK(creg.atexit_log.size() == 1);
    check_atexit(creg, 0, 0x20f0, &o.var);
}

void section_failures() {
    {
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot347Objects o;
        const Slot347Result r = init_slot347_7c93a0(o, creg);
        CHECK(!r.ok && !r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::manager);
        CHECK(creg.pending.size() == 0);
    }
    {
        // An unknown argument functor class: nothing is written and the slot fails visibly.
        ConCmdRegistry creg;
        ConCmdRangedVar v;
        poison(v);
        FloatVarSpec bad = kSlot347Spec;
        bad.arg_fn_vtbl = 0x1234567;
        const FloatVarResult r = init_float_var_static(v, bad, creg);
        CHECK(!r.ok && !r.ctor_ok && creg.pending.size() == 0 && creg.atexit_log.size() == 0);
        const auto* b = reinterpret_cast<const std::uint8_t*>(&v);
        for (std::size_t i = 0x60; i < sizeof v; ++i) CHECK(b[i] == kPoison);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_347");
    return 0;
}
