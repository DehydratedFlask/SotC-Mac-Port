// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 6): fixture for startup constructor index 411 (0x591710): two bool variables (`shadow_cameraUsePureLwsOrientation`,
// `shadow_cameraDebugPureLwsOrientation`) each followed by a 0x26db0 debug-menu toggle under `Shadow Camera`.
#include "l4_test_util.hpp"

#include "core/ctor_slot_411_591710.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot411Rva == 0x591710 && kSlot411Size == 692 && kSlot411FnTag == 0x15cb168);
    CHECK(kSlot411Var0Rva == 0x2fd98a0 && kSlot411Item0Rva == 0x2fd9a30);
    CHECK(kSlot411Var1Rva == 0x2fd9968 && kSlot411Item1Rva == 0x2fd9ad8);
    CHECK(std::strlen(kSlot411Var0Name) == kSlot411Var0NameLength && kSlot411Var0NameLength == 0x22);
    CHECK(std::strlen(kSlot411Var1Name) == kSlot411Var1NameLength && kSlot411Var1NameLength == 0x24);
    CHECK(std::strlen(kSlot411Parent) == kSlot411ParentLength && kSlot411ParentLength == 13);
    CHECK(sizeof(ConCmdBoolVar) == 0xc8);
    CHECK(offsetof(Slot411Objects, item0) == kSlot411Item0Rva - kSlot411Var0Rva);
    CHECK(offsetof(Slot411Objects, var1) == kSlot411Var1Rva - kSlot411Var0Rva);
    CHECK(offsetof(Slot411Objects, item1) == kSlot411Item1Rva - kSlot411Var0Rva);
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot411Objects o;
    poison(o);
    const Slot411Result r = init_slot411_591710(o, creg, menu);
    CHECK(r.ok && r.var0.ok && r.item0.ok && r.var1.ok && r.item1.ok);
    CHECK(r.item0.ctor_ok && r.item0.views_inert && r.item0.atexit_recorded);
    CHECK(r.item1.ctor_ok && r.item1.views_inert && r.item1.atexit_recorded);
    CHECK(r.var0.registration.ok && r.var0.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var1.registration.ok && r.var1.registration.path == ConCmdRegisterPath::pending_list);

    check_bool_with_description(o.var0, kSlot411FnTag, 1, "", 0);
    CHECK(same_view(o.var0.base.name, kSlot411Var0Name, kSlot411Var0NameLength));
    check_bool_with_description(o.var1, kSlot411FnTag, 0, "", 0);
    CHECK(same_view(o.var1.base.name, kSlot411Var1Name, kSlot411Var1NameLength));

    // Each 0x26db0 item: parent path in +0x28, caption = the variable's name view (copied) in +0x50.
    const DebugMenuItem* its[2] = {&o.item0, &o.item1};
    const ConCmdBoolVar* vars[2] = {&o.var0, &o.var1};
    const std::size_t name_len[2] = {kSlot411Var0NameLength, kSlot411Var1NameLength};
    for (int i = 0; i < 2; ++i) {
        const DebugMenuItem& it = *its[i];
        CHECK(it.target == vars[i] && it.flags == 0x30 && it.kind == 2 && it.registered == 0);
        CHECK(same_view(it.label, kSlot411Parent, kSlot411ParentLength));
        CHECK(it.text.data == vars[i]->base.name.data && it.text.size == static_cast<std::int64_t>(name_len[i]) &&
              it.text.ctl == nullptr);
        CHECK(dword(it, 0x98) == 0x3f800000u && dword(it, 0x9c) == 0x41200000u);   // range {1.0, 10.0}: bool class
        CHECK(empty_view(it.empty));
        check_item_untouched(it);
    }
    CHECK(menu.root.count == 2 && menu.root.link.next == &o.item0.link && o.item0.link.next == &o.item1.link);

    CHECK(creg.pending.size() == 2 && creg.pending.first() == &o.var0.base.pending);
    // var0, [root-list destructor on the first item], item0, var1, item1.
    CHECK(creg.atexit_log.size() == 5);
    check_atexit(creg, 0, 0x20b0, &o.var0);
    check_atexit(creg, 1, 0x27280, &menu.root);
    check_atexit(creg, 2, 0x272d0, &o.item0);
    check_atexit(creg, 3, 0x20b0, &o.var1);
    check_atexit(creg, 4, 0x272d0, &o.item1);
}

void section_failures() {
    {
        // First variable registration fails: nothing after it is reached and nothing is queued.
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot411Objects o;
        const Slot411Result r = init_slot411_591710(o, creg, menu);
        CHECK(!r.ok && !r.var0.ok && !r.item0.ctor_ok && !r.var1.ok);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent path without an insert contract: the first toggle registration fails visibly.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot411Objects o;
        const Slot411Result r = init_slot411_591710(o, creg, menu);
        CHECK(!r.ok && r.var0.ok && !r.item0.ok && r.item0.registration.path == DebugMenuRegisterPath::parent);
        CHECK(!r.var1.ok);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_411");
    return 0;
}
