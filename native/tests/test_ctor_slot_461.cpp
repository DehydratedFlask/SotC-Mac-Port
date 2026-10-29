// CPU-01-STARTUP-SLOT-461: fixture for startup constructor index 461 (0x419480): the single bool console
// variable `drawCameraControllerInfo` (value 1, default 1, functor tag 0x15b5198). Lane m3.
#include "l4_test_util.hpp"

#include "core/ctor_slot_461_419480.hpp"
#include "core/ctor_slot_456_471950.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot461Rva == 0x419480 && kSlot461Size == 323 && kSlot461FnTag == 0x15b5198 && kSlot461ObjectRva == 0x2ef7bf0);
    CHECK(sizeof kSlot461Name - 1 == 24 && std::strcmp(kSlot461Name, "drawCameraControllerInfo") == 0);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot461Objects o;
    poison(o);
    const Slot461Result r = init_slot461_419480(o, creg);
    CHECK(r.ok && r.var.ok && r.var.views_inert && r.var.atexit_recorded);
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_common(o.var, 0x15b5198, 1);
    CHECK(same_view(o.var.base.name, "drawCameraControllerInfo", 24));
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.var.base.pending);
    CHECK(creg.atexit_log.size() == 1);
    check_atexit(creg, 0, 0x20b0, &o.var);
}

void section_with_neighbour() {
    ConCmdRegistry creg;
    Slot461Objects a;
    Slot456Objects b;
    CHECK(init_slot461_419480(a, creg).ok);
    CHECK(init_slot456_471950(b, creg).ok);
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
    Slot461Objects o;
    const Slot461Result r = init_slot461_419480(o, creg);
    CHECK(!r.ok && !r.var.ok && !r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::manager);
    CHECK(creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_with_neighbour();
    section_failures();
    std::puts("OK test_ctor_slot_461");
    return 0;
}
