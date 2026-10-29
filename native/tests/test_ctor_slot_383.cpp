// CPU-01-STARTUP-SLOT-383: fixture for startup constructor index 383 (0x6c62b0). Lane M1.
#include "l4_test_util.hpp"

#include "core/ctor_slot_383_6c62b0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot383Rva == 0x6c62b0 && kSlot383Size == 386 && kSlot383FnTag == 0x15cc7d8 && kSlot383ObjectRva == 0x3239648 &&
          kSlot383ItemRva == 0x3239710);
    CHECK(std::strlen(kSlot383VarName) == 21 && std::strcmp(kSlot383VarName, "shadow_debugDrawCloth") == 0);
    CHECK(std::strlen(kSlot383Parent) == 12 && std::strcmp(kSlot383Parent, "Shadow Cloth") == 0);
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot383Objects o;
    poison(o);
    const Slot383Result r = init_slot383_6c62b0(o, creg, menu);
    CHECK(r.ok && r.var.ok && r.item.ok && r.item.ctor_ok && r.item.views_inert && r.item.atexit_recorded);

    check_bool_common(o.var, 0x15cc7d8, 0);
    CHECK(same_view(o.var.base.name, "shadow_debugDrawCloth", 21));

    // The 0x26db0 item: parent path in +0x28, caption = the variable's name view (copied) in +0x50.
    const DebugMenuItem& it = o.item;
    CHECK(it.target == &o.var && it.flags == 0x30 && it.kind == 2 && it.registered == 0);
    CHECK(same_view(it.label, "Shadow Cloth", 12));
    CHECK(it.text.data == o.var.base.name.data && it.text.size == 21 && it.text.ctl == nullptr);
    CHECK(dword(it, 0x98) == 0x3f800000u && dword(it, 0x9c) == 0x41200000u);
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
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot383Objects o;
        const Slot383Result r = init_slot383_6c62b0(o, creg, menu);
        CHECK(!r.ok && !r.var.ok && !r.item.ctor_ok);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;   // parent path without an insert contract
        Slot383Objects o;
        const Slot383Result r = init_slot383_6c62b0(o, creg, menu);
        CHECK(!r.ok && r.var.ok && !r.item.ok && r.item.registration.path == DebugMenuRegisterPath::parent);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_383");
    return 0;
}
