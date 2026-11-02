// CPU-01-STARTUP-SLOT-326-CHARACTERIZATION-01: see ctor_slot_326_837f30.hpp.
#include "ctor_slot_326_837f30.hpp"

#include <cstdlib>
#include <cstring>

#include "fmenu_ctors_slot306.hpp"
#include "init_sub_3be930.hpp"

namespace sotc {

namespace {

// Default host allocator hooks (documented stand-in for 0x12820 / 0x12af0; the same convention as
// the registered slot 306 and slot 316). The engine allocator body is not recovered and not claimed.
void* host_allocate(std::size_t size, std::uint32_t, std::uint32_t, std::uint32_t) {
    return std::malloc(size);
}
bool host_deallocate(void* ptr, std::uint32_t) {
    std::free(ptr);
    return true;
}

// The borrowed stack description view every variable of this body builds: {empty rodata, 0, 0, 0}.
SharedStr empty_description_view() noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(kSlot326EmptyText), 0, 0, nullptr};
}

// The argument block of the thirteen 0xc300 sites: four zero qwords, payload 0, tag 0x15cec00, aux 0.
RangedVarArgs float_args() noexcept {
    RangedVarArgs args{};
    args.fn_vptr = kSlot326FloatArgFnVtbl;
    return args;
}

// The collapsed functor install of 0xc300's tail (the clone / copy / copy / copy / aux-swap /
// bare-ret-destroy chain). `vtbl` is the class tag the install stores.
template <class Var>
void install_float_fn_from_args(Var& var, const RangedVarArgs& args, std::uint64_t vtbl) noexcept {
    var.args[1] = args.q[1];                // vmovups [this+0x88] <- [r8+8]
    var.args[2] = args.q[2];
    var.args[0] = args.q[0];                // [this+0x80] <- [r8]
    var.args[3] = args.q[3];                // [this+0x98] <- [r8+0x18]
    if (args.q[3] != 0) {
        var.args[0] = reinterpret_cast<std::uint64_t>(&var.args[0]);   // [this+0x80] = this+0x80
    }
    var.fn_payload = args.fn_payload;       // [this+0xb0]
    var.fn_vptr = vtbl;                     // [this+0xc0]
    var.fn_aux = args.fn_aux;               // [this+0xc8]
}

// The net state of 0xc300 on either float shape. Refuses (writes nothing) when the caller's argument
// tag is not the pinned 0x15cec00 -- the original would dispatch that table's slot bodies, which are
// only lifted for the pinned tag.
template <class Var>
bool float_c300_state(Var& var, const Slot326FloatSpec& spec, const RangedVarArgs& args,
                      void* value_ref) noexcept {
    if (args.fn_vptr != kSlot326FloatArgFnVtbl) return false;

    // The site stores the class vptr into the object BEFORE the call; 0x20af0 overwrites it, so this
    // store is invisible in the net state and is reproduced only for byte fidelity.
    var.base.vptr = spec.final_vptr;
    // 0xc336: call 0x20af0(this, name, description, flags = 1).
    init_concmd_base_20af0(var.base, l2::measured_view(spec.name, spec.name_len),
                           empty_description_view(), kSlot326VarFlags);
    // 0xc33b..0xc38e: mid vptr, the caller's value cell, both bounds engaged.
    var.base.vptr = kRangedFloatMidVptr;      // [this] = 0x1594128
    var.value_ref = value_ref;                // [this+0x60]
    var.min_value[0] = 0;                     // byte [this+0x6c] = 0, then the dword
    var.min_engaged = 1;                      // byte [this+0x70] = 1
    std::memcpy(var.min_value, &spec.min_bits, 4);
    var.max_value[0] = 0;                     // byte [this+0x74] = 0, then the dword
    var.max_engaged = 1;                      // byte [this+0x78] = 1
    std::memcpy(var.max_value, &spec.max_bits, 4);
    // 0xc374..0xc38e: the zeroed argument copy and the seed functor tag.
    std::memset(var.args, 0, sizeof var.args);
    var.fn_vptr = kRangedFloatFnVtbl;         // [this+0xc0] = 0x1594318
    var.fn_payload = 0;                       // [this+0xb0]
    var.fn_aux = 0;                           // [this+0xc8]
    // 0xc399..0xc438: the four argument dispatches collapse to this net state.
    install_float_fn_from_args(var, args, kRangedFloatFnVtbl);
    return true;
}

}  // namespace

const char* slot326_stage_name(Slot326Stage stage) noexcept {
    switch (stage) {
        case Slot326Stage::none: return "none";
        case Slot326Stage::bool_var_build: return "bool_var_build";
        case Slot326Stage::bool_var_atexit: return "bool_var_atexit";
        case Slot326Stage::float_var_build: return "float_var_build";
        case Slot326Stage::float_var_view_release: return "float_var_view_release";
        case Slot326Stage::float_var_registration: return "float_var_registration";
        case Slot326Stage::float_var_atexit: return "float_var_atexit";
        case Slot326Stage::item_ctor: return "item_ctor";
        case Slot326Stage::item_registration: return "item_registration";
        case Slot326Stage::item_view_release: return "item_view_release";
        case Slot326Stage::item_atexit: return "item_atexit";
        case Slot326Stage::canary: return "canary";
    }
    return "";
}

bool slot326_float_c300_state(ConCmdRangedVar& var, const Slot326FloatSpec& spec,
                              const RangedVarArgs& args, Slot326EyeValues&) noexcept {
    // The 0xd8 shape is the threshold's: its own +0xd0 dword is the caller's value cell.
    if (spec.eye_colour || !spec.writes_value_cell) return false;
    return float_c300_state(var, spec, args, &var.value);
}

bool slot326_float_c300_state(Slot326EyeVar& var, const Slot326FloatSpec& spec,
                              const RangedVarArgs& args, Slot326EyeValues& values) noexcept {
    // The 0xd0 shape is the eye colours': their value cell is a float of the mirrored 0x2dce400
    // table, and the object's own +0xd0 belongs to the next object.
    if (!spec.eye_colour || spec.writes_value_cell || spec.eye_row > 2 || spec.eye_channel > 3) {
        return false;
    }
    std::uint32_t* const row = spec.eye_row == 0   ? values.inactive
                               : spec.eye_row == 1 ? values.blue
                                                   : values.red;
    return float_c300_state(var, spec, args, row + spec.eye_channel);
}

bool init_slot326_label_item_26800(DebugMenuItem& item, const SharedStr& label, std::uint32_t flags,
                                   const SharedStrCtorHooks& hooks) noexcept {
    // 0x26855..0x2685c: the "MenuSeparator" text is built through 0xf1a0, which allocates. Refuse
    // BEFORE the first store when that cannot work (the original would have faulted).
    SharedStr text{};
    if (!init_shared_str_f1a0(text, kSlot326SeparatorText, hooks)) return false;

    // 0x2680d..0x26824: the intrusive link and the registered byte.
    item.link.self = nullptr;
    item.link.prev = &item.link;
    item.link.next = &item.link;
    item.link.owner = nullptr;
    item.registered = 0;
    // 0x26828..0x26847: the caller's view copied to +0x28 (16 B), +0x38/+0x40 and its counter.
    item.label = label;
    concmd_acquire_view(label);
    // 0x26855: kind = 3 (the separator kind) ...
    item.kind = kSlot326LabelTransientKind;
    // 0x26861..0x26865: text built above lands at +0x50; 0xf180 initialises the empty view at +0x70.
    item.text = text;
    init_slot_f180(&item.empty);
    // 0x26871..0x26890: target = 0, range {1.0, 10.0}, the caller's flags.
    item.target = nullptr;
    item.range_min = kSlot326LabelRangeMin;
    item.range_max = kSlot326LabelRangeMax;
    item.flags = flags;
    // 0x26897: ... and then kind is overwritten with 0. The net kind of this row is 0; the transient
    // 3 is reproduced because the original really stores both.
    item.kind = kSlot326LabelKind;
    return true;
}

Slot326Hooks default_slot326_hooks() noexcept {
    Slot326Hooks hooks{};
    hooks.string_alloc.alloc.allocate = &host_allocate;
    hooks.string_alloc.alloc.deallocate = &host_deallocate;
    return hooks;
}

DebugMenuTarget slot326_menu_target(const ConCmdBase& cmd) noexcept {
    DebugMenuTarget out{};
    out.object = &cmd;
    if (cmd.vptr == kSlot326BoolFinalVptr) {
        // 0x1594218 slot 3 = 0xb890 `mov eax,1; ret`, slot 6 = 0xb930 `xor eax,eax; ret`.
        out.slot3 = kSlot326BoolGate3;
        out.slot6 = kSlot326BoolGate6;
        out.known = true;
    } else if (cmd.vptr == kSlot326FloatFinalVptr) {
        // 0x15942b0 slot 3 = 0xbba0 `mov eax,1; ret`, slot 6 = 0xbbf0 `mov eax,3; ret`, so a toggle
        // on a float variable narrows its range to {0.1, 0.5}.
        out.slot3 = kSlot326FloatGate3;
        out.slot6 = kSlot326FloatGate6;
        out.known = true;
    }
    return out;
}

bool slot326_inert_functor_destroy(const Slot326FunctorSite& site) noexcept {
    return site.slot3_is_bare_ret;
}

ConCmdBase& slot326_bool_base(Slot326Objects& objects, std::uint32_t index) noexcept {
    static ConCmdBase fallback{};
    switch (index) {
        case 0: return objects.bool_eye_state.base;
        case 1: return objects.bool_force_angry.base;
        default: return fallback;
    }
}

ConCmdBase& slot326_float_base(Slot326Objects& objects, std::uint32_t index) noexcept {
    static ConCmdBase fallback{};
    if (index == 0) return objects.threshold.base;
    if (index <= kSlot326EyeCount) return objects.eye[index - 1].base;
    return fallback;
}

DebugMenuItem& slot326_item(Slot326Objects& objects, std::uint32_t index) noexcept {
    static DebugMenuItem fallback{};
    switch (index) {
        case 0: return objects.group_colossus;
        case 1: return objects.toggle_force_angry;
        case 2: return objects.toggle_eye_state;
        case 3: return objects.toggle_threshold;
        case 4: return objects.group_eye_color;
        case 5: return objects.item_label;
        case 6: return objects.group_inactive;
        case 7: return objects.slider_inactive[0];
        case 8: return objects.slider_inactive[1];
        case 9: return objects.slider_inactive[2];
        case 10: return objects.slider_inactive[3];
        case 11: return objects.group_blue;
        case 12: return objects.slider_blue[0];
        case 13: return objects.slider_blue[1];
        case 14: return objects.slider_blue[2];
        case 15: return objects.slider_blue[3];
        case 16: return objects.group_red;
        case 17: return objects.slider_red[0];
        case 18: return objects.slider_red[1];
        case 19: return objects.slider_red[2];
        case 20: return objects.slider_red[3];
        default: return fallback;
    }
}

std::uint32_t* slot326_value_cell(Slot326EyeValues& values, const Slot326FloatSpec& spec) noexcept {
    if (!spec.eye_colour) return nullptr;   // the threshold's cell is its own +0xd0
    if (spec.eye_row > 2 || spec.eye_channel > 3) return nullptr;
    std::uint32_t* const row = spec.eye_row == 0   ? values.inactive
                               : spec.eye_row == 1 ? values.blue
                                                   : values.red;
    return row + spec.eye_channel;
}

Slot326Result init_slot326_837f30(Slot326Objects& o, Slot326EyeValues& eye_values,
                                  ConCmdRegistry& concmd, DebugMenuRegistry& menu,
                                  const Slot326Hooks& hooks) noexcept {
    Slot326Result out{};
    l2::StageFail<Slot326Result, Slot326Stage> fail{out};

    // 0x837f48..0x837f88: guard <- *[*[0x15fe518]], stored at [rsp+0xc0]. The only branch that reads
    // it is the epilogue compare at 0x839aa8; a mismatch calls __stack_chk_fail at 0x839ab9.
    const std::uint64_t guard_entry = hooks.stack_guard != nullptr ? *hooks.stack_guard : 0;

    // 0x2dce400 is read (never written) by this body: the twelve eye-colour value cells. Mirror it.
    eye_values = kSlot326EyeValueBits;
    out.eye_values_mirrored = true;

    const auto build_bool = [&](std::uint32_t index) -> bool {
        const Slot326BoolSpec& spec = kSlot326Bools[index];
        out.blocked_index = index;
        ConCmdBoolVar& var = index == 0 ? o.bool_eye_state : o.bool_force_angry;
        // 0x837fb5..0x838062 / 0x83808f..0x83815c: the shared bool-variable model (0x20af0 with
        // flags 1, class 0x1594218, argument tag 0x15cebe0, value 0 / default 0, its two releases,
        // 0x20d60 and the __cxa_atexit record of 0x20b0).
        out.bool_vars[index] = init_bool_var_static(
            var, l2::measured_view(spec.name, spec.name_len), empty_description_view(),
            kSlot326BoolArgFnVtbl, concmd, spec.value, spec.default_value);
        out.views_inert = out.views_inert && out.bool_vars[index].views_inert;
        if (!out.bool_vars[index].ok) return (void)fail.at(Slot326Stage::bool_var_build), false;
        if (out.bool_vars[index].atexit_recorded) ++out.atexit_records;
        ++out.vars_built;
        return true;
    };

    const auto build_float = [&](std::uint32_t index) -> bool {
        const Slot326FloatSpec& spec = kSlot326Floats[index];
        out.blocked_index = index;
        const RangedVarArgs args = float_args();
        ConCmdBase* base = nullptr;
        std::uint32_t* value_cell = nullptr;
        if (spec.eye_colour) {
            Slot326EyeVar& var = o.eye[index - 1];
            if (!slot326_float_c300_state(var, spec, args, eye_values)) {
                return (void)fail.at(Slot326Stage::float_var_build), false;
            }
            base = &var.base;
            value_cell = slot326_value_cell(eye_values, spec);
            if (value_cell == nullptr) return (void)fail.at(Slot326Stage::float_var_build), false;
            // 0x838551: the name view, then 0x838564: the argument functor destroy (bare ret).
            const bool name_inert = release_inert_view_f590(
                l2::measured_view(spec.name, spec.name_len));
            const bool functor_inert = slot326_inert_functor_destroy(kSlot326FunctorSites[index]);
            out.float_vars[index].name_inert = name_inert;
            out.float_vars[index].functor_inert = functor_inert;
            out.views_inert = out.views_inert && name_inert;
            if (!name_inert) return (void)fail.at(Slot326Stage::float_var_view_release), false;
            if (!functor_inert) return (void)fail.at(Slot326Stage::float_var_build), false;
            ++out.functors_destroyed;
            // 0x838578: final vptr; 0x83857f: the default read out of the value cell.
            var.base.vptr = spec.final_vptr;
            var.default_value = *value_cell;
        } else {
            if (!slot326_float_c300_state(o.threshold, spec, args, eye_values)) {
                return (void)fail.at(Slot326Stage::float_var_build), false;
            }
            base = &o.threshold.base;
            const bool name_inert = release_inert_view_f590(
                l2::measured_view(spec.name, spec.name_len));
            const bool functor_inert = slot326_inert_functor_destroy(kSlot326FunctorSites[0]);
            out.float_vars[index].name_inert = name_inert;
            out.float_vars[index].functor_inert = functor_inert;
            out.views_inert = out.views_inert && name_inert;
            if (!name_inert) return (void)fail.at(Slot326Stage::float_var_view_release), false;
            if (!functor_inert) return (void)fail.at(Slot326Stage::float_var_build), false;
            ++out.functors_destroyed;
            // 0x838246: final vptr; 0x83824d: value; 0x838257: default -- both 3.0f.
            o.threshold.base.vptr = spec.final_vptr;
            std::memcpy(&o.threshold.value, &spec.value_bits, 4);
            o.threshold.default_value = spec.default_bits;
        }
        // 0x838261 / 0x838585: call 0x20d60.
        out.float_vars[index].registration = register_concmd_20d60(*base, concmd);
        if (!out.float_vars[index].registration.ok) {
            return (void)fail.at(Slot326Stage::float_var_registration), false;
        }
        // 0x838269 / 0x83858f: the stack description view, then the __cxa_atexit record.
        const bool desc_inert = release_inert_view_f590(empty_description_view());
        out.float_vars[index].description_inert = desc_inert;
        out.views_inert = out.views_inert && desc_inert;
        if (!desc_inert) return (void)fail.at(Slot326Stage::float_var_view_release), false;
        if (!concmd.atexit_log.record(spec.dtor_rva, base == &o.threshold.base
                                                          ? static_cast<const void*>(&o.threshold)
                                                          : static_cast<const void*>(&o.eye[index - 1]))) {
            return (void)fail.at(Slot326Stage::float_var_atexit), false;
        }
        ++out.atexit_records;
        out.float_vars[index].ok = true;
        ++out.vars_built;
        return true;
    };

    const auto build_item = [&](std::uint32_t index) -> bool {
        const Slot326ItemSpec& spec = kSlot326Items[index];
        out.blocked_index = index;
        DebugMenuItem& item = slot326_item(o, index);
        const SharedStr label = l2::measured_view(spec.label, spec.label_len);
        const SharedStr text = spec.text == nullptr
                                   ? SharedStr{}
                                   : l2::measured_view(spec.text, spec.text_len);
        bool built = false;
        switch (spec.kind) {
            case Slot326ItemKind::group:
                // 0x26c10(item, parent, caption, flags), then its tail call into 0x268b0.
                built = init_menu_group_26c10(item, label, text, spec.flags);
                break;
            case Slot326ItemKind::toggle: {
                // 0x26ea0 reads two virtual slots of the target; an unpinned class would run
                // unlifted code.
                ConCmdBase& target = spec.target_kind == Slot326TargetKind::bool_var
                                         ? slot326_bool_base(o, spec.target_index)
                                         : slot326_float_base(o, spec.target_index);
                const DebugMenuTarget gate = slot326_menu_target(target);
                if (!gate.known) return (void)fail.at(Slot326Stage::item_ctor), false;
                built = init_debug_menu_item_26ea0(item, label, text, gate, spec.flags);
                break;
            }
            case Slot326ItemKind::slider: {
                ConCmdBase& target = spec.target_kind == Slot326TargetKind::bool_var
                                         ? slot326_bool_base(o, spec.target_index)
                                         : slot326_float_base(o, spec.target_index);
                float lo = 0.0f;
                float hi = 0.0f;
                std::memcpy(&lo, &kSlot326SliderMinBits, 4);
                std::memcpy(&hi, &kSlot326SliderMaxBits, 4);
                built = init_debug_menu_item_27050(item, label, text, &target, spec.flags, lo, hi);
                break;
            }
            case Slot326ItemKind::label:
                // 0x26800: the kind-0 "MenuSeparator" row. Its text is built inside the ctor.
                built = init_slot326_label_item_26800(item, label, spec.flags, hooks.string_alloc);
                break;
        }
        if (!built) return (void)fail.at(Slot326Stage::item_ctor), false;
        // The ctor's tail call into 0x268b0.
        out.item_registrations[index] = register_menu_item_268b0(item, menu, concmd.atexit_log);
        if (!out.item_registrations[index].ok) {
            return (void)fail.at(Slot326Stage::item_registration), false;
        }
        ++out.menu_items;
        // The site releases the caption view first and the parent/label view second.
        const bool text_inert = spec.text == nullptr ? true : release_inert_view_f590(text);
        const bool label_inert = release_inert_view_f590(label);
        out.views_inert = out.views_inert && text_inert && label_inert;
        if (!text_inert || !label_inert) return (void)fail.at(Slot326Stage::item_view_release), false;
        if (!record_menu_item_dtor_272d0(concmd.atexit_log, item)) {
            return (void)fail.at(Slot326Stage::item_atexit), false;
        }
        ++out.atexit_records;
        return true;
    };

    // ---- body order: bool, bool, threshold, four items, twelve eye colours, seventeen items ----
    if (!build_bool(0)) return out;
    if (!build_bool(1)) return out;
    if (!build_float(0)) return out;
    for (std::uint32_t index = 0; index < 4; ++index) {
        if (!build_item(index)) return out;
    }
    for (std::uint32_t index = 1; index < kSlot326FloatCount; ++index) {
        if (!build_float(index)) return out;
    }
    for (std::uint32_t index = 4; index < kSlot326ItemCount; ++index) {
        if (!build_item(index)) return out;
    }

    // 0x839a96..0x839ab8: canary compare and epilogue. A changed guard takes `jne 0x839ab9`.
    if (hooks.stack_guard != nullptr) {
        out.canary_checked = true;
        out.canary_matched = *hooks.stack_guard == guard_entry;
        if (!out.canary_matched) return (void)fail.at(Slot326Stage::canary), out;
    }

    out.blocked_index = 0;
    out.ok = true;
    return out;
}

}  // namespace sotc
