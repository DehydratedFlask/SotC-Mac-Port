// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01: startup constructor index 379 / RVA 0x6d8d20.
// Thin adapter: the body's per-slot facts are the DATA tables in ctor_slot_379_6d8d20.hpp; the statement
// sequence is core/debug_page_var_item. Nothing here re-implements a shape.
#include "ctor_slot_379_6d8d20.hpp"

namespace sotc {

debug_page::Result init_slot379_6d8d20(Slot379Objects& objects, ConCmdRegistry& concmd,
                                       DebugMenuRegistry& menu, const debug_page::Hooks& hooks) noexcept {
    const debug_page::Objects view{std::span<ConCmdRangedVar>(&objects.var, 1),
                                   std::span<DebugMenuItem>(&objects.item, 1),
                                   std::span<NameHashCell>()};
    return debug_page::init_debug_page(kSlot379Script, view, concmd, menu, hooks);
}

}  // namespace sotc
