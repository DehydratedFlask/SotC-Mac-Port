// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01: startup constructor index 536 / RVA 0x1342d0.
// Thin adapter: the body's per-slot facts are the DATA tables in ctor_slot_536_1342d0.hpp; the statement
// sequence is core/debug_page_var_item. Nothing here re-implements a shape.
#include "ctor_slot_536_1342d0.hpp"

namespace sotc {

debug_page::Result init_slot536_1342d0(Slot536Objects& objects, ConCmdRegistry& concmd,
                                       DebugMenuRegistry& menu, const debug_page::Hooks& hooks) noexcept {
    const debug_page::Objects view{std::span<ConCmdRangedVar>(&objects.var1, 2), std::span<DebugMenuItem>()};
    return debug_page::init_debug_page(kSlot536Script, view, concmd, menu, hooks);
}

}  // namespace sotc
