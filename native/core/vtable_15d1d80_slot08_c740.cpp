#include "vtable_15d1d80_slot08_c740.hpp"

namespace sotc {

Vtable15d1d80Slot08_c740Result vtable_15d1d80_slot08_c740(
    const Vtable15d1d80Slot08_c740Cells& cells) noexcept {
    Vtable15d1d80Slot08_c740Result out{};
    if (cells.table_field == nullptr || cells.source_field == nullptr || cells.copy_field == nullptr) {
        return out;  // both stores and the read are unconditional in the retail body
    }
    out.table_written =
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(kVtable15d1d80Slot08_c740InstalledTable));
    *cells.table_field = out.table_written;
    out.copied = const_cast<void*>(*cells.source_field);
    *cells.copy_field = out.copied;
    out.ok = true;
    return out;
}

}  // namespace sotc
