// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 6): fixture for startup constructor index 455 (0x47bfa0): two bool variables
// (`looseAttachments_Enable`, `looseAttachments_ShowCollision`), a 0x26c10 group `Loose Attachment` under `Animation`, and two
// 0x26db0 toggles under the group caption.
#include "l4_test_util.hpp"

#include "core/ctor_slot_455_47bfa0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot455Rva == 0x47bfa0 && kSlot455Size == 817 && kSlot455FnTag == 0x15bddb8);
    CHECK(std::strlen(kSlot455Var0Name) == kSlot455Var0NameLength && kSlot455Var0NameLength == 0x17);
    CHECK(std::strlen(kSlot455Var1Name) == kSlot455Var1NameLength && kSlot455Var1NameLength == 0x1e);
    CHECK(std::strlen(kSlot455GroupParent) == kSlot455GroupParentLength && kSlot455GroupParentLength == 9);
    CHECK(std::strlen(kSlot455GroupCaption) == kSlot455GroupCaptionLength && kSlot455GroupCaptionLength == 0x10);
    CHECK(sizeof(ConCmdBoolVar) == 0xc8 && sizeof(DebugMenuItem) == 0xa8);
    CHECK(offsetof(Slot455Objects, var1) == kSlot455Var1Rva - kSlot455Var0Rva);
    CHECK(offsetof(Slot455Objects, group) == kSlot455GroupRva - kSlot455Var0Rva);
    CHECK(offsetof(Slot455Objects, item0) == kSlot455Item0Rva - kSlot455Var0Rva);
    CHECK(offsetof(Slot455Objects, item1) == kSlot455Item1Rva - kSlot455Var0Rva);
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot455Objects o;
    poison(o);
    const Slot455Result r = init_slot455_47bfa0(o, creg, menu);
    CHECK(r.ok && r.var0.ok && r.var1.ok && r.group.ok() && r.item0.ok && r.item1.ok);
    CHECK(r.var0.registration.ok && r.var0.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var1.registration.ok && r.var1.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.item0.ctor_ok && r.item0.views_inert && r.item0.atexit_recorded);
    CHECK(r.item1.ctor_ok && r.item1.views_inert && r.item1.atexit_recorded);

    check_bool_with_description(o.var0, kSlot455FnTag, 1, "", 0);
    CHECK(same_view(o.var0.base.name, kSlot455Var0Name, kSlot455Var0NameLength));
    check_bool_with_description(o.var1, kSlot455FnTag, 0, "", 0);
    CHECK(same_view(o.var1.base.name, kSlot455Var1Name, kSlot455Var1NameLength));

    const DebugMenuItem& g = o.group;
    CHECK(g.kind == kMenuKindGroup && g.target == nullptr && g.flags == kSlot455GroupFlags && g.registered == 0);
    CHECK(same_view(g.label, kSlot455GroupParent, kSlot455GroupParentLength));   // the parent name
    CHECK(same_view(g.text, kSlot455GroupCaption, kSlot455GroupCaptionLength));

    // Each 0x26db0 item: parent path (the group caption) in +0x28, caption = the variable's name view in +0x50.
    const DebugMenuItem* its[2] = {&o.item0, &o.item1};
    const ConCmdBoolVar* vars[2] = {&o.var0, &o.var1};
    const std::size_t name_len[2] = {kSlot455Var0NameLength, kSlot455Var1NameLength};
    for (int i = 0; i < 2; ++i) {
        const DebugMenuItem& it = *its[i];
        CHECK(it.target == vars[i] && it.flags == 0x30 && it.kind == 2 && it.registered == 0);
        CHECK(same_view(it.label, kSlot455GroupCaption, kSlot455GroupCaptionLength));
        CHECK(it.text.data == vars[i]->base.name.data && it.text.size == static_cast<std::int64_t>(name_len[i]) &&
              it.text.ctl == nullptr);
        CHECK(it.range_min == 1.0f && it.range_max == 10.0f);   // bool class slot 6 is 0: no narrowing
        CHECK(empty_view(it.empty));
        check_item_untouched(it);
    }
    CHECK(menu.root.count == 3);

    CHECK(creg.pending.size() == 2 && creg.pending.first() == &o.var0.base.pending);
    // var0, var1, [root-list destructor on the first item], group, item0, item1.
    CHECK(creg.atexit_log.size() == 6);
    check_atexit(creg, 0, 0x20b0, &o.var0);
    check_atexit(creg, 1, 0x20b0, &o.var1);
    check_atexit(creg, 2, 0x27280, &menu.root);
    check_atexit(creg, 3, 0x272d0, &o.group);
    check_atexit(creg, 4, 0x272d0, &o.item0);
    check_atexit(creg, 5, 0x272d0, &o.item1);
}

void section_failures() {
    {
        // First variable registration fails: nothing after it is reached and nothing is queued.
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot455Objects o;
        const Slot455Result r = init_slot455_47bfa0(o, creg, menu);
        CHECK(!r.ok && !r.var0.ok && !r.var1.ok && !r.group.ctor_ok && !r.item0.ctor_ok);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent path without an insert contract: the group registration fails visibly and no toggle is reached.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot455Objects o;
        const Slot455Result r = init_slot455_47bfa0(o, creg, menu);
        CHECK(!r.ok && r.var0.ok && r.var1.ok && !r.group.ok() && !r.item0.ctor_ok);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_455");
    return 0;
}
