// CPU-01-STARTUP-SLOT-327-01 (lane L3): fixture for startup constructor index 327 (0x836330, 455 B).
//   - bool var `colossusCanSeePlayer` (0x3291608, value 1/default 1, functor tag 0x15cebc0)
//   - 0x26ea0 toggle "Colossus"/"Can See Player" (0x32916d0)
// Sections: 1 constants, 2 every stored/untouched byte, list order and atexit order, 3 a second run on
// the same registries, 4 visible failures. Nothing here executes an original RVA.
#include "ctor_slot_family_test_util.hpp"

#include "core/ctor_slots_322_334.hpp"

namespace {

using namespace famtest;

// ------------------------------------------------------------------ 1. constants
void section_constants() {
    CHECK(kSlot327Rva == 0x836330 && kSlot327BodySize == 455);
    CHECK(kSlot327VarRva == 0x3291608);
    CHECK(kSlot327VarFnTag == 0x15cebc0 && kSlot327VarValue == 1 && kSlot327VarDefault == 1);
    CHECK(sizeof kSlot327VarName - 1 == 20 && std::strcmp(kSlot327VarName, "colossusCanSeePlayer") == 0);
    CHECK(kSlot327ItemRva == 0x32916d0);
    CHECK(kSlot327ItemFlags == 0x30);
    CHECK(sizeof kSlot327ItemLabel - 1 == 8 && std::strcmp(kSlot327ItemLabel, "Colossus") == 0);
    CHECK(sizeof kSlot327ItemText - 1 == 14 && std::strcmp(kSlot327ItemText, "Can See Player") == 0);
}

// Every observable byte of one run (`o` was poisoned before the call).
void check_objects(const Slot327Objects& o, const ConCmdRegistry& creg, const DebugMenuRegistry& menu,
                   std::size_t run) {
    (void)run;
    check_bool_var(o.var, kSlot327VarName, sizeof kSlot327VarName - 1, kSlot327VarFnTag, kSlot327VarValue, kSlot327VarDefault);
    check_leaf_item(o.item, &o.var, kSlot327ItemFlags, kSlot327ItemLabel, sizeof kSlot327ItemLabel - 1, kSlot327ItemText,
                    sizeof kSlot327ItemText - 1);
    // pending list: the variables queue in construction order
    CHECK(creg.pending.size() == 1 * (run + 1));
    CHECK(o.var.base.pending.next != nullptr);
    // root menu list: every item appended in construction order, each linked to the root
    CHECK(menu.root.count == 1 * (run + 1));
    CHECK(o.item.link.self == &o.item.link && o.item.link.owner == &menu.root.link);
}

// ------------------------------------------------------------------ 2. one run
void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot327Objects o;
    poison(o);
    const Slot327Result r = init_slot327_836330(o, creg, menu);
    CHECK(r.ok && r.var.ok && r.item.ok());
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var.views_inert && r.var.atexit_recorded);
    CHECK(r.item.ctor_ok && r.item.registration.ok && r.item.views_inert && r.item.atexit_recorded);
    CHECK(r.item.registration.path == DebugMenuRegisterPath::root_list);
    check_objects(o, creg, menu, 0);
    CHECK(creg.pending.first() == &o.var.base.pending);
    CHECK(o.var.base.pending.next == &creg.pending.head);
    CHECK(menu.root_guard_done && menu.root.link.next == &o.item.link);
    CHECK(o.item.link.prev == &menu.root.link && o.item.link.next == &menu.root.link);
    CHECK(menu.root.link.prev == &o.item.link);
    CHECK(creg.atexit_log.size() == 3);
    CHECK(creg.atexit_log.at(0).dtor_rva == 0x20b0 && creg.atexit_log.at(0).object == &o.var);
    CHECK(creg.atexit_log.at(1).dtor_rva == 0x27280 && creg.atexit_log.at(1).object == &menu.root);
    CHECK(creg.atexit_log.at(2).dtor_rva == 0x272d0 && creg.atexit_log.at(2).object == &o.item);
}

// ------------------------------------------------------------------ 3. second run, same registries
void section_twice() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot327Objects first;
    Slot327Objects second;
    poison(first);
    poison(second);
    CHECK(init_slot327_836330(first, creg, menu).ok);
    CHECK(init_slot327_836330(second, creg, menu).ok);
    check_objects(second, creg, menu, 1);
    CHECK(creg.pending.size() == 2);
    CHECK(first.var.base.pending.next == &second.var.base.pending);
    CHECK(first.item.link.next == &second.item.link);
    // the root menu destructor is recorded once; each run adds 2 entries
    CHECK(creg.atexit_log.size() == 5);
}

// ------------------------------------------------------------------ 4. visible failures
void section_failures() {
    {   // a ConCmd manager without an insert contract: every variable fails visibly
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot327Objects o;
        const Slot327Result r = init_slot327_836330(o, creg, menu);
        CHECK(!r.ok && !r.var.ok && !r.var.registration.ok);
        CHECK(creg.pending.size() == 0);
    }
    {   // a non-null debug-menu parent without a contract: the first item fails visibly
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot327Objects o;
        const Slot327Result r = init_slot327_836330(o, creg, menu);
        CHECK(!r.ok && !r.item.registration.ok);
        CHECK(menu.root.count == 0 && !menu.root_guard_done);
        // with a contract the parent path takes over: nothing lands in the root list
        parent_insert_called = false;
        DebugMenuRegistry menu2;
        menu2.parent.parent = &token;
        menu2.parent.insert = &parent_insert;
        ConCmdRegistry creg2;
        Slot327Objects o2;
        const Slot327Result r2 = init_slot327_836330(o2, creg2, menu2);
        CHECK(r2.ok && parent_insert_called && menu2.root.count == 0);
        CHECK(r2.item.registration.path == DebugMenuRegisterPath::parent);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_twice();
    section_failures();
    std::puts("OK test_ctor_slot_327");
    return 0;
}
