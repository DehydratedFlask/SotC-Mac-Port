// CPU-01-STARTUP-SLOT-463: fixture for startup constructor index 463 (0x3e0040): the bool console variable
// `cp10_playerClothDebug` and a menu toggle built by the new 0x26db0 under the parent path `Shadow Player`
// (caption taken from the variable's name). Lane M4.
#include "l4_test_util.hpp"

#include "core/ctor_slot_463_3e0040.hpp"
#include "core/ctor_slot_465_3cc9e0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot463Rva == 0x3e0040 && kSlot463Size == 410 && kSlot463FnTag == 0x15b1828);
    CHECK(sizeof kSlot463Name - 1 == 21 && std::strcmp(kSlot463Name, "cp10_playerClothDebug") == 0);
    CHECK(sizeof kSlot463Parent - 1 == 13 && std::strcmp(kSlot463Parent, "Shadow Player") == 0);
    CHECK(kSlot463ObjectRva == 0x2ef72b0 && kSlot463ItemRva == 0x2ef7378);
    CHECK(kSlot463ItemRva - kSlot463ObjectRva == sizeof(ConCmdBoolVar));   // item directly after the variable
    CHECK(kSlot463Value == 0 && kSlot463Default == 0 && kL4MenuItemFlags == 0x30);
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot463Objects o;
    poison(o);
    const Slot463Result r = init_slot463_3e0040(o, creg, menu);
    CHECK(r.ok && r.var.ok && r.item.ok && r.item.ctor_ok && r.item.views_inert && r.item.atexit_recorded);

    check_bool_common(o.var, 0x15b1828, 0);
    CHECK(same_view(o.var.base.name, "cp10_playerClothDebug", 21));

    // The 0x26db0 item: parent path in +0x28, caption = the variable's name view (copied) in +0x50.
    const DebugMenuItem& it = o.item;
    CHECK(it.target == &o.var && it.flags == 0x30 && it.kind == 2 && it.registered == 0);
    CHECK(same_view(it.label, "Shadow Player", 13));
    CHECK(it.text.data == o.var.base.name.data && it.text.size == 21 && it.text.ctl == nullptr);
    CHECK(dword(it, 0x98) == 0x3f800000u && dword(it, 0x9c) == 0x41200000u);   // default range {1.0, 10.0}
    CHECK(empty_view(it.empty));
    check_item_untouched(it);
    CHECK(menu.root_guard_done && menu.root.count == 1 && menu.root.link.next == &it.link);

    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.var.base.pending);
    CHECK(creg.atexit_log.size() == 3);
    check_atexit(creg, 0, 0x20b0, &o.var);
    check_atexit(creg, 1, 0x27280, &menu.root);
    check_atexit(creg, 2, 0x272d0, &o.item);
}

void section_after_neighbour() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot463Objects a;
    Slot465Objects b;
    CHECK(init_slot463_3e0040(a, creg, menu).ok);
    CHECK(init_slot465_3cc9e0(b, creg).ok);
    CHECK(creg.pending.size() == 2 && a.var.base.pending.next == &b.var.base.pending);
    CHECK(menu.root.count == 1);
    CHECK(creg.atexit_log.size() == 4);
    check_atexit(creg, 3, 0x20b0, &b.var);
}

void section_second_run() {
    // A second item reuses the already initialised root list: no second root destructor registration.
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot463Objects a;
    Slot463Objects b;
    CHECK(init_slot463_3e0040(a, creg, menu).ok && init_slot463_3e0040(b, creg, menu).ok);
    CHECK(menu.root.count == 2 && a.item.link.next == &b.item.link && b.item.link.next == &menu.root.link);
    CHECK(creg.atexit_log.size() == 5);
}

void section_failures() {
    {   // manager without a contract: the variable fails and the toggle stage is not reached
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot463Objects o;
        const Slot463Result r = init_slot463_3e0040(o, creg, menu);
        CHECK(!r.ok && !r.var.ok && !r.item.ctor_ok);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {   // parent path without an insert contract: the item registration fails visibly
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot463Objects o;
        const Slot463Result r = init_slot463_3e0040(o, creg, menu);
        CHECK(!r.ok && r.var.ok && !r.item.ok && r.item.registration.path == DebugMenuRegisterPath::parent);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_after_neighbour();
    section_second_run();
    section_failures();
    std::puts("OK test_ctor_slot_463");
    return 0;
}
