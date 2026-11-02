#include "ctor_slot_309_a505e0.hpp"

#include <cstring>

#include "init_sub_3be930.hpp"
#include "l2_slot_common.hpp"

namespace sotc {

namespace {

constexpr std::size_t kFunctorCopyBytes = 0x20;   // rsp+0x80 in the original frame

std::uint64_t load_tag(const Slot309VarObject& object) noexcept { return object.var.base.vptr; }

// One of the three console-variable blocks: ctor hook, functor slot 3, the two view releases, atexit.
// Returns Slot309Stage::none on success, otherwise the first step that could not be completed.
struct VarStages {
    Slot309Stage ctor;
    Slot309Stage functor;
    Slot309Stage release;
    Slot309Stage atexit;
};

Slot309Stage build_var(Slot309VarObject& object, const Slot309VarSpec& spec,
                       const Slot309FunctorSite& site, const Slot309Hooks& hooks,
                       ConCmdRegistry& concmd, const VarStages& stages, Slot309Result& out) noexcept {
    // 0xa4f4d0 / 0xa4fec0 / 0xa50280 install the class address point at object+0 inside the ctor. The
    // hook owns the interior; the slot re-checks the tag afterwards, because the three toggles answer
    // their 0x26ea0 gate from it. A missing hook writes nothing at all.
    if (hooks.var_ctor == nullptr) return stages.ctor;
    object.var.base.vptr = spec.vptr_tag;
    if (!hooks.var_ctor(object, spec, concmd)) return stages.ctor;
    if (load_tag(object) != spec.vptr_tag) return stages.ctor;
    ++out.vars_built;

    // `callq *0x18(%rax)` — functor slot 3 on the 0x20 B copy the caller keeps at rsp+0x80.
    alignas(8) std::array<std::uint8_t, kFunctorCopyBytes> functor{};
    const auto destroy = hooks.destroy_functor != nullptr ? hooks.destroy_functor
                                                          : &slot309_inert_functor_destroy;
    if (!destroy(functor.data(), site)) return stages.functor;
    ++out.functors_destroyed;

    // 0xf590(description) then 0xf590(name): both are borrowed literal views (control == nullptr).
    const SharedStr description = l2::measured_view(spec.description, spec.description_len);
    const SharedStr name = l2::measured_view(spec.name, spec.name_len);
    const bool desc_inert = release_inert_view_f590(description);
    const bool name_inert = release_inert_view_f590(name);
    out.views_inert = out.views_inert && desc_inert && name_inert;
    if (!desc_inert || !name_inert) return stages.release;

    // __cxa_atexit(spec.dtor_rva, object, 0x1604218).
    if (!concmd.atexit_log.record(spec.dtor_rva, &object)) return stages.atexit;
    ++out.atexit_records;
    return Slot309Stage::none;
}

// One toggle block: 0x26ea0 ctor, 0x268b0 registration, f590(text), f590(label), atexit(0x272d0).
struct ToggleStages {
    Slot309Stage ctor;
    Slot309Stage registration;
    Slot309Stage release;
    Slot309Stage atexit;
};

Slot309Stage build_toggle(DebugMenuItem& item, const char* text, const DebugMenuTarget& target,
                          const void* expected_target, ConCmdRegistry& concmd, DebugMenuRegistry& menu,
                          const ToggleStages& stages, Slot309Result& out,
                          MenuRegisterResult& registration) noexcept {
    const SharedStr label = l2::literal_view(kSlot309ToggleLabel);
    const SharedStr text_view = l2::measured_view(text, std::strlen(text));
    if (!init_debug_menu_item_26ea0(item, label, text_view, target, kSlot309ItemFlags)) return stages.ctor;
    // 0x26f2a stores the caller's target pointer verbatim; the model requires the same pointer.
    if (item.target != expected_target) return stages.ctor;
    registration = register_menu_item_268b0(item, menu, concmd.atexit_log);
    if (!registration.ok) return stages.registration;
    ++out.menu_items;
    if (!release_inert_view_f590(text_view) || !release_inert_view_f590(label)) return stages.release;
    if (!record_menu_item_dtor_272d0(concmd.atexit_log, item)) return stages.atexit;
    ++out.atexit_records;
    return Slot309Stage::none;
}

}  // namespace

const char* slot309_stage_name(Slot309Stage stage) noexcept {
    switch (stage) {
        case Slot309Stage::none: return "none";
        case Slot309Stage::cell: return "cell";
        case Slot309Stage::cleared: return "cleared";
        case Slot309Stage::var1_ctor: return "var1_ctor";
        case Slot309Stage::var1_functor: return "var1_functor";
        case Slot309Stage::var1_release: return "var1_release";
        case Slot309Stage::var1_atexit: return "var1_atexit";
        case Slot309Stage::name_object: return "name_object";
        case Slot309Stage::name_object_release: return "name_object_release";
        case Slot309Stage::name_object_atexit: return "name_object_atexit";
        case Slot309Stage::bool_var: return "bool_var";
        case Slot309Stage::empty_view: return "empty_view";
        case Slot309Stage::empty_view_atexit: return "empty_view_atexit";
        case Slot309Stage::toggle1_ctor: return "toggle1_ctor";
        case Slot309Stage::toggle1_registration: return "toggle1_registration";
        case Slot309Stage::toggle1_release: return "toggle1_release";
        case Slot309Stage::toggle1_atexit: return "toggle1_atexit";
        case Slot309Stage::toggle2_ctor: return "toggle2_ctor";
        case Slot309Stage::toggle2_registration: return "toggle2_registration";
        case Slot309Stage::toggle2_release: return "toggle2_release";
        case Slot309Stage::toggle2_atexit: return "toggle2_atexit";
        case Slot309Stage::var2_ctor: return "var2_ctor";
        case Slot309Stage::var2_functor: return "var2_functor";
        case Slot309Stage::var2_release: return "var2_release";
        case Slot309Stage::var2_atexit: return "var2_atexit";
        case Slot309Stage::toggle3_ctor: return "toggle3_ctor";
        case Slot309Stage::toggle3_registration: return "toggle3_registration";
        case Slot309Stage::toggle3_release: return "toggle3_release";
        case Slot309Stage::toggle3_atexit: return "toggle3_atexit";
        case Slot309Stage::var3_ctor: return "var3_ctor";
        case Slot309Stage::var3_functor: return "var3_functor";
        case Slot309Stage::var3_release: return "var3_release";
        case Slot309Stage::var3_atexit: return "var3_atexit";
        case Slot309Stage::toggle4_ctor: return "toggle4_ctor";
        case Slot309Stage::toggle4_registration: return "toggle4_registration";
        case Slot309Stage::toggle4_release: return "toggle4_release";
        case Slot309Stage::toggle4_atexit: return "toggle4_atexit";
        case Slot309Stage::canary: return "canary";
    }
    return "";
}

bool slot309_inert_functor_destroy(void* functor, const Slot309FunctorSite& site) noexcept {
    (void)functor;
    // Slot 3 of the three pinned functor vtables is a bare `ret` (0xa4f4c0 / 0xa4feb0 / 0xa50270).
    // Any other body is an unlifted destructor and must not be reported as an inert destroy.
    return site.slot3_is_bare_ret;
}

DebugMenuTarget slot309_var_menu_target(const Slot309VarObject& var) noexcept {
    const std::uint64_t tag = load_tag(var);
    const void* object = &var;
    if (tag == kSlot309Var1Vptr || tag == kSlot309Var2Vptr || tag == kSlot309Var3Vptr) {
        return DebugMenuTarget{object, kSlot309VarSlot3Value, kSlot309VarSlot6Value, true};
    }
    return DebugMenuTarget{object, 0, 0, false};
}

bool slot309_var_ctor_native(Slot309VarObject& object, const Slot309VarSpec& spec,
                             ConCmdRegistry& concmd) noexcept {
    const RangedBoundVarClass* const cls = ranged_bound_var_class_for(spec.ctor_rva);
    if (cls == nullptr) return false;
    if (cls->final_vptr != spec.vptr_tag || cls->mid_vptr != spec.mid_vptr_tag ||
        cls->fn_vtbl != spec.functor_tag || cls->dtor_rva != spec.dtor_rva) {
        return false;   // the spec and the pinned class row disagree: refuse, write nothing
    }

    // The call site's argument block (0xa5068b / 0xa506da / 0xa506e0 / 0xa506f7 and the var2/var3
    // twins): q[0..3] zero, fn payload 0, fn vptr = the class tag, aux 0.
    RangedVarArgs args{};
    args.fn_vptr = spec.functor_tag;

    // The call site's value cell is a stack temporary (`movl $0,0x3c(%rsp)`), not the object's own
    // +0xd0; the wrapper reads it twice and the object keeps the address. Reproduced as-is.
    std::int32_t cell = spec.initial_value;
    const SharedStr name = l2::measured_view(spec.name, spec.name_len);
    const SharedStr description = l2::measured_view(spec.description, spec.description_len);

    const RangedBoundVarResult built = init_ranged_bound_var_a4f4d0(
        object.var, *cls, name, &cell, static_cast<std::uint32_t>(spec.min_value),
        static_cast<std::uint32_t>(spec.max_value), spec.flags, args, description, concmd);
    return built.ok;
}

Slot309Hooks default_slot309_hooks() noexcept {
    Slot309Hooks hooks{};
    hooks.var_ctor = &slot309_var_ctor_native;
    return hooks;
}

Slot309Result init_slot309_a505e0(Slot309Objects& o, ConCmdRegistry& concmd, DebugMenuRegistry& menu,
                                  const Slot309Hooks& hooks) noexcept {
    Slot309Result out{};
    l2::StageFail<Slot309Result, Slot309Stage> fail{out};

    // 0xa505f8..0xa5061a: guard = *[0x15fe518] (__stack_chk_guard) stored at 0xc0(%rsp). The body's
    // only branch is the epilogue compare at 0xa50d25; a mismatch calls __stack_chk_fail at 0xa50d3e.
    const std::uint64_t guard_entry = hooks.stack_guard != nullptr ? *hooks.stack_guard : 0;

    // 0xa50606..0xa5062d: [0x32f79b8] = 0; __cxa_atexit(0xa46620, cell)
    o.cell.value = 0;
    if (!concmd.atexit_log.record(kSlot309CellDtorRva, &o.cell)) return (void)fail.at(Slot309Stage::cell), out;
    ++out.atexit_records;

    // 0xa50632..0xa5065d: [0x32f79c8..+0x18) = 0; __cxa_atexit(0xa47cb0, cleared)
    o.cleared.zero = {};
    if (!concmd.atexit_log.record(kSlot309ClearedDtorRva, &o.cleared)) {
        return (void)fail.at(Slot309Stage::cleared), out;
    }
    ++out.atexit_records;

    // 0xa50662..0xa50754: "force_save_debug_condition" 0..5 through 0xa4f4d0, functor tag 0x15d1b78
    {
        const Slot309Stage s = build_var(o.var1, kSlot309Vars[0], kSlot309FunctorSites[0], hooks, concmd,
                                         {Slot309Stage::var1_ctor, Slot309Stage::var1_functor,
                                          Slot309Stage::var1_release, Slot309Stage::var1_atexit},
                                         out);
        if (s != Slot309Stage::none) return (void)fail.at(s), out;
    }

    // 0xa50759..0xa507d6: name object "sSaveManagerShowInfoLevel" (vptr 0x159e1b8, borrowed view at +8)
    {
        Slot309NameObject& n = o.name_object;
        const SharedStr text = l2::literal_view(kSlot309NameObjectText);
        n.name.data = text.data;                 // 0xa5079e: [obj+0x08] = {text, 0x19}
        n.name.size = text.size;
        n.name.aux = 0;                          // 0xa507a6: +0x18..+0x27 = 0
        n.name.ctl = nullptr;
        n.tail28 = 0;                            // 0xa507ae: +0x28 = 0
        n.vptr = kSlot309NameObjectVptr;         // 0xa507bd: [obj] = 0x159e1a8 + 0x10
        if (!release_inert_view_f590(text)) {    // 0xa507c4
            return (void)fail.at(Slot309Stage::name_object_release), out;
        }
        out.name_object_released = true;
        if (!concmd.atexit_log.record(kSlot309NameObjectDtorRva, &n)) {   // 0xa507d6
            return (void)fail.at(Slot309Stage::name_object_atexit), out;
        }
        ++out.atexit_records;
    }

    // 0xa507db..0xa508e2: bool "saveSmallMemory", empty description, flags 1, value 0 / default 0,
    // functor tag 0x15d1c10 through the shared native shape (0x20af0 + 0x20d60 + 0x20b0).
    out.bool_var = init_bool_var_static(o.bool_var.var, l2::literal_view(kSlot309BoolVarName),
                                        l2::literal_view(""), kSlot309BoolVarFnTag, concmd,
                                        kSlot309BoolValue, kSlot309BoolDefault);
    if (!out.bool_var.ok) return (void)fail.at(Slot309Stage::bool_var), out;
    ++out.atexit_records;

    // 0xa508e7..0xa50903: empty SharedStr at 0x32f7bc0 through the native 0xf180 model
    init_slot_f180(&o.empty_view);
    if (!concmd.atexit_log.record(kSlot309EmptyViewDtorRva, &o.empty_view)) {   // 0xa50903
        return (void)fail.at(Slot309Stage::empty_view_atexit), out;
    }
    ++out.atexit_records;
    if (o.empty_view.ctl != nullptr) return (void)fail.at(Slot309Stage::empty_view), out;

    // 0xa50908..0xa50a15: toggles 1 and 2 (targets: the first variable and the bool variable).
    {
        const Slot309Stage s1 = build_toggle(
            o.toggle1, kSlot309Toggle1Text, slot309_var_menu_target(o.var1), &o.var1, concmd, menu,
            {Slot309Stage::toggle1_ctor, Slot309Stage::toggle1_registration, Slot309Stage::toggle1_release,
             Slot309Stage::toggle1_atexit},
            out, out.toggle_registration[0]);
        if (s1 != Slot309Stage::none) return (void)fail.at(s1), out;
        const Slot309Stage s2 = build_toggle(
            o.toggle2, kSlot309Toggle2Text, debug_menu_target_for(o.bool_var.var), &o.bool_var.var, concmd,
            menu,
            {Slot309Stage::toggle2_ctor, Slot309Stage::toggle2_registration, Slot309Stage::toggle2_release,
             Slot309Stage::toggle2_atexit},
            out, out.toggle_registration[1]);
        if (s2 != Slot309Stage::none) return (void)fail.at(s2), out;
    }

    // 0xa50a1a..0xa50b13: "saveLoadStallState" 0..18 through 0xa4fec0, functor tag 0x15d1c30
    {
        const Slot309Stage s = build_var(o.var2, kSlot309Vars[1], kSlot309FunctorSites[1], hooks, concmd,
                                         {Slot309Stage::var2_ctor, Slot309Stage::var2_functor,
                                          Slot309Stage::var2_release, Slot309Stage::var2_atexit},
                                         out);
        if (s != Slot309Stage::none) return (void)fail.at(s), out;
    }

    // 0xa50b18..0xa50b97: toggle 3 (target: the second variable)
    {
        const Slot309Stage s = build_toggle(
            o.toggle3, kSlot309Toggle3Text, slot309_var_menu_target(o.var2), &o.var2, concmd, menu,
            {Slot309Stage::toggle3_ctor, Slot309Stage::toggle3_registration, Slot309Stage::toggle3_release,
             Slot309Stage::toggle3_atexit},
            out, out.toggle_registration[2]);
        if (s != Slot309Stage::none) return (void)fail.at(s), out;
    }

    // 0xa50b9c..0xa50c92: "saveStoreStallState" 0..20 through 0xa50280, functor tag 0x15d1cc8
    {
        const Slot309Stage s = build_var(o.var3, kSlot309Vars[2], kSlot309FunctorSites[2], hooks, concmd,
                                         {Slot309Stage::var3_ctor, Slot309Stage::var3_functor,
                                          Slot309Stage::var3_release, Slot309Stage::var3_atexit},
                                         out);
        if (s != Slot309Stage::none) return (void)fail.at(s), out;
    }

    // 0xa50c97..0xa50d16: toggle 4 (target: the third variable)
    {
        const Slot309Stage s = build_toggle(
            o.toggle4, kSlot309Toggle4Text, slot309_var_menu_target(o.var3), &o.var3, concmd, menu,
            {Slot309Stage::toggle4_ctor, Slot309Stage::toggle4_registration, Slot309Stage::toggle4_release,
             Slot309Stage::toggle4_atexit},
            out, out.toggle_registration[3]);
        if (s != Slot309Stage::none) return (void)fail.at(s), out;
    }

    // 0xa50d1b..0xa50d3d: canary compare and epilogue. A changed guard would take `jne 0xa50d3e`.
    if (hooks.stack_guard != nullptr) {
        out.canary_checked = true;
        out.canary_matched = *hooks.stack_guard == guard_entry;
        if (!out.canary_matched) return (void)fail.at(Slot309Stage::canary), out;
    }

    out.ok = true;
    return out;
}

}  // namespace sotc
