// CPU-01-STARTUP-SLOT-348: fixture for startup constructor index 348 (0x7c9150): the single bool console
// variable `cp3_shadow_enableDebugMove` (value 1, default 1, functor tag 0x15ce2f8). Lane L4.
#include "l4_test_util.hpp"

#include "core/ctor_slot_348_7c9150.hpp"
#include "core/ctor_slot_356_78e2f0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot348Rva == 0x7c9150 && kSlot348Size == 299);
    CHECK(sizeof kSlot348Name - 1 == 26 && std::strcmp(kSlot348Name, "cp3_shadow_enableDebugMove") == 0);
    CHECK(kSlot348FnTag == 0x15ce2f8);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot348Objects o;
    poison(o);
    const Slot348Result r = init_slot348_7c9150(o, creg);
    CHECK(r.ok && r.var.ok && r.var.views_inert && r.var.atexit_recorded);
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_common(o.var, 0x15ce2f8, 1);
    CHECK(same_view(o.var.base.name, "cp3_shadow_enableDebugMove", 26));
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.var.base.pending);
    CHECK(creg.atexit_log.size() == 1);
    check_atexit(creg, 0, 0x20b0, &o.var);
}

void section_with_neighbour() {
    ConCmdRegistry creg;
    Slot348Objects a;
    Slot356Objects b;
    CHECK(init_slot348_7c9150(a, creg).ok);
    CHECK(init_slot356_78e2f0(b, creg).ok);
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
    Slot348Objects o;
    const Slot348Result r = init_slot348_7c9150(o, creg);
    CHECK(!r.ok && !r.var.ok && !r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::manager);
    CHECK(creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_with_neighbour();
    section_failures();
    std::puts("OK test_ctor_slot_348");
    return 0;
}
