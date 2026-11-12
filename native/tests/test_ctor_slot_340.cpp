// CPU-01-STARTUP-BATCH-10: fixture for startup constructor index 340 (0x7de7e0): six console commands (four empty-description `shadow_gameflag*`,
// `bp_giveAllOrbs`, `bp_dumpCp3GameStateInfo`), one fnv1a name cell, one menu group and one 0x26ea0 toggle whose target is a console command object.
// The six former "indirect calls" are the console-command register-once slot [0x1594dc0 + 0x20] = 0x20d60 (checker-pinned), built by init_fconcmd_object.
#include "l4_test_util.hpp"

#include "core/ctor_slot_340_7de7e0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot340Rva == 0x7de7e0 && kSlot340Size == 1800 && kSlot340ItemFlags == 0x30 && kSlot340CommandCount == 6);
    for (std::size_t n = 0; n < kSlot340CommandCount; ++n) {
        const Slot340CommandSite& s = kSlot340Commands[n];
        CHECK(std::strlen(s.description) == s.description_length && s.handler_tag != 0 && s.object_rva != 0);
    }
    CHECK(kSlot340Commands[4].description_length == 0x2f && kSlot340Commands[5].description_length == 0x4b);
    CHECK(std::strlen(kSlot340GroupParent) == kSlot340GroupParentLength && std::strlen(kSlot340GroupCaption) == kSlot340GroupCaptionLength);
    CHECK(std::strlen(kSlot340ToggleText) == kSlot340ToggleTextLength);
    CHECK(sizeof(ConCmdCommand) == 0xa0 && sizeof(DebugMenuItem) == 0xa8);
    // c0..c3 sit at a 0xa0 stride; c4 / group / toggle / c5 are the image order after the cell.
    for (std::size_t n = 1; n < 4; ++n) CHECK(kSlot340Commands[n].object_rva - kSlot340Commands[n - 1].object_rva == sizeof(ConCmdCommand));
    CHECK(kSlot340GroupRva - kSlot340Commands[4].object_rva == sizeof(ConCmdCommand));
    CHECK(kSlot340ToggleRva - kSlot340GroupRva == sizeof(DebugMenuItem));
    CHECK(kSlot340Commands[5].object_rva - kSlot340ToggleRva == sizeof(DebugMenuItem));
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot340Objects o;
    poison(o);
    const Slot340Result r = init_slot340_7de7e0(o, creg, menu);
    CHECK(r.ok && r.rows_done == 9);
    for (std::size_t n = 0; n < kSlot340CommandCount; ++n) {
        const Slot340CommandSite& s = kSlot340Commands[n];
        CHECK(r.commands[n].ok && r.commands[n].vtable_register_slot_ok && r.commands[n].views_inert && r.commands[n].atexit_recorded &&
              !r.commands[n].handler_invoked);
        check_command(o.cmd[n], s.name, s.description, s.description_length, s.handler_tag);
        CHECK(o.cmd[n].base.registered == 1 && o.cmd[n].base.flags == 2);
    }
    CHECK(empty_view(o.cmd[0].base.description) && empty_view(o.cmd[3].base.description));
    CHECK(o.cell.name == kSlot340CellName && o.cell.hash == fnv_cstr_52a990(kSlot340CellName) && o.cell.unwritten == 0xa5a5a5a5u);
    CHECK(o.cell.hash != fnv_cstr_52a990("TimeAttackSetBestTim"));

    // registration: the six commands in constructor order c0 c1 c2 c3 c4 c5
    CHECK(creg.pending.size() == 6 && creg.pending.first() == &o.cmd[0].base.pending);
    for (std::size_t n = 0; n + 1 < kSlot340CommandCount; ++n) CHECK(o.cmd[n].base.pending.next == &o.cmd[n + 1].base.pending);
    CHECK(o.cmd[5].base.pending.next == &creg.pending.head);

    // menu rows: group (kind group, no target) then the 0x26ea0 toggle (kind 2, target = the command object c4)
    CHECK(menu.root.count == 2 && menu.root.link.next == &o.group.link && o.group.link.next == &o.toggle.link && o.toggle.link.next == &menu.root.link);
    CHECK(o.group.flags == kSlot340ItemFlags && o.group.registered == 0 && o.group.kind == kMenuKindGroup && o.group.target == nullptr);
    CHECK(same_view(o.group.label, kSlot340GroupParent, kSlot340GroupParentLength) && same_view(o.group.text, kSlot340GroupCaption, kSlot340GroupCaptionLength));
    CHECK(o.toggle.flags == kSlot340ItemFlags && o.toggle.registered == 0 && o.toggle.kind == 2 && o.toggle.target == &o.cmd[4]);
    CHECK(same_view(o.toggle.label, kSlot340GroupCaption, kSlot340GroupCaptionLength) && same_view(o.toggle.text, kSlot340ToggleText, kSlot340ToggleTextLength));
    CHECK(o.toggle.range_min == 1.0f && o.toggle.range_max == 10.0f);   // command class slot 3 = 0: the {0.1, 0.5} narrowing never holds

    // atexit: c0 c1 c2 c3 c4, root 0x27280, group, toggle, c5
    CHECK(creg.atexit_log.size() == 9);
    for (std::size_t n = 0; n < 5; ++n) check_atexit(creg, n, 0x4b0, &o.cmd[n]);
    check_atexit(creg, 5, 0x27280, &menu.root);
    check_atexit(creg, 6, 0x272d0, &o.group);
    check_atexit(creg, 7, 0x272d0, &o.toggle);
    check_atexit(creg, 8, 0x4b0, &o.cmd[5]);
}

void section_failures() {
    {   // registration without an insert contract: c0 fails and nothing after it is reached
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        creg.manager.manager = &token;
        Slot340Objects o;
        const Slot340Result r = init_slot340_7de7e0(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 0 && creg.pending.size() == 0 && menu.root.count == 0);
    }
    {   // a menu parent without an insert contract: the group registration fails visibly; the five earlier rows were built
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot340Objects o;
        const Slot340Result r = init_slot340_7de7e0(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 6 && creg.pending.size() == 5);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_340");
    return 0;
}
