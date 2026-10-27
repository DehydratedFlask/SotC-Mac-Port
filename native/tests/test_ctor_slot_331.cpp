// CPU-01-STARTUP-SLOT-331-01 (lane L3): fixture for startup constructor index 331 (0x809640, 512 B).
//   - bool var `CP10_DebugWormIk` (0x3288a98, value 0/default 0, functor tag 0x15ce900)
//   - 0x26c10 group "CP10-Anim"/"Shadow IK" (0x3288b60)
//   - 0x26db0 item under "Shadow IK" (0x3288c08)
// Sections: 1 constants, 2 every stored/untouched byte, list order and atexit order, 3 a second run on
// the same registries, 4 visible failures. Nothing here executes an original RVA.
#include "ctor_slot_family_test_util.hpp"

#include "core/ctor_slots_322_334.hpp"

namespace {

using namespace famtest;

// ------------------------------------------------------------------ 1. constants
void section_constants() {
    CHECK(kSlot331Rva == 0x809640 && kSlot331BodySize == 512);
    CHECK(kSlot331VarRva == 0x3288a98);
    CHECK(kSlot331VarFnTag == 0x15ce900 && kSlot331VarValue == 0 && kSlot331VarDefault == 0);
    CHECK(sizeof kSlot331VarName - 1 == 16 && std::strcmp(kSlot331VarName, "CP10_DebugWormIk") == 0);
    CHECK(kSlot331GroupRva == 0x3288b60);
    CHECK(kSlot331GroupFlags == 0x30);
    CHECK(sizeof kSlot331GroupParent - 1 == 9 && std::strcmp(kSlot331GroupParent, "CP10-Anim") == 0);
    CHECK(sizeof kSlot331GroupCaption - 1 == 9 && std::strcmp(kSlot331GroupCaption, "Shadow IK") == 0);
    CHECK(kSlot331ItemRva == 0x3288c08);
    CHECK(kSlot331ItemFlags == 0x30);
    CHECK(sizeof kSlot331ItemParent - 1 == 9 && std::strcmp(kSlot331ItemParent, "Shadow IK") == 0);
}

// Every observable byte of one run (`o` was poisoned before the call).
void check_objects(const Slot331Objects& o, const ConCmdRegistry& creg, const DebugMenuRegistry& menu,
                   std::size_t run) {
    (void)run;
    check_bool_var(o.var, kSlot331VarName, sizeof kSlot331VarName - 1, kSlot331VarFnTag, kSlot331VarValue, kSlot331VarDefault);
    {   // group: kind 1, caption copied into text and into the third view, no target
        const DebugMenuItem& g = o.group;
        CHECK(g.kind == 1 && g.target == nullptr && g.flags == kSlot331GroupFlags && g.registered == 0);
        CHECK(same_view(g.label, kSlot331GroupParent, sizeof kSlot331GroupParent - 1));
        CHECK(same_view(g.text, kSlot331GroupCaption, sizeof kSlot331GroupCaption - 1));
        CHECK(same_view(g.empty, kSlot331GroupCaption, sizeof kSlot331GroupCaption - 1));
        CHECK(dword_at(g, 0x98) == 0x3f800000u && dword_at(g, 0x9c) == 0x41200000u);
        check_item_untouched(g);
    }
    check_leaf_item(o.item, &o.var, kSlot331ItemFlags, kSlot331ItemParent, sizeof kSlot331ItemParent - 1, kSlot331VarName,
                    sizeof kSlot331VarName - 1);
    CHECK(o.item.text.data == o.var.base.name.data);   // caption = the target name view
    // pending list: the variables queue in construction order
    CHECK(creg.pending.size() == 1 * (run + 1));
    CHECK(o.var.base.pending.next != nullptr);
    // root menu list: every item appended in construction order, each linked to the root
    CHECK(menu.root.count == 2 * (run + 1));
    CHECK(o.group.link.self == &o.group.link && o.group.link.owner == &menu.root.link);
    CHECK(o.item.link.self == &o.item.link && o.item.link.owner == &menu.root.link);
    CHECK(o.group.link.next == &o.item.link && o.item.link.prev == &o.group.link);
}

// ------------------------------------------------------------------ 2. one run
void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot331Objects o;
    poison(o);
    const Slot331Result r = init_slot331_809640(o, creg, menu);
    CHECK(r.ok && r.var.ok && r.group.ok() && r.item.ok());
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var.views_inert && r.var.atexit_recorded);
    CHECK(r.group.ctor_ok && r.group.registration.ok && r.group.views_inert && r.group.atexit_recorded);
    CHECK(r.group.registration.path == DebugMenuRegisterPath::root_list);
    CHECK(r.item.ctor_ok && r.item.registration.ok && r.item.views_inert && r.item.atexit_recorded);
    CHECK(r.item.registration.path == DebugMenuRegisterPath::root_list);
    check_objects(o, creg, menu, 0);
    CHECK(creg.pending.first() == &o.var.base.pending);
    CHECK(o.var.base.pending.next == &creg.pending.head);
    CHECK(menu.root_guard_done && menu.root.link.next == &o.group.link);
    CHECK(o.group.link.prev == &menu.root.link && o.item.link.next == &menu.root.link);
    CHECK(menu.root.link.prev == &o.item.link);
    CHECK(creg.atexit_log.size() == 4);
    CHECK(creg.atexit_log.at(0).dtor_rva == 0x20b0 && creg.atexit_log.at(0).object == &o.var);
    CHECK(creg.atexit_log.at(1).dtor_rva == 0x27280 && creg.atexit_log.at(1).object == &menu.root);
    CHECK(creg.atexit_log.at(2).dtor_rva == 0x272d0 && creg.atexit_log.at(2).object == &o.group);
    CHECK(creg.atexit_log.at(3).dtor_rva == 0x272d0 && creg.atexit_log.at(3).object == &o.item);
}

// ------------------------------------------------------------------ 3. second run, same registries
void section_twice() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot331Objects first;
    Slot331Objects second;
    poison(first);
    poison(second);
    CHECK(init_slot331_809640(first, creg, menu).ok);
    CHECK(init_slot331_809640(second, creg, menu).ok);
    check_objects(second, creg, menu, 1);
    CHECK(creg.pending.size() == 2);
    CHECK(first.var.base.pending.next == &second.var.base.pending);
    CHECK(first.item.link.next == &second.group.link);
    // the root menu destructor is recorded once; each run adds 3 entries
    CHECK(creg.atexit_log.size() == 7);
}

// ------------------------------------------------------------------ 4. visible failures
void section_failures() {
    {   // a ConCmd manager without an insert contract: every variable fails visibly
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot331Objects o;
        const Slot331Result r = init_slot331_809640(o, creg, menu);
        CHECK(!r.ok && !r.var.ok && !r.var.registration.ok);
        CHECK(creg.pending.size() == 0);
    }
    {   // a non-null debug-menu parent without a contract: the first item fails visibly
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot331Objects o;
        const Slot331Result r = init_slot331_809640(o, creg, menu);
        CHECK(!r.ok && !r.group.registration.ok);
        CHECK(menu.root.count == 0 && !menu.root_guard_done);
        // with a contract the parent path takes over: nothing lands in the root list
        parent_insert_called = false;
        DebugMenuRegistry menu2;
        menu2.parent.parent = &token;
        menu2.parent.insert = &parent_insert;
        ConCmdRegistry creg2;
        Slot331Objects o2;
        const Slot331Result r2 = init_slot331_809640(o2, creg2, menu2);
        CHECK(r2.ok && parent_insert_called && menu2.root.count == 0);
        CHECK(r2.group.registration.path == DebugMenuRegisterPath::parent);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_twice();
    section_failures();
    std::puts("OK test_ctor_slot_331");
    return 0;
}
