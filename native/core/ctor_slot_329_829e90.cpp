// CPU-01-STARTUP-SLOT-329-CHARACTERIZATION-01: see ctor_slot_329_829e90.hpp.
#include "ctor_slot_329_829e90.hpp"

#include <cstdlib>
#include <cstring>

namespace sotc {

namespace {

// Default host allocator hooks (documented stand-in for 0x12820 / 0x12af0; the same convention as
// the registered slot 306 / 316 / 326). The engine allocator body is not recovered and not claimed.
void* host_allocate(std::size_t size, std::uint32_t, std::uint32_t, std::uint32_t) {
    return std::malloc(size);
}
bool host_deallocate(void* ptr, std::uint32_t) {
    std::free(ptr);
    return true;
}

// The borrowed stack description view nine of the ten variables build: {empty rodata, 0, 0, 0}.
SharedStr empty_description_view() noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(kSlot329EmptyText), 0, 0, nullptr};
}

// The argument block of the three range sites: four zero qwords, payload 0, aux 0, and the site's
// own functor tag.
RangedVarArgs range_args(std::uint64_t tag) noexcept {
    RangedVarArgs args{};
    args.fn_vptr = tag;
    return args;
}

}  // namespace

const Slot329RangedClass* slot329_ranged_class_for(std::uint64_t fn_vtbl) noexcept {
    for (const Slot329RangedClass& cls : kSlot329RangedClasses) {
        if (cls.fn_vtbl == fn_vtbl) return &cls;
    }
    return nullptr;
}

const char* slot329_stage_name(Slot329Stage stage) noexcept {
    switch (stage) {
        case Slot329Stage::none: return "none";
        case Slot329Stage::bool_var_build: return "bool_var_build";
        case Slot329Stage::bool_var_registration: return "bool_var_registration";
        case Slot329Stage::bool_var_view_release: return "bool_var_view_release";
        case Slot329Stage::bool_var_atexit: return "bool_var_atexit";
        case Slot329Stage::ranged_var_build: return "ranged_var_build";
        case Slot329Stage::ranged_var_registration: return "ranged_var_registration";
        case Slot329Stage::ranged_var_view_release: return "ranged_var_view_release";
        case Slot329Stage::ranged_var_atexit: return "ranged_var_atexit";
        case Slot329Stage::item_ctor: return "item_ctor";
        case Slot329Stage::item_registration: return "item_registration";
        case Slot329Stage::item_view_release: return "item_view_release";
        case Slot329Stage::item_atexit: return "item_atexit";
        case Slot329Stage::canary: return "canary";
    }
    return "";
}

bool slot329_ranged_var_state(ConCmdRangedVar& var, const Slot329RangedClass& cls,
                              const SharedStr& name, void* value_ref, std::uint32_t min_cell,
                              std::uint32_t max_cell, std::uint32_t flags,
                              const RangedVarArgs& args, const SharedStr& description) noexcept {
    // 0x8298d0 / 0x829cf0 dispatch `[[args.fn_vptr]+8]` and `[[args.fn_vptr]+0x18]`; only the pinned
    // class tag's four slot bodies are lifted, so anything else writes nothing.
    if (args.fn_vptr != cls.fn_vtbl) return false;
    // The wrapper dereferences the value pointer twice after the leaf (`*value_ref` into +0xd0 and
    // +0x68); the original would fault on null, the model refuses before the first store.
    if (value_ref == nullptr) return false;
    if (slot329_ranged_class_for(cls.fn_vtbl) == nullptr) return false;

    // 0x829906 / 0x829d26: call 0x20af0(this, name, description, flags).
    init_concmd_base_20af0(var.base, name, description, flags);
    // 0x82990b: [this] = the row's mid class tag.
    var.base.vptr = cls.mid_vptr;
    // 0x829924..0x829941: value_ref, then both bounds engaged from the caller's cells.
    var.value_ref = value_ref;
    var.min_value[0] = 0;
    var.min_engaged = 1;
    std::memcpy(var.min_value, &min_cell, 4);
    var.max_value[0] = 0;
    var.max_engaged = 1;
    std::memcpy(var.max_value, &max_cell, 4);
    // 0x829944..0x82995e: the argument copy, the class functor tag, payload 0, aux 0.
    std::memset(var.args, 0, sizeof var.args);
    var.fn_vptr = cls.fn_vtbl;
    var.fn_payload = 0;
    var.fn_aux = 0;
    // 0x829969..0x829a08: the clone / copy / copy / aux-swap / bare-ret-destroy dance, collapsed to
    // its net state (the same helper the 0xa4f4d0 family uses).
    install_ranged_fn_from_args(var, args, cls.fn_vtbl);
    return true;
}

bool slot329_float_var_state(ConCmdRangedVar& var, const SharedStr& name, void* value_ref,
                             std::uint32_t min_bits, std::uint32_t max_bits, std::uint32_t flags,
                             const RangedVarArgs& args, const SharedStr& description) noexcept {
    // The site's own tag. `init_ranged_float_var_c300` accepts three equivalent tables; 0x15ceb80 is
    // a fourth whose four slot bodies are the same four bodies (checker-pinned), so the shared model
    // is called with the equivalent tag and the object's own functor tag stays 0x1594318.
    if (args.fn_vptr != kSlot329FloatArgFnVtbl) return false;
    if (value_ref == nullptr) return false;

    RangedVarArgs equivalent = args;
    equivalent.fn_vptr = kRangedFloatArgFnVtbl;
    float min_value = 0.0f;
    float max_value = 0.0f;
    std::memcpy(&min_value, &min_bits, 4);
    std::memcpy(&max_value, &max_bits, 4);
    return init_ranged_float_var_c300(var, name, value_ref, min_value, max_value, flags, equivalent,
                                      description);
}

Slot329Hooks default_slot329_hooks() noexcept {
    Slot329Hooks hooks{};
    hooks.separator_alloc.alloc.allocate = &host_allocate;
    hooks.separator_alloc.alloc.deallocate = &host_deallocate;
    return hooks;
}

DebugMenuTarget slot329_menu_target(const ConCmdBase& cmd) noexcept {
    DebugMenuTarget out{};
    out.object = &cmd;
    if (cmd.vptr == kSlot329BoolFinalVptr) {
        // 0x1594218 slot 3 = 0xb890 `mov eax,1; ret`, slot 6 = 0xb930 `xor eax,eax; ret`.
        out.slot3 = kSlot329BoolGate3;
        out.slot6 = kSlot329BoolGate6;
        out.known = true;
    } else if (cmd.vptr == kSlot329FloatFinalVptr) {
        // 0x15942b0 slot 3 = 0xbba0 `mov eax,1; ret`, slot 6 = 0xbbf0 `mov eax,3; ret`, so a toggle
        // on the float variable narrows its range to {0.1, 0.5}.
        out.slot3 = kSlot329FloatGate3;
        out.slot6 = kSlot329FloatGate6;
        out.known = true;
    } else if (cmd.vptr == kSlot329PathDataFinalVptr) {
        // Class P: slot 3 = 0x8254c0 -> 1, slot 6 = 0x825510 -> 1.
        out.slot3 = kSlot329PathDataGate3;
        out.slot6 = kSlot329PathDataGate6;
        out.known = true;
    } else if (cmd.vptr == kSlot329CharaFinalVptr) {
        // Class C: slot 3 = 0x825f40 -> 1, slot 6 = 0x8261e0 -> 1.
        out.slot3 = kSlot329CharaGate3;
        out.slot6 = kSlot329CharaGate6;
        out.known = true;
    }
    return out;
}

bool slot329_inert_functor_destroy(const Slot329FunctorSite& site) noexcept {
    return site.slot3_is_bare_ret;
}

ConCmdBase& slot329_var_base(Slot329Objects& objects, std::uint32_t index) noexcept {
    static ConCmdBase fallback{};
    switch (index) {
        case 0: return objects.bool_path_finding.base;
        case 1: return objects.bool_draw_enable.base;
        case 2: return objects.ranged_path_data.base;
        case 3: return objects.bool_attain_range.base;
        case 4: return objects.bool_enable_editing.base;
        case 5: return objects.ranged_chara_type.base;
        case 6: return objects.float_translation.base;
        case 7: return objects.bool_all_names.base;
        case 8: return objects.bool_raycasts_enable.base;
        case 9: return objects.bool_raycasts_debug.base;
        default: return fallback;
    }
}

DebugMenuItem& slot329_item(Slot329Objects& objects, std::uint32_t index) noexcept {
    static DebugMenuItem fallback{};
    if (index == 0) return objects.group;
    if (index <= kSlot329ItemCount - 1) return objects.items[index - 1];
    return fallback;
}

Slot329Result init_slot329_829e90(Slot329Objects& o, ConCmdRegistry& concmd, DebugMenuRegistry& menu,
                                  const Slot329Hooks& hooks) noexcept {
    Slot329Result out{};
    l2::StageFail<Slot329Result, Slot329Stage> fail{out};

    // 0x829ea8..0x829ee8: guard <- *[*[0x15fe518]], stored at [rsp+0xc0]. The only branch that reads
    // it is the epilogue compare at 0x82ae6b; a mismatch calls __stack_chk_fail at 0x82ae7c.
    const std::uint64_t guard_entry = hooks.stack_guard != nullptr ? *hooks.stack_guard : 0;

    const auto build_bool = [&](std::uint32_t index) -> bool {
        const Slot329VarSpec& spec = kSlot329Vars[index];
        out.blocked_index = index;
        ConCmdBoolVar& var = index == 0   ? o.bool_path_finding
                             : index == 1 ? o.bool_draw_enable
                             : index == 3 ? o.bool_attain_range
                             : index == 4 ? o.bool_enable_editing
                             : index == 7 ? o.bool_all_names
                             : index == 8 ? o.bool_raycasts_enable
                                          : o.bool_raycasts_debug;
        // The shared bool-variable model: 0x20af0 with flags 1, class 0x1594218, the site's argument
        // tag 0x15cea30, value/default from the row, its two releases, 0x20d60 and the
        // __cxa_atexit(0x20b0) record. It reports which stage failed, so map it back to the enum.
        const BoolVarResult built =
            init_bool_var_static(var, l2::measured_view(spec.name, spec.name_len),
                                 empty_description_view(), spec.fn_vtbl, concmd,
                                 static_cast<std::uint8_t>(spec.value),
                                 static_cast<std::uint8_t>(spec.default_value));
        out.vars[index].description_inert = true;   // the shared model releases it internally
        out.vars[index].name_inert = built.views_inert;
        out.vars[index].functor_inert = true;
        out.vars[index].registration = built.registration;
        out.vars[index].atexit_recorded = built.atexit_recorded;
        out.views_inert = out.views_inert && built.views_inert;
        if (!built.registration.ok) return (void)fail.at(Slot329Stage::bool_var_registration), false;
        if (!built.views_inert) return (void)fail.at(Slot329Stage::bool_var_view_release), false;
        if (!built.atexit_recorded) return (void)fail.at(Slot329Stage::bool_var_atexit), false;
        ++out.atexit_records;
        ++out.vars_built;
        out.vars[index].ok = true;
        return true;
    };

    const auto build_range = [&](std::uint32_t index) -> bool {
        const Slot329VarSpec& spec = kSlot329Vars[index];
        out.blocked_index = index;
        const bool is_float = spec.kind == Slot329VarKind::ranged_float;
        const Slot329RangedClass* const cls = is_float ? nullptr : slot329_ranged_class_for(spec.fn_vtbl);
        if (!is_float && cls == nullptr) return (void)fail.at(Slot329Stage::ranged_var_build), false;
        const bool wrapped = !is_float && cls->wrapped;
        // Leaf A / leaf B dispatch five slots of the caller's functor; wrapper B dispatches six more
        // (the local argument copy plus the dance); 0xc300 dispatches five. The site itself destroys
        // the stack argument block once, which is the body's own `callq *0x18(%rax)`.
        const std::uint32_t internal_dispatches = is_float ? 5 : cls->functor_dispatches;
        ConCmdRangedVar& var = index == 2   ? o.ranged_path_data
                               : index == 5 ? o.ranged_chara_type
                                            : o.float_translation;
        const SharedStr name = l2::measured_view(spec.name, spec.name_len);
        const SharedStr description =
            spec.description == nullptr ? empty_description_view()
                                        : l2::measured_view(spec.description, spec.description_len);

        bool built = false;
        if (is_float) {
            built = slot329_float_var_state(var, name, &var.value, spec.min_cell, spec.max_cell,
                                            kSlot329VarFlags, range_args(spec.fn_vtbl), description);
        } else {
            built = slot329_ranged_var_state(var, *cls, name, &var.value, spec.min_cell,
                                             spec.max_cell, kSlot329VarFlags,
                                             range_args(spec.fn_vtbl), description);
        }
        if (!built) return (void)fail.at(Slot329Stage::ranged_var_build), false;

        // The wrapper's tail (0x829c9d..0x829cb7) and the site's own tail (0x82a189..0x82a1a0 /
        // 0x82a566..0x82a57f): final vptr, *value_ref twice, then the register-once guard.
        const auto finish = [&]() -> bool {
            var.base.vptr = spec.final_vptr;
            std::memcpy(&var.value, &spec.value, 4);
            var.default_value = spec.default_value;
            out.vars[index].registration = register_concmd_20d60(var.base, concmd);
            return out.vars[index].registration.ok;
        };

        // Class C is the one site that CALLS its wrapper: 0x829b30 finishes and registers before it
        // returns, so the site's two releases come after the registration. Class P and 0xc300 return
        // before the site's own finish/register, so the description release order differs by one
        // step. Both orders are reproduced.
        if (wrapped) {
            if (!finish()) return (void)fail.at(Slot329Stage::ranged_var_registration), false;
        }

        // The site releases the stack description view, then dispatches the argument functor's slot 3
        // (a bare ret), then -- for the unwrapped shapes -- restores the final vptr and registers.
        const bool desc_inert = release_inert_view_f590(description);
        out.vars[index].description_inert = desc_inert;
        out.views_inert = out.views_inert && desc_inert;
        if (!desc_inert) return (void)fail.at(Slot329Stage::ranged_var_view_release), false;

        const Slot329FunctorSite& site = index == 2   ? kSlot329FunctorSites[0]
                                          : index == 5 ? kSlot329FunctorSites[1]
                                                       : kSlot329FunctorSites[2];
        out.vars[index].functor_inert = slot329_inert_functor_destroy(site);
        if (!out.vars[index].functor_inert) return (void)fail.at(Slot329Stage::ranged_var_build), false;
        ++out.functors_destroyed;
        out.vars[index].functor_dispatches = internal_dispatches + 1;
        out.functor_dispatches += out.vars[index].functor_dispatches;

        if (!wrapped) {
            if (!finish()) return (void)fail.at(Slot329Stage::ranged_var_registration), false;
        }

        const bool name_inert = release_inert_view_f590(name);
        out.vars[index].name_inert = name_inert;
        out.views_inert = out.views_inert && name_inert;
        if (!name_inert) return (void)fail.at(Slot329Stage::ranged_var_view_release), false;

        const void* const object = index == 2   ? static_cast<const void*>(&o.ranged_path_data)
                                   : index == 5 ? static_cast<const void*>(&o.ranged_chara_type)
                                                : static_cast<const void*>(&o.float_translation);
        if (!concmd.atexit_log.record(spec.dtor_rva, object)) {
            return (void)fail.at(Slot329Stage::ranged_var_atexit), false;
        }
        ++out.atexit_records;
        out.vars[index].atexit_recorded = true;
        ++out.vars_built;
        out.vars[index].ok = true;
        return true;
    };

    const auto build_item = [&](std::uint32_t index) -> bool {
        const Slot329ItemSpec& spec = kSlot329Items[index];
        out.blocked_index = index;
        DebugMenuItem& item = slot329_item(o, index);
        const SharedStr label = l2::measured_view(spec.label, spec.label_len);
        const SharedStr text =
            spec.text == nullptr ? SharedStr{} : l2::measured_view(spec.text, spec.text_len);
        bool built = false;
        switch (spec.kind) {
            case Slot329ItemKind::group:
                // 0x26c10(item, parent, caption, flags), then its tail call into 0x268b0.
                built = init_menu_group_26c10(item, label, text, spec.flags);
                break;
            case Slot329ItemKind::toggle: {
                // 0x26ea0 reads two virtual slots of the target; an unpinned class would run
                // unlifted code.
                ConCmdBase& target = slot329_var_base(o, spec.target_index);
                const DebugMenuTarget gate = slot329_menu_target(target);
                if (!gate.known) return (void)fail.at(Slot329Stage::item_ctor), false;
                built = init_debug_menu_item_26ea0(item, label, text, gate, spec.flags);
                break;
            }
            case Slot329ItemKind::separator:
                // 0x26b60(item, parent, 0, flags): builds "MenuSeparator" through the native 0xf1a0,
                // which allocates. Without the allocator contract the model refuses before the first
                // store (the original would have faulted).
                built = init_menu_separator_26b60(item, label, spec.flags, hooks.separator_alloc);
                break;
        }
        if (!built) return (void)fail.at(Slot329Stage::item_ctor), false;
        // The ctor's tail call into 0x268b0.
        out.item_registrations[index] = register_menu_item_268b0(item, menu, concmd.atexit_log);
        if (!out.item_registrations[index].ok) {
            return (void)fail.at(Slot329Stage::item_registration), false;
        }
        ++out.menu_items;
        // The site releases the caption view first and the parent/label view second.
        const bool text_inert = spec.text == nullptr ? true : release_inert_view_f590(text);
        const bool label_inert = release_inert_view_f590(label);
        out.views_inert = out.views_inert && text_inert && label_inert;
        if (!text_inert || !label_inert) return (void)fail.at(Slot329Stage::item_view_release), false;
        if (!record_menu_item_dtor_272d0(concmd.atexit_log, item)) {
            return (void)fail.at(Slot329Stage::item_atexit), false;
        }
        ++out.atexit_records;
        return true;
    };

    // ---- body order: bool, bool, range P, bool, bool, range C, float, bool, bool, bool, items ----
    if (!build_bool(0)) return out;
    if (!build_bool(1)) return out;
    if (!build_range(2)) return out;
    if (!build_bool(3)) return out;
    if (!build_bool(4)) return out;
    if (!build_range(5)) return out;
    if (!build_range(6)) return out;
    if (!build_bool(7)) return out;
    if (!build_bool(8)) return out;
    if (!build_bool(9)) return out;
    for (std::uint32_t index = 0; index < kSlot329ItemCount; ++index) {
        if (!build_item(index)) return out;
    }

    // 0x82ae59..0x82ae7b: canary compare and epilogue. A changed guard takes `jne 0x82ae7c`.
    if (hooks.stack_guard != nullptr) {
        out.canary_checked = true;
        out.canary_matched = *hooks.stack_guard == guard_entry;
        if (!out.canary_matched) return (void)fail.at(Slot329Stage::canary), out;
    }

    out.blocked_index = 0;
    out.ok = true;
    return out;
}

}  // namespace sotc
