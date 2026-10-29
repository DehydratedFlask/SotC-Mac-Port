// CPU-01-STARTUP-SLOT-384: fixture for startup constructor index 384 (0x6b9260). Lane M1.
#include "l4_test_util.hpp"

#include "core/ctor_slot_384_6b9260.hpp"
#include "core/ctor_slot_387_6acf00.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot384Rva == 0x6b9260 && kSlot384Size == 299 && kSlot384FnTag == 0x15cc7b8 && kSlot384ObjectRva == 0x3238dc0);
    CHECK(sizeof kSlot384Name - 1 == 25 && std::strcmp(kSlot384Name, "shadow_debugDrawDamageBox") == 0);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot384Objects o;
    poison(o);
    const Slot384Result r = init_slot384_6b9260(o, creg);
    CHECK(r.ok && r.var.ok && r.var.views_inert && r.var.atexit_recorded);
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_common(o.var, 0x15cc7b8, 0);
    CHECK(same_view(o.var.base.name, "shadow_debugDrawDamageBox", 25));
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.var.base.pending);
    CHECK(creg.atexit_log.size() == 1);
    check_atexit(creg, 0, 0x20b0, &o.var);
}

void section_with_neighbour() {
    ConCmdRegistry creg;
    Slot384Objects a;
    Slot387Objects b;
    CHECK(init_slot384_6b9260(a, creg).ok);
    CHECK(init_slot387_6acf00(b, creg).ok);
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
    Slot384Objects o;
    const Slot384Result r = init_slot384_6b9260(o, creg);
    CHECK(!r.ok && !r.var.ok && !r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::manager);
    CHECK(creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_with_neighbour();
    section_failures();
    std::puts("OK test_ctor_slot_384");
    return 0;
}
