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

Marshal2687b0 marshal_2687b0(GuestWindow& window, std::uint64_t self) noexcept {
    Marshal2687b0 out{};
    if (!window.valid()) {
        out.status = GuestStatus::no_window;
        return out;
    }
    if (window.host_pointer(self, 1) == nullptr) {
        out.status = GuestStatus::out_of_range;
        return out;
    }
    out.cells.self = reinterpret_cast<void*>(static_cast<std::uintptr_t>(self));
    out.status = GuestStatus::ok;
    out.ok = true;
    return out;
}

Marshal1bf710 marshal_1bf710(GuestWindow& window, std::uint64_t self) noexcept {
    Marshal1bf710 out{};
    if (!window.valid()) {
        out.status = GuestStatus::no_window;
        return out;
    }
    if (window.host_pointer(self, 1) == nullptr) {
        out.status = GuestStatus::out_of_range;
        return out;
    }
    out.cells.self = reinterpret_cast<void*>(static_cast<std::uintptr_t>(self));
    out.status = GuestStatus::ok;
    out.ok = true;
    return out;
}

Marshalc2e0 marshal_c2e0(GuestWindow& window, std::uint64_t self,
                         std::uint64_t source) noexcept {
    Marshalc2e0 out{};
    std::uint8_t* copy =
        window.host_pointer(self + kVtable15d1678SlotE8_c2e0CopyFieldOffset, sizeof(void*));
    std::uint8_t* source_field =
        window.host_pointer(source + kVtable15d1678SlotE8_c2e0SourceOffset, sizeof(void*));
    if (copy == nullptr || source_field == nullptr) {
        out.status = window.valid() ? GuestStatus::out_of_range : GuestStatus::no_window;
        return out;
    }
    out.cells.copy_field = reinterpret_cast<void**>(copy);
    out.cells.source_field = reinterpret_cast<const void* const*>(source_field);
    out.status = GuestStatus::ok;
    out.ok = true;
    return out;
}

Marshalc740 marshal_c740(GuestWindow& window, std::uint64_t self,
                         std::uint64_t source) noexcept {
    Marshalc740 out{};
    std::uint8_t* table =
        window.host_pointer(self + kVtable15d1d80Slot08_c740TableFieldOffset, sizeof(void*));
    std::uint8_t* copy =
        window.host_pointer(self + kVtable15d1d80Slot08_c740CopyFieldOffset, sizeof(void*));
    std::uint8_t* source_field =
        window.host_pointer(source + kVtable15d1d80Slot08_c740SourceOffset, sizeof(void*));
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

MarshalE16a00 marshal_e16a00(GuestWindow& window, std::uint64_t self,
                             std::uint64_t argument) noexcept {
    MarshalE16a00 out{};
    std::uint8_t* vtable = window.host_pointer(self, sizeof(void*));
    if (vtable == nullptr) {
        out.status = window.valid() ? GuestStatus::out_of_range : GuestStatus::no_window;
        return out;
    }
    out.cells.self = reinterpret_cast<void*>(static_cast<std::uintptr_t>(self));
    out.cells.vtable_field = reinterpret_cast<const void* const*>(vtable);
    out.cells.argument = reinterpret_cast<void*>(static_cast<std::uintptr_t>(argument));
    out.status = GuestStatus::ok;
    out.ok = true;
    return out;
}

MarshalDdf4b0 marshal_ddf4b0(GuestWindow& window, std::uint64_t self) noexcept {
    MarshalDdf4b0 out{};
    if (!window.valid()) {
        out.status = GuestStatus::no_window;
        return out;
    }
    if (window.host_pointer(self, 1) == nullptr) {
        out.status = GuestStatus::out_of_range;
        return out;
    }
    out.cells.self = reinterpret_cast<void*>(static_cast<std::uintptr_t>(self));
    out.status = GuestStatus::ok;
    out.ok = true;
    return out;
}

MarshalA460f0 marshal_a460f0(GuestWindow& window, std::uint64_t self) noexcept {
    MarshalA460f0 out{};
    std::uint8_t* vtable = window.host_pointer(self, sizeof(void*));
    std::uint8_t* helper =
        window.host_pointer(self + kVtable15d1678Slot280_a460f0HelperFieldOffset, sizeof(void*));
    if (vtable == nullptr || helper == nullptr) {
        out.status = window.valid() ? GuestStatus::out_of_range : GuestStatus::no_window;
        return out;
    }
    out.cells.self = reinterpret_cast<void*>(static_cast<std::uintptr_t>(self));
    out.cells.vtable_field = reinterpret_cast<void**>(vtable);
    out.cells.helper_field = reinterpret_cast<const void* const*>(helper);
    out.status = GuestStatus::ok;
    out.ok = true;
    return out;
}

MarshalE24900 marshal_e24900(GuestWindow& window, std::uint64_t self,
                             std::uint64_t out) noexcept {
    MarshalE24900 out_cells{};
    if (!window.valid()) {
        out_cells.status = GuestStatus::no_window;
        return out_cells;
    }
    std::uint64_t sub = 0;
    const GuestStatus sub_status =
        window.read_u64(self + kVtable15e2d60SlotD0_e24900SubOffset, sub);
    if (sub_status != GuestStatus::ok) {
        out_cells.status = sub_status;
        return out_cells;
    }
    std::uint8_t* out_pointer = window.host_pointer(out, sizeof(void*));
    if (out_pointer == nullptr) {
        out_cells.status = GuestStatus::out_of_range;
        return out_cells;
    }
    out_cells.cells.self = reinterpret_cast<void*>(static_cast<std::uintptr_t>(self));
    out_cells.cells.sub = reinterpret_cast<void*>(static_cast<std::uintptr_t>(sub));
    out_cells.cells.out = reinterpret_cast<void**>(out_pointer);
    out_cells.status = GuestStatus::ok;
    out_cells.ok = true;
    return out_cells;
}

MarshalE26f90 marshal_e26f90(GuestWindow& window, std::uint64_t self, std::uint64_t arg2,
                             std::int32_t arg3, std::uint64_t canary_guest,
                             std::uint64_t buffer_guest, std::uint64_t local_guest) noexcept {
    MarshalE26f90 out{};
    if (!window.valid()) {
        out.status = GuestStatus::no_window;
        return out;
    }
    if (window.host_pointer(self, 1) == nullptr) {
        out.status = GuestStatus::out_of_range;
        return out;
    }
    std::uint8_t* canary = window.host_pointer(canary_guest, sizeof(std::uint64_t));
    std::uint8_t* buffer = window.host_pointer(buffer_guest, 0x10);
    std::uint8_t* local = window.host_pointer(local_guest, sizeof(std::uint32_t));
    if (canary == nullptr || buffer == nullptr || local == nullptr) {
        out.status = GuestStatus::out_of_range;
        return out;
    }
    out.cells.self = reinterpret_cast<void*>(static_cast<std::uintptr_t>(self));
    out.cells.arg2 = reinterpret_cast<void*>(static_cast<std::uintptr_t>(arg2));
    out.cells.arg3 = arg3;
    out.cells.canary = reinterpret_cast<std::uint64_t*>(canary);
    out.cells.buffer = buffer;
    out.cells.local = local;
    out.status = GuestStatus::ok;
    out.ok = true;
    return out;
}

MarshalE1a5c0 marshal_e1a5c0(GuestWindow& window, std::uint64_t self, std::uint64_t key,
                             std::int32_t value) noexcept {
    MarshalE1a5c0 out{};
    if (!window.valid()) {
        out.status = GuestStatus::no_window;
        return out;
    }
    std::uint64_t array = 0;
    std::uint32_t count = 0;
    if (window.read_u64(self + kVtable15e2028Slot70_e1a5c0ArrayOffset, array) != GuestStatus::ok ||
        window.read_u32(self + kVtable15e2028Slot70_e1a5c0CountOffset, count) != GuestStatus::ok) {
        out.status = GuestStatus::out_of_range;
        return out;
    }
    std::uint8_t* count_pointer =
        window.host_pointer(self + kVtable15e2028Slot70_e1a5c0CountOffset, sizeof(std::uint32_t));
    if (count_pointer == nullptr) {
        out.status = GuestStatus::out_of_range;
        return out;
    }
    const char* key_pointer = nullptr;
    if (key != 0) {
        std::string_view text;
        if (window.read_cstr(key, 4096, text) != GuestStatus::ok) {
            out.status = GuestStatus::unterminated_string;
            return out;
        }
        key_pointer = reinterpret_cast<const char*>(window.host_pointer(key, text.size() + 1));
    }
    std::uint8_t* array_pointer = nullptr;
    if (count != 0) {
        array_pointer = window.host_pointer(array, static_cast<std::size_t>(count) * sizeof(void*));
        if (array_pointer == nullptr) {
            out.status = GuestStatus::out_of_range;
            return out;
        }
    }
    out.cells.self = reinterpret_cast<void*>(static_cast<std::uintptr_t>(self));
    out.cells.elements = reinterpret_cast<void* const*>(array_pointer);
    out.cells.count = reinterpret_cast<const std::uint32_t*>(count_pointer);
    out.cells.key = key_pointer;
    out.cells.value = value;
    out.status = GuestStatus::ok;
    out.ok = true;
    return out;
}

MarshalE19980 marshal_e19980(GuestWindow& window, std::uint64_t arg2, std::uint64_t arg3,
                             std::uint64_t arg4, std::uint64_t arg5, std::uint64_t canary_guest,
                             const void* const* value_cell) noexcept {
    MarshalE19980 out{};
    if (!window.valid()) {
        out.status = GuestStatus::no_window;
        return out;
    }
    std::uint8_t* canary = window.host_pointer(canary_guest, sizeof(std::uint64_t));
    if (canary == nullptr || value_cell == nullptr) {
        out.status = GuestStatus::out_of_range;
        return out;
    }
    out.args.arg2 = reinterpret_cast<void*>(static_cast<std::uintptr_t>(arg2));
    out.args.arg3 = reinterpret_cast<void*>(static_cast<std::uintptr_t>(arg3));
    out.args.arg4 = reinterpret_cast<void*>(static_cast<std::uintptr_t>(arg4));
    out.args.arg5 = reinterpret_cast<void*>(static_cast<std::uintptr_t>(arg5));
    out.cells.canary = reinterpret_cast<std::uint64_t*>(canary);
    out.cells.fast_store_value = value_cell;
    const bool aligned = (arg3 & kVtable15e1ee0SlotB0_e19980AlignmentMask) == 0;
    if (arg3 != 0 && aligned) {
        std::uint8_t* target = window.host_pointer(arg3, sizeof(void*));
        if (target == nullptr) {
            out.status = GuestStatus::out_of_range;
            return out;
        }
        out.cells.fast_store_target = reinterpret_cast<void**>(target);
    }
    out.status = GuestStatus::ok;
    out.ok = true;
    return out;
}

MarshalE21420 marshal_e21420(GuestWindow& window, std::uint64_t self,
                             std::uint64_t name) noexcept {
    MarshalE21420 out{};
    if (!window.valid()) {
        out.status = GuestStatus::no_window;
        return out;
    }
    std::uint64_t provider = 0;
    if (window.read_u64(self + kVtable15e2028Slot2A0_e21420ProviderOffset, provider) !=
        GuestStatus::ok) {
        out.status = GuestStatus::out_of_range;
        return out;
    }
    const char* name_pointer = nullptr;
    if (name != 0) {
        std::string_view text;
        if (window.read_cstr(name, 4096, text) != GuestStatus::ok) {
            out.status = GuestStatus::unterminated_string;
            return out;
        }
        name_pointer = reinterpret_cast<const char*>(window.host_pointer(name, text.size() + 1));
    }
    out.cells.self = reinterpret_cast<void*>(static_cast<std::uintptr_t>(self));
    out.cells.provider = reinterpret_cast<void*>(static_cast<std::uintptr_t>(provider));
    out.cells.name = name_pointer;
    out.status = GuestStatus::ok;
    out.ok = true;
    return out;
}

}  // namespace sotc
