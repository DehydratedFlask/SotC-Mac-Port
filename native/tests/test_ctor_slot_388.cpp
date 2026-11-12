// CPU-01-STARTUP-BATCH-10: fixture for startup constructor index 388 (0x6a8c60): bool variable `cp10EnableBPAnimStateOverrides`, its 0x26ea0 toggle and the
// console command `gCP10ExtractCP3EventFrameTables`. The survey's "indirect call" is the console-command register-once slot [0x1594dc0 + 0x20] = 0x20d60
// (checker-pinned).
#include "l4_test_util.hpp"

#include "core/ctor_slot_388_6a8c60.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot388Rva == 0x6a8c60 && kSlot388Size == 733);
    CHECK(std::strlen(kSlot388BoolName) == kSlot388BoolNameLength && kSlot388BoolNameLength == 0x1e);
    CHECK(std::strlen(kSlot388ToggleParent) == kSlot388ToggleParentLength && std::strlen(kSlot388ToggleText) == kSlot388ToggleTextLength);
    CHECK(std::strlen(kSlot388CommandDescription) == kSlot388CommandDescriptionLength && kSlot388CommandDescriptionLength == 0x1d);
    CHECK(sizeof(ConCmdBoolVar) == 0xc8 && sizeof(DebugMenuItem) == 0xa8 && sizeof(ConCmdCommand) == 0xa0);
    CHECK(kSlot388ItemRva - kSlot388BoolRva == offsetof(Slot388Objects, item));
    CHECK(kSlot388CommandRva - kSlot388BoolRva == offsetof(Slot388Objects, command));
    CHECK(sizeof(Slot388Objects) == 0x3017820 - kSlot388BoolRva);
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot388Objects o;
    poison(o);
    const Slot388Result r = init_slot388_6a8c60(o, creg, menu);
    CHECK(r.ok && r.rows_done == 3);
    check_bool_common(o.bvar, kSlot388BoolFnTag, kSlot388BoolValue);
    CHECK(same_view(o.bvar.base.name, kSlot388BoolName, kSlot388BoolNameLength));
    CHECK(o.item.flags == kSlot388ItemFlags && o.item.registered == 0 && o.item.kind == 2 && o.item.target == &o.bvar);
    CHECK(same_view(o.item.label, kSlot388ToggleParent, kSlot388ToggleParentLength));
    CHECK(same_view(o.item.text, kSlot388ToggleText, kSlot388ToggleTextLength));
    CHECK(o.item.range_min == 1.0f && o.item.range_max == 10.0f);   // bool class: {1, 10}
    CHECK(r.command.ok && r.command.vtable_register_slot_ok && r.command.views_inert && r.command.atexit_recorded && !r.command.handler_invoked);
    check_command(o.command, kSlot388CommandName, kSlot388CommandDescription, kSlot388CommandDescriptionLength, kSlot388HandlerTag);
    CHECK(menu.root.count == 1 && menu.root.link.next == &o.item.link);
    // registration order: the variable, then the command (the toggle is a menu item)
    CHECK(creg.pending.size() == 2 && creg.pending.first() == &o.bvar.base.pending);
    CHECK(o.bvar.base.pending.next == &o.command.base.pending && o.command.base.pending.next == &creg.pending.head);
    // atexit: bool 0x20b0, the menu-root destructor 0x27280 (first item), toggle 0x272d0, command 0x4b0
    CHECK(creg.atexit_log.size() == 4);
    check_atexit(creg, 0, 0x20b0, &o.bvar);
    check_atexit(creg, 1, 0x27280, &menu.root);
    check_atexit(creg, 2, 0x272d0, &o.item);
    check_atexit(creg, 3, 0x4b0, &o.command);
}

void section_failures() {
    {   // manager without an insert contract: the bool registration fails and nothing after it is built
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot388Objects o;
        poison(o);
        const Slot388Result r = init_slot388_6a8c60(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 0 && creg.pending.size() == 0 && menu.root.count == 0);
    }
    {   // menu parent without an insert contract: the toggle fails visibly after the variable was built
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot388Objects o;
        const Slot388Result r = init_slot388_6a8c60(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 1);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_388");
    return 0;
}
