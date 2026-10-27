// CPU-01-STARTUP-SLOT-332-01 (lane L3): fixture for startup constructor index 332 (0x805440, 694 B).
//   - bool var `shadow_gazeIkEnableSelectiveRecomputeGlobal` (0x32887b0, value 1/default 1, functor tag 0x15ce8e0)
//   - 0x26db0 item under "Shadow IK" (0x3288878)
//   - bool var `cp10_enableGazeIk` (0x3288920, value 1/default 1, functor tag 0x15ce8e0)
//   - 0x26db0 item under "CP10-Anim" (0x32889e8)
// Sections: 1 constants, 2 every stored/untouched byte, list order and atexit order, 3 a second run on
// the same registries, 4 visible failures. Nothing here executes an original RVA.
#include "ctor_slot_family_test_util.hpp"

#include "core/ctor_slots_322_334.hpp"

namespace {

using namespace famtest;

// ------------------------------------------------------------------ 1. constants
void section_constants() {
    CHECK(kSlot332Rva == 0x805440 && kSlot332BodySize == 694);
    CHECK(kSlot332VarARva == 0x32887b0);
    CHECK(kSlot332VarAFnTag == 0x15ce8e0 && kSlot332VarAValue == 1 && kSlot332VarADefault == 1);
    CHECK(sizeof kSlot332VarAName - 1 == 43 && std::strcmp(kSlot332VarAName, "shadow_gazeIkEnableSelectiveRecomputeGlobal") == 0);
    CHECK(kSlot332ItemARva == 0x3288878);
    CHECK(kSlot332ItemAFlags == 0x30);
    CHECK(sizeof kSlot332ItemAParent - 1 == 9 && std::strcmp(kSlot332ItemAParent, "Shadow IK") == 0);
    CHECK(kSlot332VarBRva == 0x3288920);
    CHECK(kSlot332VarBFnTag == 0x15ce8e0 && kSlot332VarBValue == 1 && kSlot332VarBDefault == 1);
    CHECK(sizeof kSlot332VarBName - 1 == 17 && std::strcmp(kSlot332VarBName, "cp10_enableGazeIk") == 0);
    CHECK(kSlot332ItemBRva == 0x32889e8);
    CHECK(kSlot332ItemBFlags == 0x30);
    CHECK(sizeof kSlot332ItemBParent - 1 == 9 && std::strcmp(kSlot332ItemBParent, "CP10-Anim") == 0);
}

// Every observable byte of one run (`o` was poisoned before the call).
void check_objects(const Slot332Objects& o, const ConCmdRegistry& creg, const DebugMenuRegistry& menu,
                   std::size_t run) {
    (void)run;
    check_bool_var(o.var_a, kSlot332VarAName, sizeof kSlot332VarAName - 1, kSlot332VarAFnTag, kSlot332VarAValue, kSlot332VarADefault);
    check_bool_var(o.var_b, kSlot332VarBName, sizeof kSlot332VarBName - 1, kSlot332VarBFnTag, kSlot332VarBValue, kSlot332VarBDefault);
    check_leaf_item(o.item_a, &o.var_a, kSlot332ItemAFlags, kSlot332ItemAParent, sizeof kSlot332ItemAParent - 1, kSlot332VarAName,
                    sizeof kSlot332VarAName - 1);
    CHECK(o.item_a.text.data == o.var_a.base.name.data);   // caption = the target name view
    check_leaf_item(o.item_b, &o.var_b, kSlot332ItemBFlags, kSlot332ItemBParent, sizeof kSlot332ItemBParent - 1, kSlot332VarBName,
                    sizeof kSlot332VarBName - 1);
    CHECK(o.item_b.text.data == o.var_b.base.name.data);   // caption = the target name view
    // pending list: the variables queue in construction order
    CHECK(creg.pending.size() == 2 * (run + 1));
    CHECK(o.var_a.base.pending.next != nullptr);
    CHECK(o.var_b.base.pending.next != nullptr);
    // root menu list: every item appended in construction order, each linked to the root
    CHECK(menu.root.count == 2 * (run + 1));
    CHECK(o.item_a.link.self == &o.item_a.link && o.item_a.link.owner == &menu.root.link);
    CHECK(o.item_b.link.self == &o.item_b.link && o.item_b.link.owner == &menu.root.link);
    CHECK(o.item_a.link.next == &o.item_b.link && o.item_b.link.prev == &o.item_a.link);
}

// ------------------------------------------------------------------ 2. one run
void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot332Objects o;
    poison(o);
    const Slot332Result r = init_slot332_805440(o, creg, menu);
    CHECK(r.ok && r.var_a.ok && r.var_b.ok && r.item_a.ok() && r.item_b.ok());
    CHECK(r.var_a.registration.ok && r.var_a.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var_a.views_inert && r.var_a.atexit_recorded);
    CHECK(r.var_b.registration.ok && r.var_b.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var_b.views_inert && r.var_b.atexit_recorded);
    CHECK(r.item_a.ctor_ok && r.item_a.registration.ok && r.item_a.views_inert && r.item_a.atexit_recorded);
    CHECK(r.item_a.registration.path == DebugMenuRegisterPath::root_list);
    CHECK(r.item_b.ctor_ok && r.item_b.registration.ok && r.item_b.views_inert && r.item_b.atexit_recorded);
    CHECK(r.item_b.registration.path == DebugMenuRegisterPath::root_list);
    check_objects(o, creg, menu, 0);
    CHECK(creg.pending.first() == &o.var_a.base.pending);
    CHECK(o.var_a.base.pending.next == &o.var_b.base.pending);
    CHECK(o.var_b.base.pending.next == &creg.pending.head);
    CHECK(menu.root_guard_done && menu.root.link.next == &o.item_a.link);
    CHECK(o.item_a.link.prev == &menu.root.link && o.item_b.link.next == &menu.root.link);
    CHECK(menu.root.link.prev == &o.item_b.link);
    CHECK(creg.atexit_log.size() == 5);
    CHECK(creg.atexit_log.at(0).dtor_rva == 0x20b0 && creg.atexit_log.at(0).object == &o.var_a);
    CHECK(creg.atexit_log.at(1).dtor_rva == 0x27280 && creg.atexit_log.at(1).object == &menu.root);
    CHECK(creg.atexit_log.at(2).dtor_rva == 0x272d0 && creg.atexit_log.at(2).object == &o.item_a);
    CHECK(creg.atexit_log.at(3).dtor_rva == 0x20b0 && creg.atexit_log.at(3).object == &o.var_b);
    CHECK(creg.atexit_log.at(4).dtor_rva == 0x272d0 && creg.atexit_log.at(4).object == &o.item_b);
}

// ------------------------------------------------------------------ 3. second run, same registries
void section_twice() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot332Objects first;
    Slot332Objects second;
    poison(first);
    poison(second);
    CHECK(init_slot332_805440(first, creg, menu).ok);
    CHECK(init_slot332_805440(second, creg, menu).ok);
    check_objects(second, creg, menu, 1);
    CHECK(creg.pending.size() == 4);
    CHECK(first.var_b.base.pending.next == &second.var_a.base.pending);
    CHECK(first.item_b.link.next == &second.item_a.link);
    // the root menu destructor is recorded once; each run adds 4 entries
    CHECK(creg.atexit_log.size() == 9);
}

// ------------------------------------------------------------------ 4. visible failures
void section_failures() {
    {   // a ConCmd manager without an insert contract: every variable fails visibly
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot332Objects o;
        const Slot332Result r = init_slot332_805440(o, creg, menu);
        CHECK(!r.ok && !r.var_a.ok && !r.var_a.registration.ok);
        CHECK(creg.pending.size() == 0);
    }
    {   // a non-null debug-menu parent without a contract: the first item fails visibly
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot332Objects o;
        const Slot332Result r = init_slot332_805440(o, creg, menu);
        CHECK(!r.ok && !r.item_a.registration.ok);
        CHECK(menu.root.count == 0 && !menu.root_guard_done);
        // with a contract the parent path takes over: nothing lands in the root list
        parent_insert_called = false;
        DebugMenuRegistry menu2;
        menu2.parent.parent = &token;
        menu2.parent.insert = &parent_insert;
        ConCmdRegistry creg2;
        Slot332Objects o2;
        const Slot332Result r2 = init_slot332_805440(o2, creg2, menu2);
        CHECK(r2.ok && parent_insert_called && menu2.root.count == 0);
        CHECK(r2.item_a.registration.path == DebugMenuRegisterPath::parent);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_twice();
    section_failures();
    std::puts("OK test_ctor_slot_332");
    return 0;
}
