// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01: startup constructor index 361 / RVA 0x777390.
// Thin adapter: the body's per-slot facts are the DATA tables in ctor_slot_361_777390.hpp; the statement
// sequence is core/debug_page_var_item. Nothing here re-implements a shape.
#include "ctor_slot_361_777390.hpp"

namespace sotc {

debug_page::Result init_slot361_777390(Slot361Objects& objects, ConCmdRegistry& concmd,
                                       DebugMenuRegistry& menu, const debug_page::Hooks& hooks) noexcept {
    const debug_page::Objects view{std::span<ConCmdRangedVar>(&objects.var, 1),
                                   std::span<DebugMenuItem>(&objects.item, 1),
                                   std::span<NameHashCell>()};
    return debug_page::init_debug_page(kSlot361Script, view, concmd, menu, hooks);
}

}  // namespace sotc
