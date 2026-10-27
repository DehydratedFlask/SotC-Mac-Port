// CPU-01-STARTUP-SLOT-345: fixture for startup constructor index 345 (0x7cec60): bool variable
// `shadow_debugPlayerControllerState` (value 0 / default 0, tag 0x15ce390) and its 0x26ea0 toggle. Lane L4.
#include "l4_test_util.hpp"

#include "core/ctor_slot_345_7cec60.hpp"
#include "core/ctor_slot_355_7946d0.hpp"

namespace {
using namespace l4;

bool parent_insert_called = false;
bool parent_insert(void*, DebugMenuItem&) noexcept { parent_insert_called = true; return true; }

void section_constants() {
    CHECK(kSlot345Rva == 0x7cec60 && kSlot345Size == 431 && kSlot345FnTag == 0x15ce390);
    CHECK(sizeof kSlot345Name - 1 == 0x21 && sizeof kSlot345Label - 1 == 0xd && sizeof kSlot345Text - 1 == 0x1d);
    CHECK(std::strcmp(kSlot345Name, "shadow_debugPlayerControllerState") == 0);
    CHECK(std::strcmp(kSlot345Label, "Shadow Player") == 0 && std::strcmp(kSlot345Text, "Debug Player Controller State") == 0);
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot345Objects o;
    poison(o);
    const Slot345Result r = init_slot345_7cec60(o, creg, menu);
    CHECK(r.ok && r.var.ok && r.item.ok && r.item.views_inert && r.item.atexit_recorded);
    check_bool_common(o.var, 0x15ce390, 0);
    CHECK(same_view(o.var.base.name, "shadow_debugPlayerControllerState", 0x21));
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.var.base.pending);

    const DebugMenuItem& it = o.item;
    CHECK(it.target == &o.var && it.flags == 0x30 && it.kind == 2 && it.registered == 0);
    CHECK(dword(it, 0x98) == 0x3f800000u && dword(it, 0x9c) == 0x41200000u);
    CHECK(same_view(it.label, "Shadow Player", 0xd) && same_view(it.text, "Debug Player Controller State", 0x1d));
    CHECK(empty_view(it.empty));
    CHECK(it.link.self == &it.link && it.link.next == &menu.root.link && it.link.prev == &menu.root.link &&
          it.link.owner == &menu.root.link);
    CHECK(r.item.registration.path == DebugMenuRegisterPath::root_list && r.item.registration.root_initialised);
    CHECK(menu.root_guard_done && menu.root.count == 1 && menu.root.link.next == &it.link);
    check_item_untouched(it);

    CHECK(creg.atexit_log.size() == 3);
    check_atexit(creg, 0, 0x20b0, &o.var);
    check_atexit(creg, 1, 0x27280, &menu.root);
    check_atexit(creg, 2, 0x272d0, &o.item);
}

void section_after_neighbour() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot355Objects n;
    Slot345Objects o;
    CHECK(init_slot355_7946d0(n, creg, menu).ok);
    CHECK(creg.atexit_log.size() == 4 && menu.root.count == 1);
    const Slot345Result r = init_slot345_7cec60(o, creg, menu);
    CHECK(r.ok && !r.item.registration.root_initialised && !r.item.registration.atexit_recorded);
    CHECK(menu.root.count == 2 && n.item.link.next == &o.item.link && o.item.link.next == &menu.root.link);
    CHECK(creg.atexit_log.size() == 6);
    check_atexit(creg, 4, 0x20b0, &o.var);
    check_atexit(creg, 5, 0x272d0, &o.item);
    CHECK(creg.pending.size() == 3);
}

void section_failures() {
    {
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot345Objects o;
        const Slot345Result r = init_slot345_7cec60(o, creg, menu);
        CHECK(!r.ok && !r.var.ok && r.var.registration.path == ConCmdRegisterPath::manager);
        CHECK(creg.pending.size() == 0 && menu.root.count == 0);   // the item stage is skipped after a failed variable
    }
    {
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot345Objects o;
        const Slot345Result r = init_slot345_7cec60(o, creg, menu);
        CHECK(!r.ok && r.var.ok && !r.item.ok && r.item.registration.path == DebugMenuRegisterPath::parent);
        DebugMenuRegistry menu2;
        menu2.parent.parent = &token;
        menu2.parent.insert = &parent_insert;
        ConCmdRegistry creg2;
        Slot345Objects o2;
        const Slot345Result r2 = init_slot345_7cec60(o2, creg2, menu2);
        CHECK(r2.ok && parent_insert_called && o2.item.registered == 1 && menu2.root.count == 0);
        CHECK(creg2.atexit_log.size() == 2);
    }
    {
        // A target of an unlifted class is refused by the 0x26ea0 shape (virtual slots not lifted).
        ConCmdBoolVar fake{};
        fake.base.vptr = 0x1234;
        DebugMenuItem item{};
        CHECK(!init_debug_menu_item_26ea0(item, SharedStr{}, SharedStr{}, debug_menu_target_for(fake), 0x30));
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_after_neighbour();
    section_failures();
    std::puts("OK test_ctor_slot_345");
    return 0;
}
