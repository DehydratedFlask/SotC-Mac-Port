#include "ctor_slot_307_a5b090.hpp"

namespace sotc {

namespace {

constexpr char kEmpty[] = "";

}  // namespace

const char* slot307_stage_name(Slot307Stage stage) noexcept {
    switch (stage) {
        case Slot307Stage::none: return "none";
        case Slot307Stage::cell_atexit: return "cell_atexit";
        case Slot307Stage::var_build: return "var_build";
        case Slot307Stage::caption_atexit: return "caption_atexit";
        case Slot307Stage::group_ctor: return "group_ctor";
        case Slot307Stage::group_registration: return "group_registration";
        case Slot307Stage::group_view_release: return "group_view_release";
        case Slot307Stage::group_atexit: return "group_atexit";
        case Slot307Stage::toggle1_ctor: return "toggle1_ctor";
        case Slot307Stage::toggle1_registration: return "toggle1_registration";
        case Slot307Stage::toggle1_view_release: return "toggle1_view_release";
        case Slot307Stage::toggle1_atexit: return "toggle1_atexit";
        case Slot307Stage::tail1_atexit: return "tail1_atexit";
        case Slot307Stage::tail2_atexit: return "tail2_atexit";
        case Slot307Stage::command1_build: return "command1_build";
        case Slot307Stage::command2_build: return "command2_build";
        case Slot307Stage::toggle2_ctor: return "toggle2_ctor";
        case Slot307Stage::toggle2_registration: return "toggle2_registration";
        case Slot307Stage::toggle2_view_release: return "toggle2_view_release";
        case Slot307Stage::toggle2_atexit: return "toggle2_atexit";
    }
    return "";
}

Slot307Result init_slot307_a5b090(Slot307Objects& o, ConCmdRegistry& concmd, DebugMenuRegistry& menu,
                                  const DebugMenuAssertHooks& hooks) noexcept {
    Slot307Result out{};
    l2::StageFail<Slot307Result, Slot307Stage> fail{out};

    // 0xa5b0d2 / 0xa5b0d9: [0x32f8b80] = 0; __cxa_atexit(0xa59370, cell)
    o.cell = 0;
    if (!concmd.atexit_log.record(kSlot307CellDtorRva, &o.cell)) return (void)fail.at(Slot307Stage::cell_atexit), out;

    // 0xa5b0de..0xa5b1e5: trophyProcessingEnabled (flags 1, empty description view, functor tag, 0 / 0)
    const SharedStr name = l2::literal_view(kSlot307VarName);
    const SharedStr description = l2::literal_view(kEmpty);
    out.var = init_bool_var_static(o.var, name, description, kSlot307FnVtbl, concmd, kSlot307Value, kSlot307Default);
    if (!out.var.ok) return (void)fail.at(Slot307Stage::var_build), out;

    // 0xa5b1f8..0xa5b223: global caption view {"Trophy System", 0xd, 0, 0}; __cxa_atexit(0xf7b0, caption)
    o.caption = l2::literal_view(kSlot307Caption);
    if (!concmd.atexit_log.record(kSlot307CaptionDtorRva, &o.caption)) {
        return (void)fail.at(Slot307Stage::caption_atexit), out;
    }

    // 0xa5b228..0xa5b27e: group item (parent = stack view, caption = the global)
    const SharedStr parent = l2::literal_view(kSlot307Parent);
    if (!init_menu_group_26c10(o.group, parent, o.caption, kSlot307ItemFlags)) {
        return (void)fail.at(Slot307Stage::group_ctor), out;
    }
    out.group_registration = register_menu_item_268b0(o.group, menu, concmd.atexit_log, hooks);
    if (!out.group_registration.ok) return (void)fail.at(Slot307Stage::group_registration), out;
    if (!release_inert_view_f590(parent)) return (void)fail.at(Slot307Stage::group_view_release), out;   // 0xa5b269
    if (!record_menu_item_dtor_272d0(concmd.atexit_log, o.group)) return (void)fail.at(Slot307Stage::group_atexit), out;

    // 0xa5b283..0xa5b2da: toggle 1 targets the bool variable
    const SharedStr text1 = l2::literal_view(kSlot307Toggle1Text);
    if (!init_debug_menu_item_26ea0(o.toggle1, o.caption, text1, debug_menu_target_for(o.var), kSlot307ItemFlags)) {
        return (void)fail.at(Slot307Stage::toggle1_ctor), out;
    }
    out.toggle1_registration = register_menu_item_268b0(o.toggle1, menu, concmd.atexit_log, hooks);
    if (!out.toggle1_registration.ok) return (void)fail.at(Slot307Stage::toggle1_registration), out;
    if (!release_inert_view_f590(text1)) return (void)fail.at(Slot307Stage::toggle1_view_release), out;  // 0xa5b2cc
    if (!record_menu_item_dtor_272d0(concmd.atexit_log, o.toggle1)) {
        return (void)fail.at(Slot307Stage::toggle1_atexit), out;
    }

    // 0xa5b2df..0xa5b334: two 24-byte zero objects, each with a destructor tag
    o.tail1 = {0, 0, 0};
    if (!concmd.atexit_log.record(kSlot307Tail1DtorRva, &o.tail1)) return (void)fail.at(Slot307Stage::tail1_atexit), out;
    o.tail2 = {0, 0, 0};
    if (!concmd.atexit_log.record(kSlot307Tail2DtorRva, &o.tail2)) return (void)fail.at(Slot307Stage::tail2_atexit), out;

    // 0xa5b339..0xa5b42c and 0xa5b431..0xa5b50b: the two console commands
    const FConCmdCommandSpec spec1{kSlot307Command1Name, kSlot307Command1Desc, sizeof kSlot307Command1Desc - 1,
                                   kSlot307Handler1Rva};
    out.command1 = init_fconcmd_object(o.command1, spec1, concmd);
    if (!out.command1.ok) return (void)fail.at(Slot307Stage::command1_build), out;
    const FConCmdCommandSpec spec2{kSlot307Command2Name, kSlot307Command2Desc, sizeof kSlot307Command2Desc - 1,
                                   kSlot307Handler2Rva};
    out.command2 = init_fconcmd_object(o.command2, spec2, concmd);
    if (!out.command2.ok) return (void)fail.at(Slot307Stage::command2_build), out;

    // 0xa5b510..0xa5b56b: toggle 2 targets command 2
    const SharedStr text2 = l2::literal_view(kSlot307Toggle2Text);
    if (!init_debug_menu_item_26ea0(o.toggle2, o.caption, text2, l2::command_menu_target(o.command2),
                                    kSlot307ItemFlags)) {
        return (void)fail.at(Slot307Stage::toggle2_ctor), out;
    }
    out.toggle2_registration = register_menu_item_268b0(o.toggle2, menu, concmd.atexit_log, hooks);
    if (!out.toggle2_registration.ok) return (void)fail.at(Slot307Stage::toggle2_registration), out;
    if (!release_inert_view_f590(text2)) return (void)fail.at(Slot307Stage::toggle2_view_release), out;  // 0xa5b559
    if (!record_menu_item_dtor_272d0(concmd.atexit_log, o.toggle2)) {
        return (void)fail.at(Slot307Stage::toggle2_atexit), out;
    }

    out.ok = true;
    return out;
}

}  // namespace sotc
