// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 6): fixture for startup constructor index 366 (0x744ac0): two bool variables (`shadow_smoothAvoidControlRootY`,
// `shadow_debugAvoidControlRootY`) each followed by a 0x26db0 debug-menu toggle under `Shadow IK`.
#include "l4_test_util.hpp"

#include "core/ctor_slot_366_744ac0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot366Rva == 0x744ac0 && kSlot366Size == 695 && kSlot366FnTag == 0x15cd200);
    CHECK(kSlot366Var0Rva == 0x3252120 && kSlot366Item0Rva == 0x32521e8);
    CHECK(kSlot366Var1Rva == 0x3252290 && kSlot366Item1Rva == 0x3252358);
    CHECK(std::strlen(kSlot366Var0Name) == kSlot366Var0NameLength && kSlot366Var0NameLength == 0x1e);
    CHECK(std::strlen(kSlot366Var1Name) == kSlot366Var1NameLength && kSlot366Var1NameLength == 0x1d);
    CHECK(std::strlen(kSlot366Parent) == kSlot366ParentLength && kSlot366ParentLength == 9);
    CHECK(sizeof(ConCmdBoolVar) == 0xc8);
    CHECK(offsetof(Slot366Objects, item0) == kSlot366Item0Rva - kSlot366Var0Rva);
    CHECK(offsetof(Slot366Objects, var1) == kSlot366Var1Rva - kSlot366Var0Rva);
    CHECK(offsetof(Slot366Objects, item1) == kSlot366Item1Rva - kSlot366Var0Rva);
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot366Objects o;
    poison(o);
    const Slot366Result r = init_slot366_744ac0(o, creg, menu);
    CHECK(r.ok && r.var0.ok && r.item0.ok && r.var1.ok && r.item1.ok);
    CHECK(r.item0.ctor_ok && r.item0.views_inert && r.item0.atexit_recorded);
    CHECK(r.item1.ctor_ok && r.item1.views_inert && r.item1.atexit_recorded);
    CHECK(r.var0.registration.ok && r.var0.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var1.registration.ok && r.var1.registration.path == ConCmdRegisterPath::pending_list);

    check_bool_with_description(o.var0, kSlot366FnTag, 1, "", 0);
    CHECK(same_view(o.var0.base.name, kSlot366Var0Name, kSlot366Var0NameLength));
    check_bool_with_description(o.var1, kSlot366FnTag, 0, "", 0);
    CHECK(same_view(o.var1.base.name, kSlot366Var1Name, kSlot366Var1NameLength));

    // Each 0x26db0 item: parent path in +0x28, caption = the variable's name view (copied) in +0x50.
    const DebugMenuItem* its[2] = {&o.item0, &o.item1};
    const ConCmdBoolVar* vars[2] = {&o.var0, &o.var1};
    const std::size_t name_len[2] = {kSlot366Var0NameLength, kSlot366Var1NameLength};
    for (int i = 0; i < 2; ++i) {
        const DebugMenuItem& it = *its[i];
        CHECK(it.target == vars[i] && it.flags == 0x30 && it.kind == 2 && it.registered == 0);
        CHECK(same_view(it.label, kSlot366Parent, kSlot366ParentLength));
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
        Slot366Objects o;
        const Slot366Result r = init_slot366_744ac0(o, creg, menu);
        CHECK(!r.ok && !r.var0.ok && !r.item0.ctor_ok && !r.var1.ok);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent path without an insert contract: the first toggle registration fails visibly.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot366Objects o;
        const Slot366Result r = init_slot366_744ac0(o, creg, menu);
        CHECK(!r.ok && r.var0.ok && !r.item0.ok && r.item0.registration.path == DebugMenuRegisterPath::parent);
        CHECK(!r.var1.ok);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_366");
    return 0;
}
