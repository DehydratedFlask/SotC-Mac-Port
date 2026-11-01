#include "retail_cells.hpp"

namespace sotc {

Marshal20d60 marshal_20d60(GuestWindow& window, std::uint64_t self,
                           std::uint64_t global_address) noexcept {
    Marshal20d60 out{};
    std::uint8_t* flag =
        window.host_pointer(self + kVtable1594dc0Slot20_20d60OnceFlagOffset, sizeof(std::uint8_t));
    if (flag == nullptr) {
        out.status = window.valid() ? GuestStatus::out_of_range : GuestStatus::no_window;
        return out;
    }
    out.cells.once_flag = flag;
    if (global_address != 0) {
        std::uint8_t* global = window.host_pointer(global_address, sizeof(void*));
        if (global == nullptr) {
            out.status = GuestStatus::out_of_range;
            return out;
        }
        out.cells.global_cell = reinterpret_cast<const void* const*>(global);
    }
    out.status = GuestStatus::ok;
    out.ok = true;
    return out;
}

Marshal358c0 marshal_358c0(GuestWindow& window, std::uint64_t self,
                           std::uint64_t source) noexcept {
    Marshal358c0 out{};
    std::uint8_t* table =
        window.host_pointer(self + kVtable15955c0Slot08_358c0TableFieldOffset, sizeof(void*));
    std::uint8_t* copy =
        window.host_pointer(self + kVtable15955c0Slot08_358c0CopyFieldOffset, sizeof(void*));
    std::uint8_t* source_field =
        window.host_pointer(source + kVtable15955c0Slot08_358c0SourceOffset, sizeof(void*));
    if (table == nullptr || copy == nullptr || source_field == nullptr) {
        out.status = window.valid() ? GuestStatus::out_of_range : GuestStatus::no_window;
        return out;
    }
    out.cells.table_field = reinterpret_cast<void**>(table);
    out.cells.copy_field = reinterpret_cast<void**>(copy);
    out.cells.source_field = reinterpret_cast<const void* const*>(source_field);
    out.status = GuestStatus::ok;
    out.ok = true;
    return out;
}

Marshal13d20 marshal_13d20(GuestWindow& window, std::uint64_t self) noexcept {
    Marshal13d20 out{};
    std::uint8_t* field =
        window.host_pointer(self + kVtable1594ae8Slot28_13d20FieldOffset, sizeof(std::uint32_t));
    if (field == nullptr) {
        out.status = window.valid() ? GuestStatus::out_of_range : GuestStatus::no_window;
        return out;
    }
    out.cells.field = reinterpret_cast<std::uint32_t*>(field);
    out.status = GuestStatus::ok;
    out.ok = true;
    return out;
}

Marshal187c0 marshal_187c0(GuestWindow& window, std::uint64_t self) noexcept {
    Marshal187c0 out{};
    if (!window.valid()) {
        out.status = GuestStatus::no_window;
        return out;
    }
    // The model only needs `self`; verifying the address is inside the window catches a bad frame.
    if (window.host_pointer(self, 1) == nullptr) {
        out.status = GuestStatus::out_of_range;
        return out;
    }
    out.cells.self = reinterpret_cast<void*>(static_cast<std::uintptr_t>(self));
    out.status = GuestStatus::ok;
    out.ok = true;
    return out;
}

}  // namespace sotc
