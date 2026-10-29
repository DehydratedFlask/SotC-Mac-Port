// CPU-01-STARTUP-SLOT-459: fixture for startup constructor index 459 (0x43f460): the single bool console
// variable `SM_printAnimPhase` (value 0, default 0, functor tag 0x15b70d0). Lane m3.
#include "l4_test_util.hpp"

#include "core/ctor_slot_459_43f460.hpp"
#include "core/ctor_slot_461_419480.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot459Rva == 0x43f460 && kSlot459Size == 299 && kSlot459FnTag == 0x15b70d0 && kSlot459ObjectRva == 0x2ef7f30);
    CHECK(sizeof kSlot459Name - 1 == 17 && std::strcmp(kSlot459Name, "SM_printAnimPhase") == 0);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot459Objects o;
    poison(o);
    const Slot459Result r = init_slot459_43f460(o, creg);
    CHECK(r.ok && r.var.ok && r.var.views_inert && r.var.atexit_recorded);
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_common(o.var, 0x15b70d0, 0);
    CHECK(same_view(o.var.base.name, "SM_printAnimPhase", 17));
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.var.base.pending);
    CHECK(creg.atexit_log.size() == 1);
    check_atexit(creg, 0, 0x20b0, &o.var);
}

void section_with_neighbour() {
    ConCmdRegistry creg;
    Slot459Objects a;
    Slot461Objects b;
    CHECK(init_slot459_43f460(a, creg).ok);
    CHECK(init_slot461_419480(b, creg).ok);
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
    Slot459Objects o;
    const Slot459Result r = init_slot459_43f460(o, creg);
    CHECK(!r.ok && !r.var.ok && !r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::manager);
    CHECK(creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_with_neighbour();
    section_failures();
    std::puts("OK test_ctor_slot_459");
    return 0;
}
