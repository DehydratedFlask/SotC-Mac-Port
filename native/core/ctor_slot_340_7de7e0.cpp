#include "ctor_slot_340_7de7e0.hpp"

namespace sotc {

namespace {

bool build_command(Slot340Result& out, Slot340Objects& objects, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const Slot340CommandSite& site = kSlot340Commands[n];
    const FConCmdCommandSpec spec{site.name, site.description, site.description_length, site.handler_tag};
    out.commands[n] = init_fconcmd_object(objects.cmd[n], spec, concmd);
    return out.commands[n].ok;
}

// 0x26ea0 reads the target's virtual slot 3 (and slot 6 only when slot 3 == 1). A console command (vptr 0x1594dd0) answers slot 3 with 0x22ae0 = 0;
// any other class is an unlifted contract and fails closed (known == false).
DebugMenuTarget command_menu_target(const ConCmdCommand& cmd) noexcept {
    DebugMenuTarget target{};
    target.object = &cmd;
    if (cmd.base.vptr == kFConCmdVptr) {
        target.slot3 = kSlot340CommandGate3;
        target.slot6 = 0;
        target.known = true;
    }
    return target;
}

}  // namespace

Slot340Result init_slot340_7de7e0(Slot340Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot340Result out{};
    for (std::size_t n = 0; n < 4; ++n) {   // 0x7de81e..0x7deb85: four commands c0..c3
        if (!build_command(out, objects, n, concmd)) return out;
        ++out.rows_done;
    }
    // 0x7debb2..0x7debc3: [cell] = name; cell->hash = 0x52a990(name); the four bytes at +0x0c are never written.
    objects.cell.name = kSlot340CellName;
    objects.cell.hash = fnv_cstr_52a990(kSlot340CellName);
    ++out.rows_done;
    if (!build_command(out, objects, 4, concmd)) return out;   // 0x7debdf..0x7dec85: c4 (its object address stays live in r13)
    ++out.rows_done;
    // 0x7decb6..0x7ded2b: 0x26c10(group, {CP10}, {Shadow Cheat}, 0x30); f590(caption); f590(parent); atexit(0x272d0, group).
    if (!family_group_item_26c10(objects.group, kSlot340GroupParent, kSlot340GroupParentLength, kSlot340GroupCaption, kSlot340GroupCaptionLength,
                                 kSlot340ItemFlags, concmd, menu).ok()) {
        return out;
    }
    ++out.rows_done;
    // 0x7ded49..0x7dedcd: 0x26ea0(toggle, {Shadow Cheat}, {Give All Orbs}, target = c4, 0x30); f590(text); f590(label); atexit(0x272d0, toggle).
    if (!family_toggle_item_26ea0(objects.toggle, kSlot340GroupCaption, kSlot340GroupCaptionLength, kSlot340ToggleText, kSlot340ToggleTextLength,
                                  command_menu_target(objects.cmd[4]), kSlot340ItemFlags, concmd, menu).ok()) {
        return out;
    }
    ++out.rows_done;
    if (!build_command(out, objects, 5, concmd)) return out;   // 0x7dedd2..0x7deeba: c5, dispatched through the spilled 0x38(%rsp)
    ++out.rows_done;
    out.ok = true;
    return out;
}

}  // namespace sotc
