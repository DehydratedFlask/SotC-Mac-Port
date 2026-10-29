// CPU-01-STARTUP-SLOT-389: fixture for startup constructor index 389 (0x695a50): the bool variable
// `shadow_mungeFishFinCloth` (value/default 1, tag 0x15cc690) and its `Shadow Cloth` 0x26db0 menu toggle. Lane M2.
#include "core/lane_m2_testkit.hpp"

#include "core/ctor_slot_389_695a50.hpp"

namespace {
using namespace sotc::lane_m2;

void section_constants() {
    CHECK(kSlot389Rva == 0x695a50 && kSlot389Size == 386 && kSlot389FnTag == 0x15cc690);
    CHECK(std::strlen(kSlot389VarName) == 0x18 && std::strlen(kSlot389Parent) == 0xc);
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot389Objects o;
    poison(o);
    const Slot389Result r = init_slot389_695a50(o, creg, menu);
    CHECK(r.ok && r.var.ok && r.item.ok && r.var.views_inert && r.item.views_inert);
    check_bool_var(o.var, "shadow_mungeFishFinCloth", 0x15cc690, 1, 1);
    check_toggle_item(o.item, o.var, "Shadow Cloth");
    CHECK(menu.root.count == 1 && menu.root.link.next == &o.item.link);
    check_var_atexit_and_pending(creg, o.var);
    CHECK(creg.atexit_log.size() == 3);                  // var dtor, menu-root dtor (first registration), item dtor
    check_atexit(creg, 1, 0x27280, &menu.root);
    check_atexit(creg, 2, 0x272d0, &o.item);
}

void section_failures() {
    {
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;                   // 0x20d60 has no contract for a foreign manager
        DebugMenuRegistry menu;
        Slot389Objects o;
        const Slot389Result r = init_slot389_695a50(o, creg, menu);
        CHECK(!r.ok && !r.var.ok && !r.item.ctor_ok && menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;                     // parent path without an insert contract
        Slot389Objects o;
        const Slot389Result r = init_slot389_695a50(o, creg, menu);
        CHECK(!r.ok && r.var.ok && !r.item.ok && r.item.registration.path == DebugMenuRegisterPath::parent);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_389");
    return 0;
}
