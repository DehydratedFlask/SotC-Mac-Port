// CPU-01-STARTUP-SLOT-439: fixture for startup constructor index 439 (0x4f7b70): the bool variable
// `r_waterWetHelperDebugDraw` and its debug-menu toggle built by 0x26db0 under the parent path `Water`. Lane m3.
#include "l4_test_util.hpp"

#include "core/ctor_slot_439_4f7b70.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot439Rva == 0x4f7b70 && kSlot439Size == 410 && kSlot439FnTag == 0x15c4a50);
    CHECK(kSlot439ObjectRva == 0x2efc180 && kSlot439ItemRva == 0x2efc248);
    CHECK(std::strlen(kSlot439VarName) == 25 && std::strcmp(kSlot439VarName, "r_waterWetHelperDebugDraw") == 0);
    CHECK(std::strlen(kSlot439Parent) == 5 && std::strcmp(kSlot439Parent, "Water") == 0);
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot439Objects o;
    poison(o);
    const Slot439Result r = init_slot439_4f7b70(o, creg, menu);
    CHECK(r.ok && r.var.ok && r.item.ok && r.item.ctor_ok && r.item.views_inert && r.item.atexit_recorded);
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);

    check_bool_common(o.var, 0x15c4a50, 0);
    CHECK(same_view(o.var.base.name, "r_waterWetHelperDebugDraw", 25));

    // The 0x26db0 item: parent path in +0x28, caption = the variable's name view (copied) in +0x50.
    const DebugMenuItem& it = o.item;
    CHECK(it.target == &o.var && it.flags == 0x30 && it.kind == 2 && it.registered == 0);
    CHECK(same_view(it.label, "Water", 5));
    CHECK(it.text.data == o.var.base.name.data && it.text.size == 25 && it.text.ctl == nullptr);
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
        Slot439Objects o;
        const Slot439Result r = init_slot439_4f7b70(o, creg, menu);
        CHECK(!r.ok && !r.var.ok && !r.item.ctor_ok);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent path without an insert contract: the toggle registration fails visibly.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot439Objects o;
        const Slot439Result r = init_slot439_4f7b70(o, creg, menu);
        CHECK(!r.ok && r.var.ok && !r.item.ok && r.item.registration.path == DebugMenuRegisterPath::parent);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_439");
    return 0;
}
