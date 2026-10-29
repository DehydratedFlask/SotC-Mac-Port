// CPU-01-STARTUP-SLOT-509: fixture for startup constructor index 509 (0x2196f0): bool variable `showBroadcastEvents`
// (value 0 / default 0, tag 0x159b170) and its 0x26ea0 toggle. Lane M5.
#include "l4_test_util.hpp"

#include "core/ctor_slot_509_2196f0.hpp"
#include "core/ctor_slot_345_7cec60.hpp"

namespace {
using namespace l4;

bool parent_insert_called = false;
bool parent_insert(void*, DebugMenuItem&) noexcept { parent_insert_called = true; return true; }

void section_constants() {
    CHECK(kSlot509Rva == 0x2196f0 && kSlot509Size == 431 && kSlot509FnTag == 0x159b170);
    CHECK(sizeof kSlot509Name - 1 == 19 && sizeof kSlot509Label - 1 == 12 && sizeof kSlot509Text - 1 == 21);
    CHECK(std::strcmp(kSlot509Name, "showBroadcastEvents") == 0);
    CHECK(std::strcmp(kSlot509Label, "Game Manager") == 0 && std::strcmp(kSlot509Text, "Show Broadcast Events") == 0);
    CHECK(kSlot509VarRva == 0x2ee86b0 && kSlot509ItemRva == 0x2ee8778);
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot509Objects o;
    poison(o);
    const Slot509Result r = init_slot509_2196f0(o, creg, menu);
    CHECK(r.ok && r.var.ok && r.item.ok && r.item.views_inert && r.item.atexit_recorded);
    check_bool_common(o.var, 0x159b170, 0);
    CHECK(same_view(o.var.base.name, "showBroadcastEvents", 19));
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.var.base.pending);

    const DebugMenuItem& it = o.item;
    CHECK(it.target == &o.var && it.flags == 0x30 && it.kind == 2 && it.registered == 0);
    CHECK(dword(it, 0x98) == 0x3f800000u && dword(it, 0x9c) == 0x41200000u);
    CHECK(same_view(it.label, "Game Manager", 12) && same_view(it.text, "Show Broadcast Events", 21));
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
    Slot345Objects n;
    Slot509Objects o;
    CHECK(init_slot345_7cec60(n, creg, menu).ok);
    CHECK(creg.atexit_log.size() == 3 && menu.root.count == 1);
    const Slot509Result r = init_slot509_2196f0(o, creg, menu);
    CHECK(r.ok && !r.item.registration.root_initialised && !r.item.registration.atexit_recorded);
    CHECK(menu.root.count == 2 && n.item.link.next == &o.item.link && o.item.link.next == &menu.root.link);
    CHECK(creg.atexit_log.size() == 5);
    check_atexit(creg, 3, 0x20b0, &o.var);
    check_atexit(creg, 4, 0x272d0, &o.item);
    CHECK(creg.pending.size() == 2);
}

void section_failures() {
    {
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot509Objects o;
        const Slot509Result r = init_slot509_2196f0(o, creg, menu);
        CHECK(!r.ok && !r.var.ok && r.var.registration.path == ConCmdRegisterPath::manager);
        CHECK(creg.pending.size() == 0 && menu.root.count == 0);   // the item stage is skipped after a failed variable
    }
    {
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot509Objects o;
        const Slot509Result r = init_slot509_2196f0(o, creg, menu);
        CHECK(!r.ok && r.var.ok && !r.item.ok && r.item.registration.path == DebugMenuRegisterPath::parent);
        DebugMenuRegistry menu2;
        menu2.parent.parent = &token;
        menu2.parent.insert = &parent_insert;
        ConCmdRegistry creg2;
        Slot509Objects o2;
        const Slot509Result r2 = init_slot509_2196f0(o2, creg2, menu2);
        CHECK(r2.ok && parent_insert_called && o2.item.registered == 1 && menu2.root.count == 0);
        CHECK(creg2.atexit_log.size() == 2);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_after_neighbour();
    section_failures();
    std::puts("OK test_ctor_slot_509");
    return 0;
}
