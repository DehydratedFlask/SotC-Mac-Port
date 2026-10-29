// CPU-01-STARTUP-SLOT-457: fixture for startup constructor index 457 (0x45c440): the single bool console
// variable `showCollisionMeshTests` (value 0, default 0, functor tag 0x15bad48). Lane m3.
#include "l4_test_util.hpp"

#include "core/ctor_slot_457_45c440.hpp"
#include "core/ctor_slot_458_45ace0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot457Rva == 0x45c440 && kSlot457Size == 323 && kSlot457FnTag == 0x15bad48 && kSlot457ObjectRva == 0x2ef80c0);
    CHECK(sizeof kSlot457Name - 1 == 22 && std::strcmp(kSlot457Name, "showCollisionMeshTests") == 0);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot457Objects o;
    poison(o);
    const Slot457Result r = init_slot457_45c440(o, creg);
    CHECK(r.ok && r.var.ok && r.var.views_inert && r.var.atexit_recorded);
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_common(o.var, 0x15bad48, 0);
    CHECK(same_view(o.var.base.name, "showCollisionMeshTests", 22));
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.var.base.pending);
    CHECK(creg.atexit_log.size() == 1);
    check_atexit(creg, 0, 0x20b0, &o.var);
}

void section_with_neighbour() {
    ConCmdRegistry creg;
    Slot457Objects a;
    Slot458Objects b;
    CHECK(init_slot457_45c440(a, creg).ok);
    CHECK(init_slot458_45ace0(b, creg).ok);
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
    Slot457Objects o;
    const Slot457Result r = init_slot457_45c440(o, creg);
    CHECK(!r.ok && !r.var.ok && !r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::manager);
    CHECK(creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_with_neighbour();
    section_failures();
    std::puts("OK test_ctor_slot_457");
    return 0;
}
