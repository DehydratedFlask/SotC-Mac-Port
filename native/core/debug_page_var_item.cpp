// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01: see core/debug_page_var_item.hpp for the cluster, the exact
// statement sequence and the modelled-vs-hook record. Nothing here loads, relocates or executes an
// original RVA: the class tags, functor tags and destructor RVAs are identity numbers, recorded only.
#include "debug_page_var_item.hpp"

#include <cstring>

#include "fnv_cstr_161fb0.hpp"

namespace sotc::debug_page {

namespace {

// Borrowed view over the pinned (data, immediate length) pair of the original's stack view.
SharedStr borrowed_view(const char* text, std::uint32_t length) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(length), 0, nullptr};
}

// The 0x26ea0 gate answer of the one class this batch targets (see the header).
DebugMenuTarget float_menu_target(const ConCmdRangedVar& var) noexcept {
    DebugMenuTarget target{};
    target.object = &var;
    target.known = var.base.vptr == kFloatClassVptr;
    target.slot3 = target.known ? kFloatClassSlot3 : 0;
    target.slot6 = target.known ? kFloatClassSlot6 : 0;
    return target;
}

bool script_fits(const Script& script, const Objects& objects) noexcept {
    if (script.vars.size() > kMaxScriptVars || script.items.size() > kMaxScriptItems) return false;
    if (script.hashed_names.size() > kMaxScriptHashedNames) return false;
    if (script.vars.size() > objects.vars.size() || script.items.size() > objects.items.size()) return false;
    if (script.hashed_names.size() > objects.hashed_names.size()) return false;
    for (const MenuRow& row : script.items) {
        if (row.target_var >= script.vars.size()) return false;
    }
    return true;
}

// One variable row: pre-stored class tag, 0xc300, description release, functor destroy, class tag +
// value/default, register, name release, __cxa_atexit(0x20f0). Returns the stage; `none` on success.
Stage build_var(const FloatVarRow& row, ConCmdRangedVar& var, ConCmdRegistry& concmd, FloatVarResult& result,
                Result& out) noexcept {
    // The site selects the caller functor's four bodies through the tag and never stores it. An unpinned
    // tag would dispatch unread code, so it is refused and NOTHING is written.
    if (!is_batch_float_site_tag(row.arg_fn_vtbl) && !is_known_float_arg_functor(row.arg_fn_vtbl)) {
        return Stage::var_ctor;
    }

    const SharedStr name = borrowed_view(row.name, row.name_len);
    const SharedStr description = borrowed_view(row.description, row.description_len);

    // Exact IEEE bits: the site moves the pinned word into a stack cell and passes its address.
    float lo = 0.0f;
    float hi = 0.0f;
    std::memcpy(&lo, &row.lo_bits, sizeof lo);
    std::memcpy(&hi, &row.hi_bits, sizeof hi);

    RangedVarArgs args{};                     // q[0..3] = 0, fn payload 0, aux 0
    args.fn_vptr = kRangedFloatFnVtbl;        // == the real tag's four bodies; never stored by the ctor

    var.base.vptr = kFloatClassVptr;          // the site pre-stores 0x15942a0 + 0x10; the ctor overwrites it
    if (!init_bounded_float_var_c300(var, name, &var.value, lo, hi, kVarFlags, args, description)) {
        return Stage::var_ctor;
    }
    result.ctor_ok = true;

    // `callq 0xf590(description)`: a borrowed stack view, so the release is an exact no-op.
    const bool description_inert = release_inert_view_f590(description);
    if (!description_inert) return Stage::var_view;

    // `callq *0x18(%rax)` with %rax = the site tag: slot 3 is the bare `ret` 0xc4f0.
    result.functor_destroy_ok = true;
    ++out.functors_destroyed;

    // `[obj] = 0x15942b0`, then the two value stores (the site reads its own +0xd0 cell through
    // value_ref, then overwrites both words with the pinned immediate).
    var.base.vptr = kFloatClassVptr;
    var.value = row.value_bits;
    var.default_value = row.value_bits;

    // `callq 0x20d60`.
    result.registration = register_concmd_20d60(var.base, concmd);
    if (!result.registration.ok) return Stage::var_register;

    // `callq 0xf590(name)`.
    const bool name_inert = release_inert_view_f590(name);
    result.views_inert = description_inert && name_inert;
    if (!name_inert) return Stage::var_view;

    // `__cxa_atexit(0x20f0, obj, 0x1604218)`.
    result.atexit_recorded = concmd.atexit_log.record(kFloatVarDtorRva, &var);
    if (!result.atexit_recorded) return Stage::var_atexit;
    ++out.atexit_records;
    result.ok = true;
    return Stage::none;
}

// One item row: 0x27050 / 0x26ea0, 0x268b0, text release, label release, __cxa_atexit(0x272d0).
Stage build_item(const MenuRow& row, DebugMenuItem& item, const ConCmdRangedVar& target_var,
                 ConCmdRegistry& concmd, DebugMenuRegistry& menu, MenuSiteResult& result,
                 Result& out) noexcept {
    const SharedStr label = borrowed_view(row.label, row.label_len);
    const SharedStr text = borrowed_view(row.text, row.text_len);

    if (row.kind == MenuKind::slider_27050) {
        if (!init_debug_menu_item_27050(item, label, text, &target_var, kItemFlags, row.range_min,
                                        row.range_max)) {
            return Stage::item_ctor;
        }
    } else {
        const DebugMenuTarget target = float_menu_target(target_var);
        if (!target.known) return Stage::item_ctor;   // an unpinned class would run unlifted slot code
        if (!init_debug_menu_item_26ea0(item, label, text, target, kItemFlags)) return Stage::item_ctor;
    }
    result.ctor_ok = true;

    // 0x268b0 (the ctor's tail call in the original).
    result.registration = register_debug_menu_item_268b0(item, menu, concmd.atexit_log);
    if (!result.registration.ok) return Stage::item_register;

    // `callq 0xf590(text)` then `callq 0xf590(label)`.
    const bool text_inert = release_inert_view_f590(text);
    const bool label_inert = release_inert_view_f590(label);
    result.views_inert = text_inert && label_inert;
    if (!text_inert || !label_inert) return Stage::item_view;

    // `__cxa_atexit(0x272d0, item, 0x1604218)`.
    result.atexit_recorded = concmd.atexit_log.record(kMenuItemDtorRva, &item);
    if (!result.atexit_recorded) return Stage::item_atexit;
    ++out.atexit_records;
    result.ok = true;
    return Stage::none;
}

}  // namespace

bool is_batch_float_site_tag(std::uint64_t tag) noexcept {
    return tag == kArgFnVtblSlot361 || tag == kArgFnVtblSlot467 || tag == kArgFnVtblSlot379 ||
           tag == kArgFnVtblSlot536 || tag == kArgFnVtblSlot374 || tag == kArgFnVtblSlot518;
}

const char* stage_name(Stage stage) noexcept {
    switch (stage) {
        case Stage::none: return "none";
        case Stage::shape: return "shape";
        case Stage::var_ctor: return "var_ctor";
        case Stage::var_functor: return "var_functor";
        case Stage::var_view: return "var_view";
        case Stage::var_register: return "var_register";
        case Stage::var_atexit: return "var_atexit";
        case Stage::item_ctor: return "item_ctor";
        case Stage::item_register: return "item_register";
        case Stage::item_view: return "item_view";
        case Stage::item_atexit: return "item_atexit";
        case Stage::canary: return "canary";
    }
    return "";
}

Result init_debug_page(const Script& script, const Objects& objects, ConCmdRegistry& concmd,
                       DebugMenuRegistry& menu, const Hooks& hooks) noexcept {
    Result out{};
    if (!script_fits(script, objects)) {
        out.blocked = Stage::shape;
        return out;
    }

    // Prologue: `guard <- *[0x15fe518]` stored at [rsp+0xc0]. The epilogue re-reads the cell and
    // compares; a mismatch takes `jne <__stack_chk_fail>`.
    const std::uint64_t guard_entry = hooks.stack_guard != nullptr ? *hooks.stack_guard : 0;

    // Slot 374 only: the leading `{name, fnv1a(name)}` record, written before the first variable row.
    for (std::size_t index = 0; index < script.hashed_names.size(); ++index) {
        const HashedNameRow& row = script.hashed_names[index];
        NameHashCell& cell = objects.hashed_names[index];
        cell.name = row.name;                              // `mov [cell], name`
        cell.hash = fnv_cstr_52a990(row.name);             // `callq 0x52a990` then `mov [cell+8], eax`
        ++out.hashed_names_built;
    }

    for (std::size_t index = 0; index < script.vars.size(); ++index) {
        out.blocked_index = static_cast<std::uint32_t>(index);
        const Stage stage = build_var(script.vars[index], objects.vars[index], concmd, out.vars[index], out);
        if (stage != Stage::none) {
            out.blocked = stage;
            return out;
        }
        ++out.vars_built;
    }

    for (std::size_t index = 0; index < script.items.size(); ++index) {
        out.blocked_index = static_cast<std::uint32_t>(index);
        const MenuRow& row = script.items[index];
        const Stage stage =
            build_item(row, objects.items[index], objects.vars[row.target_var], concmd, menu,
                       out.items[index], out);
        if (stage != Stage::none) {
            out.blocked = stage;
            return out;
        }
        ++out.items_built;
    }

    out.blocked_index = 0;
    if (hooks.stack_guard != nullptr) {
        out.canary_checked = true;
        out.canary_matched = *hooks.stack_guard == guard_entry;
        if (!out.canary_matched) {
            out.blocked = Stage::canary;
            return out;
        }
    }

    out.ok = true;
    return out;
}

}  // namespace sotc::debug_page
