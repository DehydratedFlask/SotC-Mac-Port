// CPU-01-STARTUP-SLOT-499: fixture for startup constructor index 499 (0x2500e0): the single bool console
// variable `r_showDirectionalBoxLights` (value 0, default 0, functor tag 0x159c318). Lane M5.
#include "l4_test_util.hpp"

#include "core/ctor_slot_499_2500e0.hpp"
#include "core/ctor_slot_502_249270.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot499Rva == 0x2500e0 && kSlot499Size == 323);
    CHECK(sizeof kSlot499Name - 1 == 26 && std::strcmp(kSlot499Name, "r_showDirectionalBoxLights") == 0);
    CHECK(kSlot499FnTag == 0x159c318 && kSlot499ObjectRva == 0x2eebc58);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot499Objects o;
    poison(o);
    const Slot499Result r = init_slot499_2500e0(o, creg);
    CHECK(r.ok && r.var.ok && r.var.views_inert && r.var.atexit_recorded);
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_common(o.var, 0x159c318, 0);
    CHECK(same_view(o.var.base.name, "r_showDirectionalBoxLights", 26));
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.var.base.pending);
    CHECK(creg.atexit_log.size() == 1);
    check_atexit(creg, 0, 0x20b0, &o.var);
}

void section_with_neighbour() {
    ConCmdRegistry creg;
    Slot499Objects a;
    Slot502Objects b;
    CHECK(init_slot499_2500e0(a, creg).ok);
    CHECK(init_slot502_249270(b, creg).ok);
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
    Slot499Objects o;
    const Slot499Result r = init_slot499_2500e0(o, creg);
    CHECK(!r.ok && !r.var.ok && !r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::manager);
    CHECK(creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_with_neighbour();
    section_failures();
    std::puts("OK test_ctor_slot_499");
    return 0;
}
