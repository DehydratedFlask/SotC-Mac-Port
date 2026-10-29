// CPU-01-STARTUP-SLOT-376: fixture for startup constructor index 376 (0x6e4930). Lane M1.
#include "l4_test_util.hpp"

#include "core/ctor_slot_376_6e4930.hpp"
#include "core/ctor_slot_377_6de050.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot376Rva == 0x6e4930 && kSlot376Size == 299 && kSlot376FnTag == 0x15cc9b8 && kSlot376ObjectRva == 0x32415d0);
    CHECK(sizeof kSlot376Name - 1 == 20 && std::strcmp(kSlot376Name, "skippableEndSequence") == 0);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot376Objects o;
    poison(o);
    const Slot376Result r = init_slot376_6e4930(o, creg);
    CHECK(r.ok && r.var.ok && r.var.views_inert && r.var.atexit_recorded);
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_common(o.var, 0x15cc9b8, 0);
    CHECK(same_view(o.var.base.name, "skippableEndSequence", 20));
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.var.base.pending);
    CHECK(creg.atexit_log.size() == 1);
    check_atexit(creg, 0, 0x20b0, &o.var);
}

void section_with_neighbour() {
    ConCmdRegistry creg;
    Slot376Objects a;
    Slot377Objects b;
    CHECK(init_slot376_6e4930(a, creg).ok);
    CHECK(init_slot377_6de050(b, creg).ok);
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
    Slot376Objects o;
    const Slot376Result r = init_slot376_6e4930(o, creg);
    CHECK(!r.ok && !r.var.ok && !r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::manager);
    CHECK(creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_with_neighbour();
    section_failures();
    std::puts("OK test_ctor_slot_376");
    return 0;
}
