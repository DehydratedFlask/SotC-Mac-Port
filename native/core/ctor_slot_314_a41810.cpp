// CPU-01-STARTUP-SLOT-314-CHARACTERIZATION-01 / -NATIVE-LIFT-01: see ctor_slot_314_a41810.hpp.
#include "ctor_slot_314_a41810.hpp"

namespace sotc {

namespace {

// The borrowed stack description view every variable of this body builds: {empty rodata, 0, 0, 0}.
SharedStr empty_description_view() noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(kSlot314EmptyText), 0, 0, nullptr};
}

std::uintptr_t page_address(const Slot314Page& page, std::size_t offset) noexcept {
    return reinterpret_cast<std::uintptr_t>(reinterpret_cast<const std::uint8_t*>(&page) + offset);
}

// The original's argument block for the five 0xa41570 sites: q[0] a self pointer, q[1] the fixed
// 0x274710, q[2] zero, q[3] the per-site on-change tag, payload 0, aux 0, tag 0x15d1750.
RangedVarArgs bool_callback_args(const Slot314VarSpec& spec) noexcept {
    RangedVarArgs args{};
    args.q[0] = 0;
    args.q[1] = spec.arg_q1;
    args.q[2] = 0;
    args.q[3] = spec.arg_q3;
    args.fn_payload = 0;
    args.fn_vptr = spec.fn_tag;
    args.fn_aux = 0;
    return args;
}

// The argument block of the three 0xc300 sites: four zero qwords, payload 0, tag 0x15d1770, aux 0.
RangedVarArgs float_args() noexcept {
    RangedVarArgs args{};
    args.fn_vptr = kSlot314FloatArgFnVtbl;
    return args;
}

// The argument copy at +0x70 and the collapsed functor install of 0xa41570. The callee performs two
// four-dispatch installs (locals first, then the object's own functor at +0x90) plus a destroy each;
// their net state is exactly these stores, with 0x1594280 (what the 0xc2a0/0xc2c0 copy slots store)
// as the final tag. `vtbl` is the class tag the install stores.
void install_bool_fn_from_args(Slot314BoolVar& var, const RangedVarArgs& args,
                               std::uint64_t vtbl) noexcept {
    var.args[0] = args.q[0];
    var.args[1] = args.q[1];
    var.args[2] = args.q[2];
    var.args[3] = args.q[3];
    if (args.q[3] != 0) {
        var.args[0] = reinterpret_cast<std::uint64_t>(&var.args[0]);   // [obj+0x70] = obj+0x70
    }
    var.fn_payload = args.fn_payload;
    var.fn_vptr = vtbl;
    var.fn_aux = args.fn_aux;
}

}  // namespace

const char* slot314_stage_name(Slot314Stage stage) noexcept {
    switch (stage) {
        case Slot314Stage::none: return "none";
        case Slot314Stage::page_ctor: return "page_ctor";
        case Slot314Stage::page_stores: return "page_stores";
        case Slot314Stage::page_atexit: return "page_atexit";
        case Slot314Stage::page_tail: return "page_tail";
        case Slot314Stage::variable_build: return "variable_build";
        case Slot314Stage::variable_registration: return "variable_registration";
        case Slot314Stage::variable_view_release: return "variable_view_release";
        case Slot314Stage::variable_atexit: return "variable_atexit";
        case Slot314Stage::item_ctor: return "item_ctor";
        case Slot314Stage::item_registration: return "item_registration";
        case Slot314Stage::item_view_release: return "item_view_release";
        case Slot314Stage::item_atexit: return "item_atexit";
        case Slot314Stage::cell_atexit: return "cell_atexit";
        case Slot314Stage::canary: return "canary";
    }
    return "";
}

Slot314PageResult init_slot314_page_475c0(Slot314Page& page, const SharedStr& name) noexcept {
    Slot314PageResult out{};
    // 0x475c0: lea rax,[0x15958f0]; add rax,0x10; mov [rdi],rax  (the BASE tag)
    page.vptr = kSlot314PageCtorVptr;
    page.byte08 = 0;                                   // 0x475d6 byte [rdi+8] = 0
    page.byte09 = 1;                                   // 0x475da byte [rdi+9] = 1
    page.name = reinterpret_cast<std::uint64_t>(name.data);   // 0x475de [rdi+0x10] = rsi
    page.word18 = 0;                                   // 0x475e2 dword [rdi+0x18] = edx
    std::memset(page.zero1c, 0, sizeof page.zero1c);   // 0x475e5 ymm [rdi+0x1c] = 0
    page.link40 = page_address(page, 0x38);            // 0x475f1 [rdi+0x40] = rdi+0x38
    page.link48 = page_address(page, 0x38);            // 0x475f5 [rdi+0x48] = rdi+0x38
    page.word50 = 0;                                   // 0x475f9
    page.word58 = 0;                                   // 0x47601 dword [rdi+0x58] = 0
    page.word60 = 0;                                   // 0x4760a xchg [rdi+0x60],rax with rax = 0
    page.word70 = 0;                                   // 0x4760e
    out.ok = name.data != nullptr && name.size == static_cast<std::int64_t>(sizeof kSlot314PageName - 1) &&
             name.ctl == nullptr;
    return out;
}

void init_slot314_page_stores(Slot314Page& page) noexcept {
    // 0xa41852..0xa4187d: lea rax,[0x1594c90]; add rax,0x10; mov [page],rax -- the derived vptr.
    page.vptr = kSlot314PageVptr;
    // 0xa41884: qword [page+0x80] = 0; 0xa4188f: dword [page+0x88] = 0.
    page.word80 = 0;
    page.word88 = 0;
    // 0xa41899: ymmword [page+0x90] = 0.
    std::memset(page.zero90, 0, sizeof page.zero90);
    // 0xa418b9..0xa41a12: twelve self-referential nodes; eleven of them are followed by 16 zero bytes.
    for (std::size_t i = 0; i < page.nodes.size(); ++i) {
        Slot314Node& node = page.nodes[i];
        node.next = &node;
        node.prev = &node;
        std::memset(node.zero20, 0, sizeof node.zero20);
    }
    Slot314NodeTail& last = page.last_node;
    last.next = &last;
    last.prev = &last;
}

bool init_slot314_float_var_c300(ConCmdRangedVar& var, const Slot314VarSpec& spec) noexcept {
    if (spec.shape != Slot314VarShape::ranged_float ||
        spec.fn_tag != kSlot314FloatArgFnVtbl) {
        return false;   // the spec and the pinned argument class disagree: refuse, write nothing
    }
    float lo = 0.0f;
    float hi = 0.0f;
    std::memcpy(&lo, &spec.float_min_bits, 4);
    std::memcpy(&hi, &spec.float_max_bits, 4);
    const SharedStr name = l2::measured_view(spec.name, spec.name_len);
    const SharedStr description = empty_description_view();
    const RangedVarArgs args = float_args();

    // 0xc336: call 0x20af0(this, name, description, flags).
    init_concmd_base_20af0(var.base, name, description, spec.flags);
    // 0xc33b..0xc38e: the mid vptr, the caller's value cell (this object's own +0xd0), both bounds
    // engaged with their floats, the zeroed argument copy and the seed functor tag.
    var.base.vptr = kSlot314FloatMidVptr;
    var.value_ref = &var.value;
    var.min_value[0] = 0;
    var.min_engaged = 1;
    std::memcpy(var.min_value, &lo, 4);
    var.max_value[0] = 0;
    var.max_engaged = 1;
    std::memcpy(var.max_value, &hi, 4);
    std::memset(var.args, 0, sizeof var.args);
    var.fn_vptr = kSlot314FloatFnVtbl;
    var.fn_payload = 0;
    var.fn_aux = 0;
    // The tail: the four argument-functor dispatches collapse to this net state (0xc4a0/0xc4c0 store
    // 0x1594318, the destroy slot 0xc4f0 is a bare ret).
    install_ranged_fn_from_args(var, args, kSlot314FloatFnVtbl);
    return true;
}

Slot314VarResult init_slot314_bool_var_callback(Slot314BoolVar& var, const Slot314VarSpec& spec,
                                                ConCmdRegistry& registry) noexcept {
    Slot314VarResult out{};
    if (spec.shape != Slot314VarShape::bool_callback ||
        spec.fn_tag != kSlot314BoolArgFnVtbl) {
        return out;   // the spec and the pinned argument class disagree: refuse, write nothing
    }
    const SharedStr name = l2::measured_view(spec.name, spec.name_len);
    const SharedStr description = empty_description_view();
    const RangedVarArgs args = bool_callback_args(spec);

    // 0xa415c2: [this] = 0x1594218 BEFORE the base constructor (which overwrites it).
    var.base.vptr = kSlot314BoolFinalVptr;
    init_concmd_base_20af0(var.base, name, description, spec.flags);   // 0xa416c0
    var.base.vptr = kSlot314BoolMidVptr;                               // 0xa416d7
    var.value_ref = &var.value;                                        // 0xa416e1 [obj+0x60]
    std::memset(var.zero69, 0, sizeof var.zero69);                     // 0xa416e5 dword [obj+0x69]
    var.fn_vptr = spec.fn_tag;                                         // 0xa416ec (the argument tag)
    var.fn_payload = 0;                                                // 0xa416f3 [obj+0xa0]
    var.fn_aux = 0;                                                    // 0xa416fe [obj+0xb8]
    // 0xa41711..0xa4172b: the 32 B argument copy at +0x70 and its self-pointer fixup.
    install_bool_fn_from_args(var, args, kSlot314BoolCallbackFnVtbl);
    out.functor_dispatches = kSlot314BoolCallbackDispatches;
    out.local_functor_inert = true;   // both destroy slots of 0x15d1750 are the bare ret 0xc2f0

    // 0xa417a9: 0xf590 on the stack description view (borrowed, so an exact no-op).
    out.description_inert = release_inert_view_f590(description);

    // 0xa417c6..0xa417dd: the final vptr and the single value byte read twice from the caller's cell.
    var.base.vptr = kSlot314BoolFinalVptr;
    var.value = spec.bool_value;
    var.default_value = spec.bool_default;

    out.registration = register_concmd_20d60(var.base, registry);   // 0xa417e0
    out.ok = out.description_inert && out.local_functor_inert && out.registration.ok;
    return out;
}

Slot314Hooks default_slot314_hooks() noexcept {
    Slot314Hooks hooks{};
    hooks.page_ctor = &init_slot314_page_475c0;
    hooks.bool_callback_ctor = &init_slot314_bool_var_callback;
    return hooks;
}

DebugMenuTarget slot314_var_menu_target(const ConCmdBase& cmd) noexcept {
    DebugMenuTarget out{};
    out.object = &cmd;
    if (cmd.vptr == kSlot314BoolFinalVptr) {
        out.slot3 = kSlot314BoolGate3;
        out.slot6 = kSlot314BoolGate6;
        out.known = true;
    } else if (cmd.vptr == kSlot314FloatFinalVptr) {
        out.slot3 = kSlot314FloatGate3;
        out.slot6 = kSlot314FloatGate6;
        out.known = true;
    }
    return out;
}

bool slot314_inert_functor_destroy(const Slot314FunctorSite& site) noexcept {
    return site.slot3_is_bare_ret;
}

ConCmdBase& slot314_var_base(Slot314Objects& objects, std::uint32_t index) noexcept {
    static ConCmdBase fallback{};
    switch (index) {
        case 0: return objects.var0.base;
        case 1: return objects.var1.base;
        case 2: return objects.var2.base;
        case 3: return objects.var3.base;
        case 4: return objects.var4.base;
        case 5: return objects.var5.base;
        case 6: return objects.var6.base;
        case 7: return objects.var7.base;
        case 8: return objects.var8.base;
        case 9: return objects.var9.base;
        default: return fallback;
    }
}

DebugMenuItem& slot314_item(Slot314Objects& objects, std::uint32_t index) noexcept {
    switch (index) {
        case 0: return objects.item0;
        case 1: return objects.item1;
        case 2: return objects.item2;
        case 3: return objects.item3;
        case 4: return objects.item4;
        case 5: return objects.item5;
        case 6: return objects.item6;
        case 7: return objects.item7;
        case 8: return objects.item8;
        default: return objects.item9;
    }
}

Slot314Result init_slot314_a41810(Slot314Objects& o, ConCmdRegistry& concmd, DebugMenuRegistry& menu,
                                  const Slot314Hooks& hooks) noexcept {
    Slot314Result out{};
    l2::StageFail<Slot314Result, Slot314Stage> fail{out};

    // 0xa41828..0xa41845: guard <- *[*[0xbbcd08]], stored at [rsp+0xc0]. The body's only branch is the
    // epilogue compare at 0xa428e2; a mismatch calls __stack_chk_fail at 0xa428f3.
    const std::uint64_t guard_entry = hooks.stack_guard != nullptr ? *hooks.stack_guard : 0;

    // 0xa4184d: the page constructor, then the body's own page stores and its atexit record.
    {
        if (hooks.page_ctor == nullptr) return (void)fail.at(Slot314Stage::page_ctor), out;
        out.page = hooks.page_ctor(o.page, l2::literal_view(kSlot314PageName));
        if (!out.page.ok) return (void)fail.at(Slot314Stage::page_ctor), out;
        init_slot314_page_stores(o.page);
        if (!concmd.atexit_log.record(kSlot314PageDtorRva, &o.page)) {
            return (void)fail.at(Slot314Stage::page_atexit), out;
        }
        ++out.atexit_records;
    }

    // 0xa41a47: the first variable's step writes dword 1 at page+0x2e0 (the last node's +0x20 slot).
    o.page.tail2e0 = 1;

    for (std::uint32_t index = 0; index < kSlot314VarCount; ++index) {
        const Slot314VarSpec& spec = kSlot314Vars[index];
        out.blocked_index = index;

        // ---- the variable -------------------------------------------------------------------
        if (spec.shape == Slot314VarShape::bool_inline) {
            // The inline shape: exactly the shared bool-variable model (0x20af0 + 0x15940b0 mid /
            // 0x1594218 final, functor tag 0x15d1750, 0x20d60, both releases and the atexit record).
            ConCmdBoolVar& var = index == 0 ? o.var0 : o.var1;
            const BoolVarResult built =
                init_bool_var_static(var, l2::measured_view(spec.name, spec.name_len),
                                     empty_description_view(), spec.fn_tag, concmd, spec.bool_value,
                                     spec.bool_default);
            out.views_inert = out.views_inert && built.views_inert;
            if (!built.ok) return (void)fail.at(Slot314Stage::variable_build), out;
            out.var_results[index].ok = true;
            out.var_results[index].description_inert = built.views_inert;
            out.var_results[index].registration = built.registration;
            ++out.vars_built;
            ++out.atexit_records;
        } else if (spec.shape == Slot314VarShape::ranged_float) {
            ConCmdRangedVar& var = index == 2 ? o.var2 : (index == 3 ? o.var3 : o.var4);
            if (!init_slot314_float_var_c300(var, spec)) {
                return (void)fail.at(Slot314Stage::variable_build), out;
            }
            // 0xa41e03: 0xf590 on the stack description view.
            const bool desc_inert = release_inert_view_f590(empty_description_view());
            out.views_inert = out.views_inert && desc_inert;
            out.var_results[index].description_inert = desc_inert;
            if (!desc_inert) return (void)fail.at(Slot314Stage::variable_view_release), out;
            // 0xa41e1b / 0xa41fc4 / 0xa42167: the argument functor destroy (bare ret).
            const Slot314FunctorSite& site = kSlot314FunctorSites[index - 2];
            if (!slot314_inert_functor_destroy(site)) {
                return (void)fail.at(Slot314Stage::variable_build), out;
            }
            ++out.functors_destroyed;
            // 0xa41e21: final vptr, then the value/default pair read from the caller's cell.
            var.base.vptr = kSlot314FloatFinalVptr;
            std::memcpy(&var.value, &spec.float_value_bits, 4);
            std::memcpy(&var.default_value, &spec.float_value_bits, 4);
            out.var_results[index].registration = register_concmd_20d60(var.base, concmd);
            if (!out.var_results[index].registration.ok) {
                return (void)fail.at(Slot314Stage::variable_registration), out;
            }
            if (!release_inert_view_f590(l2::measured_view(spec.name, spec.name_len))) {
                return (void)fail.at(Slot314Stage::variable_view_release), out;
            }
            if (!concmd.atexit_log.record(kSlot314FloatVarDtorRva, &var)) {
                return (void)fail.at(Slot314Stage::variable_atexit), out;
            }
            out.var_results[index].ok = true;
            ++out.vars_built;
            ++out.atexit_records;
        } else {
            if (hooks.bool_callback_ctor == nullptr) return (void)fail.at(Slot314Stage::variable_build), out;
            Slot314BoolVar& var = index == 5   ? o.var5
                                  : index == 6 ? o.var6
                                  : index == 7 ? o.var7
                                  : index == 8 ? o.var8
                                               : o.var9;
            out.var_results[index] = hooks.bool_callback_ctor(var, spec, concmd);
            out.views_inert = out.views_inert && out.var_results[index].description_inert;
            out.functor_dispatches += out.var_results[index].functor_dispatches;
            if (!out.var_results[index].ok) return (void)fail.at(Slot314Stage::variable_build), out;
            ++out.vars_built;
            // 0xa4232d / 0xa42472 / 0xa425b1 / 0xa426f0 / 0xa4282f: the row-local functor destroy.
            const Slot314FunctorSite& site = kSlot314FunctorSites[3 + (index - 5)];
            if (!slot314_inert_functor_destroy(site)) return (void)fail.at(Slot314Stage::variable_build), out;
            ++out.functors_destroyed;
            // 0xa42333: 0xf590 on the stack name view.
            if (!release_inert_view_f590(l2::measured_view(spec.name, spec.name_len))) {
                return (void)fail.at(Slot314Stage::variable_view_release), out;
            }
            if (!concmd.atexit_log.record(kSlot314BoolVarDtorRva, &var)) {
                return (void)fail.at(Slot314Stage::variable_atexit), out;
            }
            ++out.atexit_records;
        }

        // ---- the toggle ----------------------------------------------------------------------
        const Slot314ItemSpec& item_spec = kSlot314Items[index];
        DebugMenuItem& item = slot314_item(o, index);
        const DebugMenuTarget target = slot314_var_menu_target(slot314_var_base(o, index));
        if (!target.known) return (void)fail.at(Slot314Stage::item_ctor), out;
        if (!init_debug_menu_item_26ea0(item, l2::literal_view(kSlot314MenuParent),
                                        l2::measured_view(item_spec.caption, item_spec.caption_len),
                                        target, item_spec.flags)) {
            return (void)fail.at(Slot314Stage::item_ctor), out;
        }
        out.item_registrations[index] = register_debug_menu_item_268b0(item, menu, concmd.atexit_log);
        if (!out.item_registrations[index].ok) {
            return (void)fail.at(Slot314Stage::item_registration), out;
        }
        ++out.menu_items;
        // 0xa41b9f/0xa41ba7: the caption view then the parent view.
        const bool caption_inert = release_inert_view_f590(
            l2::measured_view(item_spec.caption, item_spec.caption_len));
        const bool parent_inert = release_inert_view_f590(l2::literal_view(kSlot314MenuParent));
        out.views_inert = out.views_inert && caption_inert && parent_inert;
        if (!caption_inert || !parent_inert) return (void)fail.at(Slot314Stage::item_view_release), out;
        if (!concmd.atexit_log.record(kDebugMenuItemDtorRva, &item)) {
            return (void)fail.at(Slot314Stage::item_atexit), out;
        }
        ++out.atexit_records;

        // 0xa4223f: the zero cell before the sixth variable's step.
        if (index == 4) {
            o.cell.word = 0;
            if (!concmd.atexit_log.record(kSlot314CellDtorRva, &o.cell)) {
                return (void)fail.at(Slot314Stage::cell_atexit), out;
            }
            ++out.atexit_records;
        }
    }

    // 0xa428d0..0xa428f2: canary compare and epilogue. A changed guard would take `jne 0xa428f3`.
    if (hooks.stack_guard != nullptr) {
        out.canary_checked = true;
        out.canary_matched = *hooks.stack_guard == guard_entry;
        if (!out.canary_matched) return (void)fail.at(Slot314Stage::canary), out;
    }

    out.blocked_index = 0;
    out.ok = true;
    return out;
}

}  // namespace sotc
