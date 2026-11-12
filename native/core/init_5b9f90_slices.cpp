// Native 0x5b9f90 slices. See init_5b9f90_slices.hpp for the measured slice map and the claim.
#include "init_5b9f90_slices.hpp"

#include <cstdio>
#include <cstring>

namespace sotc {
namespace {

SharedStr borrowed(const char* text, std::size_t len) noexcept {
    SharedStr view{};
    view.data = reinterpret_cast<const std::uint8_t*>(text);
    view.size = static_cast<std::int64_t>(len);
    view.aux = 0;
    view.ctl = nullptr;
    return view;
}

struct BuildContext {
    const Init5b9f90Hooks* hooks = nullptr;
    Init5b9f90Result* out = nullptr;
};

bool fail(Init5b9f90Result& out, const char* reason) noexcept {
    out.blocked = reason;
    return false;
}

// The original builds every item in its own 0x27a0 stack frame; the host storage stands in for
// that frame so the root list's pointers stay valid after the slice runner returns.
DebugMenuItem* next_item(const BuildContext& ctx) noexcept {
    Init5b9f90Result& out = *ctx.out;
    if (ctx.hooks->item_storage == nullptr || out.items_used >= ctx.hooks->item_capacity) {
        return nullptr;
    }
    DebugMenuItem* item = &ctx.hooks->item_storage[out.items_used++];
    *item = DebugMenuItem{};
    return item;
}

bool release(const BuildContext& ctx, const SharedStr& view) noexcept {
    if (!ctx.hooks->release_view(ctx.hooks->host, view)) return false;
    ++ctx.out->releases;
    return true;
}

void add_record(Init5b9f90Result& out, const char* id, std::uint64_t start, std::uint64_t end,
                const char* detail) noexcept {
    if (out.record_count >= out.records.size()) return;
    InitPostCallRecord& record = out.records[out.record_count++];
    record.callee_rva = start;
    record.caller = "0x5b9f90";
    record.executed = true;
    record.arg_count = 0;
    record.has_return = false;
    record.return_value = 0;
    record.blocker = nullptr;
    std::snprintf(record.detail, sizeof record.detail, "slice %s 0x%llx..0x%llx: %s", id,
                  static_cast<unsigned long long>(start),
                  static_cast<unsigned long long>(end), detail);
}

// 0x26c10 group item + 0x268b0 registration + the view releases the body issues. S0/S3.1/S3.2
// release both views; S3.3..S3.6 release only the parent view (the caption view is owned by the
// item and released by its 0x272d0 destroy, which is a hook here).
bool build_group(const BuildContext& ctx, const char* parent_text, std::uint8_t parent_len,
                 const char* caption_text, std::uint8_t caption_len, std::uint64_t site_rva,
                 bool release_caption) noexcept {
    Init5b9f90Result& out = *ctx.out;
    if (std::strlen(parent_text) != parent_len || std::strlen(caption_text) != caption_len) {
        (void)site_rva;
        return fail(out, "group_view_length_mismatch");
    }
    const SharedStr parent = borrowed(parent_text, parent_len);
    const SharedStr caption = borrowed(caption_text, caption_len);
    DebugMenuItem* const item = next_item(ctx);
    if (item == nullptr) return fail(out, "item_storage_exhausted");
    if (!init_menu_group_26c10(*item, parent, caption, kInit5b9f90ItemFlags)) {
        return fail(out, "group_ctor_failed");
    }
    ++out.groups;
    if (!register_menu_item_268b0(*item, *ctx.hooks->menu, *ctx.hooks->atexit).ok) {
        return fail(out, "group_register_failed");
    }
    ++out.registered;
    // The body releases the parent view; on S0/S3.1/S3.2 it also releases the caption.
    if (!release(ctx, parent)) return fail(out, "group_release_failed");
    if (release_caption && !release(ctx, caption)) return fail(out, "group_release_failed");
    return true;
}

// 0x26f90 command item (kind 2, target stored verbatim) + registration + the caption/command
// view releases.
bool build_command(const BuildContext& ctx, const SharedStr& parent, const Init5b9f90ItemSpec& spec) noexcept {
    Init5b9f90Result& out = *ctx.out;
    if (std::strlen(spec.caption) != spec.caption_len || spec.command == nullptr ||
        std::strlen(spec.command) != spec.command_len) {
        return fail(out, "command_view_length_mismatch");
    }
    const SharedStr caption = borrowed(spec.caption, spec.caption_len);
    const SharedStr command = borrowed(spec.command, spec.command_len);
    const void* const target = reinterpret_cast<const void*>(
        static_cast<std::uintptr_t>(spec.target_rva));
    DebugMenuItem* const item = next_item(ctx);
    if (item == nullptr) return fail(out, "item_storage_exhausted");
    if (!init_menu_command_26f90(*item, parent, caption, target, command, kInit5b9f90ItemFlags)) {
        return fail(out, "command_ctor_failed");
    }
    ++out.commands;
    if (!register_menu_item_268b0(*item, *ctx.hooks->menu, *ctx.hooks->atexit).ok) {
        return fail(out, "command_register_failed");
    }
    ++out.registered;
    if (!release(ctx, caption) || !release(ctx, command)) {
        return fail(out, "command_release_failed");
    }
    return true;
}

// 0x26ea0 toggle: the target is a host bool stand-in with the measured bool-variable identity
// (slot3 = 1, slot6 = 0). The body releases only the caption view.
bool build_toggle(const BuildContext& ctx, const SharedStr& parent, const Init5b9f90ItemSpec& spec,
                  std::size_t index) noexcept {
    Init5b9f90Result& out = *ctx.out;
    if (std::strlen(spec.caption) != spec.caption_len) return fail(out, "toggle_view_length_mismatch");
    if (ctx.hooks->toggle_value_storage == nullptr || index >= ctx.hooks->toggle_value_capacity) {
        return fail(out, "toggle_storage_missing");
    }
    const SharedStr caption = borrowed(spec.caption, spec.caption_len);
    DebugMenuTarget target{};
    target.object = &ctx.hooks->toggle_value_storage[index];
    target.slot3 = 1;
    target.slot6 = 0;
    target.known = true;
    DebugMenuItem* const item = next_item(ctx);
    if (item == nullptr) return fail(out, "item_storage_exhausted");
    if (!init_debug_menu_item_26ea0(*item, parent, caption, target, kInit5b9f90ItemFlags)) {
        return fail(out, "toggle_ctor_failed");
    }
    ++out.toggles;
    if (!register_menu_item_268b0(*item, *ctx.hooks->menu, *ctx.hooks->atexit).ok) {
        return fail(out, "toggle_register_failed");
    }
    ++out.registered;
    if (!release(ctx, caption)) return fail(out, "toggle_release_failed");
    return true;
}

// 0x27050 float range: caption, target variable tag and the two xmm constants (0.01f / 0.1f).
bool build_float_item(const BuildContext& ctx, const SharedStr& parent,
                      const Init5b9f90ItemSpec& spec) noexcept {
    Init5b9f90Result& out = *ctx.out;
    if (std::strlen(spec.caption) != spec.caption_len) return fail(out, "float_view_length_mismatch");
    const SharedStr caption = borrowed(spec.caption, spec.caption_len);
    const void* const target = reinterpret_cast<const void*>(
        static_cast<std::uintptr_t>(spec.target_rva));
    DebugMenuItem* const item = next_item(ctx);
    if (item == nullptr) return fail(out, "item_storage_exhausted");
    if (!init_debug_menu_item_27050(*item, parent, caption, target, kInit5b9f90ItemFlags,
                                    spec.range_min, spec.range_max)) {
        return fail(out, "float_ctor_failed");
    }
    ++out.float_items;
    if (!register_menu_item_268b0(*item, *ctx.hooks->menu, *ctx.hooks->atexit).ok) {
        return fail(out, "float_register_failed");
    }
    ++out.registered;
    if (!release(ctx, caption)) return fail(out, "float_release_failed");
    return true;
}

// 0x20af0 base ctor, the bool-class stores, __cxa_atexit(0xacf0), 0x20d60 registration and the
// 0x26db0 named item. The original guards the whole construction with one Itanium guard byte per
// variable; the host owns the guard bytes and the variable/value storage.
bool build_named_bool_var(const BuildContext& ctx, const SharedStr& parent,
                          const Init5b9f90ItemSpec& spec, std::size_t index) noexcept {
    Init5b9f90Result& out = *ctx.out;
    if (std::strlen(spec.caption) != spec.caption_len) return fail(out, "var_name_length_mismatch");
    if (ctx.hooks->registry == nullptr || ctx.hooks->var_storage == nullptr ||
        ctx.hooks->var_value_storage == nullptr || ctx.hooks->var_guard_storage == nullptr ||
        ctx.hooks->atexit == nullptr) {
        return fail(out, "var_state_missing");
    }
    if (index >= ctx.hooks->var_capacity || index >= ctx.hooks->var_guard_capacity) {
        return fail(out, "var_storage_exhausted");
    }
    const SharedStr name = borrowed(spec.caption, spec.caption_len);
    const SharedStr empty = borrowed("", 0);
    ConCmdBoolVar& var = ctx.hooks->var_storage[index];
    bool& value = ctx.hooks->var_value_storage[index];
    bool& guard = ctx.hooks->var_guard_storage[index];
    if (!guard) {
        var = ConCmdBoolVar{};
        init_concmd_base_20af0(var.base, name, empty, 1);
        var.base.vptr = kInit5b9f90BoolVarVptr;
        var.value_ref = reinterpret_cast<std::uint8_t*>(&value);
        var.default_value = value ? 1 : 0;
        var.fn_vptr = kInit5b9f90BoolVarFnVptr;
        if (!ctx.hooks->atexit->record(kInit5b9f90BoolVarDtorRva, &var)) {
            return fail(out, "var_atexit_failed");
        }
        const ConCmdRegisterResult r = register_concmd_20d60(var.base, *ctx.hooks->registry);
        if (!r.ok) return fail(out, "var_register_failed");
        guard = true;
    }
    ++out.named_vars;
    DebugMenuTarget target{};
    target.object = &var;
    target.slot3 = 1;
    target.slot6 = 0;
    target.known = true;
    DebugMenuItem* const item = next_item(ctx);
    if (item == nullptr) return fail(out, "item_storage_exhausted");
    if (!init_debug_menu_item_26db0(*item, parent, var.base, target, kInit5b9f90ItemFlags)) {
        return fail(out, "var_item_ctor_failed");
    }
    if (!register_menu_item_268b0(*item, *ctx.hooks->menu, *ctx.hooks->atexit).ok) {
        return fail(out, "var_item_register_failed");
    }
    ++out.registered;
    // The body releases both stack views after the variable definition.
    if (!release(ctx, name) || !release(ctx, empty)) return fail(out, "var_release_failed");
    return true;
}

bool build_item(const BuildContext& ctx, const SharedStr& parent, const Init5b9f90ItemSpec& spec,
                std::size_t toggle_index, std::size_t var_index) noexcept {
    switch (spec.kind) {
        case Init5b9f90ItemKind::command: return build_command(ctx, parent, spec);
        case Init5b9f90ItemKind::toggle: return build_toggle(ctx, parent, spec, toggle_index);
        case Init5b9f90ItemKind::float_item: return build_float_item(ctx, parent, spec);
        case Init5b9f90ItemKind::named_bool_var:
            return build_named_bool_var(ctx, parent, spec, var_index);
    }
    return fail(*ctx.out, "item_kind_unknown");
}

bool build_page(const BuildContext& ctx, const Init5b9f90PageSpec& page,
                std::size_t& toggle_index, std::size_t& var_index) noexcept {
    Init5b9f90Result& out = *ctx.out;
    if (page.item_count != 0 && page.items == nullptr) return fail(out, "page_spec_mismatch");
    const bool release_caption = page.start_rva < 0x5BB4BE;   // S3.1/S3.2 release both views
    if (!build_group(ctx, page.parent, page.parent_len, page.caption, page.caption_len,
                     page.group_site_rva, release_caption)) {
        return false;
    }
    const SharedStr parent = borrowed(page.parent, page.parent_len);
    for (std::size_t i = 0; i < page.item_count; ++i) {
        const Init5b9f90ItemSpec& spec = page.items[i];
        const std::size_t before = out.items_used;
        if (!build_item(ctx, parent, spec, toggle_index, var_index)) return false;
        if (spec.kind == Init5b9f90ItemKind::toggle) ++toggle_index;
        if (spec.kind == Init5b9f90ItemKind::named_bool_var) ++var_index;
        if (out.items_used != before + 1) return fail(out, "item_order_mismatch");
    }
    // The 0x272d0 destroy block: items first, in reverse order, then the group, then the page's
    // parent view (the measured tail release at 0x5bbcca for S3.3 and its S3.4 twin).
    if (page.destroy_count != 0) {
        if (page.destroy_count != page.item_count + 1) return fail(out, "destroy_count_mismatch");
        for (std::size_t i = 0; i < page.item_count; ++i) ++out.destroys;
        ++out.destroys;   // the group
        if (!release(ctx, parent)) return fail(out, "page_tail_release_failed");
    }
    return true;
}

}  // namespace

Init5b9f90Result run_init_5b9f90_slices(const Init5b9f90Hooks& hooks) noexcept {
    Init5b9f90Result out{};
    if (hooks.host == nullptr || hooks.menu == nullptr || hooks.atexit == nullptr ||
        hooks.registry == nullptr || hooks.release_view == nullptr ||
        hooks.page_builder == nullptr || hooks.canary_cell == nullptr ||
        hooks.item_storage == nullptr) {
        out.blocked = "state_missing";
        return out;
    }
    BuildContext ctx{&hooks, &out};

    // 0x5b9fa8..0x5b9fe4: the frame canary store (host token, never an original RVA read).
    *hooks.canary_cell = hooks.canary_token;
    out.canary_stored = true;

    // S0: group "Shadow" under "CP10", then its two view releases.
    if (!build_group(ctx, "CP10", 4, "Shadow", 6, 0x5BA02C, true)) return out;
    add_record(out, "S0", kInit5b9f90Rva, 0x5BA041,
               "canary store, group \"Shadow\" under \"CP10\", 2 releases");
    ++out.executed_slices;

    // S1/S2: the two page builders. Their bodies are not lifted; the host observes the call and
    // the slices stay `contract` in the map.
    {
        const InitPostCallOutcome o = hooks.page_builder(
            hooks.host, kInit5b9f90PageBuilderShadowMiscRva, "Shadow Misc");
        if (!o.executed) {
            out.blocked = o.blocker != nullptr ? o.blocker : "page_builder_2170_refused";
            return out;
        }
        ++out.contracts;
    }
    {
        const InitPostCallOutcome o = hooks.page_builder(
            hooks.host, kInit5b9f90PageBuilderSecondRva, "unnamed page builder");
        if (!o.executed) {
            out.blocked = o.blocker != nullptr ? o.blocker : "page_builder_5cf140_refused";
            return out;
        }
        ++out.contracts;
    }

    // S3.1..S3.6 in body order. Execution stops after S3.6 by construction (S4 is unlifted).
    std::size_t toggle_index = 0;
    std::size_t var_index = 0;
    for (const Init5b9f90PageSpec& page : kInit5b9f90Pages) {
        if (!build_page(ctx, page, toggle_index, var_index)) return out;
        add_record(out, page.id, page.start_rva, page.end_rva, "page executed natively");
        ++out.executed_slices;
    }

    // Everything from 0x5bc3d4 on (S4 ... S9) is unlifted.
    out.resume_rva = kInit5b9f90ResumeRva;
    out.complete = false;
    out.ok = true;
    return out;
}

}  // namespace sotc
