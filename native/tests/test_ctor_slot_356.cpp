// CPU-01-STARTUP-SLOT-356: fixture for startup constructor index 356 (0x78e2f0): the single bool console
// variable `shadow_weapons_newWeaponCyclingLogic` (value 1, default 1, functor tag 0x15cdfb0). Lane L4.
#include "l4_test_util.hpp"

#include "core/ctor_slot_356_78e2f0.hpp"
#include "core/ctor_slot_348_7c9150.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot356Rva == 0x78e2f0 && kSlot356Size == 299);
    CHECK(sizeof kSlot356Name - 1 == 36 && std::strcmp(kSlot356Name, "shadow_weapons_newWeaponCyclingLogic") == 0);
    CHECK(kSlot356FnTag == 0x15cdfb0);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot356Objects o;
    poison(o);
    const Slot356Result r = init_slot356_78e2f0(o, creg);
    CHECK(r.ok && r.var.ok && r.var.views_inert && r.var.atexit_recorded);
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_common(o.var, 0x15cdfb0, 1);
    CHECK(same_view(o.var.base.name, "shadow_weapons_newWeaponCyclingLogic", 36));
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.var.base.pending);
    CHECK(creg.atexit_log.size() == 1);
    check_atexit(creg, 0, 0x20b0, &o.var);
}

void section_with_neighbour() {
    ConCmdRegistry creg;
    Slot356Objects a;
    Slot348Objects b;
    CHECK(init_slot356_78e2f0(a, creg).ok);
    CHECK(init_slot348_7c9150(b, creg).ok);
    CHECK(creg.pending.size() == 2);
    CHECK(a.var.base.pending.next == &b.var.base.pending && b.var.base.pending.next == &creg.pending.head);
    CHECK(creg.atexit_log.size() == 2);
    check_atexit(creg, 0, 0x20b0, &a.var);
    check_atexit(creg, 1, 0x20b0, &b.var);
}

void section_failures() {
    // A manager without an insert contract: the registration fails visibly and nothing is queued.
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;
    Slot356Objects o;
    const Slot356Result r = init_slot356_78e2f0(o, creg);
    CHECK(!r.ok && !r.var.ok && !r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::manager);
    CHECK(creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_with_neighbour();
    section_failures();
    std::puts("OK test_ctor_slot_356");
    return 0;
}
