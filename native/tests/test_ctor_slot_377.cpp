// CPU-01-STARTUP-SLOT-377: fixture for startup constructor index 377 (0x6de050). Lane M1.
#include "l4_test_util.hpp"

#include "core/ctor_slot_377_6de050.hpp"
#include "core/ctor_slot_384_6b9260.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot377Rva == 0x6de050 && kSlot377Size == 299 && kSlot377FnTag == 0x15cc998 && kSlot377ObjectRva == 0x3240cd0);
    CHECK(sizeof kSlot377Name - 1 == 33 && std::strcmp(kSlot377Name, "pauseGameIfControllerDisconnected") == 0);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot377Objects o;
    poison(o);
    const Slot377Result r = init_slot377_6de050(o, creg);
    CHECK(r.ok && r.var.ok && r.var.views_inert && r.var.atexit_recorded);
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_common(o.var, 0x15cc998, 1);
    CHECK(same_view(o.var.base.name, "pauseGameIfControllerDisconnected", 33));
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.var.base.pending);
    CHECK(creg.atexit_log.size() == 1);
    check_atexit(creg, 0, 0x20b0, &o.var);
}

void section_with_neighbour() {
    ConCmdRegistry creg;
    Slot377Objects a;
    Slot384Objects b;
    CHECK(init_slot377_6de050(a, creg).ok);
    CHECK(init_slot384_6b9260(b, creg).ok);
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
    Slot377Objects o;
    const Slot377Result r = init_slot377_6de050(o, creg);
    CHECK(!r.ok && !r.var.ok && !r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::manager);
    CHECK(creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_with_neighbour();
    section_failures();
    std::puts("OK test_ctor_slot_377");
    return 0;
}
