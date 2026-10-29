// CPU-01-STARTUP-SLOT-479: fixture for startup constructor index 479 (0x31b660): the single bool console
// variable `uiWarnOnUnhandledEvents` (value 0, default 0, functor tag 0x15a6eb0). Lane M4.
#include "l4_test_util.hpp"

#include "core/ctor_slot_479_31b660.hpp"
#include "core/ctor_slot_482_2fb2a0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot479Rva == 0x31b660 && kSlot479Size == 299);
    CHECK(sizeof kSlot479Name - 1 == 23 && std::strcmp(kSlot479Name, "uiWarnOnUnhandledEvents") == 0);
    CHECK(kSlot479FnTag == 0x15a6eb0 && kSlot479ObjectRva == 0x2ef03d8);
    CHECK(kSlot479Value == 0 && kSlot479Default == 0);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot479Objects o;
    poison(o);
    const Slot479Result r = init_slot479_31b660(o, creg);
    CHECK(r.ok && r.var.ok && r.var.views_inert && r.var.atexit_recorded);
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_common(o.var, 0x15a6eb0, 0);
    CHECK(same_view(o.var.base.name, "uiWarnOnUnhandledEvents", 23));
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.var.base.pending);
    CHECK(creg.atexit_log.size() == 1);
    check_atexit(creg, 0, 0x20b0, &o.var);
}

void section_with_neighbour() {
    ConCmdRegistry creg;
    Slot479Objects a;
    Slot482Objects b;
    CHECK(init_slot479_31b660(a, creg).ok);
    CHECK(init_slot482_2fb2a0(b, creg).ok);
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
    Slot479Objects o;
    const Slot479Result r = init_slot479_31b660(o, creg);
    CHECK(!r.ok && !r.var.ok && !r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::manager);
    CHECK(creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_with_neighbour();
    section_failures();
    std::puts("OK test_ctor_slot_479");
    return 0;
}
