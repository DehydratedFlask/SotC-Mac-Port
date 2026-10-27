#include "ctor_slot_313_a447a0.hpp"

namespace sotc {

const char* slot313_stage_name(Slot313Stage stage) noexcept {
    switch (stage) {
        case Slot313Stage::none: return "none";
        case Slot313Stage::cell_atexit: return "cell_atexit";
        case Slot313Stage::caption_atexit: return "caption_atexit";
        case Slot313Stage::command_build: return "command_build";
        case Slot313Stage::group_ctor: return "group_ctor";
        case Slot313Stage::group_registration: return "group_registration";
        case Slot313Stage::group_view_release: return "group_view_release";
        case Slot313Stage::group_atexit: return "group_atexit";
        case Slot313Stage::toggle_ctor: return "toggle_ctor";
        case Slot313Stage::toggle_registration: return "toggle_registration";
        case Slot313Stage::toggle_view_release: return "toggle_view_release";
        case Slot313Stage::toggle_atexit: return "toggle_atexit";
        case Slot313Stage::tail_atexit: return "tail_atexit";
    }
    return "";
}

Slot313Result init_slot313_a447a0(Slot313Objects& o, ConCmdRegistry& concmd, DebugMenuRegistry& menu,
                                  const DebugMenuAssertHooks& hooks) noexcept {
    Slot313Result out{};
    l2::StageFail<Slot313Result, Slot313Stage> fail{out};

    // 0xa447df / 0xa447e6: [0x32f6d18] = 0; __cxa_atexit(0xa42900, cell)
    o.cell = 0;
    if (!concmd.atexit_log.record(kSlot313CellDtorRva, &o.cell)) return (void)fail.at(Slot313Stage::cell_atexit), out;

    // 0xa447eb..0xa44821: the global caption view {data, 0x12, 0, 0}; __cxa_atexit(0xf7b0, caption)
    o.caption = l2::literal_view(kSlot313Caption);
    if (!concmd.atexit_log.record(kSlot313CaptionDtorRva, &o.caption)) {
        return (void)fail.at(Slot313Stage::caption_atexit), out;
    }

    // 0xa44826..0xa4890c: the console command (shared shape; the handler is an identity tag)
    const FConCmdCommandSpec spec{kSlot313CommandName, kSlot313Description, kSlot313DescriptionLength,
                                  kSlot313HandlerRva};
    out.command = init_fconcmd_object(o.command, spec, concmd);
    if (!out.command.ok) return (void)fail.at(Slot313Stage::command_build), out;

    // 0xa44925..0xa44970: group item. parent = stack view "Platform Services", caption = the global.
    const SharedStr parent = l2::literal_view(kSlot313Parent);
    if (!init_menu_group_26c10(o.group, parent, o.caption, kSlot313ItemFlags)) {
        return (void)fail.at(Slot313Stage::group_ctor), out;
    }
    out.group_registration = register_menu_item_268b0(o.group, menu, concmd.atexit_log, hooks);
    if (!out.group_registration.ok) return (void)fail.at(Slot313Stage::group_registration), out;
    if (!release_inert_view_f590(parent)) return (void)fail.at(Slot313Stage::group_view_release), out;  // 0xa4496d
    if (!record_menu_item_dtor_272d0(concmd.atexit_log, o.group)) return (void)fail.at(Slot313Stage::group_atexit), out;

    // 0xa44987..0xa449de: toggle item. label = the global caption, text = stack view, target = command.
    const SharedStr text = l2::literal_view(kSlot313Text);
    if (!init_debug_menu_item_26ea0(o.toggle, o.caption, text, l2::command_menu_target(o.command),
                                    kSlot313ItemFlags)) {
        return (void)fail.at(Slot313Stage::toggle_ctor), out;
    }
    out.toggle_registration = register_menu_item_268b0(o.toggle, menu, concmd.atexit_log, hooks);
    if (!out.toggle_registration.ok) return (void)fail.at(Slot313Stage::toggle_registration), out;
    if (!release_inert_view_f590(text)) return (void)fail.at(Slot313Stage::toggle_view_release), out;   // 0xa449d0
    if (!record_menu_item_dtor_272d0(concmd.atexit_log, o.toggle)) {
        return (void)fail.at(Slot313Stage::toggle_atexit), out;
    }

    // 0xa449e3..0xa44a0b: 24 zero bytes at 0x32f6f38; __cxa_atexit(0xa42950, tail)
    o.tail = {0, 0, 0};
    if (!concmd.atexit_log.record(kSlot313TailDtorRva, &o.tail)) return (void)fail.at(Slot313Stage::tail_atexit), out;

    out.ok = true;
    return out;
}

}  // namespace sotc
