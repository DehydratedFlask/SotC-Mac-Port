// CPU-01-STARTUP-SLOT-312-CHARACTERIZATION-01 / -NATIVE-LIFT-01: see ctor_slot_312_a46130.hpp.
#include "ctor_slot_312_a46130.hpp"

#include <cstring>

namespace sotc {

namespace {

// The wrapper's stack description view: {rodata NUL at 0x12c2fa6, 0, 0, 0}. Borrowed, so the 0xf590
// the wrapper performs on it is an exact no-op.
SharedStr var_description_view() noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(kSlot312EmptyText), 0, 0, nullptr};
}

// One int-variable block of the body: the wrapper/leaf hook, the row-local functor destroy, the name
// release and the atexit record, in body order.
Slot312Stage build_int_var(ConCmdRangedVar& var, const Slot312IntVarSpec& spec,
                           const Slot312FunctorSite& site, const Slot312Hooks& hooks,
                           ConCmdRegistry& registry, Slot312Result& out,
                           Slot312Stage ctor_stage, Slot312Stage release_stage,
                           Slot312Stage functor_stage, Slot312Stage atexit_stage) noexcept {
    // The wrapper installs the final vptr before the leaf; a missing hook writes nothing after that.
    if (hooks.var_ctor == nullptr) return ctor_stage;
    Slot312IntVarResult built{};
    {
        // The hook owns the interior; the slot re-checks the final tag afterwards.
        const Slot312IntVarClass* const cls = slot312_int_var_class_for(spec.wrapper_rva);
        if (cls == nullptr) return ctor_stage;
        var.base.vptr = cls->final_vptr;
        if (!hooks.var_ctor(var, spec, registry)) return ctor_stage;
        if (var.base.vptr != cls->final_vptr) return ctor_stage;
    }
    built.functor_slot_calls = kSlot312VarFunctorDispatches;
    out.int_vars[static_cast<std::size_t>(out.vars_built) - 1u] = built;
    out.var_functor_calls += built.functor_slot_calls;
    ++out.vars_built;

    // `callq *0x18(%rax)` on the caller's 0x20 B functor copy, then 0xf590(name).
    if (!slot312_inert_functor_destroy(site)) return functor_stage;
    ++out.functors_destroyed;

    const SharedStr name = l2::measured_view(spec.name, spec.name_len);
    const bool inert = release_inert_view_f590(name);
    out.views_inert = out.views_inert && inert;
    if (!inert) return release_stage;

    if (!registry.atexit_log.record(spec.dtor_rva, &var)) return atexit_stage;
    ++out.atexit_records;
    return Slot312Stage::none;
}

}  // namespace

const char* slot312_stage_name(Slot312Stage stage) noexcept {
    switch (stage) {
        case Slot312Stage::none: return "none";
        case Slot312Stage::title_sku_build: return "title_sku_build";
        case Slot312Stage::title_sku_view_release: return "title_sku_view_release";
        case Slot312Stage::title_sku_atexit: return "title_sku_atexit";
        case Slot312Stage::age_restriction_build: return "age_restriction_build";
        case Slot312Stage::age_restriction_view_release: return "age_restriction_view_release";
        case Slot312Stage::age_restriction_atexit: return "age_restriction_atexit";
        case Slot312Stage::region_build: return "region_build";
        case Slot312Stage::region_view_release: return "region_view_release";
        case Slot312Stage::region_atexit: return "region_atexit";
        case Slot312Stage::group_ctor: return "group_ctor";
        case Slot312Stage::group_registration: return "group_registration";
        case Slot312Stage::group_view_release: return "group_view_release";
        case Slot312Stage::group_atexit: return "group_atexit";
        case Slot312Stage::toggle_sku_ctor: return "toggle_sku_ctor";
        case Slot312Stage::toggle_sku_registration: return "toggle_sku_registration";
        case Slot312Stage::toggle_sku_view_release: return "toggle_sku_view_release";
        case Slot312Stage::toggle_sku_atexit: return "toggle_sku_atexit";
        case Slot312Stage::toggle_age_ctor: return "toggle_age_ctor";
        case Slot312Stage::toggle_age_registration: return "toggle_age_registration";
        case Slot312Stage::toggle_age_view_release: return "toggle_age_view_release";
        case Slot312Stage::toggle_age_atexit: return "toggle_age_atexit";
        case Slot312Stage::toggle_region_ctor: return "toggle_region_ctor";
        case Slot312Stage::toggle_region_registration: return "toggle_region_registration";
        case Slot312Stage::toggle_region_view_release: return "toggle_region_view_release";
        case Slot312Stage::toggle_region_atexit: return "toggle_region_atexit";
        case Slot312Stage::record_stores: return "record_stores";
        case Slot312Stage::record_views: return "record_views";
        case Slot312Stage::record_atexit: return "record_atexit";
        case Slot312Stage::canary: return "canary";
    }
    return "";
}

const Slot312IntVarClass* slot312_int_var_class_for(std::uint64_t wrapper_rva) noexcept {
    for (const Slot312IntVarClass& row : kSlot312IntVarClasses) {
        if (row.wrapper_rva == wrapper_rva) return &row;
    }
    return nullptr;
}

Slot312IntVarResult init_slot312_int_var(ConCmdRangedVar& var, const Slot312IntVarSpec& spec,
                                        ConCmdRegistry& registry) noexcept {
    Slot312IntVarResult out{};
    const Slot312IntVarClass* const cls = slot312_int_var_class_for(spec.wrapper_rva);
    if (cls == nullptr || cls->leaf_rva != spec.leaf_rva || cls->dtor_rva != spec.dtor_rva) {
        return out;   // the spec and the pinned class row disagree: refuse, write nothing
    }

    // Wrapper 0xa45bec / 0xa45e2c: `[obj] = final vptr` before the leaf runs.
    var.base.vptr = cls->final_vptr;

    // The call site's argument block: q[0] is a self-pointer fixed up by the q[3] rule inside, q[2]
    // is zero, q[1] and q[3] are the two pinned identity values, and the source functor class is the
    // site's tag with payload 0 / aux 0.
    RangedVarArgs block{};
    block.q[1] = spec.arg_q1;
    block.q[3] = spec.arg_q3;
    block.fn_vptr = cls->arg_fn_tag;

    const SharedStr name = l2::measured_view(spec.name, spec.name_len);
    const SharedStr description = var_description_view();

    // Leaf 0x3a980 / 0xa45fa0: 0x20af0(this, name, description, 1), the mid vptr, the value cell
    // (this object's own +0xd0), both bounds disengaged, the argument copy and the functor install.
    init_concmd_base_20af0(var.base, name, description, kSlot312CtorFlags);
    var.base.vptr = cls->mid_vptr;
    var.value_ref = &var.value;                       // `lea rsi,[rbx+0xd0]` in the wrapper
    std::memset(var.min_value, 0, sizeof var.min_value);   // byte [obj+0x6c] = 0
    var.min_engaged = 0;                                   // byte [obj+0x70] = 0
    std::memset(var.max_value, 0, sizeof var.max_value);   // byte [obj+0x74] = 0
    var.max_engaged = 0;                                   // byte [obj+0x78] = 0
    std::memset(var.args, 0, sizeof var.args);             // ymm [obj+0x80] = 0
    var.fn_vptr = cls->seed_fn_tag;                        // qword [obj+0xc0] = 0x1595660 / 0x15d18c0
    var.fn_payload = 0;                                    // qword [obj+0xb0] = 0
    var.fn_aux = 0;                                        // qword [obj+0xc8] = 0
    install_ranged_fn_from_args(var, block, cls->final_fn_tag);
    out.functor_slot_calls = kSlot312VarFunctorDispatches;

    // Wrapper tail: 0xf590(description), then `call [arg tag + 0x18]` on the wrapper's local functor
    // block (bare ret for both classes), then the final vptr and the two reads of the caller's cell.
    out.description_inert = release_inert_view_f590(description);
    out.local_functor_inert = true;   // slot 3 of 0x15d18a0 and 0x15d18c0 is a bare `ret` (pinned)
    var.base.vptr = cls->final_vptr;
    var.value = spec.value;           // dword [obj+0xd0] <- *value_cell
    var.default_value = spec.value;   // dword [obj+0x68] <- *value_cell (a second load)
    out.registration = register_concmd_20d60(var.base, registry);
    out.ok = out.description_inert && out.local_functor_inert && out.registration.ok;
    return out;
}

Slot312Hooks default_slot312_hooks() noexcept {
    Slot312Hooks hooks{};
    hooks.var_ctor = &init_slot312_int_var;
    return hooks;
}

DebugMenuTarget slot312_int_var_menu_target(const ConCmdRangedVar& var) noexcept {
    DebugMenuTarget out{};
    out.object = &var;
    for (const Slot312IntVarClass& row : kSlot312IntVarClasses) {
        if (var.base.vptr != row.final_vptr) continue;
        out.slot3 = row.slot3;
        out.slot6 = row.slot6;
        out.known = true;
        break;
    }
    return out;
}

bool slot312_inert_functor_destroy(const Slot312FunctorSite& site) noexcept {
    return site.slot3_is_bare_ret;
}

Slot312Result init_slot312_a46130(Slot312Objects& o, ConCmdRegistry& concmd, DebugMenuRegistry& menu,
                                  const Slot312Hooks& hooks) noexcept {
    Slot312Result out{};
    l2::StageFail<Slot312Result, Slot312Stage> fail{out};

    // 0xa46144..0xa46178: guard = *[0x15fe518] stored in the frame. The body's only branch is the
    // epilogue compare at 0xa465f8; a mismatch calls __stack_chk_fail at 0xa4660c.
    const std::uint64_t guard_entry = hooks.stack_guard != nullptr ? *hooks.stack_guard : 0;

    // 0xa4614f..0xa461d3: `titleskuid` string variable, init/default view "DEFAULT", then the two
    // stack-view releases and __cxa_atexit(0x23c90).
    {
        const SharedStr name = l2::literal_view(kSlot312SkuName);
        const SharedStr init = l2::literal_view(kSlot312SkuDefault);
        out.string_var = init_string_var_257a0(o.title_sku, name, init, concmd);
        if (!out.string_var.ok) return (void)fail.at(Slot312Stage::title_sku_build), out;
        const bool views = release_inert_view_f590(init) && release_inert_view_f590(name);
        out.views_inert = out.views_inert && views;
        if (!views) return (void)fail.at(Slot312Stage::title_sku_view_release), out;
        if (!concmd.atexit_log.record(kSlot312StringVarDtorRva, &o.title_sku)) {
            return (void)fail.at(Slot312Stage::title_sku_atexit), out;
        }
        ++out.atexit_records;
        ++out.vars_built;
    }

    // 0xa461d8..0xa46287: `titleAgeRestriction` (class A, value cell -1), row-local functor destroy,
    // f590(name), __cxa_atexit(0x18a60).
    {
        const Slot312Stage s = build_int_var(
            o.age_restriction, kSlot312IntVars[0], kSlot312FunctorSites[0], hooks, concmd, out,
            Slot312Stage::age_restriction_build, Slot312Stage::age_restriction_view_release,
            Slot312Stage::age_restriction_build, Slot312Stage::age_restriction_atexit);
        if (s != Slot312Stage::none) return (void)fail.at(s), out;
    }

    // 0xa4628c..0xa46338: `titleRegion` (class B, value cell 0), the same four steps.
    {
        const Slot312Stage s = build_int_var(
            o.region, kSlot312IntVars[1], kSlot312FunctorSites[1], hooks, concmd, out,
            Slot312Stage::region_build, Slot312Stage::region_view_release, Slot312Stage::region_build,
            Slot312Stage::region_atexit);
        if (s != Slot312Stage::none) return (void)fail.at(s), out;
    }

    // 0xa4633d..0xa463c6: menu group "Platform Services" / "TitleInfo" (0x26c10), register, the two
    // stack-view releases (caption then parent), __cxa_atexit(0x272d0).
    {
        const SharedStr parent = l2::literal_view(kSlot312GroupParent);
        const SharedStr caption = l2::literal_view(kSlot312MenuLabel);
        if (!init_menu_group_26c10(o.group, parent, caption, kSlot312ItemFlags)) {
            return (void)fail.at(Slot312Stage::group_ctor), out;
        }
        out.group_registration = register_menu_item_268b0(o.group, menu, concmd.atexit_log);
        if (!out.group_registration.ok) return (void)fail.at(Slot312Stage::group_registration), out;
        ++out.menu_items;
        const bool views = release_inert_view_f590(caption) && release_inert_view_f590(parent);
        out.views_inert = out.views_inert && views;
        if (!views) return (void)fail.at(Slot312Stage::group_view_release), out;
        if (!record_menu_item_dtor_272d0(concmd.atexit_log, o.group)) {
            return (void)fail.at(Slot312Stage::group_atexit), out;
        }
        ++out.atexit_records;
    }

    // 0xa463cb..0xa46447: toggle "Sku Name" (target = titleskuid), text then label release, atexit.
    {
        const SharedStr label = l2::literal_view(kSlot312MenuLabel);
        const SharedStr text = l2::literal_view(kSlot312ToggleSkuText);
        if (!init_debug_menu_item_26ea0(o.toggle_sku, label, text, string_var_menu_target(o.title_sku),
                                        kSlot312ItemFlags)) {
            return (void)fail.at(Slot312Stage::toggle_sku_ctor), out;
        }
        out.toggle_registrations[0] = register_menu_item_268b0(o.toggle_sku, menu, concmd.atexit_log);
        if (!out.toggle_registrations[0].ok) return (void)fail.at(Slot312Stage::toggle_sku_registration), out;
        ++out.menu_items;
        const bool views = release_inert_view_f590(text) && release_inert_view_f590(label);
        out.views_inert = out.views_inert && views;
        if (!views) return (void)fail.at(Slot312Stage::toggle_sku_view_release), out;
        if (!record_menu_item_dtor_272d0(concmd.atexit_log, o.toggle_sku)) {
            return (void)fail.at(Slot312Stage::toggle_sku_atexit), out;
        }
        ++out.atexit_records;
    }

    // 0xa4644c..0xa464cf: toggle "Age Restriction" (target = titleAgeRestriction).
    {
        const SharedStr label = l2::literal_view(kSlot312MenuLabel);
        const SharedStr text = l2::literal_view(kSlot312ToggleAgeText);
        if (!init_debug_menu_item_26ea0(o.toggle_age, label, text,
                                        slot312_int_var_menu_target(o.age_restriction), kSlot312ItemFlags)) {
            return (void)fail.at(Slot312Stage::toggle_age_ctor), out;
        }
        out.toggle_registrations[1] = register_menu_item_268b0(o.toggle_age, menu, concmd.atexit_log);
        if (!out.toggle_registrations[1].ok) return (void)fail.at(Slot312Stage::toggle_age_registration), out;
        ++out.menu_items;
        const bool views = release_inert_view_f590(text) && release_inert_view_f590(label);
        out.views_inert = out.views_inert && views;
        if (!views) return (void)fail.at(Slot312Stage::toggle_age_view_release), out;
        if (!record_menu_item_dtor_272d0(concmd.atexit_log, o.toggle_age)) {
            return (void)fail.at(Slot312Stage::toggle_age_atexit), out;
        }
        ++out.atexit_records;
    }

    // 0xa464d4..0xa46557: toggle "Region" (target = titleRegion).
    {
        const SharedStr label = l2::literal_view(kSlot312MenuLabel);
        const SharedStr text = l2::literal_view(kSlot312ToggleRegionText);
        if (!init_debug_menu_item_26ea0(o.toggle_region, label, text,
                                        slot312_int_var_menu_target(o.region), kSlot312ItemFlags)) {
            return (void)fail.at(Slot312Stage::toggle_region_ctor), out;
        }
        out.toggle_registrations[2] = register_menu_item_268b0(o.toggle_region, menu, concmd.atexit_log);
        if (!out.toggle_registrations[2].ok) return (void)fail.at(Slot312Stage::toggle_region_registration), out;
        ++out.menu_items;
        const bool views = release_inert_view_f590(text) && release_inert_view_f590(label);
        out.views_inert = out.views_inert && views;
        if (!views) return (void)fail.at(Slot312Stage::toggle_region_view_release), out;
        if (!record_menu_item_dtor_272d0(concmd.atexit_log, o.toggle_region)) {
            return (void)fail.at(Slot312Stage::toggle_region_atexit), out;
        }
        ++out.atexit_records;
    }

    // 0xa4655c..0xa465e5: the 0x80 B record at 0x32f74f0, its two empty views and __cxa_atexit(0xa44ac0).
    {
        o.record.name = l2::measured_view(kSlot312RecordNameRva, 7);
        o.record.word20 = kSlot312RecordWord20;
        o.record.block24 = kSlot312RecordBlock24;
        o.record.word34 = kSlot312RecordWord34;
        o.record.word38 = kSlot312RecordWord38;
        o.record.word3c = kSlot312RecordWord3c;
        init_slot_f180(&o.record.empty40);
        init_slot_f180(&o.record.empty60);
        if (o.record.name.size != 7 || o.record.empty40.ctl != nullptr || o.record.empty60.ctl != nullptr) {
            return (void)fail.at(Slot312Stage::record_stores), out;
        }
        if (!concmd.atexit_log.record(kSlot312RecordDtorRva, &o.record)) {
            return (void)fail.at(Slot312Stage::record_views), out;
        }
        ++out.atexit_records;
    }

    // 0xa465ea..0xa4660b: canary compare and epilogue. A changed guard would take `jne 0xa4660c`.
    if (hooks.stack_guard != nullptr) {
        out.canary_checked = true;
        out.canary_matched = *hooks.stack_guard == guard_entry;
        if (!out.canary_matched) return (void)fail.at(Slot312Stage::canary), out;
    }

    out.ok = true;
    return out;
}

}  // namespace sotc
