// CPU-01-STARTUP-SLOT-365: fixture for startup constructor index 365 (0x747870). Lane M1.
#include "l4_test_util.hpp"

#include "core/ctor_slot_365_747870.hpp"
#include "core/ctor_slot_376_6e4930.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot365Rva == 0x747870 && kSlot365Size == 299 && kSlot365FnTag == 0x15cd220 && kSlot365ObjectRva == 0x32524c8);
    CHECK(sizeof kSlot365Name - 1 == 15 && std::strcmp(kSlot365Name, "nico_freezeBoss") == 0);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot365Objects o;
    poison(o);
    const Slot365Result r = init_slot365_747870(o, creg);
    CHECK(r.ok && r.var.ok && r.var.views_inert && r.var.atexit_recorded);
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_common(o.var, 0x15cd220, 0);
    CHECK(same_view(o.var.base.name, "nico_freezeBoss", 15));
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.var.base.pending);
    CHECK(creg.atexit_log.size() == 1);
    check_atexit(creg, 0, 0x20b0, &o.var);
}

void section_with_neighbour() {
    ConCmdRegistry creg;
    Slot365Objects a;
    Slot376Objects b;
    CHECK(init_slot365_747870(a, creg).ok);
    CHECK(init_slot376_6e4930(b, creg).ok);
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
    Slot365Objects o;
    const Slot365Result r = init_slot365_747870(o, creg);
    CHECK(!r.ok && !r.var.ok && !r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::manager);
    CHECK(creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_with_neighbour();
    section_failures();
    std::puts("OK test_ctor_slot_365");
    return 0;
}
