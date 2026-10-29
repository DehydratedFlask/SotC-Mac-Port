// CPU-01-STARTUP-SLOT-476: fixture for startup constructor index 476 (0x329650): the single bool console
// variable `r_showFogVolumes` (value 0, default 0, functor tag 0x15a7c50). Lane M4.
#include "l4_test_util.hpp"

#include "core/ctor_slot_476_329650.hpp"
#include "core/ctor_slot_479_31b660.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot476Rva == 0x329650 && kSlot476Size == 323);
    CHECK(sizeof kSlot476Name - 1 == 16 && std::strcmp(kSlot476Name, "r_showFogVolumes") == 0);
    CHECK(kSlot476FnTag == 0x15a7c50 && kSlot476ObjectRva == 0x2ef0888);
    CHECK(kSlot476Value == 0 && kSlot476Default == 0);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot476Objects o;
    poison(o);
    const Slot476Result r = init_slot476_329650(o, creg);
    CHECK(r.ok && r.var.ok && r.var.views_inert && r.var.atexit_recorded);
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_common(o.var, 0x15a7c50, 0);
    CHECK(same_view(o.var.base.name, "r_showFogVolumes", 16));
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.var.base.pending);
    CHECK(creg.atexit_log.size() == 1);
    check_atexit(creg, 0, 0x20b0, &o.var);
}

void section_with_neighbour() {
    ConCmdRegistry creg;
    Slot476Objects a;
    Slot479Objects b;
    CHECK(init_slot476_329650(a, creg).ok);
    CHECK(init_slot479_31b660(b, creg).ok);
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
    Slot476Objects o;
    const Slot476Result r = init_slot476_329650(o, creg);
    CHECK(!r.ok && !r.var.ok && !r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::manager);
    CHECK(creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_with_neighbour();
    section_failures();
    std::puts("OK test_ctor_slot_476");
    return 0;
}
