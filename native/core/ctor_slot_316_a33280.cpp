// CPU-01-STARTUP-SLOT-316-CHARACTERIZATION-01 / -NATIVE-LIFT-01: see ctor_slot_316_a33280.hpp.
#include "ctor_slot_316_a33280.hpp"

#include <cstdlib>
#include <cstring>

namespace sotc {

namespace {

// Default host allocator hooks (documented stand-in for 0x12820 / 0x12af0; the same convention as
// the registered slot 306). The engine allocator body is not recovered and is not claimed.
void* host_allocate(std::size_t size, std::uint32_t, std::uint32_t, std::uint32_t) {
    return std::malloc(size);
}
bool host_deallocate(void* ptr, std::uint32_t) {
    std::free(ptr);
    return true;
}

// The borrowed stack description view of the bool variable and the command: {empty rodata, 0, 0, 0}.
SharedStr empty_description_view() noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(kSlot316EmptyText), 0, 0, nullptr};
}

// The caller's argument block for one language variable: q[0] a self pointer, q[1] the 0xa324a0
// invoker thunk, q[2] zero, q[3] the per-site callback, payload 0, tag 0x15d1188, aux 0.
RangedVarArgs language_args(const Slot316LanguageVarSpec& spec) noexcept {
    RangedVarArgs args{};
    args.q[0] = 0;
    args.q[1] = kSlot316LanguageThunkRva;
    args.q[2] = 0;
    args.q[3] = spec.callback_rva;
    args.fn_payload = 0;
    args.fn_vptr = kSlot316LanguageFnVtbl;
    args.fn_aux = 0;
    return args;
}

// The spec must BE the pinned row for its index; anything else would store a different callback (or a
// different object address) into the functor than the original does.
bool is_pinned_row(const Slot316LanguageVarSpec& spec) noexcept {
    if (spec.index >= kSlot316LanguageVarCount) return false;
    const Slot316LanguageVarSpec& row = kSlot316LanguageVars[spec.index];
    return spec.name_rva == row.name_rva && spec.name_len == row.name_len &&
           spec.description_rva == row.description_rva && spec.description_len == row.description_len &&
           spec.object_rva == row.object_rva && spec.callback_rva == row.callback_rva;
}

}  // namespace

const char* slot316_stage_name(Slot316Stage stage) noexcept {
    switch (stage) {
        case Slot316Stage::none: return "none";
        case Slot316Stage::cell_atexit: return "cell_atexit";
        case Slot316Stage::language_var_build: return "language_var_build";
        case Slot316Stage::language_var_view_release: return "language_var_view_release";
        case Slot316Stage::language_var_atexit: return "language_var_atexit";
        case Slot316Stage::bool_var_build: return "bool_var_build";
        case Slot316Stage::bool_var_atexit: return "bool_var_atexit";
        case Slot316Stage::command_build: return "command_build";
        case Slot316Stage::command_atexit: return "command_atexit";
        case Slot316Stage::item_ctor: return "item_ctor";
        case Slot316Stage::item_registration: return "item_registration";
        case Slot316Stage::item_view_release: return "item_view_release";
        case Slot316Stage::item_atexit: return "item_atexit";
        case Slot316Stage::unlocalized_build: return "unlocalized_build";
        case Slot316Stage::unlocalized_atexit: return "unlocalized_atexit";
        case Slot316Stage::canary: return "canary";
    }
    return "";
}

Slot316LanguageVarResult init_slot316_language_var(ConCmdRangedVar& var,
                                                   const Slot316LanguageVarSpec& spec,
                                                   ConCmdRegistry& registry) noexcept {
    Slot316LanguageVarResult out{};
    if (!is_pinned_row(spec)) return out;   // refuse, write nothing
    const SharedStr name = l2::measured_view(spec.name, spec.name_len);
    const SharedStr description = l2::measured_view(spec.description, spec.description_len);
    const RangedVarArgs args = language_args(spec);

    // 0xa326b6: call 0x20af0(this, name, description, flags = 1).
    init_concmd_base_20af0(var.base, name, description, kSlot316VarFlags);
    // 0xa326bb..0xa326d4: `lea 0x15d1110; add 0x10; [this] = 0x15d1120`, then the caller's value cell
    // (`&this->value`, this+0xd0) at +0x60.
    var.base.vptr = kSlot316LanguageVarMidVptr;
    var.value_ref = &var.value;
    // 0xa326d8..0xa326f1: both bounds, disengaged byte then engaged byte then the bound dword.
    const std::uint32_t min_value = kSlot316LanguageMin;
    const std::uint32_t max_value = kSlot316LanguageMax;
    var.min_value[0] = 0;
    var.min_engaged = 1;
    std::memcpy(var.min_value, &min_value, 4);
    var.max_value[0] = 0;
    var.max_engaged = 1;
    std::memcpy(var.max_value, &max_value, 4);
    // 0xa326f4..0xa3270e: the zeroed argument copy, the functor tag, payload and aux.
    std::memset(var.args, 0, sizeof var.args);
    var.fn_vptr = kSlot316LanguageFnVtbl;
    var.fn_payload = 0;
    var.fn_aux = 0;
    // 0xa32719..0xa327b8: the 32 B argument copy with the q[3] self-pointer rule, then the collapsed
    // 11-dispatch functor install (clone / copy / copy / copy / aux swap / bare-ret destroy, twice).
    install_ranged_fn_from_args(var, args, kSlot316LanguageFnVtbl);

    // 0xa3262d..0xa32644: the final vptr, then the caller's cell read TWICE (+0xd0 then +0x68).
    var.base.vptr = kSlot316LanguageVarFinalVptr;
    var.value = kSlot316LanguageUnset;
    var.default_value = kSlot316LanguageUnset;

    // 0xa32647: call 0x20d60 (the register-once guard, queued on the null manager path).
    out.functor_dispatches = kSlot316LanguageDispatches;
    out.registration = register_concmd_20d60(var.base, registry);
    out.ok = out.registration.ok;
    return out;
}

Slot316Hooks default_slot316_hooks() noexcept {
    Slot316Hooks hooks{};
    hooks.language_var_ctor = &init_slot316_language_var;
    hooks.string_alloc.alloc.allocate = &host_allocate;
    hooks.string_alloc.alloc.deallocate = &host_deallocate;
    return hooks;
}

DebugMenuTarget slot316_menu_target(const ConCmdBase& cmd) noexcept {
    DebugMenuTarget out{};
    out.object = &cmd;
    if (cmd.vptr == kSlot316LanguageVarFinalVptr) {
        // 0x15d11b8 slot 3 = 0xa31d90 `mov eax,1; ret`; slot 6 = 0xa31f10 `mov eax,1; ret`.
        out.slot3 = kSlot316LanguageGate3;
        out.slot6 = kSlot316LanguageGate6;
        out.known = true;
    } else if (cmd.vptr == kSlot316BoolFinalVptr) {
        out.slot3 = kSlot316BoolGate3;
        out.slot6 = kSlot316BoolGate6;
        out.known = true;
    } else if (cmd.vptr == kSlot316CommandVptr) {
        // slot 3 = 0x22ae0 `xor eax,eax; ret`, so the `slot3 == 1` narrowing never holds and the
        // original never reads slot 6: the second value is unobserved, not lifted.
        out.slot3 = kSlot316CommandGate3;
        out.slot6 = 0;
        out.known = true;
    }
    return out;
}

bool slot316_inert_functor_destroy(const Slot316FunctorSite& site) noexcept {
    return site.slot3_is_bare_ret;
}

ConCmdBase& slot316_target_base(Slot316Objects& objects, std::uint32_t index) noexcept {
    static ConCmdBase fallback{};
    switch (index) {
        case 0: return objects.var_text.base;
        case 1: return objects.var_subtitles.base;
        case 2: return objects.var_speech.base;
        case 3: return objects.var_system.base;
        case 4: return objects.var_english.base;
        case 5: return objects.command.base;
        default: return fallback;
    }
}

DebugMenuItem& slot316_item(Slot316Objects& objects, std::uint32_t index) noexcept {
    switch (index) {
        case 0: return objects.item_text;
        case 1: return objects.item_subtitles;
        case 2: return objects.item_speech;
        case 3: return objects.item_system;
        case 4: return objects.item_english;
        default: return objects.item_reload;
    }
}

Slot316Result init_slot316_a33280(Slot316Objects& o, ConCmdRegistry& concmd, DebugMenuRegistry& menu,
                                  const Slot316Hooks& hooks) noexcept {
    Slot316Result out{};
    l2::StageFail<Slot316Result, Slot316Stage> fail{out};

    // 0xa33298..0xa332b7: guard <- *[*[0x15fe518]], stored at [rsp+0xc0]. The only branch that reads
    // it is the epilogue compare at 0xa33c6a; a mismatch calls __stack_chk_fail at 0xa33c7b.
    const std::uint64_t guard_entry = hooks.stack_guard != nullptr ? *hooks.stack_guard : 0;

    // 0xa332bf: qword [0x32f4fe0] = 0; 0xa332c6: __cxa_atexit(0xa2c760, 0x32f4fe0, &__dso_handle).
    o.cell.word = 0;
    if (!concmd.atexit_log.record(kSlot316CellDtorRva, &o.cell)) {
        return (void)fail.at(Slot316Stage::cell_atexit), out;
    }
    ++out.atexit_records;

    for (std::uint32_t index = 0; index < kSlot316LanguageVarCount; ++index) {
        const Slot316LanguageVarSpec& spec = kSlot316LanguageVars[index];
        out.blocked_index = index;

        // 0xa333a4 / 0xa334ae / 0xa335b2 / 0xa336bd: the wrapper (which calls the leaf and 0x20d60).
        if (hooks.language_var_ctor == nullptr) {
            return (void)fail.at(Slot316Stage::language_var_build), out;
        }
        ConCmdRangedVar& var = index == 0   ? o.var_text
                               : index == 1 ? o.var_subtitles
                               : index == 2 ? o.var_speech
                                            : o.var_system;
        out.language_vars[index] = hooks.language_var_ctor(var, spec, concmd);
        if (!out.language_vars[index].ok) {
            return (void)fail.at(Slot316Stage::language_var_build), out;
        }
        out.functor_dispatches += out.language_vars[index].functor_dispatches;
        ++out.vars_built;

        // 0xa333ad: 0xf590 on the stack description view (borrowed, so an exact no-op).
        const bool desc_inert =
            release_inert_view_f590(l2::measured_view(spec.description, spec.description_len));
        out.language_vars[index].description_inert = desc_inert;
        out.views_inert = out.views_inert && desc_inert;
        if (!desc_inert) return (void)fail.at(Slot316Stage::language_var_view_release), out;

        // 0xa333c5 / 0xa334cf / 0xa335d3 / 0xa336de: the row-local functor destroy (bare ret 0xa32490).
        if (!slot316_inert_functor_destroy(kSlot316FunctorSites[index])) {
            return (void)fail.at(Slot316Stage::language_var_build), out;
        }
        out.language_vars[index].local_functor_inert = true;
        ++out.functors_destroyed;
        ++out.functor_dispatches;

        // 0xa333cb: 0xf590 on the stack name view.
        const bool name_inert = release_inert_view_f590(l2::measured_view(spec.name, spec.name_len));
        out.language_vars[index].name_inert = name_inert;
        out.views_inert = out.views_inert && name_inert;
        if (!name_inert) return (void)fail.at(Slot316Stage::language_var_view_release), out;

        if (!concmd.atexit_log.record(kSlot316LanguageVarDtorRva, &var)) {
            return (void)fail.at(Slot316Stage::language_var_atexit), out;
        }
        ++out.atexit_records;
        out.language_vars[index].ok = true;
    }

    // 0xa336fe..0xa33805: the bool variable `sLanguageUnlocalizedAsEnglish` (class 0x1594218, functor
    // tag 0x15d1220, value 0 / default 0), including its two releases, registration and atexit record.
    out.blocked_index = 0;
    out.bool_var = init_bool_var_static(o.var_english,
                                        l2::measured_view(kSlot316BoolVar.name, kSlot316BoolVar.name_len),
                                        empty_description_view(), kSlot316BoolVar.fn_tag, concmd,
                                        kSlot316BoolVar.value, kSlot316BoolVar.default_value);
    out.views_inert = out.views_inert && out.bool_var.views_inert;
    if (!out.bool_var.ok) return (void)fail.at(Slot316Stage::bool_var_build), out;
    ++out.vars_built;
    ++out.atexit_records;

    // 0xa3380f..0xa338f6: the `loc_reload` console command -- 0xf2f0 name view, 0x20af0 with flags 2,
    // the 32 B delegate store with its self-pointer fix-up, the zeroed vector, the
    // `call [0x1594dc0 + 0x20]` (0x20d60) registration, both releases and atexit(0x4b0).
    {
        FConCmdCommandSpec spec{};
        spec.name = kSlot316Command.name;
        spec.description = kSlot316Command.description;
        spec.description_length = kSlot316Command.description_len;
        spec.handler_tag = kSlot316Command.handler_tag;
        out.command = init_fconcmd_object(o.command, spec, concmd);
        out.views_inert = out.views_inert && out.command.views_inert;
        if (!out.command.ok) return (void)fail.at(Slot316Stage::command_build), out;
        ++out.vars_built;
        ++out.atexit_records;
    }

    for (std::uint32_t index = 0; index < kSlot316ItemCount; ++index) {
        const Slot316ItemRow& row = kSlot316Items[index];
        out.blocked_index = index;
        DebugMenuItem& item = slot316_item(o, index);
        // 0x26ea0 reads two virtual slots of the target; an unpinned class would run unlifted code.
        const DebugMenuTarget target = slot316_menu_target(slot316_target_base(o, row.spec.target));
        if (!target.known) return (void)fail.at(Slot316Stage::item_ctor), out;
        if (!init_debug_menu_item_26ea0(item, l2::literal_view(kSlot316MenuParent),
                                        l2::measured_view(row.spec.caption, row.spec.caption_len), target,
                                        kSlot316ItemFlags)) {
            return (void)fail.at(Slot316Stage::item_ctor), out;
        }
        out.item_registrations[index] = register_debug_menu_item_268b0(item, menu, concmd.atexit_log);
        if (!out.item_registrations[index].ok) {
            return (void)fail.at(Slot316Stage::item_registration), out;
        }
        ++out.menu_items;
        // The site releases the caption view then the parent view, then records the destructor.
        const bool caption_inert =
            release_inert_view_f590(l2::measured_view(row.spec.caption, row.spec.caption_len));
        const bool parent_inert = release_inert_view_f590(l2::literal_view(kSlot316MenuParent));
        out.views_inert = out.views_inert && caption_inert && parent_inert;
        if (!caption_inert || !parent_inert) return (void)fail.at(Slot316Stage::item_view_release), out;
        if (!concmd.atexit_log.record(kSlot316MenuItemDtorRva, &item)) {
            return (void)fail.at(Slot316Stage::item_atexit), out;
        }
        ++out.atexit_records;
    }

    // 0xa33c30..0xa33c53: 0xf1a0(&"!!!Unlocalized!!!") then __cxa_atexit(0xf7b0, that holder).
    out.blocked_index = 0;
    if (!init_shared_str_f1a0(o.unlocalized, kSlot316UnlocalizedText, hooks.string_alloc)) {
        return (void)fail.at(Slot316Stage::unlocalized_build), out;
    }
    out.unlocalized_built = true;
    if (!concmd.atexit_log.record(kSlot316UnlocalizedDtorRva, &o.unlocalized)) {
        return (void)fail.at(Slot316Stage::unlocalized_atexit), out;
    }
    ++out.atexit_records;

    // 0xa33c58..0xa33c7a: canary compare and epilogue. A changed guard takes `jne 0xa33c7b`.
    if (hooks.stack_guard != nullptr) {
        out.canary_checked = true;
        out.canary_matched = *hooks.stack_guard == guard_entry;
        if (!out.canary_matched) return (void)fail.at(Slot316Stage::canary), out;
    }

    out.blocked_index = 0;
    out.ok = true;
    return out;
}

}  // namespace sotc
