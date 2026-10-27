// CPU-01-STARTUP-SLOT-310-01: see ctor_slot_310_a479d0.hpp.
#include "ctor_slot_310_a479d0.hpp"

namespace sotc {

const char* slot310_stage_name(Slot310Stage stage) noexcept {
    switch (stage) {
        case Slot310Stage::none: return "none";
        case Slot310Stage::titleid_build: return "titleid_build";
        case Slot310Stage::titleid_view_release: return "titleid_view_release";
        case Slot310Stage::titleid_atexit: return "titleid_atexit";
        case Slot310Stage::circle_build: return "circle_build";
        case Slot310Stage::title_ctor: return "title_ctor";
        case Slot310Stage::title_registration: return "title_registration";
        case Slot310Stage::title_view_release: return "title_view_release";
        case Slot310Stage::title_atexit: return "title_atexit";
        case Slot310Stage::circle_toggle_ctor: return "circle_toggle_ctor";
        case Slot310Stage::circle_toggle_registration: return "circle_toggle_registration";
        case Slot310Stage::circle_toggle_view_release: return "circle_toggle_view_release";
        case Slot310Stage::circle_toggle_atexit: return "circle_toggle_atexit";
        case Slot310Stage::tail_atexit: return "tail_atexit";
    }
    return "unknown";
}

Slot310Result init_slot310_a479d0(Slot310Objects& o, ConCmdRegistry& concmd, DebugMenuRegistry& menu,
                                  const DebugMenuAssertHooks& hooks) noexcept {
    Slot310Result out{};
    l2::StageFail<Slot310Result, Slot310Stage> fail{out};

    // 0xa479f0..0xa47a61: `titleid` (string variable), the two stack views released, __cxa_atexit(0x23c90)
    const SharedStr name = l2::literal_view(kSlot310TitleIdName);
    const SharedStr empty = l2::literal_view("");
    out.titleid = init_string_var_257a0(o.titleid, name, empty, concmd);
    if (!out.titleid.ok) return (void)fail.at(Slot310Stage::titleid_build), out;
    if (!release_inert_view_f590(empty) || !release_inert_view_f590(name)) {      // 0xa47a43, 0xa47a4b
        return (void)fail.at(Slot310Stage::titleid_view_release), out;
    }
    if (!concmd.atexit_log.record(kStringVarDtorRva, &o.titleid)) {
        return (void)fail.at(Slot310Stage::titleid_atexit), out;
    }

    // 0xa47a66..0xa47b65: `useCircleAsEnter` (bool variable, value 0 / default 0, functor tag 0x15d19b0)
    out.circle = init_bool_var_static(o.circle, l2::literal_view(kSlot310CircleName), l2::literal_view(""),
                                      kSlot310CircleFnVtbl, concmd, 0, 0);
    if (!out.circle.ok) return (void)fail.at(Slot310Stage::circle_build), out;

    // 0xa47b6a..0xa47be8: toggle A (target = titleid)
    const SharedStr label_a = l2::literal_view(kSlot310Label);
    const SharedStr text_a = l2::literal_view(kSlot310TitleText);
    if (!init_debug_menu_item_26ea0(o.toggle_title, label_a, text_a, string_var_menu_target(o.titleid),
                                    kSlot310ItemFlags)) {
        return (void)fail.at(Slot310Stage::title_ctor), out;
    }
    out.title_registration = register_menu_item_268b0(o.toggle_title, menu, concmd.atexit_log, hooks);
    if (!out.title_registration.ok) return (void)fail.at(Slot310Stage::title_registration), out;
    if (!release_inert_view_f590(text_a) || !release_inert_view_f590(label_a)) {   // 0xa47bc8, 0xa47bd0
        return (void)fail.at(Slot310Stage::title_view_release), out;
    }
    if (!record_menu_item_dtor_272d0(concmd.atexit_log, o.toggle_title)) {
        return (void)fail.at(Slot310Stage::title_atexit), out;
    }

    // 0xa47bed..0xa47c5d: toggle B (target = circle)
    const SharedStr label_b = l2::literal_view(kSlot310Label);
    const SharedStr text_b = l2::literal_view(kSlot310CircleText);
    if (!init_debug_menu_item_26ea0(o.toggle_circle, label_b, text_b, debug_menu_target_for(o.circle),
                                    kSlot310ItemFlags)) {
        return (void)fail.at(Slot310Stage::circle_toggle_ctor), out;
    }
    out.circle_registration = register_menu_item_268b0(o.toggle_circle, menu, concmd.atexit_log, hooks);
    if (!out.circle_registration.ok) return (void)fail.at(Slot310Stage::circle_toggle_registration), out;
    if (!release_inert_view_f590(text_b) || !release_inert_view_f590(label_b)) {   // 0xa47c44, 0xa47c4c
        return (void)fail.at(Slot310Stage::circle_toggle_view_release), out;
    }
    if (!record_menu_item_dtor_272d0(concmd.atexit_log, o.toggle_circle)) {
        return (void)fail.at(Slot310Stage::circle_toggle_atexit), out;
    }

    // 0xa47c62..0xa47c7a: qword 0 at 0x32f79a8; __cxa_atexit(0xa46ba0, tail)
    o.tail = 0;
    if (!concmd.atexit_log.record(kSlot310TailDtorRva, &o.tail)) return (void)fail.at(Slot310Stage::tail_atexit), out;

    out.ok = true;
    return out;
}

}  // namespace sotc
