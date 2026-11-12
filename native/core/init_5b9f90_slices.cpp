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

// 0x26c10 group item + 0x268b0 registration + the two 0xf590 view releases, exactly the order the
// body uses ("Shadow" at 0x5ba02c, "Shadow Boss Jump" at 0x5ba0a3, "Shadow Boss Outro Jump" at
// 0x5baab2).
bool build_group(const BuildContext& ctx, const char* parent_text, std::uint8_t parent_len,
                 const char* caption_text, std::uint8_t caption_len,
                 std::uint64_t site_rva) noexcept {
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
    if (!release(ctx, parent) || !release(ctx, caption)) return fail(out, "group_release_failed");
    return true;
}

// 0x26f90 command item (kind 2, target stored verbatim) + registration + the caption/command
// view releases.
bool build_command(const BuildContext& ctx, const char* parent_text, std::uint8_t parent_len,
                   const Init5b9f90ItemSpec& spec) noexcept {
    Init5b9f90Result& out = *ctx.out;
    if (std::strlen(parent_text) != parent_len ||
        std::strlen(spec.caption) != spec.caption_len ||
        std::strlen(spec.command) != spec.command_len) {
        return fail(out, "command_view_length_mismatch");
    }
    const SharedStr parent = borrowed(parent_text, parent_len);
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

bool build_page(const BuildContext& ctx, const Init5b9f90PageSpec& page,
                const Init5b9f90ItemSpec* items, std::size_t item_count,
                const char* record_detail) noexcept {
    Init5b9f90Result& out = *ctx.out;
    if (items == nullptr || item_count != page.item_count) return fail(out, "page_spec_mismatch");
    if (!build_group(ctx, page.parent, page.parent_len, page.caption, page.caption_len,
                     page.group_site_rva)) {
        return false;
    }
    for (std::size_t i = 0; i < item_count; ++i) {
        if (!build_command(ctx, page.caption, page.caption_len, items[i])) return false;
    }
    add_record(out, page.id, page.start_rva, page.end_rva, record_detail);
    ++out.executed_slices;
    return true;
}

}  // namespace

Init5b9f90Result run_init_5b9f90_slices(const Init5b9f90Hooks& hooks) noexcept {
    Init5b9f90Result out{};
    if (hooks.host == nullptr || hooks.menu == nullptr || hooks.atexit == nullptr ||
        hooks.release_view == nullptr || hooks.page_builder == nullptr ||
        hooks.canary_cell == nullptr || hooks.item_storage == nullptr ||
        hooks.item_capacity < 2 + kInit5b9f90BossJumpItemsCount +
                               kInit5b9f90BossOutroJumpItemsCount) {
        out.blocked = "state_missing";
        return out;
    }
    BuildContext ctx{&hooks, &out};

    // 0x5b9fa8..0x5b9fe4: the frame canary store (host token, never an original RVA read).
    *hooks.canary_cell = hooks.canary_token;
    out.canary_stored = true;

    // S0: group "Shadow" under "CP10", then its two view releases.
    if (!build_group(ctx, "CP10", 4, "Shadow", 6, 0x5BA02C)) return out;
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

    // S3.1 and S3.2: the two boss pages. Execution stops at 0x5bb4be by construction.
    if (!build_page(ctx, kInit5b9f90Pages[0], kInit5b9f90BossJumpItems,
                    kInit5b9f90BossJumpItemsCount,
                    "group + 16 command items, target 0x2fed5b8")) {
        return out;
    }
    if (!build_page(ctx, kInit5b9f90Pages[1], kInit5b9f90BossOutroJumpItems,
                    kInit5b9f90BossOutroJumpItemsCount,
                    "group + 16 command items, target 0x2fed6f8")) {
        return out;
    }

    // Everything from 0x5bb4be on ("Shadow Cheat" ... "Fullscreen Effects Tweaks") is unlifted.
    out.resume_rva = kInit5b9f90ResumeRva;
    out.complete = false;
    out.ok = true;
    return out;
}

}  // namespace sotc
