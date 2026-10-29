// CPU-01-STARTUP-SLOT-403: fixture for startup constructor index 403 (0x5f0160): the bool variable
// `shadow_drawModelCharacterClasses` (value/default 0, tag 0x15cc078) and its `Shadow Misc` 0x26db0 menu toggle. Lane M2.
#include "core/lane_m2_testkit.hpp"

#include "core/ctor_slot_403_5f0160.hpp"

namespace {
using namespace sotc::lane_m2;

void section_constants() {
    CHECK(kSlot403Rva == 0x5f0160 && kSlot403Size == 386 && kSlot403FnTag == 0x15cc078);
    CHECK(std::strlen(kSlot403VarName) == 0x20 && std::strlen(kSlot403Parent) == 0xb);
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot403Objects o;
    poison(o);
    const Slot403Result r = init_slot403_5f0160(o, creg, menu);
    CHECK(r.ok && r.var.ok && r.item.ok && r.var.views_inert && r.item.views_inert);
    check_bool_var(o.var, "shadow_drawModelCharacterClasses", 0x15cc078, 0, 1);
    check_toggle_item(o.item, o.var, "Shadow Misc");
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
        Slot403Objects o;
        const Slot403Result r = init_slot403_5f0160(o, creg, menu);
        CHECK(!r.ok && !r.var.ok && !r.item.ctor_ok && menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;                     // parent path without an insert contract
        Slot403Objects o;
        const Slot403Result r = init_slot403_5f0160(o, creg, menu);
        CHECK(!r.ok && r.var.ok && !r.item.ok && r.item.registration.path == DebugMenuRegisterPath::parent);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_403");
    return 0;
}
