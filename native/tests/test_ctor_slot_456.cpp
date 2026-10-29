// CPU-01-STARTUP-SLOT-456: fixture for startup constructor index 456 (0x471950): the single bool console
// variable `wwise_postSounds` (value 1, default 1, functor tag 0x15bc748). Lane m3.
#include "l4_test_util.hpp"

#include "core/ctor_slot_456_471950.hpp"
#include "core/ctor_slot_457_45c440.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot456Rva == 0x471950 && kSlot456Size == 323 && kSlot456FnTag == 0x15bc748 && kSlot456ObjectRva == 0x2ef8188);
    CHECK(sizeof kSlot456Name - 1 == 16 && std::strcmp(kSlot456Name, "wwise_postSounds") == 0);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot456Objects o;
    poison(o);
    const Slot456Result r = init_slot456_471950(o, creg);
    CHECK(r.ok && r.var.ok && r.var.views_inert && r.var.atexit_recorded);
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_common(o.var, 0x15bc748, 1);
    CHECK(same_view(o.var.base.name, "wwise_postSounds", 16));
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.var.base.pending);
    CHECK(creg.atexit_log.size() == 1);
    check_atexit(creg, 0, 0x20b0, &o.var);
}

void section_with_neighbour() {
    ConCmdRegistry creg;
    Slot456Objects a;
    Slot457Objects b;
    CHECK(init_slot456_471950(a, creg).ok);
    CHECK(init_slot457_45c440(b, creg).ok);
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
    Slot456Objects o;
    const Slot456Result r = init_slot456_471950(o, creg);
    CHECK(!r.ok && !r.var.ok && !r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::manager);
    CHECK(creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_with_neighbour();
    section_failures();
    std::puts("OK test_ctor_slot_456");
    return 0;
}
