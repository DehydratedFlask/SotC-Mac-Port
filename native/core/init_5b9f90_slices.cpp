// Native 0x5b9f90 slices. See init_5b9f90_slices.hpp for the measured slice map and the claim.
#include "init_5b9f90_slices.hpp"

#include <cstdio>
#include <cstring>

#include "ctor_shapes_343_356.hpp"
#include "helper_c580_ranged_int.hpp"

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
bool build_bool_var_impl(const BuildContext& ctx, const SharedStr& parent, const char* name_text,
                         std::uint8_t name_len, std::size_t index, std::uint64_t fn_vptr) noexcept {
    Init5b9f90Result& out = *ctx.out;
    if (std::strlen(name_text) != name_len) return fail(out, "var_name_length_mismatch");
    if (ctx.hooks->registry == nullptr || ctx.hooks->var_storage == nullptr ||
        ctx.hooks->var_value_storage == nullptr || ctx.hooks->var_guard_storage == nullptr ||
        ctx.hooks->atexit == nullptr) {
        return fail(out, "var_state_missing");
    }
    if (index >= ctx.hooks->var_capacity || index >= ctx.hooks->var_guard_capacity) {
        return fail(out, "var_storage_exhausted");
    }
    const SharedStr name = borrowed(name_text, name_len);
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
        var.fn_vptr = fn_vptr;
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

bool build_named_bool_var(const BuildContext& ctx, const SharedStr& parent,
                          const Init5b9f90ItemSpec& spec, std::size_t index) noexcept {
    return build_bool_var_impl(ctx, parent, spec.caption, spec.caption_len, index,
                               kInit5b9f90BoolVarFnVptr);
}

// ---- S2, the 0x5cf140 "Shadow Credits" page builder ------------------------------------------

bool build_s2_group(const BuildContext& ctx, const Init5cf140ItemSpec& spec) noexcept {
    Init5b9f90Result& out = *ctx.out;
    if (std::strlen(spec.parent) != spec.parent_len || std::strlen(spec.text) != spec.text_len) {
        return fail(out, "s2_group_view_length_mismatch");
    }
    const SharedStr parent = borrowed(spec.parent, spec.parent_len);
    const SharedStr caption = borrowed(spec.text, spec.text_len);
    DebugMenuItem* const item = next_item(ctx);
    if (item == nullptr) return fail(out, "item_storage_exhausted");
    if (!init_menu_group_26c10(*item, parent, caption, kInit5b9f90ItemFlags)) {
        return fail(out, "s2_group_ctor_failed");
    }
    ++out.groups;
    if (!register_menu_item_268b0(*item, *ctx.hooks->menu, *ctx.hooks->atexit).ok) {
        return fail(out, "s2_group_register_failed");
    }
    ++out.registered;
    if (!release(ctx, parent)) return fail(out, "s2_group_release_failed");
    return true;
}

bool build_s2_command(const BuildContext& ctx, const Init5cf140ItemSpec& spec) noexcept {
    Init5b9f90Result& out = *ctx.out;
    if (std::strlen(spec.parent) != spec.parent_len || std::strlen(spec.text) != spec.text_len ||
        spec.command == nullptr || std::strlen(spec.command) != spec.command_len) {
        return fail(out, "s2_command_view_length_mismatch");
    }
    const SharedStr parent = borrowed(spec.parent, spec.parent_len);
    const SharedStr caption = borrowed(spec.text, spec.text_len);
    const SharedStr command = borrowed(spec.command, spec.command_len);
    DebugMenuItem* const item = next_item(ctx);
    if (item == nullptr) return fail(out, "item_storage_exhausted");
    if (!init_menu_command_26f90(*item, parent, caption,
                                 reinterpret_cast<const void*>(static_cast<std::uintptr_t>(
                                     spec.target_rva)),
                                 command, kInit5b9f90ItemFlags)) {
        return fail(out, "s2_command_ctor_failed");
    }
    ++out.commands;
    if (!register_menu_item_268b0(*item, *ctx.hooks->menu, *ctx.hooks->atexit).ok) {
        return fail(out, "s2_command_register_failed");
    }
    ++out.registered;
    if (!release(ctx, caption) || !release(ctx, command)) {
        return fail(out, "s2_command_release_failed");
    }
    return true;
}

bool build_s2_separator(const BuildContext& ctx, const Init5cf140ItemSpec& spec) noexcept {
    Init5b9f90Result& out = *ctx.out;
    if (std::strlen(spec.parent) != spec.parent_len) return fail(out, "s2_separator_length_mismatch");
    const SharedStr parent = borrowed(spec.parent, spec.parent_len);
    DebugMenuItem* const item = next_item(ctx);
    if (item == nullptr) return fail(out, "item_storage_exhausted");
    if (!init_menu_separator_26b60(*item, parent, kInit5b9f90ItemFlags,
                                   ctx.hooks->separator_alloc)) {
        return fail(out, "s2_separator_ctor_failed");
    }
    ++out.separators;
    if (!register_menu_item_268b0(*item, *ctx.hooks->menu, *ctx.hooks->atexit).ok) {
        return fail(out, "s2_separator_register_failed");
    }
    ++out.registered;
    return true;
}

// 0x26db0 over a variable built by unlifted code: the host stand-in carries the name read from
// the ELF and the measured bool-class identity; no constructor is claimed for the variable itself.
bool build_s2_named_external(const BuildContext& ctx, const Init5cf140ItemSpec& spec,
                             std::size_t index) noexcept {
    Init5b9f90Result& out = *ctx.out;
    if (ctx.hooks->var_storage == nullptr || index >= ctx.hooks->var_capacity) {
        return fail(out, "s2_var_storage_missing");
    }
    const SharedStr parent = borrowed(spec.parent, spec.parent_len);
    const SharedStr name = borrowed(spec.text, spec.text_len);
    ConCmdBoolVar& var = ctx.hooks->var_storage[index];
    var = ConCmdBoolVar{};
    var.base.name = name;
    var.base.vptr = kInit5b9f90BoolVarVptr;
    DebugMenuTarget target{};
    target.object = &var;
    target.slot3 = 1;
    target.slot6 = 0;
    target.known = true;
    DebugMenuItem* const item = next_item(ctx);
    if (item == nullptr) return fail(out, "item_storage_exhausted");
    if (!init_debug_menu_item_26db0(*item, parent, var.base, target, kInit5b9f90ItemFlags)) {
        return fail(out, "s2_named_ctor_failed");
    }
    ++out.named_vars;
    if (!register_menu_item_268b0(*item, *ctx.hooks->menu, *ctx.hooks->atexit).ok) {
        return fail(out, "s2_named_register_failed");
    }
    ++out.registered;
    return true;
}

// 0xc300 bounded float variable + 0x20d60 + __cxa_atexit(0xad30) + the 0x27120 slider.
bool build_s2_ranged(const BuildContext& ctx, const Init5cf140ItemSpec& spec,
                     std::size_t index) noexcept {
    Init5b9f90Result& out = *ctx.out;
    if (ctx.hooks->registry == nullptr || ctx.hooks->ranged_storage == nullptr ||
        ctx.hooks->ranged_value_storage == nullptr || ctx.hooks->ranged_guard_storage == nullptr ||
        index >= ctx.hooks->ranged_capacity) {
        return fail(out, "s2_ranged_state_missing");
    }
    const SharedStr parent = borrowed(spec.parent, spec.parent_len);
    const SharedStr name = borrowed(spec.text, spec.text_len);
    const SharedStr empty = borrowed("", 0);
    ConCmdRangedVar& var = ctx.hooks->ranged_storage[index];
    bool& guard = ctx.hooks->ranged_guard_storage[index];
    float& value = ctx.hooks->ranged_value_storage[index];
    if (!guard) {
        var = ConCmdRangedVar{};
        RangedVarArgs args{};
        args.fn_vptr = 0x15CB238;   // the caller functor tag at all ten sites
        if (!init_bounded_float_var_c300(var, name, &value, spec.var_lo, spec.var_hi, 1, args, empty)) {
            return fail(out, "s2_ranged_ctor_failed");
        }
        if (!ctx.hooks->atexit->record(kInit5cf140BoolVarDtorRva, &var)) {
            return fail(out, "s2_ranged_atexit_failed");
        }
        if (!register_concmd_20d60(var.base, *ctx.hooks->registry).ok) {
            return fail(out, "s2_ranged_register_failed");
        }
        guard = true;
    }
    DebugMenuItem* const item = next_item(ctx);
    if (item == nullptr) return fail(out, "item_storage_exhausted");
    if (!init_debug_menu_item_27120(*item, parent, var.base, kInit5b9f90ItemFlags, spec.lo, spec.hi)) {
        return fail(out, "s2_slider_ctor_failed");
    }
    ++out.sliders;
    if (!register_menu_item_268b0(*item, *ctx.hooks->menu, *ctx.hooks->atexit).ok) {
        return fail(out, "s2_slider_register_failed");
    }
    ++out.registered;
    if (!release(ctx, name) || !release(ctx, empty)) return fail(out, "s2_ranged_release_failed");
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

// ---- S4.1, the 0x5dbf40 "Particle" page builder ---------------------------------------------

bool build_s4_group(const BuildContext& ctx, const Init5cf140ItemSpec& spec,
                    bool release_parent = true, bool destroy = true) noexcept {
    Init5b9f90Result& out = *ctx.out;
    if (std::strlen(spec.parent) != spec.parent_len || std::strlen(spec.text) != spec.text_len) {
        return fail(out, "s4_group_view_length_mismatch");
    }
    const SharedStr parent = borrowed(spec.parent, spec.parent_len);
    const SharedStr caption = borrowed(spec.text, spec.text_len);
    DebugMenuItem* const item = next_item(ctx);
    if (item == nullptr) return fail(out, "item_storage_exhausted");
    if (!init_menu_group_26c10(*item, parent, caption, kInit5b9f90ItemFlags)) {
        return fail(out, "s4_group_ctor_failed");
    }
    ++out.groups;
    if (!register_menu_item_268b0(*item, *ctx.hooks->menu, *ctx.hooks->atexit).ok) {
        return fail(out, "s4_group_register_failed");
    }
    ++out.registered;
    if (destroy) ++out.destroys;   // the body's 0x272d0 destroy follows the group
    // S5.1's group has no release in the body; S4.1's does.
    if (release_parent && !release(ctx, parent)) return fail(out, "s4_group_release_failed");
    return true;
}

bool build_s4_ranged_float(const BuildContext& ctx, const Init5cf140ItemSpec& spec,
                           std::size_t index) noexcept {
    Init5b9f90Result& out = *ctx.out;
    if (ctx.hooks->registry == nullptr || ctx.hooks->ranged_storage == nullptr ||
        ctx.hooks->ranged_value_storage == nullptr || ctx.hooks->ranged_guard_storage == nullptr ||
        index >= ctx.hooks->ranged_capacity) {
        return fail(out, "s4_ranged_state_missing");
    }
    const SharedStr parent = borrowed(spec.parent, spec.parent_len);
    const SharedStr name = borrowed(spec.text, spec.text_len);
    const SharedStr empty = borrowed("", 0);
    ConCmdRangedVar& var = ctx.hooks->ranged_storage[index];
    bool& guard = ctx.hooks->ranged_guard_storage[index];
    float& value = ctx.hooks->ranged_value_storage[index];
    if (!guard) {
        var = ConCmdRangedVar{};
        RangedVarArgs args{};
        args.fn_vptr = 0x15CBDA0;   // the float-family caller tag at every 0xc300 site in this body
        if (!init_bounded_float_var_c300(var, name, &value, spec.var_lo, spec.var_hi, 1, args, empty)) {
            return fail(out, "s4_ranged_ctor_failed");
        }
        if (!ctx.hooks->atexit->record(kInit5cf140BoolVarDtorRva, &var)) {
            return fail(out, "s4_ranged_atexit_failed");
        }
        if (!register_concmd_20d60(var.base, *ctx.hooks->registry).ok) {
            return fail(out, "s4_ranged_register_failed");
        }
        guard = true;
    }
    DebugMenuItem* const item = next_item(ctx);
    if (item == nullptr) return fail(out, "item_storage_exhausted");
    if (!init_debug_menu_item_27120(*item, parent, var.base, kInit5b9f90ItemFlags, spec.lo, spec.hi)) {
        return fail(out, "s4_slider_ctor_failed");
    }
    ++out.sliders;
    if (!register_menu_item_268b0(*item, *ctx.hooks->menu, *ctx.hooks->atexit).ok) {
        return fail(out, "s4_slider_register_failed");
    }
    ++out.registered;
    ++out.destroys;
    if (!release(ctx, name) || !release(ctx, empty)) return fail(out, "s4_ranged_release_failed");
    return true;
}

bool build_s4_ranged_int(const BuildContext& ctx, const Init5cf140ItemSpec& spec,
                         std::size_t index) noexcept {
    Init5b9f90Result& out = *ctx.out;
    if (ctx.hooks->registry == nullptr || ctx.hooks->ranged_storage == nullptr ||
        ctx.hooks->ranged_int_value_storage == nullptr || ctx.hooks->ranged_guard_storage == nullptr ||
        index >= ctx.hooks->ranged_capacity) {
        return fail(out, "s4_ranged_int_state_missing");
    }
    const SharedStr parent = borrowed(spec.parent, spec.parent_len);
    const SharedStr name = borrowed(spec.text, spec.text_len);
    const SharedStr empty = borrowed("", 0);
    ConCmdRangedVar& var = ctx.hooks->ranged_storage[index];
    bool& guard = ctx.hooks->ranged_guard_storage[index];
    std::int32_t& value = ctx.hooks->ranged_int_value_storage[index];
    if (!guard) {
        var = ConCmdRangedVar{};
        RangedVarArgs args{};
        args.fn_vptr = kInit5dbf40RangedIntTag;
        const auto to_int = [](float v) {
            if (v >= 2147483647.0F) return std::int32_t{2147483647};
            if (v <= -2147483648.0F) return std::int32_t{-2147483647 - 1};
            return static_cast<std::int32_t>(v);
        };
        const HelperCtorResult r = helper_c580_ranged_int(
            var, name, &value, to_int(spec.var_lo), to_int(spec.var_hi), 1, args, empty);
        if (!r.ok) return fail(out, "s4_ranged_int_ctor_failed");
        if (!ctx.hooks->atexit->record(kInit5cf140BoolVarDtorRva, &var)) {
            return fail(out, "s4_ranged_int_atexit_failed");
        }
        if (!register_concmd_20d60(var.base, *ctx.hooks->registry).ok) {
            return fail(out, "s4_ranged_int_register_failed");
        }
        guard = true;
    }
    DebugMenuItem* const item = next_item(ctx);
    if (item == nullptr) return fail(out, "item_storage_exhausted");
    if (!init_debug_menu_item_27120(*item, parent, var.base, kInit5b9f90ItemFlags, spec.lo, spec.hi)) {
        return fail(out, "s4_int_slider_ctor_failed");
    }
    ++out.int_sliders;
    if (!register_menu_item_268b0(*item, *ctx.hooks->menu, *ctx.hooks->atexit).ok) {
        return fail(out, "s4_int_slider_register_failed");
    }
    ++out.registered;
    ++out.destroys;
    if (!release(ctx, name) || !release(ctx, empty)) return fail(out, "s4_ranged_int_release_failed");
    return true;
}

bool build_s4_bool(const BuildContext& ctx, const Init5cf140ItemSpec& spec,
                   std::size_t index) noexcept {
    if (!build_bool_var_impl(ctx, borrowed(spec.parent, spec.parent_len), spec.text, spec.text_len,
                             index, kInit5dbf40BoolVarFnVptr)) {
        return false;
    }
    Init5b9f90Result& out = *ctx.out;
    // The body's 0x272d0 destroy follows each bool item; the item site is byte-pinned in the table.
    if (spec.slider_site_rva == 0) return fail(out, "s4_bool_item_site_missing");
    ++out.destroys;
    return true;
}

// S5.1: the 0x5bc413..0x5bc78c "Stereo 3D" page (group + two bounded floats and sliders).
bool run_s5_1(const BuildContext& ctx) noexcept {
    Init5b9f90Result& out = *ctx.out;
    const int registered_before = out.registered;
    std::size_t ranged_index = 17;   // S2 uses 0..9, S4 uses 10..16
    for (std::size_t i = 0; i < kInit5b9f90S5_1ItemsCount; ++i) {
        const Init5cf140ItemSpec& spec = kInit5b9f90S5_1Items[i];
        const bool ok = spec.kind == Init5cf140ItemKind::group
                            ? build_s4_group(ctx, spec, false, false)
                            : build_s4_ranged_float(ctx, spec, ranged_index++);
        if (!ok) return false;
    }
    out.s5_items = static_cast<std::size_t>(out.registered - registered_before);
    out.s5_bytes = kInit5b9f90S5_2EndRva - kInit5b9f90S5Rva;
    out.s5_executed = true;
    add_record(out, "S5.1", kInit5b9f90S5Rva, kInit5b9f90S5_1EndRva,
               "0x5bc3de prologue + 0x5bc413 \"Stereo 3D\": group + Auto Focal Distance + Auto Distortion");
    ++out.executed_slices;
    return true;
}

// S4.2: the 0x7f8930 wrapper and the 0x7f8610 label builder. The builder writes the measured
// "--" prefix and copies the four dwords at 0x16061a0 into +0x34..+0x40; the geometry block at
// 0x7f8664..0x7f87d8 is gated on the live debug-tool singletons 0x3288478/0x3288480, which the
// init runtime does not have (they are world/debug-tool state, not ELF data), so the runtime takes
// the measured early return at 0x7f8647. That gated range stays a contract with its offset.
bool run_s4_2(const BuildContext& ctx) noexcept {
    Init5b9f90Result& out = *ctx.out;
    unsigned char label[0x58]{};
    label[0] = '-';
    label[1] = '-';
    label[2] = 0;
    const std::uint32_t table[4] = {0u, 0u, 0u, 0x3F800000u};   // 0x16061a0: {0,0,0,1.0f}
    std::memcpy(label + 0x34, table, sizeof table);
    // The wrapper at 0x7f8930 checks the same two globals and the stack canary; with the globals
    // absent the builder returns 0 at 0x7f87d9 and the wrapper's OR is non-zero only because the
    // canary slot is untouched, which is not an observable result here.
    out.s4_2_label_len = 2;
    out.s4_2_executed = true;
    add_record(out, "S4.2", kInit7f8930Rva, kInit7f8930EndRva,
               "0x7f8930 wrapper + 0x7f8610 label prefix/table copy; geometry gated on 0x3288478/0x3288480");
    ++out.executed_slices;
    return true;
}

// Generic page runner: the same builders over any byte-pinned item table (S6.1/S6.2/S7/S9).
bool run_page(const BuildContext& ctx, const Init5cf140ItemSpec* items, std::size_t count,
              std::uint64_t start_rva, std::uint64_t end_rva, const char* id, const char* detail,
              std::size_t& ranged_index, std::size_t& bool_index) noexcept {
    Init5b9f90Result& out = *ctx.out;
    for (std::size_t i = 0; i < count; ++i) {
        const Init5cf140ItemSpec& spec = items[i];
        bool ok = false;
        switch (spec.kind) {
            case Init5cf140ItemKind::group: ok = build_s4_group(ctx, spec); break;
            case Init5cf140ItemKind::separator: {
                const SharedStr parent = borrowed(spec.parent, spec.parent_len);
                DebugMenuItem* const item = next_item(ctx);
                if (item == nullptr) return fail(out, "item_storage_exhausted");
                if (!init_menu_separator_26b60(*item, parent, kInit5b9f90ItemFlags,
                                               ctx.hooks->separator_alloc)) {
                    return fail(out, "page_separator_ctor_failed");
                }
                if (!register_menu_item_268b0(*item, *ctx.hooks->menu, *ctx.hooks->atexit).ok) {
                    return fail(out, "page_separator_register_failed");
                }
                ++out.separators;
                ++out.registered;
                ++out.destroys;
                ok = true;
                break;
            }
            case Init5cf140ItemKind::command: {
                const SharedStr parent = borrowed(spec.parent, spec.parent_len);
                const SharedStr caption = borrowed(spec.text, spec.text_len);
                const SharedStr name = borrowed(spec.command, spec.command_len);
                DebugMenuItem* const item = next_item(ctx);
                if (item == nullptr) return fail(out, "item_storage_exhausted");
                if (!init_menu_command_26f90(*item, parent, caption,
                                              reinterpret_cast<const void*>(static_cast<std::uintptr_t>(
                                                  spec.target_rva)),
                                              name, kInit5b9f90ItemFlags)) {
                    return fail(out, "page_command_ctor_failed");
                }
                if (!register_menu_item_268b0(*item, *ctx.hooks->menu, *ctx.hooks->atexit).ok) {
                    return fail(out, "page_command_register_failed");
                }
                ++out.commands;
                ++out.registered;
                ++out.destroys;
                if (!release(ctx, caption) || !release(ctx, name)) {
                    return fail(out, "page_command_release_failed");
                }
                ok = true;
                break;
            }
            case Init5cf140ItemKind::bool_var: ok = build_s4_bool(ctx, spec, bool_index++); break;
            case Init5cf140ItemKind::ranged_slider:
                ok = build_s4_ranged_float(ctx, spec, ranged_index++);
                break;
            case Init5cf140ItemKind::ranged_int_slider:
                ok = build_s4_ranged_int(ctx, spec, ranged_index++);
                break;
            default: return fail(out, "page_unknown_item_kind");
        }
        if (!ok) return false;
    }
    add_record(out, id, start_rva, end_rva, detail);
    ++out.executed_slices;
    return true;
}

// S5.2: the 0x5bc78c..0x5c1196 "Dynamic 3D" and wormhole pages.
bool run_s5_2(const BuildContext& ctx) noexcept {
    Init5b9f90Result& out = *ctx.out;
    const int registered_before = out.registered;
    std::size_t ranged_index = 19;   // S2 0..9, S4 10..16, S5.1 17..18
    std::size_t bool_index = 19;     // S3 0..3, S2 4..6, S4 7..18
    for (std::size_t i = 0; i < kInit5b9f90S5_2ItemsCount; ++i) {
        const Init5cf140ItemSpec& spec = kInit5b9f90S5_2Items[i];
        bool ok = false;
        switch (spec.kind) {
            case Init5cf140ItemKind::group: ok = build_s4_group(ctx, spec, false, false); break;
            case Init5cf140ItemKind::separator: {
                const SharedStr parent = borrowed(spec.parent, spec.parent_len);
                DebugMenuItem* const item = next_item(ctx);
                if (item == nullptr) return fail(out, "item_storage_exhausted");
                if (!init_menu_separator_26b60(*item, parent, kInit5b9f90ItemFlags,
                                               ctx.hooks->separator_alloc)) {
                    return fail(out, "s5_2_separator_ctor_failed");
                }
                if (!register_menu_item_268b0(*item, *ctx.hooks->menu, *ctx.hooks->atexit).ok) {
                    return fail(out, "s5_2_separator_register_failed");
                }
                ++out.separators;
                ++out.registered;
                ++out.destroys;
                if (!release(ctx, parent)) return fail(out, "s5_2_separator_release_failed");
                ok = true;
                break;
            }
            case Init5cf140ItemKind::command: {
                const SharedStr parent = borrowed(spec.parent, spec.parent_len);
                const SharedStr caption = borrowed(spec.text, spec.text_len);
                const SharedStr name = borrowed(spec.command, spec.command_len);
                DebugMenuItem* const item = next_item(ctx);
                if (item == nullptr) return fail(out, "item_storage_exhausted");
                if (!init_menu_command_26f90(*item, parent, caption,
                                            reinterpret_cast<const void*>(static_cast<std::uintptr_t>(0x2DFCBD0)),
                                            name, kInit5b9f90ItemFlags)) {
                    return fail(out, "s5_2_command_ctor_failed");
                }
                if (!register_menu_item_268b0(*item, *ctx.hooks->menu, *ctx.hooks->atexit).ok) {
                    return fail(out, "s5_2_command_register_failed");
                }
                ++out.commands;
                ++out.registered;
                ++out.destroys;
                if (!release(ctx, caption) || !release(ctx, name)) {
                    return fail(out, "s5_2_command_release_failed");
                }
                ok = true;
                break;
            }
            case Init5cf140ItemKind::bool_var: ok = build_s4_bool(ctx, spec, bool_index++); break;
            case Init5cf140ItemKind::ranged_slider:
                ok = build_s4_ranged_float(ctx, spec, ranged_index++);
                break;
            case Init5cf140ItemKind::ranged_int_slider:
                ok = build_s4_ranged_int(ctx, spec, ranged_index++);
                break;
            default: return fail(out, "s5_2_unknown_item_kind");
        }
        if (!ok) return false;
    }
    out.s5_items += static_cast<std::size_t>(out.registered - registered_before);
    out.s5_bytes = kInit5b9f90S5_2EndRva - kInit5b9f90S5Rva;
    add_record(out, "S5.2", kInit5b9f90S5_2Rva, kInit5b9f90S5_2EndRva,
               "0x5bc78c \"Dynamic 3D\" + \"Shadow Boss Worm Hole Effect\": 2 groups, 9 bools, "
               "35 bounded vars, 7 separators, 3 commands");
    ++out.executed_slices;
    return true;
}

bool run_s4(const BuildContext& ctx) noexcept {
    Init5b9f90Result& out = *ctx.out;
    const int registered_before = out.registered;
    std::size_t bool_index = 7;      // S3 uses 0..3, S2 uses 4..6
    std::size_t ranged_index = 10;   // S2 uses 0..9
    for (std::size_t i = 0; i < kInit5dbf40ItemsCount; ++i) {
        const Init5cf140ItemSpec& spec = kInit5dbf40Items[i];
        bool ok = false;
        switch (spec.kind) {
            case Init5cf140ItemKind::group: ok = build_s4_group(ctx, spec); break;
            case Init5cf140ItemKind::ranged_slider:
                ok = build_s4_ranged_float(ctx, spec, ranged_index++);
                break;
            case Init5cf140ItemKind::ranged_int_slider:
                ok = build_s4_ranged_int(ctx, spec, ranged_index++);
                break;
            case Init5cf140ItemKind::bool_var: ok = build_s4_bool(ctx, spec, bool_index++); break;
            default: return fail(out, "s4_unknown_item_kind");
        }
        if (!ok) return false;
    }
    out.s4_items = static_cast<std::size_t>(out.registered - registered_before);
    // The body releases its last stack view once after the final item.
    if (!release(ctx, borrowed("", 0))) return false;
    out.s4_executed = true;
    out.s4_bytes = kInit5dbf40Size;
    add_record(out, "S4.1", kInit5dbf40Rva, kInit5dbf40EndRva,
               "0x5dbf40 \"Particle\": 1 group, 2 ranged ints, 5 bounded floats, 12 bool variables, "
               "7 sliders, 20 destroys");
    ++out.executed_slices;
    return true;
}

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

    // S1: the 0x2170 page builder. Its body is not lifted; the host observes the call and the
    // slice stays `contract` in the map.
    {
        const InitPostCallOutcome o = hooks.page_builder(
            hooks.host, kInit5b9f90PageBuilderShadowMiscRva, "Shadow Misc");
        if (!o.executed) {
            out.blocked = o.blocker != nullptr ? o.blocker : "page_builder_2170_refused";
            return out;
        }
        ++out.contracts;
    }

    // S2: the 0x5cf140 "Shadow Credits" page builder, executed natively in body order (after the
    // 0x2170 call, before the S3 pages).
    // The two S2 bool variables use slots 4..6 (S3 uses 0..3).
    std::size_t s2_var_index = 4;
    std::size_t s2_ranged_index = 0;
    for (std::size_t i = 0; i < kInit5cf140ItemsCount; ++i) {
        const Init5cf140ItemSpec& spec = kInit5cf140Items[i];
        bool ok = false;
        switch (spec.kind) {
            case Init5cf140ItemKind::group: ok = build_s2_group(ctx, spec); break;
            case Init5cf140ItemKind::command: ok = build_s2_command(ctx, spec); break;
            case Init5cf140ItemKind::separator: ok = build_s2_separator(ctx, spec); break;
            case Init5cf140ItemKind::named_external:
                ok = build_s2_named_external(ctx, spec, s2_var_index++);
                break;
            case Init5cf140ItemKind::bool_var:
                ok = build_bool_var_impl(ctx, borrowed(spec.parent, spec.parent_len), spec.text,
                                         spec.text_len, s2_var_index++, kInit5b9f90BoolVarFnVptr);
                break;
            case Init5cf140ItemKind::ranged_slider:
                ok = build_s2_ranged(ctx, spec, s2_ranged_index++);
                break;
            case Init5cf140ItemKind::ranged_int_slider:
                fail(out, "s2_ranged_int_unsupported");
                return out;
        }
        if (!ok) return out;
        ++out.s2_items;
    }
    // The body ends with one release and its 0x272d0 destroy block over the 21 items.
    if (!release(ctx, borrowed("", 0))) return out;
    for (std::size_t i = 0; i < out.s2_items; ++i) ++out.destroys;
    out.s2_executed = true;
    add_record(out, "S2", kInit5cf140Rva, kInit5cf140EndRva,
               "0x5cf140 \"Shadow Credits\": 1 group, 1 command, 6 separators, 3 named variables, "
               "10 bounded floats + sliders, 21 destroys");
    ++out.executed_slices;

    // S3.1..S3.6 in body order.
    std::size_t toggle_index = 0;
    std::size_t var_index = 0;
    for (const Init5b9f90PageSpec& page : kInit5b9f90Pages) {
        if (!build_page(ctx, page, toggle_index, var_index)) return out;
        add_record(out, page.id, page.start_rva, page.end_rva, "page executed natively");
        ++out.executed_slices;
    }

    // S4.1 executes; S4.2 (the 0x7f8930 guard helper) stays a named contract on its measured reason.
    if (!run_s4(ctx)) return out;
    ++out.contracts;
    // S5.1 executes the "Stereo 3D" page; S5.2 and S6..S9 stay blocked.
    if (!run_s5_1(ctx)) return out;
    if (!run_s5_2(ctx)) return out;
    // S6: the 10 B call pair, then both page bodies it invokes.
    std::size_t ranged_index = 54;   // S2 0..9, S4 10..16, S5 17..53
    std::size_t bool_index = 28;     // S3 0..3, S2 4..6, S4 7..18, S5 19..27
    add_record(out, "S6", 0x5C1196, 0x5C11A0,
               "0x5c1196 calls 0x5c94b0 (Shadow UI/HUD/Map) and 0xb25840 pages");
    ++out.executed_slices;
    if (!run_page(ctx, Init5c94b0Items, Init5c94b0ItemsCount, kInit5c94b0Rva,
                  kInit5c94b0EndRva, "S6.1",
                  "0x5c94b0 \"Shadow UI/HUD/Map\": 76 byte-pinned items",
                  ranged_index, bool_index)) {
        return out;
    }
    if (!run_page(ctx, Initb25840Items, Initb25840ItemsCount, kInitb25840Rva,
                  kInitb25840EndRva, "S6.2", "0xb25840 page: 60 byte-pinned items",
                  ranged_index, bool_index)) {
        return out;
    }
    if (!run_page(ctx, Init5c11a0Items, Init5c11a0ItemsCount, kInit5c11a0Rva,
                  kInit5c11a0EndRva, "S7",
                  "\"Shadow Sound\" 5 bools, \"Shadow Cursor\" int, \"Layers\" bool",
                  ranged_index, bool_index)) {
        return out;
    }
    if (!run_page(ctx, Init5c1e61Items, Init5c1e61ItemsCount, kInit5c1e61Rva,
                  kInit5c1e61EndRva, "S9",
                  "Performance/Transparency/Texture/Fullscreen: 40 byte-pinned items",
                  ranged_index, bool_index)) {
        return out;
    }
    if (!run_page(ctx, Init5c1d50Items, Init5c1d50ItemsCount, kInit5c1d50Rva,
                  kInit5c1d50EndRva, "S8",
                  "33-iteration loop over the 0x2dcaa90 table: 33 layer bools",
                  ranged_index, bool_index)) {
        return out;
    }

    // S4.2: the 0x7f8930 wrapper and the 0x7f8610 prefix the init runtime can execute without the
    // live debug-tool singletons.
    if (!run_s4_2(ctx)) return out;
    out.resume_rva = kInit5b9f90ResumeRva;
    out.complete = false;
    out.ok = true;
    return out;
}

}  // namespace sotc
