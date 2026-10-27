#include "ctor_slot_308_a58f10.hpp"

namespace sotc {

namespace {

constexpr char kEmpty[] = "";

}  // namespace

const char* slot308_stage_name(Slot308Stage stage) noexcept {
    switch (stage) {
        case Slot308Stage::none: return "none";
        case Slot308Stage::cell_atexit: return "cell_atexit";
        case Slot308Stage::level_build: return "level_build";
        case Slot308Stage::level_registration: return "level_registration";
        case Slot308Stage::level_view_release: return "level_view_release";
        case Slot308Stage::level_atexit: return "level_atexit";
        case Slot308Stage::enabled_build: return "enabled_build";
        case Slot308Stage::group_ctor: return "group_ctor";
        case Slot308Stage::group_registration: return "group_registration";
        case Slot308Stage::group_view_release: return "group_view_release";
        case Slot308Stage::group_atexit: return "group_atexit";
        case Slot308Stage::toggle1_ctor: return "toggle1_ctor";
        case Slot308Stage::toggle1_registration: return "toggle1_registration";
        case Slot308Stage::toggle1_view_release: return "toggle1_view_release";
        case Slot308Stage::toggle1_atexit: return "toggle1_atexit";
        case Slot308Stage::toggle2_ctor: return "toggle2_ctor";
        case Slot308Stage::toggle2_registration: return "toggle2_registration";
        case Slot308Stage::toggle2_view_release: return "toggle2_view_release";
        case Slot308Stage::toggle2_atexit: return "toggle2_atexit";
        case Slot308Stage::tail_atexit: return "tail_atexit";
    }
    return "";
}

Slot308Result init_slot308_a58f10(Slot308Objects& o, ConCmdRegistry& concmd, DebugMenuRegistry& menu,
                                  const DebugMenuAssertHooks& hooks) noexcept {
    Slot308Result out{};
    l2::StageFail<Slot308Result, Slot308Stage> fail{out};

    // 0xa58f4f / 0xa58f56: [0x32f84f8] = 0; __cxa_atexit(0xa50d50, cell)
    o.cell = 0;
    if (!concmd.atexit_log.record(kSlot308CellDtorRva, &o.cell)) return (void)fail.at(Slot308Stage::cell_atexit), out;

    // 0xa58f5b..0xa59026: sSaveManagerShowInfoLevel through 0xc580 (min 0, max 3, flags 1, zero args block)
    const SharedStr level_name = l2::literal_view(kSlot308LevelName);
    const SharedStr level_desc = l2::literal_view(kSlot308LevelDesc);
    RangedVarArgs args{};
    args.fn_vptr = kSlot308ArgFnVtbl;                 // [rsp+0xa0] = 0x15d1d80, payload 0, aux 0
    if (!init_ranged_int_var_c580(o.level, level_name, &o.level.value, kSlot308LevelMin, kSlot308LevelMax,
                                  kSlot308LevelFlags, args, level_desc)) {
        return (void)fail.at(Slot308Stage::level_build), out;
    }
    // 0xa5902f..0xa5903a: argument functor destroyed through slot 3 (0xc770 = bare ret): no effect.
    o.level.base.vptr = kRangedIntFinalVptr;          // 0xa5903d: [this] = 0x1594bf8 + 0x10
    o.level.value = 0;                                // 0xa59047: dword [this+0xd0] = 0
    o.level.default_value = 0;                        // 0xa59051: dword [this+0x68] = 0
    out.level_registration = register_concmd_20d60(o.level.base, concmd);    // 0xa5905b
    if (!out.level_registration.ok) return (void)fail.at(Slot308Stage::level_registration), out;
    if (!release_inert_view_f590(level_desc) || !release_inert_view_f590(level_name)) {   // 0xa59063, 0xa5906b
        return (void)fail.at(Slot308Stage::level_view_release), out;
    }
    if (!concmd.atexit_log.record(kRangedIntDtorRva, &o.level)) {                   // 0xa59084
        return (void)fail.at(Slot308Stage::level_atexit), out;
    }

    // 0xa59089..0xa5917e: sSaveManagerSaveEnabled (bool shape, tag 0x15d1da0, value 1 / default 1)
    const SharedStr enabled_name = l2::literal_view(kSlot308EnabledName);
    const SharedStr enabled_desc = l2::literal_view(kEmpty);
    out.enabled = init_bool_var_static(o.enabled, enabled_name, enabled_desc, kSlot308BoolFnVtbl, concmd,
                                       kSlot308EnabledValue, kSlot308EnabledDefault);
    if (!out.enabled.ok) return (void)fail.at(Slot308Stage::enabled_build), out;

    // 0xa59195..0xa59203: group item; both caption and parent are stack views here
    const SharedStr parent = l2::literal_view(kSlot308Parent);
    const SharedStr caption = l2::literal_view(kSlot308Caption);
    if (!init_menu_group_26c10(o.group, parent, caption, kSlot308ItemFlags)) {
        return (void)fail.at(Slot308Stage::group_ctor), out;
    }
    out.group_registration = register_menu_item_268b0(o.group, menu, concmd.atexit_log, hooks);
    if (!out.group_registration.ok) return (void)fail.at(Slot308Stage::group_registration), out;
    if (!release_inert_view_f590(caption) || !release_inert_view_f590(parent)) {    // 0xa591f3, 0xa591fb
        return (void)fail.at(Slot308Stage::group_view_release), out;
    }
    if (!record_menu_item_dtor_272d0(concmd.atexit_log, o.group)) return (void)fail.at(Slot308Stage::group_atexit), out;

    // 0xa5921f..0xa59297: toggle 1 targets the int variable
    const SharedStr label1 = l2::literal_view(kSlot308Caption);
    const SharedStr text1 = l2::literal_view(kSlot308Toggle1Text);
    if (!init_debug_menu_item_26ea0(o.toggle1, label1, text1, ranged_int_c580_menu_target(o.level),
                                    kSlot308ItemFlags)) {
        return (void)fail.at(Slot308Stage::toggle1_ctor), out;
    }
    out.toggle1_registration = register_menu_item_268b0(o.toggle1, menu, concmd.atexit_log, hooks);
    if (!out.toggle1_registration.ok) return (void)fail.at(Slot308Stage::toggle1_registration), out;
    if (!release_inert_view_f590(text1) || !release_inert_view_f590(label1)) {      // 0xa5927e, 0xa59286
        return (void)fail.at(Slot308Stage::toggle1_view_release), out;
    }
    if (!record_menu_item_dtor_272d0(concmd.atexit_log, o.toggle1)) {
        return (void)fail.at(Slot308Stage::toggle1_atexit), out;
    }

    // 0xa5929c..0xa5931b: toggle 2 targets the bool variable
    const SharedStr label2 = l2::literal_view(kSlot308Caption);
    const SharedStr text2 = l2::literal_view(kSlot308Toggle2Text);
    if (!init_debug_menu_item_26ea0(o.toggle2, label2, text2, debug_menu_target_for(o.enabled),
                                    kSlot308ItemFlags)) {
        return (void)fail.at(Slot308Stage::toggle2_ctor), out;
    }
    out.toggle2_registration = register_menu_item_268b0(o.toggle2, menu, concmd.atexit_log, hooks);
    if (!out.toggle2_registration.ok) return (void)fail.at(Slot308Stage::toggle2_registration), out;
    if (!release_inert_view_f590(text2) || !release_inert_view_f590(label2)) {      // 0xa59302, 0xa5930a
        return (void)fail.at(Slot308Stage::toggle2_view_release), out;
    }
    if (!record_menu_item_dtor_272d0(concmd.atexit_log, o.toggle2)) {
        return (void)fail.at(Slot308Stage::toggle2_atexit), out;
    }

    // 0xa59320..0xa59342: tail object (+0x08..+0x1f zeroed), __cxa_atexit(0xa55f30)
    o.tail.zero = {0, 0, 0};
    if (!concmd.atexit_log.record(kSlot308TailDtorRva, &o.tail)) return (void)fail.at(Slot308Stage::tail_atexit), out;

    out.ok = true;
    return out;
}

}  // namespace sotc
