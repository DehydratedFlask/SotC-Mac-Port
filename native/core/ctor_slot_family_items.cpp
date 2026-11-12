#include <cstdlib>
#include "ctor_slot_family_items.hpp"
#include "ctor_slot_326_837f30.hpp"

namespace sotc {

namespace {
constexpr char kEmptyDescription[] = "";   // the original points at a NUL byte in rodata
}

SharedStr family_view(const char* text, std::size_t len) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(len), 0, nullptr};
}

BoolVarResult family_bool_var(ConCmdBoolVar& var, const char* name, std::size_t name_len,
                              std::uint64_t fn_tag, std::uint8_t value, std::uint8_t default_value,
                              ConCmdRegistry& registry) noexcept {
    return init_bool_var_static(var, family_view(name, name_len), family_view(kEmptyDescription, 0), fn_tag,
                                registry, value, default_value);
}

FamilyItemResult family_toggle_item_26ea0(DebugMenuItem& item, const char* label, std::size_t label_len,
                                         const char* text, std::size_t text_len, const DebugMenuTarget& target,
                                         std::uint32_t flags, ConCmdRegistry& registry,
                                         DebugMenuRegistry& menu) noexcept {
    FamilyItemResult out{};
    const SharedStr l = family_view(label, label_len);
    const SharedStr t = family_view(text, text_len);
    out.ctor_ok = init_debug_menu_item_26ea0(item, l, t, target, flags);
    if (!out.ctor_ok) return out;
    out.registration = register_menu_item_268b0(item, menu, registry.atexit_log);
    out.views_inert = release_inert_view_f590(t) && release_inert_view_f590(l);
    out.atexit_recorded = registry.atexit_log.record(kDebugMenuItemDtorRva, &item);
    return out;
}

FamilyItemResult family_named_item_26db0(DebugMenuItem& item, const char* parent, std::size_t parent_len,
                                        const ConCmdBase& target_base, const DebugMenuTarget& target,
                                        std::uint32_t flags, ConCmdRegistry& registry,
                                        DebugMenuRegistry& menu) noexcept {
    FamilyItemResult out{};
    const SharedStr p = family_view(parent, parent_len);
    out.ctor_ok = init_debug_menu_item_26db0(item, p, target_base, target, flags);
    if (!out.ctor_ok) return out;
    out.registration = register_menu_item_268b0(item, menu, registry.atexit_log);
    out.views_inert = release_inert_view_f590(p);
    out.atexit_recorded = registry.atexit_log.record(kDebugMenuItemDtorRva, &item);
    return out;
}

FamilyItemResult family_group_item_26c10(DebugMenuItem& item, const char* parent, std::size_t parent_len,
                                        const char* caption, std::size_t caption_len, std::uint32_t flags,
                                        ConCmdRegistry& registry, DebugMenuRegistry& menu) noexcept {
    FamilyItemResult out{};
    const SharedStr p = family_view(parent, parent_len);
    const SharedStr c = family_view(caption, caption_len);
    out.ctor_ok = init_menu_group_26c10(item, p, c, flags);
    if (!out.ctor_ok) return out;
    out.registration = register_menu_item_268b0(item, menu, registry.atexit_log);
    out.views_inert = release_inert_view_f590(c) && release_inert_view_f590(p);
    out.atexit_recorded = registry.atexit_log.record(kDebugMenuItemDtorRva, &item);
    return out;
}

FamilyItemResult family_separator_item_26b60(DebugMenuItem& item, const char* parent, std::size_t parent_len,
                                            std::uint32_t flags, const SharedStrCtorHooks& alloc,
                                            ConCmdRegistry& registry, DebugMenuRegistry& menu) noexcept {
    FamilyItemResult out{};
    const SharedStr p = family_view(parent, parent_len);
    out.ctor_ok = init_menu_separator_26b60(item, p, flags, alloc);
    if (!out.ctor_ok) return out;
    out.registration = register_menu_item_268b0(item, menu, registry.atexit_log);
    out.views_inert = release_inert_view_f590(p);
    out.atexit_recorded = registry.atexit_log.record(kDebugMenuItemDtorRva, &item);
    return out;
}

FamilyItemResult family_label_item_26800(DebugMenuItem& item, const char* parent, std::size_t parent_len,
                                        std::uint32_t flags, const SharedStrCtorHooks& alloc,
                                        ConCmdRegistry& registry, DebugMenuRegistry& menu) noexcept {
    FamilyItemResult out{};
    const SharedStr p = family_view(parent, parent_len);
    out.ctor_ok = init_slot326_label_item_26800(item, p, flags, alloc);
    if (!out.ctor_ok) return out;
    out.registration = register_menu_item_268b0(item, menu, registry.atexit_log);
    out.views_inert = release_inert_view_f590(p);
    out.atexit_recorded = registry.atexit_log.record(kDebugMenuItemDtorRva, &item);
    return out;
}

namespace {
void* separator_host_allocate(std::size_t size, std::uint32_t, std::uint32_t, std::uint32_t) noexcept { return std::malloc(size); }
bool separator_host_deallocate(void* ptr, std::uint32_t) noexcept {
    std::free(ptr);
    return true;
}
}  // namespace

SharedStrCtorHooks default_family_separator_alloc() noexcept {
    SharedStrCtorHooks hooks{};
    hooks.alloc.allocate = &separator_host_allocate;
    hooks.alloc.deallocate = &separator_host_deallocate;
    return hooks;
}

FamilyItemResult family_command_item_26f90(DebugMenuItem& item, const char* parent, std::size_t parent_len,
                                          const char* caption, std::size_t caption_len, const char* command,
                                          std::size_t command_len, const void* target, std::uint32_t flags,
                                          ConCmdRegistry& registry, DebugMenuRegistry& menu) noexcept {
    FamilyItemResult out{};
    if (target == nullptr) return out;   // fail closed: the original passes a real global's address
    const SharedStr p = family_view(parent, parent_len);
    const SharedStr c = family_view(caption, caption_len);
    const SharedStr m = family_view(command, command_len);
    out.ctor_ok = init_menu_command_26f90(item, p, c, target, m, flags);
    if (!out.ctor_ok) return out;
    out.registration = register_menu_item_268b0(item, menu, registry.atexit_log);
    out.views_inert = release_inert_view_f590(m) && release_inert_view_f590(c) && release_inert_view_f590(p);
    out.atexit_recorded = registry.atexit_log.record(kDebugMenuItemDtorRva, &item);
    return out;
}

}  // namespace sotc
