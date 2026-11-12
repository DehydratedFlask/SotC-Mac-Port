// CPU-01-AOT-CFG-SCALE-01-BATCH27 (slot 462): see ctor_slot_462_3e2730.hpp.
#include "ctor_slot_462_3e2730.hpp"

#include <cstring>

#include "ctor_slot_family_items.hpp"
#include "fmenu_ctors_slot306.hpp"

namespace sotc {
namespace {

bool init_command(Slot462Command& cmd, const char* name, std::size_t name_length,
                  const char* description, std::size_t description_length, std::uint32_t flags,
                  std::uint64_t callback, std::uint64_t dtor, ConCmdRegistry& registry) noexcept {
    const SharedStr name_view = family_view(name, name_length);
    const SharedStr description_view = family_view(description, description_length);
    init_concmd_base_20af0(cmd.base, name_view, description_view, flags);
    cmd.base.vptr = kSlot462CommandVptr;
    cmd.tail0 = reinterpret_cast<std::uint64_t>(&cmd.tail0);
    cmd.tail8 = kSlot462CommandTail8;
    cmd.tail10 = kSlot462CommandTail10;
    cmd.tail18 = callback;
    std::memset(cmd.zero80, 0, sizeof cmd.zero80);
    if (!register_concmd_20d60(cmd.base, registry).ok) return false;
    if (!release_inert_view_f590(description_view)) return false;
    if (!release_inert_view_f590(name_view)) return false;
    return registry.atexit_log.record(dtor, &cmd);
}

bool init_show_description(Slot462Objects& objects, ConCmdRegistry& registry) noexcept {
    ConCmdBoolVar& var = objects.show_description;
    const SharedStr name = family_view(kSlot462ShowDescriptionName, kSlot462ShowDescriptionNameLength);
    const SharedStr description = family_view("", 0);
    var.base.vptr = kSlot462BoolFinalVptr;   // pre-constructor store at 0x3e2a50
    init_concmd_base_20af0(var.base, name, description, kSlot462VarFlags);
    var.base.vptr = kSlot462BoolMidVptr;
    var.value_ref = &var.value;
    std::memset(var.zero69, 0, sizeof var.zero69);
    std::memset(var.zero70, 0, sizeof var.zero70);
    var.fn_payload = 0;
    var.fn_vptr = kSlot462BoolFnTag;
    var.fn_aux = 0;
    if (!release_inert_view_f590(description)) return false;
    var.base.vptr = kSlot462BoolFinalVptr;   // 0x3e2ac1
    var.value = 0;
    var.default_value = 0;
    if (!register_concmd_20d60(var.base, registry).ok) return false;
    if (!release_inert_view_f590(name)) return false;
    return registry.atexit_log.record(kSlot462BoolDtor, &var);
}

bool init_menu_item(DebugMenuItem& item, const SharedStr& parent, const SharedStr& text,
                    const DebugMenuTarget& target, std::uint32_t flags, ConCmdRegistry& registry,
                    DebugMenuRegistry& menu, std::uint64_t dtor) noexcept {
    if (!init_debug_menu_item_26ea0(item, parent, text, target, flags)) return false;
    if (!register_menu_item_268b0(item, menu, registry.atexit_log).ok) return false;
    if (!release_inert_view_f590(text)) return false;
    return registry.atexit_log.record(dtor, &item);
}

}  // namespace

Slot462Result init_slot462_3e2730(Slot462Objects& objects, ConCmdRegistry& registry,
                                  DebugMenuRegistry& menu, const Slot462Hooks& hooks) noexcept {
    Slot462Result out{};

    // ---- 1. 0x2ef7420 zero cell (0x3e2772..0x3e2779) ------------------------------------------
    objects.zero_cell = 0;
    if (!registry.atexit_log.record(kSlot462ZeroCellDtor, &objects.zero_cell)) return out;
    ++out.atexit_records;

    // ---- 2/3. the two static views (0x3e2785..0x3e27ee) ---------------------------------------
    objects.presentation = family_view(kSlot462PresentationText, kSlot462PresentationLength);
    if (!registry.atexit_log.record(kSlot462ViewDtor, &objects.presentation)) return out;
    ++out.atexit_records;
    objects.clear_active = family_view(kSlot462ClearActiveText, kSlot462ClearActiveLength);
    if (!registry.atexit_log.record(kSlot462ViewDtor, &objects.clear_active)) return out;
    ++out.atexit_records;
    out.static_views = 2;

    // ---- 4. activatePOI (0x3e27f3..0x3e28ec) --------------------------------------------------
    if (!init_command(objects.activate, kSlot462ActivateName, kSlot462ActivateNameLength,
                      kSlot462ActivateDescription, kSlot462ActivateDescriptionLength,
                      kSlot462CommandFlags, kSlot462ActivateCallback, kSlot462CommandDtor, registry)) {
        return out;
    }
    ++out.registered_commands;
    ++out.atexit_records;

    // ---- 5. clearPOI (0x3e28f1..0x3e29db) -----------------------------------------------------
    if (!init_command(objects.clear, kSlot462ClearName, kSlot462ClearNameLength,
                      kSlot462ClearDescription, kSlot462ClearDescriptionLength, kSlot462CommandFlags,
                      kSlot462ClearCallback, kSlot462CommandDtor, registry)) {
        return out;
    }
    ++out.registered_commands;
    ++out.atexit_records;

    // ---- 6. showPoiDescription bool variable (0x3e29e0..0x3e2af3) ----------------------------
    if (!init_show_description(objects, registry)) return out;
    ++out.registered_commands;
    ++out.atexit_records;

    // ---- 7. renderErrorMsgs holder (0x3e2af8..0x3e2b84) --------------------------------------
    objects.render_errors.vptr = kSlot462StringVptr;
    objects.render_errors.view =
        family_view(kSlot462RenderErrorsText, kSlot462RenderErrorsLength);
    objects.render_errors.zero28 = 0;
    if (!registry.atexit_log.record(kSlot462StringDtor, &objects.render_errors)) return out;
    ++out.atexit_records;

    // ---- 8. presentationShowRenderTarget via 0x6af60 (0x3e2b89..0x3e2c7d) --------------------
    {
        const SharedStr name = family_view(kSlot462ShowTargetName, kSlot462ShowTargetNameLength);
        const SharedStr description = family_view("", 0);
        ConCmdFunctorView view{};
        view.vtable = kSlot462FunctorViewVtable;
        const ConCmdFunctorSpec spec{reinterpret_cast<const void*>(kSlot462FunctorSpec), 0};
        const ConCmdFunctorResult built =
            init_concmd_functor_6af60(objects.show_render_target, name, description,
                                      kSlot462VarFlags, kSlot462FunctorLow, kSlot462FunctorHigh,
                                      spec, view, registry, hooks.functor);
        out.functor_transfers = built.transfers;
        if (!built.ok) return out;
        if (!release_inert_view_f590(description)) return out;
        if (hooks.destroy_view != nullptr) {
            if (!hooks.destroy_view(hooks.view_context, objects.show_render_target)) return out;
            ++out.view_destructions;
        }
        if (!release_inert_view_f590(name)) return out;
        if (!registry.atexit_log.record(kSlot462FunctorDtor, &objects.show_render_target)) return out;
        ++out.registered_commands;
        ++out.atexit_records;
    }

    // ---- 9. poiClearErrorMessages (0x3e2c82..0x3e2d6e) ---------------------------------------
    if (!init_command(objects.clear_errors, kSlot462ClearErrorsName,
                      kSlot462ClearErrorsNameLength, kSlot462ClearErrorsText,
                      kSlot462ClearErrorsTextLength, kSlot462CommandFlags,
                      kSlot462ClearErrorsCallback, kSlot462CommandDtor, registry)) {
        return out;
    }
    ++out.registered_commands;
    ++out.atexit_records;

    // ---- 10. Show Description toggle (0x3e2d73..0x3e2de1) ------------------------------------
    {
        const SharedStr label =
            family_view(kSlot462ItemShowDescriptionText, kSlot462ItemShowDescriptionLength);
        const DebugMenuTarget target = debug_menu_target_for(objects.show_description);
        if (!init_menu_item(objects.item_show_description, objects.presentation, label, target,
                            kSlot462MenuFlags, registry, menu, kSlot462MenuItemDtor)) {
            return out;
        }
        ++out.menu_items;
        ++out.atexit_records;
    }

    // ---- 11. Clear Error Messages command item (0x3e2de6..0x3e2e7b) --------------------------
    {
        const SharedStr command =
            family_view(kSlot462ClearErrorsText, kSlot462ClearErrorsTextLength);
        const SharedStr caption =
            family_view(kSlot462ClearErrorsName, kSlot462ClearErrorsNameLength);
        if (!init_menu_command_26f90(objects.item_clear_errors, objects.presentation, command,
                                     reinterpret_cast<const void*>(kSlot462TargetGlobal), caption,
                                     kSlot462MenuFlags)) {
            return out;
        }
        if (!register_menu_item_268b0(objects.item_clear_errors, menu, registry.atexit_log).ok) {
            return out;
        }
        if (!release_inert_view_f590(caption)) return out;
        if (!release_inert_view_f590(command)) return out;
        if (!registry.atexit_log.record(kSlot462MenuItemDtor, &objects.item_clear_errors)) {
            return out;
        }
        ++out.menu_items;
        ++out.atexit_records;
    }

    // ---- 12. Show Render Target toggle (0x3e2e80..0x3e2ee3) ----------------------------------
    {
        const SharedStr label =
            family_view(kSlot462ItemShowTargetText, kSlot462ItemShowTargetLength);
        // 0x1596980 +0x18 = 0x5c6b0 and +0x30 = 0x5c830, both `mov eax,1; ret`.
        const DebugMenuTarget target{&objects.show_render_target, 1, 1, true};
        if (!init_menu_item(objects.item_show_target, objects.presentation, label, target,
                            kSlot462MenuFlags, registry, menu, kSlot462MenuItemDtor)) {
            return out;
        }
        ++out.menu_items;
        ++out.atexit_records;
    }

    // ---- 13. Shadow Boss Jump group (0x3e2ee8..0x3e2f43) -------------------------------------
    {
        const SharedStr parent = family_view(kSlot462PresentationText, kSlot462PresentationLength);
        const SharedStr caption =
            family_view(kSlot462ItemShadowJumpText, kSlot462ItemShadowJumpLength);
        if (!init_menu_group_26c10(objects.item_shadow_jump, parent, caption, kSlot462MenuFlags)) {
            return out;
        }
        if (!register_menu_item_268b0(objects.item_shadow_jump, menu, registry.atexit_log).ok) {
            return out;
        }
        if (!release_inert_view_f590(caption)) return out;
        if (!registry.atexit_log.record(kSlot462MenuItemDtor, &objects.item_shadow_jump)) {
            return out;
        }
        ++out.menu_items;
        ++out.atexit_records;
    }

    // ---- 14. poiGroup (0x3e2f48..0x3e3034) ----------------------------------------------------
    if (!init_command(objects.poi_group, kSlot462PoiGroupName, kSlot462PoiGroupNameLength,
                      kSlot462PoiGroupDescription, kSlot462PoiGroupDescriptionLength,
                      kSlot462CommandFlags, kSlot462PoiGroupCallback, kSlot462CommandDtor,
                      registry)) {
        return out;
    }
    ++out.registered_commands;
    ++out.atexit_records;

    // ---- 15. poi (0x3e3039..0x3e3116) ---------------------------------------------------------
    if (!init_command(objects.poi, kSlot462PoiName, kSlot462PoiNameLength, kSlot462PoiDescription,
                      kSlot462PoiDescriptionLength, kSlot462CommandFlags, kSlot462PoiCallback,
                      kSlot462CommandDtor, registry)) {
        return out;
    }
    ++out.registered_commands;
    ++out.atexit_records;

    // The canary compare (0x3e311b..0x3e313d) has no native counterpart: the two guard words are
    // the same local in the original, so the compare is satisfied by construction.
    out.canary_matched = true;
    out.ok = out.registered_commands == 7 && out.menu_items == 4 && out.static_views == 2 &&
             out.atexit_records == 15 && out.canary_matched;
    return out;
}

}  // namespace sotc
