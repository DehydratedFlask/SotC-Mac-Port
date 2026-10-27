// CPU-01-STARTUP-SLOT-328-01 (lane L3): fixture for startup constructor index 328 (0x82e040, 519 B).
//   - bool var `shadow_showBossScrModeStatus` (0x3291470, value 0/default 0, functor tag 0x15ceba0)
//   - bool var `shadow_showBossMotKind` (0x3291538, value 0/default 0, functor tag 0x15ceba0)
// Sections: 1 constants, 2 every stored/untouched byte, list order and atexit order, 3 a second run on
// the same registries, 4 visible failures. Nothing here executes an original RVA.
#include "ctor_slot_family_test_util.hpp"

#include "core/ctor_slots_322_334.hpp"

namespace {

using namespace famtest;

// ------------------------------------------------------------------ 1. constants
void section_constants() {
    CHECK(kSlot328Rva == 0x82e040 && kSlot328BodySize == 519);
    CHECK(kSlot328VarARva == 0x3291470);
    CHECK(kSlot328VarAFnTag == 0x15ceba0 && kSlot328VarAValue == 0 && kSlot328VarADefault == 0);
    CHECK(sizeof kSlot328VarAName - 1 == 28 && std::strcmp(kSlot328VarAName, "shadow_showBossScrModeStatus") == 0);
    CHECK(kSlot328VarBRva == 0x3291538);
    CHECK(kSlot328VarBFnTag == 0x15ceba0 && kSlot328VarBValue == 0 && kSlot328VarBDefault == 0);
    CHECK(sizeof kSlot328VarBName - 1 == 22 && std::strcmp(kSlot328VarBName, "shadow_showBossMotKind") == 0);
}

// Every observable byte of one run (`o` was poisoned before the call).
void check_objects(const Slot328Objects& o, const ConCmdRegistry& creg,
                   std::size_t run) {
    (void)run;
    check_bool_var(o.var_a, kSlot328VarAName, sizeof kSlot328VarAName - 1, kSlot328VarAFnTag, kSlot328VarAValue, kSlot328VarADefault);
    check_bool_var(o.var_b, kSlot328VarBName, sizeof kSlot328VarBName - 1, kSlot328VarBFnTag, kSlot328VarBValue, kSlot328VarBDefault);
    // pending list: the variables queue in construction order
    CHECK(creg.pending.size() == 2 * (run + 1));
    CHECK(o.var_a.base.pending.next != nullptr);
    CHECK(o.var_b.base.pending.next != nullptr);
}

// ------------------------------------------------------------------ 2. one run
void section_alone() {
    ConCmdRegistry creg;
    Slot328Objects o;
    poison(o);
    const Slot328Result r = init_slot328_82e040(o, creg);
    CHECK(r.ok && r.var_a.ok && r.var_b.ok);
    CHECK(r.var_a.registration.ok && r.var_a.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var_a.views_inert && r.var_a.atexit_recorded);
    CHECK(r.var_b.registration.ok && r.var_b.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var_b.views_inert && r.var_b.atexit_recorded);
    check_objects(o, creg, 0);
    CHECK(creg.pending.first() == &o.var_a.base.pending);
    CHECK(o.var_a.base.pending.next == &o.var_b.base.pending);
    CHECK(o.var_b.base.pending.next == &creg.pending.head);
    CHECK(creg.atexit_log.size() == 2);
    CHECK(creg.atexit_log.at(0).dtor_rva == 0x20b0 && creg.atexit_log.at(0).object == &o.var_a);
    CHECK(creg.atexit_log.at(1).dtor_rva == 0x20b0 && creg.atexit_log.at(1).object == &o.var_b);
}

// ------------------------------------------------------------------ 3. second run, same registries
void section_twice() {
    ConCmdRegistry creg;
    Slot328Objects first;
    Slot328Objects second;
    poison(first);
    poison(second);
    CHECK(init_slot328_82e040(first, creg).ok);
    CHECK(init_slot328_82e040(second, creg).ok);
    check_objects(second, creg, 1);
    CHECK(creg.pending.size() == 4);
    CHECK(first.var_b.base.pending.next == &second.var_a.base.pending);
    // the root menu destructor is recorded once; each run adds 2 entries
    CHECK(creg.atexit_log.size() == 4);
}

// ------------------------------------------------------------------ 4. visible failures
void section_failures() {
    {   // a ConCmd manager without an insert contract: every variable fails visibly
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot328Objects o;
        const Slot328Result r = init_slot328_82e040(o, creg);
        CHECK(!r.ok && !r.var_a.ok && !r.var_a.registration.ok);
        CHECK(creg.pending.size() == 0);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_twice();
    section_failures();
    std::puts("OK test_ctor_slot_328");
    return 0;
}
