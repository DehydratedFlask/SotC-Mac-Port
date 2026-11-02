// CPU-01-STARTUP-SLOT-335-CHARACTERIZATION-01: see ctor_slot_335_7edf10.hpp.
#include "ctor_slot_335_7edf10.hpp"

#include <cstring>

namespace sotc {

namespace {

// The borrowed description view every variable site of this body builds: {empty rodata, 0, 0, 0}.
SharedStr empty_description_view() noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(kSlot335EmptyText), 0, 0, nullptr};
}

// The argument block of the four 0x6aa00 sites: four zero qwords, payload 0, aux 0, the site's tag.
RangedVarArgs plain_args() noexcept {
    RangedVarArgs args{};
    args.fn_vptr = kSlot335FloatArgFnVtbl;
    return args;
}

// The argument block of the two 0x7edd80 sites. The site writes q[0] = &q[0], q[1] = the value of the
// 0x15fe600 cell (the member-function thunk 0x7edd60), q[2] = 0 and q[3] = the change callback; the
// non-zero q[3] is what makes the install re-point the object's args[0] at itself.
RangedVarArgs callback_args(std::uint64_t callback_rva) noexcept {
    RangedVarArgs args{};
    args.q[0] = reinterpret_cast<std::uint64_t>(&args.q[0]);
    args.q[1] = kSlot335CallbackThunkRva;
    args.q[2] = 0;
    args.q[3] = callback_rva;
    args.fn_vptr = kSlot335FloatArgFnVtbl;
    return args;
}

// The argument copy at +0x80 and the functor install shared by 0x6aa00 / 0xc300: clone -> copy ->
// copy(this.fn) -> copy(this.fn) -> aux swap -> bare-ret destroy, collapsed to its net state.
void copy_args_and_install_fn(ConCmdRangedVar& var, const RangedVarArgs& args) noexcept {
    var.args[1] = args.q[1];
    var.args[2] = args.q[2];
    var.args[0] = args.q[0];
    var.args[3] = args.q[3];
    if (args.q[3] != 0) {
        var.args[0] = reinterpret_cast<std::uint64_t>(&var.args[0]);
    }
    const std::uint64_t saved_aux = args.fn_aux;
    var.fn_payload = args.fn_payload;
    var.fn_vptr = kSlot335FloatClassFnVtbl;   // every copy/clone slot stores 0x1594318
    var.fn_aux = saved_aux;
}

// The same install for the 0xd0 shape (field offsets are identical up to +0xcf).
void copy_args_and_install_fn(Slot335BoundedVar& var, const RangedVarArgs& args) noexcept {
    var.args[1] = args.q[1];
    var.args[2] = args.q[2];
    var.args[0] = args.q[0];
    var.args[3] = args.q[3];
    if (args.q[3] != 0) {
        var.args[0] = reinterpret_cast<std::uint64_t>(&var.args[0]);
    }
    const std::uint64_t saved_aux = args.fn_aux;
    var.fn_payload = args.fn_payload;
    var.fn_vptr = kSlot335FloatClassFnVtbl;
    var.fn_aux = saved_aux;
}

}  // namespace

const char* slot335_stage_name(Slot335Stage stage) noexcept {
    switch (stage) {
        case Slot335Stage::none: return "none";
        case Slot335Stage::unbounded_var_build: return "unbounded_var_build";
        case Slot335Stage::unbounded_var_registration: return "unbounded_var_registration";
        case Slot335Stage::unbounded_var_view_release: return "unbounded_var_view_release";
        case Slot335Stage::unbounded_var_atexit: return "unbounded_var_atexit";
        case Slot335Stage::bool_var_build: return "bool_var_build";
        case Slot335Stage::bool_var_registration: return "bool_var_registration";
        case Slot335Stage::bool_var_view_release: return "bool_var_view_release";
        case Slot335Stage::bool_var_atexit: return "bool_var_atexit";
        case Slot335Stage::hash_lookup: return "hash_lookup";
        case Slot335Stage::bounded_var_build: return "bounded_var_build";
        case Slot335Stage::bounded_var_registration: return "bounded_var_registration";
        case Slot335Stage::bounded_var_view_release: return "bounded_var_view_release";
        case Slot335Stage::bounded_var_atexit: return "bounded_var_atexit";
        case Slot335Stage::item_ctor: return "item_ctor";
        case Slot335Stage::item_registration: return "item_registration";
        case Slot335Stage::item_view_release: return "item_view_release";
        case Slot335Stage::item_atexit: return "item_atexit";
        case Slot335Stage::canary: return "canary";
    }
    return "";
}

std::uint32_t slot335_fnv1a32(const char* text) noexcept {
    if (text == nullptr) return kSlot335FnvBasis;
    std::uint32_t hash = kSlot335FnvBasis;
    for (const unsigned char* p = reinterpret_cast<const unsigned char*>(text); *p != 0; ++p) {
        // 0x161fd0..0x161fd5: `movsbl %cl,%ecx; xorl %eax,%ecx; imull $0x1000193,%ecx,%eax`.
        const std::uint32_t byte = static_cast<std::uint32_t>(static_cast<std::int32_t>(
            static_cast<std::int8_t>(*p)));
        hash = (hash ^ byte) * kSlot335FnvPrime;
    }
    return hash;
}

std::uint32_t* slot335_value_cell(Slot335ValueCells& cells, std::uint32_t index) noexcept {
    if (index >= kSlot335ValueArrayWindow) return nullptr;
    return &cells.cell[index];
}

void slot335_borrowed_str(SharedStr& out, const char* text) noexcept {
    // 0xf2f0: [out] = text; [out+8] = strlen(text); [out+0x10..0x20) = 0.
    out.data = reinterpret_cast<const std::uint8_t*>(text);
    out.size = text == nullptr ? 0 : static_cast<std::int64_t>(std::strlen(text));
    out.aux = 0;
    out.ctl = nullptr;
}

bool slot335_bounded_var_state(Slot335BoundedVar& var, const SharedStr& name, void* value_ref,
                               float min_value, float max_value, std::uint32_t flags,
                               const RangedVarArgs& args, const SharedStr& description) noexcept {
    // 0xc300 dispatches slots 0 / 1 / 3 of the caller's tag; only 0x15ce630's four bodies are lifted
    // for this block, so any other tag writes NOTHING (the original would run unlifted code after
    // most of the stores, and a partial object would be a fabrication).
    if (args.fn_vptr != kSlot335FloatArgFnVtbl) return false;
    // The object's +0x60 points at the caller's cell; a null cell would fault in the original.
    if (value_ref == nullptr) return false;

    init_concmd_base_20af0(var.base, name, description, flags);   // 0xc336
    var.base.vptr = kSlot335FloatMidVptr;                         // 0xc33b..0xc351
    var.value_ref = value_ref;                                    // 0xc354
    var.min_value[0] = 0;
    var.min_engaged = 1;
    std::memcpy(var.min_value, &min_value, 4);
    var.max_value[0] = 0;
    var.max_engaged = 1;
    std::memcpy(var.max_value, &max_value, 4);
    std::memset(var.args, 0, sizeof var.args);
    var.fn_vptr = kSlot335FloatClassFnVtbl;
    var.fn_payload = 0;
    var.fn_aux = 0;
    copy_args_and_install_fn(var, args);
    return true;
}

bool slot335_unbounded_float_6aa00(ConCmdRangedVar& var, const SharedStr& name, void* value_ref,
                                   std::uint32_t flags, const RangedVarArgs& args,
                                   const SharedStr& description) noexcept {
    if (args.fn_vptr != kSlot335FloatArgFnVtbl) return false;
    if (value_ref == nullptr) return false;

    init_concmd_base_20af0(var.base, name, description, flags);   // 0x6aa2c
    var.base.vptr = kSlot335FloatMidVptr;                         // 0x6aa31..0x6aa47
    var.value_ref = value_ref;                                    // 0x6aa4a
    var.min_value[0] = 0;                                         // 0x6aa4e
    var.min_engaged = 0;                                          // 0x6aa52
    var.max_value[0] = 0;                                         // 0x6aa56
    var.max_engaged = 0;                                          // 0x6aa5a
    std::memset(var.args, 0, sizeof var.args);                    // 0x6aa5e
    var.fn_vptr = 0x1596878;                                      // 0x6aa66 (the ctor's own first tag)
    var.fn_payload = 0;                                           // 0x6aa6d
    var.fn_aux = 0;                                               // 0x6aa78
    copy_args_and_install_fn(var, args);                          // 0x6aa83..0x6ab21
    return true;
}

bool slot335_bounded_var_7edd80(Slot335BoundedVar& var, const SharedStr& name,
                                const std::uint32_t* value_cell, float min_value, float max_value,
                                const RangedVarArgs& args, const SharedStr& description,
                                ConCmdRegistry& registry) noexcept {
    if (value_cell == nullptr) return false;   // the original would fault on `movl (%r14),%eax`
    // 0x7ede7c..0x7edea8: the constructor call with flags r9d = 1.
    if (!slot335_bounded_var_state(var, name, &var.default_value, min_value, max_value,
                                   kSlot335VarFlags, args, description)) {
        return false;
    }
    // 0x7edebe..0x7edecc: [obj] = 0x15943b0 + 0x10.
    var.base.vptr = kSlot335BoundedFinalVptr;
    // 0x7edecf..0x7eded2: default = *value_cell (and the +0x60 pointer already targets +0x68).
    std::memcpy(&var.default_value, value_cell, 4);
    // 0x7eded5: register-once guard.
    return register_concmd_20d60(var.base, registry).ok;
}

bool slot335_inert_functor_destroy(const Slot335FunctorSite& site) noexcept {
    return site.slot3_is_bare_ret;
}

Slot335Hooks default_slot335_hooks() noexcept {
    // No allocator stand-in is needed: every view this body builds is borrowed and every callee has a
    // native body. The guard source stays null unless the host supplies it.
    return Slot335Hooks{};
}

ConCmdBase& slot335_unbounded_base(Slot335Objects& objects, std::uint32_t index) noexcept {
    static ConCmdBase fallback{};
    if (index < kSlot335UnboundedCount) return objects.unbounded[index].base;
    return fallback;
}

ConCmdBase& slot335_bool_base(Slot335Objects& objects, std::uint32_t index) noexcept {
    static ConCmdBase fallback{};
    if (index < kSlot335BoolCount) return objects.bools[index].base;
    return fallback;
}

Slot335BoundedVar& slot335_bounded_var(Slot335Objects& objects, std::uint32_t index) noexcept {
    static Slot335BoundedVar fallback{};
    if (index == 0) return objects.bounded_hp;
    if (index == 1) return objects.bounded_grip;
    return fallback;
}

Slot335HashRecord& slot335_hash_record(Slot335Objects& objects, std::uint32_t index) noexcept {
    static Slot335HashRecord fallback{};
    if (index == 0) return objects.hash_health;
    if (index == 1) return objects.hash_grip;
    return fallback;
}

DebugMenuItem& slot335_item(Slot335Objects& objects, std::uint32_t index) noexcept {
    static DebugMenuItem fallback{};
    if (index == 0) return objects.slider_hp;
    if (index == 1) return objects.slider_grip;
    return fallback;
}

Slot335Result init_slot335_7edf10(Slot335Objects& o, Slot335ValueCells& cells,
                                  ConCmdRegistry& concmd, DebugMenuRegistry& menu,
                                  const Slot335Hooks& hooks) noexcept {
    Slot335Result out{};
    l2::StageFail<Slot335Result, Slot335Stage> fail{out};

    // 0x7edf28..0x7edf7f: guard <- *[*[0x15fe518]], stored at [rsp+0xe0]. The only branch that reads
    // it is the epilogue compare at 0x7ee99e; a mismatch calls __stack_chk_fail at 0x7ee9af.
    const std::uint64_t guard_entry = hooks.stack_guard != nullptr ? *hooks.stack_guard : 0;

    const auto build_unbounded = [&](std::uint32_t index) -> bool {
        const Slot335UnboundedSpec& spec = kSlot335Unbounded[index];
        out.blocked_index = index;
        ConCmdRangedVar& var = o.unbounded[index];
        const SharedStr name = l2::measured_view(spec.name, spec.name_len);
        const SharedStr description = empty_description_view();

        // 0x7edfb2: [obj] = 0x15942a0 + 0x10 before the constructor call (the ctor overwrites it).
        var.base.vptr = kSlot335FloatFinalVptr;
        if (!slot335_unbounded_float_6aa00(var, name, &var.value, kSlot335VarFlags, plain_args(),
                                           description)) {
            return (void)fail.at(Slot335Stage::unbounded_var_build), false;
        }
        // 0x7edff5..0x7ee008: release the description view, then dispatch the argument functor's
        // slot 3 (a bare ret).
        out.unbounded[index].description_inert = release_inert_view_f590(description);
        out.views_inert = out.views_inert && out.unbounded[index].description_inert;
        if (!out.unbounded[index].description_inert) {
            return (void)fail.at(Slot335Stage::unbounded_var_view_release), false;
        }
        out.unbounded[index].functor_inert = slot335_inert_functor_destroy(kSlot335FunctorSites[index]);
        if (!out.unbounded[index].functor_inert) {
            return (void)fail.at(Slot335Stage::unbounded_var_build), false;
        }
        ++out.functors_destroyed;

        // 0x7ee00b..0x7ee022: final vptr, value into +0xd0, default into +0x68, register.
        var.base.vptr = kSlot335FloatFinalVptr;
        var.value = spec.value_bits;
        var.default_value = spec.value_bits;
        out.unbounded[index].registration = register_concmd_20d60(var.base, concmd);
        if (!out.unbounded[index].registration.ok) {
            return (void)fail.at(Slot335Stage::unbounded_var_registration), false;
        }
        // 0x7ee027: release the name view.
        out.unbounded[index].name_inert = release_inert_view_f590(name);
        out.views_inert = out.views_inert && out.unbounded[index].name_inert;
        if (!out.unbounded[index].name_inert) {
            return (void)fail.at(Slot335Stage::unbounded_var_view_release), false;
        }
        // 0x7ee02f..0x7ee040: __cxa_atexit(0x20f0, obj, &__dso_handle).
        if (!concmd.atexit_log.record(kSlot335FloatVarDtorRva, &var)) {
            return (void)fail.at(Slot335Stage::unbounded_var_atexit), false;
        }
        ++out.atexit_records;
        ++out.vars_built;
        out.unbounded[index].ok = true;
        return true;
    };

    const auto build_bool = [&](std::uint32_t index) -> bool {
        const Slot335BoolSpec& spec = kSlot335Bools[index];
        out.blocked_index = index;
        ConCmdBoolVar& var = o.bools[index];
        // The shared bool-variable model: 0x20af0 with flags 1, class 0x1594218, the site's argument
        // tag 0x15ce650, value / default from the row, its two 0xf590 releases, 0x20d60 and the
        // __cxa_atexit(0x20b0) record. It reports which stage failed, so map it back to the enum.
        const BoolVarResult built =
            init_bool_var_static(var, l2::measured_view(spec.name, spec.name_len),
                                 empty_description_view(), kSlot335BoolArgFnVtbl, concmd,
                                 kSlot335BoolValue, kSlot335BoolDefault);
        out.bools[index] = built;
        out.views_inert = out.views_inert && built.views_inert;
        if (!built.registration.ok) return (void)fail.at(Slot335Stage::bool_var_registration), false;
        if (!built.views_inert) return (void)fail.at(Slot335Stage::bool_var_view_release), false;
        if (!built.atexit_recorded) return (void)fail.at(Slot335Stage::bool_var_atexit), false;
        ++out.atexit_records;
        ++out.vars_built;
        out.bools[index].ok = true;
        return true;
    };

    const auto build_hash = [&](std::uint32_t index) -> bool {
        const Slot335HashSpec& spec = kSlot335Hashes[index];
        out.blocked_index = index;
        // 0x7ee606..0x7ee61c: `[obj] = name; [obj+8] = 0x52a990(name)`. 0x161fb0 walks the string
        // until a NUL byte, so an absent name is a fault in the original and is refused here.
        if (spec.name == nullptr || spec.name[0] == '\0') {
            return (void)fail.at(Slot335Stage::hash_lookup), false;
        }
        Slot335HashRecord& record = slot335_hash_record(o, index);
        record.name = spec.name;
        record.pad0c = 0;
        record.hash = slot335_fnv1a32(spec.name);
        out.hashes[index] = record.hash;
        ++out.hashes_built;
        return true;
    };

    const auto build_bounded = [&](std::uint32_t index) -> bool {
        const Slot335BoundedSpec& spec = kSlot335Bounded[index];
        out.blocked_index = index;
        Slot335BoundedVar& var = slot335_bounded_var(o, index);
        std::uint32_t* const cell = slot335_value_cell(cells, spec.cell_index);
        // 0x7ee635..0x7ee65c / 0x7ee7e6..0x7ee80a: the value cell of 0x325f410, then the name view and
        // then the borrowed description built by 0xf2f0 from the empty string.
        if (cell == nullptr) return (void)fail.at(Slot335Stage::bounded_var_build), false;
        const SharedStr name = l2::measured_view(spec.name, spec.name_len);
        SharedStr description{};
        slot335_borrowed_str(description, kSlot335EmptyText);

        // 0x7ee6da / 0x7ee880: the f2f0 call itself; then 0x7ee705 / 0x7ee8ab: 0x7edd80.
        if (!slot335_bounded_var_7edd80(var, name, cell, spec.min_value, spec.max_value,
                                        callback_args(spec.callback_rva), description, concmd)) {
            return (void)fail.at(Slot335Stage::bounded_var_build), false;
        }
        out.bounded[index].registration.ok = true;   // 0x7edd80 reports through its return value
        // 0x7ee70d..0x7ee722: release the description view, then the argument functor's bare ret.
        out.bounded[index].description_inert = release_inert_view_f590(description);
        out.views_inert = out.views_inert && out.bounded[index].description_inert;
        if (!out.bounded[index].description_inert) {
            return (void)fail.at(Slot335Stage::bounded_var_view_release), false;
        }
        const Slot335FunctorSite& site =
            index == 0 ? kSlot335FunctorSites[4] : kSlot335FunctorSites[5];
        out.bounded[index].functor_inert = slot335_inert_functor_destroy(site);
        if (!out.bounded[index].functor_inert) {
            return (void)fail.at(Slot335Stage::bounded_var_build), false;
        }
        ++out.functors_destroyed;
        // 0x7ee725: release the name view.
        out.bounded[index].name_inert = release_inert_view_f590(name);
        out.views_inert = out.views_inert && out.bounded[index].name_inert;
        if (!out.bounded[index].name_inert) {
            return (void)fail.at(Slot335Stage::bounded_var_view_release), false;
        }
        // 0x7ee72d..0x7ee741: __cxa_atexit(0xad30, obj, &__dso_handle).
        if (!concmd.atexit_log.record(kSlot335BoundedVarDtorRva, &var)) {
            return (void)fail.at(Slot335Stage::bounded_var_atexit), false;
        }
        ++out.atexit_records;
        ++out.vars_built;
        out.bounded[index].ok = true;
        return true;
    };

    const auto build_item = [&](std::uint32_t index) -> bool {
        const Slot335ItemSpec& spec = kSlot335Items[index];
        out.blocked_index = index;
        DebugMenuItem& item = slot335_item(o, index);
        const SharedStr label = l2::measured_view(spec.label, spec.label_len);
        const SharedStr text = l2::measured_view(spec.text, spec.text_len);
        // 0x27050(item, label, text, target, 0x30, xmm0 = range min, xmm1 = range max), then its tail
        // call into 0x268b0.
        const Slot335BoundedVar& target = slot335_bounded_var(o, spec.target_bounded);
        if (!init_debug_menu_item_27050(item, label, text, &target, spec.flags, spec.range_min,
                                        spec.range_max)) {
            return (void)fail.at(Slot335Stage::item_ctor), false;
        }
        out.item_registrations[index] = register_menu_item_268b0(item, menu, concmd.atexit_log);
        if (!out.item_registrations[index].ok) {
            return (void)fail.at(Slot335Stage::item_registration), false;
        }
        ++out.menu_items;
        // 0x7ee7c4..0x7ee7cf / 0x7ee96a..0x7ee975: the caption view first, the parent view second.
        const bool text_inert = release_inert_view_f590(text);
        const bool label_inert = release_inert_view_f590(label);
        out.views_inert = out.views_inert && text_inert && label_inert;
        if (!text_inert || !label_inert) return (void)fail.at(Slot335Stage::item_view_release), false;
        if (!record_menu_item_dtor_272d0(concmd.atexit_log, item)) {
            return (void)fail.at(Slot335Stage::item_atexit), false;
        }
        ++out.atexit_records;
        return true;
    };

    // ---- body order: 4 unbounded floats, 3 bools, 2 hashes, then (bounded + slider) twice ----
    for (std::uint32_t index = 0; index < kSlot335UnboundedCount; ++index) {
        if (!build_unbounded(index)) return out;
    }
    for (std::uint32_t index = 0; index < kSlot335BoolCount; ++index) {
        if (!build_bool(index)) return out;
    }
    for (std::uint32_t index = 0; index < kSlot335HashCount; ++index) {
        if (!build_hash(index)) return out;
    }
    for (std::uint32_t index = 0; index < kSlot335BoundedCount; ++index) {
        if (!build_bounded(index)) return out;
        if (!build_item(index)) return out;
    }

    // 0x7ee98c..0x7ee9ae: canary compare and epilogue. A changed guard takes `jne 0x7ee9af`.
    if (hooks.stack_guard != nullptr) {
        out.canary_checked = true;
        out.canary_matched = *hooks.stack_guard == guard_entry;
        if (!out.canary_matched) return (void)fail.at(Slot335Stage::canary), out;
    }

    out.blocked_index = 0;
    out.ok = true;
    return out;
}

}  // namespace sotc
