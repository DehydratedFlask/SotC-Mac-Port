// Shared building blocks of the startup constructor slots 322, 325, 327, 328, 330, 331, 332 and 334
// (L3-SLOTS-322-334; original ELF sha256 d0723afb...5861d3). Every one of those bodies is a short
// sequence of the same two shapes that slot 303 introduced, so the shapes are factored here and each
// slot module only carries its own constants and order:
//
//   bool variable   : the inlined `ConCmdBoolVar` static initialiser (concmd_vars.hpp) with a
//                     borrowed name view, the shared empty description and a per-site functor tag,
//                     value and default byte. Calls 0x20af0, 0xf590(description), 0x20d60,
//                     0xf590(name), __cxa_atexit(0x20b0, var).
//   menu items      : after the variable, a debug-menu item built by one of three constructors
//                     (0x26ea0 toggle with label+text, 0x26db0 named item, 0x26c10 group), each
//                     followed by the 0xf590 releases of its stack views (order pinned per slot)
//                     and __cxa_atexit(0x272d0, item).
//
// Strings are borrowed views (control == nullptr): the original builds them on the stack from rodata
// literals, so 0xf590 on them is an exact no-op (release_inert_view_f590). Callers keep the literals
// alive. Functor tags, vtable pointers and destructor RVAs are identity data and are never executed.
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "concmd_vars.hpp"
#include "debug_menu_item.hpp"
#include "fmenu_ctors_slot306.hpp"
#include "menu_item_26db0.hpp"

namespace sotc {

struct FamilyItemResult {
    bool ctor_ok = false;
    MenuRegisterResult registration{};
    bool views_inert = false;
    bool atexit_recorded = false;
    [[nodiscard]] bool ok() const noexcept {
        return ctor_ok && registration.ok && views_inert && atexit_recorded;
    }
};

// A borrowed view over a literal of `len` bytes.
[[nodiscard]] SharedStr family_view(const char* text, std::size_t len) noexcept;

// The inlined bool-variable initialiser with the shared empty description (an in-module NUL byte,
// like the original's rodata byte). Thin wrapper over init_bool_var_static.
[[nodiscard]] BoolVarResult family_bool_var(ConCmdBoolVar& var, const char* name, std::size_t name_len,
                                            std::uint64_t fn_tag, std::uint8_t value,
                                            std::uint8_t default_value, ConCmdRegistry& registry) noexcept;

// 0x26ea0(item, label, text, target, flags); 0xf590(text); 0xf590(label); atexit(0x272d0, item).
[[nodiscard]] FamilyItemResult family_toggle_item_26ea0(DebugMenuItem& item, const char* label,
                                                       std::size_t label_len, const char* text,
                                                       std::size_t text_len, const DebugMenuTarget& target,
                                                       std::uint32_t flags, ConCmdRegistry& registry,
                                                       DebugMenuRegistry& menu) noexcept;

// 0x26db0(item, parent, target, flags); 0xf590(parent); atexit(0x272d0, item).
[[nodiscard]] FamilyItemResult family_named_item_26db0(DebugMenuItem& item, const char* parent,
                                                      std::size_t parent_len, const ConCmdBase& target_base,
                                                      const DebugMenuTarget& target, std::uint32_t flags,
                                                      ConCmdRegistry& registry, DebugMenuRegistry& menu) noexcept;

// 0x26c10(item, parent, caption, flags); 0xf590(caption); 0xf590(parent); atexit(0x272d0, item).
[[nodiscard]] FamilyItemResult family_group_item_26c10(DebugMenuItem& item, const char* parent,
                                                      std::size_t parent_len, const char* caption,
                                                      std::size_t caption_len, std::uint32_t flags,
                                                      ConCmdRegistry& registry, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
