#include "ctor_slot_317_a2c390.hpp"

namespace sotc {

namespace {

constexpr char kEmpty[] = "";

FloatVarSpec float_spec(const char* name) noexcept {
    FloatVarSpec spec{};
    spec.name = name;
    spec.description = kEmpty;
    spec.bounded = false;                           // 0x6aa00
    spec.default_bits = kSlot317DefaultBits;        // value and default 0.5f
    spec.arg_fn_vtbl = kSlot317ArgFnEquivalent;     // slots identical to the site's 0x15d10f0 (checker-pinned)
    spec.flags = kSlot317VarFlags;
    return spec;
}

}  // namespace

DebugMenuTarget float_var_menu_target(const ConCmdRangedVar& var) noexcept {
    DebugMenuTarget target{};
    target.object = &var;
    target.known = var.base.vptr == kRangedFloatFinalVptr;
    target.slot3 = target.known ? kFloatClassSlot3Result : 0;
    target.slot6 = target.known ? kFloatClassSlot6Result : 0;
    return target;
}

const char* slot317_stage_name(Slot317Stage stage) noexcept {
    switch (stage) {
        case Slot317Stage::none: return "none";
        case Slot317Stage::large_build: return "large_build";
        case Slot317Stage::small_build: return "small_build";
        case Slot317Stage::group_ctor: return "group_ctor";
        case Slot317Stage::group_registration: return "group_registration";
        case Slot317Stage::group_view_release: return "group_view_release";
        case Slot317Stage::group_atexit: return "group_atexit";
        case Slot317Stage::toggle1_ctor: return "toggle1_ctor";
        case Slot317Stage::toggle1_registration: return "toggle1_registration";
        case Slot317Stage::toggle1_view_release: return "toggle1_view_release";
        case Slot317Stage::toggle1_atexit: return "toggle1_atexit";
        case Slot317Stage::toggle2_ctor: return "toggle2_ctor";
        case Slot317Stage::toggle2_registration: return "toggle2_registration";
        case Slot317Stage::toggle2_view_release: return "toggle2_view_release";
        case Slot317Stage::toggle2_atexit: return "toggle2_atexit";
    }
    return "";
}

Slot317Result init_slot317_a2c390(Slot317Objects& o, ConCmdRegistry& concmd, DebugMenuRegistry& menu,
                                  const DebugMenuAssertHooks& hooks) noexcept {
    Slot317Result out{};
    l2::StageFail<Slot317Result, Slot317Stage> fail{out};

    // 0xa2c3af..0xa2c4af: vibration_large_scalar (unbounded float, 0.5 / 0.5, __cxa_atexit(0x20f0))
    out.large = init_float_var_static(o.large, float_spec(kSlot317LargeName), concmd);
    if (!out.large.ok) return (void)fail.at(Slot317Stage::large_build), out;

    // 0xa2c4b4..0xa2c59d: vibration_small_scalar
    out.small = init_float_var_static(o.small, float_spec(kSlot317SmallName), concmd);
    if (!out.small.ok) return (void)fail.at(Slot317Stage::small_build), out;

    // 0xa2c5a2..0xa2c61d: group item (parent and caption are stack views)
    const SharedStr parent = l2::literal_view(kSlot317Parent);
    const SharedStr caption = l2::literal_view(kSlot317Caption);
    if (!init_menu_group_26c10(o.group, parent, caption, kSlot317ItemFlags)) {
        return (void)fail.at(Slot317Stage::group_ctor), out;
    }
    out.group_registration = register_menu_item_268b0(o.group, menu, concmd.atexit_log, hooks);
    if (!out.group_registration.ok) return (void)fail.at(Slot317Stage::group_registration), out;
    if (!release_inert_view_f590(caption) || !release_inert_view_f590(parent)) {    // 0xa2c603, 0xa2c60b
        return (void)fail.at(Slot317Stage::group_view_release), out;
    }
    if (!record_menu_item_dtor_272d0(concmd.atexit_log, o.group)) return (void)fail.at(Slot317Stage::group_atexit), out;

    // 0xa2c622..0xa2c6a8: toggle 1 targets the large scalar; the float class narrows the range
    const SharedStr label1 = l2::literal_view(kSlot317Caption);
    const SharedStr text1 = l2::literal_view(kSlot317Toggle1Text);
    if (!init_debug_menu_item_26ea0(o.toggle1, label1, text1, float_var_menu_target(o.large), kSlot317ItemFlags)) {
        return (void)fail.at(Slot317Stage::toggle1_ctor), out;
    }
    out.toggle1_registration = register_menu_item_268b0(o.toggle1, menu, concmd.atexit_log, hooks);
    if (!out.toggle1_registration.ok) return (void)fail.at(Slot317Stage::toggle1_registration), out;
    if (!release_inert_view_f590(text1) || !release_inert_view_f590(label1)) {      // 0xa2c688, 0xa2c690
        return (void)fail.at(Slot317Stage::toggle1_view_release), out;
    }
    if (!record_menu_item_dtor_272d0(concmd.atexit_log, o.toggle1)) {
        return (void)fail.at(Slot317Stage::toggle1_atexit), out;
    }

    // 0xa2c6ad..0xa2c72c: toggle 2 targets the small scalar
    const SharedStr label2 = l2::literal_view(kSlot317Caption);
    const SharedStr text2 = l2::literal_view(kSlot317Toggle2Text);
    if (!init_debug_menu_item_26ea0(o.toggle2, label2, text2, float_var_menu_target(o.small), kSlot317ItemFlags)) {
        return (void)fail.at(Slot317Stage::toggle2_ctor), out;
    }
    out.toggle2_registration = register_menu_item_268b0(o.toggle2, menu, concmd.atexit_log, hooks);
    if (!out.toggle2_registration.ok) return (void)fail.at(Slot317Stage::toggle2_registration), out;
    if (!release_inert_view_f590(text2) || !release_inert_view_f590(label2)) {      // 0xa2c70f, 0xa2c717
        return (void)fail.at(Slot317Stage::toggle2_view_release), out;
    }
    if (!record_menu_item_dtor_272d0(concmd.atexit_log, o.toggle2)) {
        return (void)fail.at(Slot317Stage::toggle2_atexit), out;
    }

    out.ok = true;
    return out;
}

}  // namespace sotc
