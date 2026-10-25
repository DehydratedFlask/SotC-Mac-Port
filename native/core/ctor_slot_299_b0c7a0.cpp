#include "ctor_slot_299_b0c7a0.hpp"

#include <cstring>

namespace sotc {

namespace {

constexpr char kEmpty[] = "";

template <std::size_t N>
SharedStr borrowed(const char (&text)[N]) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(N - 1), 0,
                     nullptr};
}

}  // namespace

Slot299Result init_slot299_b0c7a0(Slot299Objects& objects, ConCmdRegistry& concmd,
                                  DebugMenuRegistry& menu) noexcept {
    Slot299Result out{};

    // ---- A: globalDebugTextScale (0xb0c7bf..0xb0c8f4) -------------------------------------------
    const SharedStr name_a = borrowed(kSlot299ScaleName);
    const SharedStr desc_a{reinterpret_cast<const std::uint8_t*>(kEmpty), 0, 0, nullptr};
    RangedVarArgs args_a{};
    args_a.fn_payload = 0;                       // [rsp+0x90] = 0
    args_a.fn_vptr = kRangedFloatArgFnVtbl;      // [rsp+0xa0] = 0x15d57e0
    args_a.fn_aux = 0;                           // [rsp+0xa8] = 0
    ConCmdRangedVar& a = objects.scale;
    a.base.vptr = kRangedFloatFinalVptr;         // 0xb0c85a: [A] = 0x15942b0 (overwritten by ctor)
    out.scale_ctor_ok = init_ranged_float_var_c300(a, name_a, &a.value, 0.1f, 10.0f, 1, args_a, desc_a);
    if (!out.scale_ctor_ok) return out;
    const bool desc_a_inert = release_inert_view_f590(desc_a);               // 0xb0c89d
    out.scale_functor_destroy_noop = args_a.fn_vptr == kRangedFloatArgFnVtbl;  // 0xb0c8b2 -> 0xc4f0
    a.base.vptr = kRangedFloatFinalVptr;                                      // 0xb0c8b8
    a.value = 0x3f800000u;                                                    // dword [A+0xd0] = 1.0f
    a.default_value = 0x3f800000u;                                            // dword [A+0x68] = 1.0f
    out.scale_registration = register_concmd_20d60(a.base, concmd);           // 0xb0c8d3
    out.scale_views_inert = desc_a_inert && release_inert_view_f590(name_a);  // 0xb0c8db
    out.scale_atexit_recorded = concmd.atexit_log.record(kRangedFloatDtorRva, &a);  // 0xb0c8f4

    // ---- B: globalDebugTextAdjustFor4K (0xb0c8f9..0xb0c9fc) -------------------------------------
    const SharedStr name_b = borrowed(kSlot299AdjustName);
    const SharedStr desc_b{reinterpret_cast<const std::uint8_t*>(kEmpty), 0, 0, nullptr};
    out.adjust = init_bool_var_static(objects.adjust, name_b, desc_b, kSlot299AdjustFnVtbl, concmd);

    // ---- C: debug-menu item for A (0xb0ca01..0xb0ca94) -----------------------------------------
    const SharedStr label = borrowed(kSlot299Label);
    const SharedStr text_c = borrowed(kSlot299ScaleText);
    out.scale_item_ctor_ok =
        init_debug_menu_item_27050(objects.scale_item, label, text_c, &a, 0, 0.1f, 1.0f);
    if (out.scale_item_ctor_ok) {
        out.scale_item_registration =
            register_debug_menu_item_268b0(objects.scale_item, menu, concmd.atexit_log);   // tail call
        out.scale_item_views_inert =
            release_inert_view_f590(text_c) && release_inert_view_f590(label);              // 0xb0ca77/7c
        out.scale_item_atexit_recorded =
            concmd.atexit_log.record(kDebugMenuItemDtorRva, &objects.scale_item);           // 0xb0ca94
    }

    // ---- D: debug-menu item for B (0xb0ca99..0xb0cb11) -----------------------------------------
    const SharedStr text_d = borrowed(kSlot299AdjustText);
    const DebugMenuTarget target_b = debug_menu_target_for(objects.adjust);
    out.adjust_item_ctor_ok = init_debug_menu_item_26ea0(objects.adjust_item, label, text_d, target_b,
                                                         kSlot299AdjustItemFlags);
    if (out.adjust_item_ctor_ok) {
        out.adjust_item_registration =
            register_debug_menu_item_268b0(objects.adjust_item, menu, concmd.atexit_log);
        out.adjust_item_views_inert =
            release_inert_view_f590(text_d) && release_inert_view_f590(label);              // 0xb0cafb/b00
        out.adjust_item_atexit_recorded =
            concmd.atexit_log.record(kDebugMenuItemDtorRva, &objects.adjust_item);          // 0xb0cb11
    }

    out.ok = out.scale_ctor_ok && out.scale_functor_destroy_noop && out.scale_registration.ok &&
             out.scale_views_inert && out.scale_atexit_recorded && out.adjust.ok &&
             out.scale_item_ctor_ok && out.scale_item_registration.ok && out.scale_item_views_inert &&
             out.scale_item_atexit_recorded && out.adjust_item_ctor_ok &&
             out.adjust_item_registration.ok && out.adjust_item_views_inert &&
             out.adjust_item_atexit_recorded;
    return out;
}

}  // namespace sotc
