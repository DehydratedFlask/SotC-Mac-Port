// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01: startup constructor index 374 / RVA 0x701230.
// Thin adapter: the body's per-slot facts are the DATA tables in ctor_slot_374_701230.hpp; the statement
// sequence is core/debug_page_var_item. Nothing here re-implements a shape.
#include "ctor_slot_374_701230.hpp"

namespace sotc {

debug_page::Result init_slot374_701230(Slot374Objects& objects, ConCmdRegistry& concmd,
                                       DebugMenuRegistry& menu, const debug_page::Hooks& hooks) noexcept {
    const debug_page::Objects view{std::span<ConCmdRangedVar>(&objects.var, 1),
                                   std::span<DebugMenuItem>(&objects.item, 1),
                                   std::span<NameHashCell>(&objects.record, 1)};
    return debug_page::init_debug_page(kSlot374Script, view, concmd, menu, hooks);
}

}  // namespace sotc
