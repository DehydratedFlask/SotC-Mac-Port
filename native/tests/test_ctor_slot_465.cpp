// CPU-01-STARTUP-SLOT-465: fixture for startup constructor index 465 (0x3cc9e0): the single bool console
// variable `showLoadPackages` (value 0, default 0, functor tag 0x15b0fc8). Lane M4.
#include "l4_test_util.hpp"

#include "core/ctor_slot_465_3cc9e0.hpp"
#include "core/ctor_slot_476_329650.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot465Rva == 0x3cc9e0 && kSlot465Size == 323);
    CHECK(sizeof kSlot465Name - 1 == 16 && std::strcmp(kSlot465Name, "showLoadPackages") == 0);
    CHECK(kSlot465FnTag == 0x15b0fc8 && kSlot465ObjectRva == 0x2ef6fe8);
    CHECK(kSlot465Value == 0 && kSlot465Default == 0);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot465Objects o;
    poison(o);
    const Slot465Result r = init_slot465_3cc9e0(o, creg);
    CHECK(r.ok && r.var.ok && r.var.views_inert && r.var.atexit_recorded);
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_common(o.var, 0x15b0fc8, 0);
    CHECK(same_view(o.var.base.name, "showLoadPackages", 16));
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.var.base.pending);
    CHECK(creg.atexit_log.size() == 1);
    check_atexit(creg, 0, 0x20b0, &o.var);
}

void section_with_neighbour() {
    ConCmdRegistry creg;
    Slot465Objects a;
    Slot476Objects b;
    CHECK(init_slot465_3cc9e0(a, creg).ok);
    CHECK(init_slot476_329650(b, creg).ok);
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
    Slot465Objects o;
    const Slot465Result r = init_slot465_3cc9e0(o, creg);
    CHECK(!r.ok && !r.var.ok && !r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::manager);
    CHECK(creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_with_neighbour();
    section_failures();
    std::puts("OK test_ctor_slot_465");
    return 0;
}
