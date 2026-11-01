#include "vtable_15955c0_slot08_358c0.hpp"

namespace sotc {

Vtable15955c0Slot08_358c0Result vtable_15955c0_slot08_358c0(
    const Vtable15955c0Slot08_358c0Cells& cells) noexcept {
    Vtable15955c0Slot08_358c0Result out{};
    if (cells.table_field == nullptr || cells.source_field == nullptr || cells.copy_field == nullptr) {
        return out;  // both stores and the read are unconditional in the retail body
    }
    out.table_written =
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(kVtable15955c0Slot08_358c0InstalledTable));
    *cells.table_field = out.table_written;
    out.copied = const_cast<void*>(*cells.source_field);
    *cells.copy_field = out.copied;
    out.ok = true;
    return out;
}

}  // namespace sotc
