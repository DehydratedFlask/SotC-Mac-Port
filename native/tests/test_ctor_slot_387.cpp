// CPU-01-STARTUP-SLOT-387: fixture for startup constructor index 387 (0x6acf00). Lane M1.
#include "l4_test_util.hpp"

#include "core/ctor_slot_387_6acf00.hpp"
#include "core/ctor_slot_365_747870.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot387Rva == 0x6acf00 && kSlot387Size == 299 && kSlot387FnTag == 0x15cc748 && kSlot387ObjectRva == 0x3017890);
    CHECK(sizeof kSlot387Name - 1 == 24 && std::strcmp(kSlot387Name, "CP3FileResource_copy_mem") == 0);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot387Objects o;
    poison(o);
    const Slot387Result r = init_slot387_6acf00(o, creg);
    CHECK(r.ok && r.var.ok && r.var.views_inert && r.var.atexit_recorded);
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_common(o.var, 0x15cc748, 1);
    CHECK(same_view(o.var.base.name, "CP3FileResource_copy_mem", 24));
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.var.base.pending);
    CHECK(creg.atexit_log.size() == 1);
    check_atexit(creg, 0, 0x20b0, &o.var);
}

void section_with_neighbour() {
    ConCmdRegistry creg;
    Slot387Objects a;
    Slot365Objects b;
    CHECK(init_slot387_6acf00(a, creg).ok);
    CHECK(init_slot365_747870(b, creg).ok);
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
    Slot387Objects o;
    const Slot387Result r = init_slot387_6acf00(o, creg);
    CHECK(!r.ok && !r.var.ok && !r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::manager);
    CHECK(creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_with_neighbour();
    section_failures();
    std::puts("OK test_ctor_slot_387");
    return 0;
}
