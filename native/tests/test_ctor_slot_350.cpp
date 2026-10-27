// CPU-01-STARTUP-SLOT-350: fixture for startup constructor index 350 (0x7af0b0): `nico_PhotoModeLwsZoomControlSpeed`
// (bounded, 0xc300) and `shadow_bowZoomScaleOnHorseback` (unbounded, 0x6aa00), both with functor 0x15ce260. Lane L4.
#include "l4_test_util.hpp"

#include "core/ctor_slot_350_7af0b0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot350Rva == 0x7af0b0 && kSlot350Size == 610);
    CHECK(std::strlen(kSlot350SpecA.name) == 0x21 && std::strlen(kSlot350SpecB.name) == 0x1e);
    CHECK(std::strcmp(kSlot350SpecA.name, "nico_PhotoModeLwsZoomControlSpeed") == 0);
    CHECK(std::strcmp(kSlot350SpecB.name, "shadow_bowZoomScaleOnHorseback") == 0);
    CHECK(kSlot350SpecA.bounded && !kSlot350SpecB.bounded);
    CHECK(kSlot350SpecA.lo_bits == 0x3dcccccdu && kSlot350SpecA.hi_bits == 0x40800000u);
    CHECK(kSlot350SpecA.default_bits == 0x3f000000u && kSlot350SpecB.default_bits == 0x3f800000u);
    CHECK(kSlot350SpecA.arg_fn_vtbl == 0x15ce260 && kSlot350SpecB.arg_fn_vtbl == 0x15ce260);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot350Objects o;
    poison(o);
    const Slot350Result r = init_slot350_7af0b0(o, creg);
    CHECK(r.ok && r.var_a.ok && r.var_b.ok);
    check_float_common(o.var_a, true, 0x3dcccccdu, 0x40800000u, 0x3f000000u);
    check_float_common(o.var_b, false, 0u, 0u, 0x3f800000u);
    CHECK(same_view(o.var_a.base.name, "nico_PhotoModeLwsZoomControlSpeed", 0x21));
    CHECK(same_view(o.var_b.base.name, "shadow_bowZoomScaleOnHorseback", 0x1e));
    CHECK(creg.pending.size() == 2);
    CHECK(creg.pending.first() == &o.var_a.base.pending && o.var_a.base.pending.next == &o.var_b.base.pending);
    CHECK(creg.atexit_log.size() == 2);
    check_atexit(creg, 0, 0x20f0, &o.var_a);
    check_atexit(creg, 1, 0x20f0, &o.var_b);
}

void section_failures() {
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;
    Slot350Objects o;
    const Slot350Result r = init_slot350_7af0b0(o, creg);
    CHECK(!r.ok && !r.var_a.ok && !r.var_b.ctor_ok);   // B is never started after A fails
    CHECK(creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_350");
    return 0;
}
