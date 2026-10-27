// CPU-01-STARTUP-SLOT-330-01 (lane L3): fixture for startup constructor index 330 (0x810a50, 299 B).
//   - bool var `shadow_debugDrawHorsePassageDir` (0x3288cd0, value 0/default 0, functor tag 0x15ce920)
// Sections: 1 constants, 2 every stored/untouched byte, list order and atexit order, 3 a second run on
// the same registries, 4 visible failures. Nothing here executes an original RVA.
#include "ctor_slot_family_test_util.hpp"

#include "core/ctor_slots_322_334.hpp"

namespace {

using namespace famtest;

// ------------------------------------------------------------------ 1. constants
void section_constants() {
    CHECK(kSlot330Rva == 0x810a50 && kSlot330BodySize == 299);
    CHECK(kSlot330VarRva == 0x3288cd0);
    CHECK(kSlot330VarFnTag == 0x15ce920 && kSlot330VarValue == 0 && kSlot330VarDefault == 0);
    CHECK(sizeof kSlot330VarName - 1 == 31 && std::strcmp(kSlot330VarName, "shadow_debugDrawHorsePassageDir") == 0);
}

// Every observable byte of one run (`o` was poisoned before the call).
void check_objects(const Slot330Objects& o, const ConCmdRegistry& creg,
                   std::size_t run) {
    (void)run;
    check_bool_var(o.var, kSlot330VarName, sizeof kSlot330VarName - 1, kSlot330VarFnTag, kSlot330VarValue, kSlot330VarDefault);
    // pending list: the variables queue in construction order
    CHECK(creg.pending.size() == 1 * (run + 1));
    CHECK(o.var.base.pending.next != nullptr);
}

// ------------------------------------------------------------------ 2. one run
void section_alone() {
    ConCmdRegistry creg;
    Slot330Objects o;
    poison(o);
    const Slot330Result r = init_slot330_810a50(o, creg);
    CHECK(r.ok && r.var.ok);
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var.views_inert && r.var.atexit_recorded);
    check_objects(o, creg, 0);
    CHECK(creg.pending.first() == &o.var.base.pending);
    CHECK(o.var.base.pending.next == &creg.pending.head);
    CHECK(creg.atexit_log.size() == 1);
    CHECK(creg.atexit_log.at(0).dtor_rva == 0x20b0 && creg.atexit_log.at(0).object == &o.var);
}

// ------------------------------------------------------------------ 3. second run, same registries
void section_twice() {
    ConCmdRegistry creg;
    Slot330Objects first;
    Slot330Objects second;
    poison(first);
    poison(second);
    CHECK(init_slot330_810a50(first, creg).ok);
    CHECK(init_slot330_810a50(second, creg).ok);
    check_objects(second, creg, 1);
    CHECK(creg.pending.size() == 2);
    CHECK(first.var.base.pending.next == &second.var.base.pending);
    // the root menu destructor is recorded once; each run adds 1 entries
    CHECK(creg.atexit_log.size() == 2);
}

// ------------------------------------------------------------------ 4. visible failures
void section_failures() {
    {   // a ConCmd manager without an insert contract: every variable fails visibly
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot330Objects o;
        const Slot330Result r = init_slot330_810a50(o, creg);
        CHECK(!r.ok && !r.var.ok && !r.var.registration.ok);
        CHECK(creg.pending.size() == 0);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_twice();
    section_failures();
    std::puts("OK test_ctor_slot_330");
    return 0;
}
