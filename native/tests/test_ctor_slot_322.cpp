// CPU-01-STARTUP-SLOT-322-01 (lane L3): fixture for startup constructor index 322 (0x849e30, 386 B).
//   - bool var `shadow_autoEquipNewWeapons` (0x3294d20, value 1/default 1, functor tag 0x15cec90)
//   - 0x26db0 item under "Shadow Items" (0x3294de8)
// Sections: 1 constants, 2 every stored/untouched byte, list order and atexit order, 3 a second run on
// the same registries, 4 visible failures. Nothing here executes an original RVA.
#include "ctor_slot_family_test_util.hpp"

#include "core/ctor_slots_322_334.hpp"

namespace {

using namespace famtest;

// ------------------------------------------------------------------ 1. constants
void section_constants() {
    CHECK(kSlot322Rva == 0x849e30 && kSlot322BodySize == 386);
    CHECK(kSlot322VarRva == 0x3294d20);
    CHECK(kSlot322VarFnTag == 0x15cec90 && kSlot322VarValue == 1 && kSlot322VarDefault == 1);
    CHECK(sizeof kSlot322VarName - 1 == 26 && std::strcmp(kSlot322VarName, "shadow_autoEquipNewWeapons") == 0);
    CHECK(kSlot322ItemRva == 0x3294de8);
    CHECK(kSlot322ItemFlags == 0x30);
    CHECK(sizeof kSlot322ItemParent - 1 == 12 && std::strcmp(kSlot322ItemParent, "Shadow Items") == 0);
}

// Every observable byte of one run (`o` was poisoned before the call).
void check_objects(const Slot322Objects& o, const ConCmdRegistry& creg, const DebugMenuRegistry& menu,
                   std::size_t run) {
    (void)run;
    check_bool_var(o.var, kSlot322VarName, sizeof kSlot322VarName - 1, kSlot322VarFnTag, kSlot322VarValue, kSlot322VarDefault);
    check_leaf_item(o.item, &o.var, kSlot322ItemFlags, kSlot322ItemParent, sizeof kSlot322ItemParent - 1, kSlot322VarName,
                    sizeof kSlot322VarName - 1);
    CHECK(o.item.text.data == o.var.base.name.data);   // caption = the target name view
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
    Slot322Objects o;
    poison(o);
    const Slot322Result r = init_slot322_849e30(o, creg, menu);
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
    Slot322Objects first;
    Slot322Objects second;
    poison(first);
    poison(second);
    CHECK(init_slot322_849e30(first, creg, menu).ok);
    CHECK(init_slot322_849e30(second, creg, menu).ok);
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
        Slot322Objects o;
        const Slot322Result r = init_slot322_849e30(o, creg, menu);
        CHECK(!r.ok && !r.var.ok && !r.var.registration.ok);
        CHECK(creg.pending.size() == 0);
    }
    {   // a non-null debug-menu parent without a contract: the first item fails visibly
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot322Objects o;
        const Slot322Result r = init_slot322_849e30(o, creg, menu);
        CHECK(!r.ok && !r.item.registration.ok);
        CHECK(menu.root.count == 0 && !menu.root_guard_done);
        // with a contract the parent path takes over: nothing lands in the root list
        parent_insert_called = false;
        DebugMenuRegistry menu2;
        menu2.parent.parent = &token;
        menu2.parent.insert = &parent_insert;
        ConCmdRegistry creg2;
        Slot322Objects o2;
        const Slot322Result r2 = init_slot322_849e30(o2, creg2, menu2);
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
    std::puts("OK test_ctor_slot_322");
    return 0;
}
