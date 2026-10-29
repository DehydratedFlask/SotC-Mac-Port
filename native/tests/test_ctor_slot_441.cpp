// CPU-01-STARTUP-SLOT-441: fixture for startup constructor index 441 (0x4e68c0): the bool variable
// `cp10_debugSwordLight` and its debug-menu toggle built by 0x26db0 under the parent path `Shadow Player`. Lane m3.
#include "l4_test_util.hpp"

#include "core/ctor_slot_441_4e68c0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot441Rva == 0x4e68c0 && kSlot441Size == 410 && kSlot441FnTag == 0x15c2c20);
    CHECK(kSlot441ObjectRva == 0x2efbf08 && kSlot441ItemRva == 0x2efbfd0);
    CHECK(std::strlen(kSlot441VarName) == 20 && std::strcmp(kSlot441VarName, "cp10_debugSwordLight") == 0);
    CHECK(std::strlen(kSlot441Parent) == 13 && std::strcmp(kSlot441Parent, "Shadow Player") == 0);
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot441Objects o;
    poison(o);
    const Slot441Result r = init_slot441_4e68c0(o, creg, menu);
    CHECK(r.ok && r.var.ok && r.item.ok && r.item.ctor_ok && r.item.views_inert && r.item.atexit_recorded);
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);

    check_bool_common(o.var, 0x15c2c20, 0);
    CHECK(same_view(o.var.base.name, "cp10_debugSwordLight", 20));

    // The 0x26db0 item: parent path in +0x28, caption = the variable's name view (copied) in +0x50.
    const DebugMenuItem& it = o.item;
    CHECK(it.target == &o.var && it.flags == 0x30 && it.kind == 2 && it.registered == 0);
    CHECK(same_view(it.label, "Shadow Player", 13));
    CHECK(it.text.data == o.var.base.name.data && it.text.size == 20 && it.text.ctl == nullptr);
    CHECK(dword(it, 0x98) == 0x3f800000u && dword(it, 0x9c) == 0x41200000u);   // range {1.0, 10.0}: bool class
    CHECK(empty_view(it.empty));
    check_item_untouched(it);
    CHECK(menu.root.count == 1 && menu.root.link.next == &it.link);

    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.var.base.pending);
    CHECK(creg.atexit_log.size() == 3);
    check_atexit(creg, 0, 0x20b0, &o.var);
    check_atexit(creg, 1, 0x27280, &menu.root);
    check_atexit(creg, 2, 0x272d0, &o.item);
}

void section_failures() {
    {
        // Variable registration fails: the toggle stage is not reached and nothing is queued.
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot441Objects o;
        const Slot441Result r = init_slot441_4e68c0(o, creg, menu);
        CHECK(!r.ok && !r.var.ok && !r.item.ctor_ok);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent path without an insert contract: the toggle registration fails visibly.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot441Objects o;
        const Slot441Result r = init_slot441_4e68c0(o, creg, menu);
        CHECK(!r.ok && r.var.ok && !r.item.ok && r.item.registration.path == DebugMenuRegisterPath::parent);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_441");
    return 0;
}
