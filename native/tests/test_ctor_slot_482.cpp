// CPU-01-STARTUP-SLOT-482: fixture for startup constructor index 482 (0x2fb2a0): the single bool console
// variable `network_WarnAboutServerObjectsOnClient` (value 1, default 1, functor tag 0x15a52f8). Lane M4.
#include "l4_test_util.hpp"

#include "core/ctor_slot_482_2fb2a0.hpp"
#include "core/ctor_slot_490_296d90.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot482Rva == 0x2fb2a0 && kSlot482Size == 323);
    CHECK(sizeof kSlot482Name - 1 == 38 && std::strcmp(kSlot482Name, "network_WarnAboutServerObjectsOnClient") == 0);
    CHECK(kSlot482FnTag == 0x15a52f8 && kSlot482ObjectRva == 0x2eefd30);
    CHECK(kSlot482Value == 1 && kSlot482Default == 1);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot482Objects o;
    poison(o);
    const Slot482Result r = init_slot482_2fb2a0(o, creg);
    CHECK(r.ok && r.var.ok && r.var.views_inert && r.var.atexit_recorded);
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_common(o.var, 0x15a52f8, 1);
    CHECK(same_view(o.var.base.name, "network_WarnAboutServerObjectsOnClient", 38));
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.var.base.pending);
    CHECK(creg.atexit_log.size() == 1);
    check_atexit(creg, 0, 0x20b0, &o.var);
}

void section_with_neighbour() {
    ConCmdRegistry creg;
    Slot482Objects a;
    Slot490Objects b;
    CHECK(init_slot482_2fb2a0(a, creg).ok);
    CHECK(init_slot490_296d90(b, creg).ok);
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
    Slot482Objects o;
    const Slot482Result r = init_slot482_2fb2a0(o, creg);
    CHECK(!r.ok && !r.var.ok && !r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::manager);
    CHECK(creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_with_neighbour();
    section_failures();
    std::puts("OK test_ctor_slot_482");
    return 0;
}
