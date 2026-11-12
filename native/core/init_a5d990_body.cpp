// Native 0xa5d990 body implementation. See init_a5d990_body.hpp for the measured region map.
#include "init_a5d990_body.hpp"

#include <array>
#include <cstdio>
#include <cstring>

#include "fmenu_ctors_slot306.hpp"

namespace sotc {
namespace {

// The body advances the engine's own span fields (`addq $0xc8,0x8bd8(%rbx)` and the 0x72b0
// sibling), so the loop's copy must be written back or the engine would still look empty.
void store_span(std::uint8_t* engine, std::size_t offset,
                const SndxCategoryVectorSpan& span) noexcept {
    std::memcpy(engine + offset, &span.begin, sizeof span.begin);
    std::memcpy(engine + offset + 8, &span.end, sizeof span.end);
    std::memcpy(engine + offset + 0x10, &span.cap, sizeof span.cap);
}

SharedStr borrowed(const char* text, std::size_t len) noexcept {
    SharedStr view{};
    view.data = reinterpret_cast<const std::uint8_t*>(text);
    view.size = static_cast<std::int64_t>(len);
    view.aux = 0;
    view.ctl = nullptr;
    return view;
}

// The two per-index toggle items the loop builds (0x26ea0 x2 + 0x268b0 x2 + 0x272d0 x2).
// `context` is a ToggleContext; a failure is sticky so a partial pair is never reported as ok.
struct ToggleContext {
    const InitA5d990Hooks* hooks = nullptr;
    InitA5d990Result* out = nullptr;
    bool failed = false;
};

void build_toggle_pair(void* host, std::size_t index, const char* name, void* mute_var,
                       void* filter_var) noexcept {
    auto* context = static_cast<ToggleContext*>(host);
    if (context == nullptr || context->failed) return;
    const InitA5d990Hooks& hooks = *context->hooks;

    char caption[64];
    const int written = std::snprintf(caption, sizeof caption, "%d - %s",
                                      static_cast<int>(index), name != nullptr ? name : "");
    if (written <= 0 || static_cast<std::size_t>(written) >= sizeof caption) {
        context->failed = true;
        return;
    }
    const SharedStr text = borrowed(caption, static_cast<std::size_t>(written));
    const SharedStr mute_label = borrowed("Category Mute", kInitA5d990MuteParentLen);
    const SharedStr filter_label = borrowed("Category Filters", kInitA5d990FilterParentLen);

    void* targets[2] = {mute_var, filter_var};
    const SharedStr* labels[2] = {&mute_label, &filter_label};
    for (int i = 0; i < 2; ++i) {
        if (targets[i] == nullptr) {
            context->failed = true;
            return;
        }
        DebugMenuItem item{};
        const DebugMenuTarget target =
            debug_menu_target_for(*static_cast<const ConCmdBoolVar*>(targets[i]));
        if (!init_debug_menu_item_26ea0(item, *labels[i], text, target, kInitA5d990ItemFlags)) {
            context->failed = true;
            return;
        }
        const MenuRegisterResult registered =
            register_menu_item_268b0(item, *hooks.menu, *hooks.atexit);
        if (!registered.ok) {
            context->failed = true;
            return;
        }
        if (hooks.destroy_item != nullptr) {
            if (!hooks.destroy_item(hooks.host, item)) {
                context->failed = true;
                return;
            }
            ++context->out->destroys;
        }
        ++context->out->toggle_items;
    }
}

}  // namespace

InitA5d990Result run_init_a5d990_body(const InitA5d990Hooks& hooks) noexcept {
    InitA5d990Result out{};
    if (hooks.engine == nullptr || hooks.registry == nullptr || hooks.categories == nullptr ||
        hooks.menu == nullptr || hooks.atexit == nullptr || hooks.destroy_item == nullptr ||
        !hooks.categories->complete()) {
        out.blocked = "state_missing";
        return out;
    }
    if (hooks.menu->parent.parent == nullptr || hooks.menu->parent.insert == nullptr) {
        out.blocked = "menu_parent_contract_missing";
        return out;
    }

    // Region 1: the 64-variable loop, with the per-index toggle pair built natively above.
    ToggleContext context{&hooks, &out, false};
    SndxCategoryHooks categories = *hooks.categories;
    categories.host = &context;
    categories.menu_toggle = &build_toggle_pair;

    SndxCategoryVectorSpan mute = sndx_category_vectors_from_engine(hooks.engine,
                                                                  SndxCategoryVectorId::mute);
    SndxCategoryVectorSpan filter = sndx_category_vectors_from_engine(hooks.engine,
                                                                     SndxCategoryVectorId::filter);
    const SndxCategoriesResult loop =
        build_sndx_category_vars(mute, filter, hooks.category_names, *hooks.registry, categories);
    out.stage = loop.stage;
    out.mute_built = loop.mute_built;
    out.filter_built = loop.filter_built;
    auto* const engine = static_cast<std::uint8_t*>(hooks.engine);
    store_span(engine, 0x8bd0, mute);
    store_span(engine, 0x72a8, filter);
    if (!loop.ok || context.failed) {
        out.blocked = context.failed ? "toggle_item_failed" : "category_loop_failed";
        return out;
    }

    // Region 2 is dead: the gate byte is only written from a zero eax at the loop head.
    out.dead_unnamed_block = true;

    // Region 3: two separators and four command items, in the body's own order.
    const SharedStr mute_parent = borrowed("Category Mute", kInitA5d990MuteParentLen);
    const SharedStr filter_parent = borrowed("Category Filters", kInitA5d990FilterParentLen);
    const SharedStr mute_all = borrowed("Mute All", kInitA5d990MuteAllLen);
    const SharedStr mute_all_cmd = borrowed("sndx_categoryMuteAll=true", kInitA5d990MuteAllCmdLen);
    const SharedStr unmute_all = borrowed("Unmute All", kInitA5d990UnmuteAllLen);
    const SharedStr unmute_all_cmd = borrowed("sndx_categoryMuteAll=false",
                                              kInitA5d990UnmuteAllCmdLen);
    const SharedStr filter_all = borrowed("Filter All", kInitA5d990FilterAllLen);
    const SharedStr filter_all_cmd = borrowed("sndx_categoryFilterAll=true",
                                              kInitA5d990FilterAllCmdLen);
    const SharedStr unfilter_all = borrowed("Unfilter All", kInitA5d990UnfilterAllLen);
    const SharedStr unfilter_all_cmd = borrowed("sndx_categoryFilterAll=false",
                                                kInitA5d990UnfilterAllCmdLen);
    const void* const target = reinterpret_cast<const void*>(
        static_cast<std::uintptr_t>(kInitA5d990MenuTargetRva));

    std::array<DebugMenuItem, kInitA5d990StaticItems> items{};

    // 0xa5e09d separator, parent "Category Mute".
    if (!init_menu_separator_26b60(items[0], mute_parent, kInitA5d990ItemFlags,
                                   hooks.separator_alloc)) {
        out.blocked = "separator_ctor_failed";
        return out;
    }
    ++out.separators;
    // 0xa5e143 "Mute All" -> sndx_categoryMuteAll=true.
    if (!init_menu_command_26f90(items[1], mute_parent, mute_all, target, mute_all_cmd,
                                 kInitA5d990ItemFlags)) {
        out.blocked = "command_ctor_failed";
        return out;
    }
    ++out.commands;
    // 0xa5e1ee "Unmute All" -> sndx_categoryMuteAll=false.
    if (!init_menu_command_26f90(items[2], mute_parent, unmute_all, target, unmute_all_cmd,
                                 kInitA5d990ItemFlags)) {
        out.blocked = "command_ctor_failed";
        return out;
    }
    ++out.commands;
    // 0xa5e248 separator, parent "Category Filters".
    if (!init_menu_separator_26b60(items[3], filter_parent, kInitA5d990ItemFlags,
                                   hooks.separator_alloc)) {
        out.blocked = "separator_ctor_failed";
        return out;
    }
    ++out.separators;
    // 0xa5e2ea "Filter All" -> sndx_categoryFilterAll=true.
    if (!init_menu_command_26f90(items[4], filter_parent, filter_all, target, filter_all_cmd,
                                 kInitA5d990ItemFlags)) {
        out.blocked = "command_ctor_failed";
        return out;
    }
    ++out.commands;
    // 0xa5e39c "Unfilter All" -> sndx_categoryFilterAll=false.
    if (!init_menu_command_26f90(items[5], filter_parent, unfilter_all, target, unfilter_all_cmd,
                                 kInitA5d990ItemFlags)) {
        out.blocked = "command_ctor_failed";
        return out;
    }
    ++out.commands;

    for (std::size_t i = 0; i < items.size(); ++i) {
        const MenuRegisterResult registered =
            register_menu_item_268b0(items[i], *hooks.menu, *hooks.atexit);
        if (!registered.ok) {
            out.blocked = "register_failed";
            return out;
        }
        ++out.registered;
    }

    // 0xa5e3b9..0xa5e3f8: direct destruction, in the order -0x6d0, -0x5c8, -0x400, -0x338,
    // -0x1f0, -0x128 (the four command items and two separators, reverse of construction).
    const std::size_t order[kInitA5d990StaticItems] = {5, 4, 3, 2, 1, 0};
    for (std::size_t idx : order) {
        if (!hooks.destroy_item(hooks.host, items[idx])) {
            out.blocked = "destroy_failed";
            return out;
        }
        ++out.destroys;
    }

    out.ok = true;
    return out;
}

}  // namespace sotc
