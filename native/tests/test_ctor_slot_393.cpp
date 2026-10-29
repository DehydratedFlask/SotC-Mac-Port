// CPU-01-STARTUP-SLOT-393: fixture for startup constructor index 393 (0x66cc00): the single bool variable
// `cp10_debugDrawFruit` (value/default 0, tag 0x15cc5c8). Lane M2.
#include "core/lane_m2_testkit.hpp"

#include "core/ctor_slot_393_66cc00.hpp"

namespace {
using namespace sotc::lane_m2;

void section_constants() {
    CHECK(kSlot393Rva == 0x66cc00 && kSlot393Size == 299 && kSlot393FnTag == 0x15cc5c8);
    CHECK(std::strlen(kSlot393VarName) == 0x13);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot393Objects o;
    poison(o);
    const Slot393Result r = init_slot393_66cc00(o, creg);
    CHECK(r.ok && r.var.ok && r.var.views_inert && r.var.atexit_recorded);
    check_bool_var(o.var, "cp10_debugDrawFruit", 0x15cc5c8, 0, 1);
    check_var_atexit_and_pending(creg, o.var);
    CHECK(creg.atexit_log.size() == 1);
}

void section_two_registrations_keep_order() {
    ConCmdRegistry creg;
    Slot393Objects a;
    Slot393Objects b;
    CHECK(init_slot393_66cc00(a, creg).ok && init_slot393_66cc00(b, creg).ok);
    CHECK(creg.pending.size() == 2 && creg.pending.first() == &a.var.base.pending &&
          a.var.base.pending.next == &b.var.base.pending);
    check_atexit(creg, 0, 0x20b0, &a.var);
    check_atexit(creg, 1, 0x20b0, &b.var);
}

void section_failures() {
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;
    Slot393Objects o;
    const Slot393Result r = init_slot393_66cc00(o, creg);
    CHECK(!r.ok && !r.var.ok && !r.var.registration.ok && creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_two_registrations_keep_order();
    section_failures();
    std::puts("OK test_ctor_slot_393");
    return 0;
}
