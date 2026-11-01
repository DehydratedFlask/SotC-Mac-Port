#include "guest_memory.hpp"

#include <cstring>

namespace sotc {

const char* guest_status_name(GuestStatus status) noexcept {
    switch (status) {
        case GuestStatus::ok:
            return "ok";
        case GuestStatus::no_window:
            return "no_window";
        case GuestStatus::out_of_range:
            return "out_of_range";
        case GuestStatus::unterminated_string:
            return "unterminated_string";
    }
    return "unknown";
}

std::uint8_t* GuestWindow::host_pointer(std::uint64_t address, std::size_t bytes) noexcept {
    if (!valid() || address < guest_base_) {
        return nullptr;
    }
    const std::uint64_t offset = address - guest_base_;
    if (offset > size_ || bytes > size_ - static_cast<std::size_t>(offset)) {
        return nullptr;
    }
    return base_ + offset;
}

const std::uint8_t* GuestWindow::host_pointer(std::uint64_t address,
                                              std::size_t bytes) const noexcept {
    if (!valid() || address < guest_base_) {
        return nullptr;
    }
    const std::uint64_t offset = address - guest_base_;
    if (offset > size_ || bytes > size_ - static_cast<std::size_t>(offset)) {
        return nullptr;
    }
    return base_ + offset;
}

GuestStatus GuestWindow::read_u8(std::uint64_t address, std::uint8_t& out) const noexcept {
    const std::uint8_t* pointer = host_pointer(address, sizeof(std::uint8_t));
    if (pointer == nullptr) {
        return valid() ? GuestStatus::out_of_range : GuestStatus::no_window;
    }
    out = *pointer;
    return GuestStatus::ok;
}

GuestStatus GuestWindow::read_u32(std::uint64_t address, std::uint32_t& out) const noexcept {
    const std::uint8_t* pointer = host_pointer(address, sizeof(std::uint32_t));
    if (pointer == nullptr) {
        return valid() ? GuestStatus::out_of_range : GuestStatus::no_window;
    }
    std::memcpy(&out, pointer, sizeof(out));
    return GuestStatus::ok;
}

GuestStatus GuestWindow::read_u64(std::uint64_t address, std::uint64_t& out) const noexcept {
    const std::uint8_t* pointer = host_pointer(address, sizeof(std::uint64_t));
    if (pointer == nullptr) {
        return valid() ? GuestStatus::out_of_range : GuestStatus::no_window;
    }
    std::memcpy(&out, pointer, sizeof(out));
    return GuestStatus::ok;
}

GuestStatus GuestWindow::write_u8(std::uint64_t address, std::uint8_t value) noexcept {
    std::uint8_t* pointer = host_pointer(address, sizeof(std::uint8_t));
    if (pointer == nullptr) {
        return valid() ? GuestStatus::out_of_range : GuestStatus::no_window;
    }
    *pointer = value;
    return GuestStatus::ok;
}

GuestStatus GuestWindow::write_u32(std::uint64_t address, std::uint32_t value) noexcept {
    std::uint8_t* pointer = host_pointer(address, sizeof(std::uint32_t));
    if (pointer == nullptr) {
        return valid() ? GuestStatus::out_of_range : GuestStatus::no_window;
    }
    std::memcpy(pointer, &value, sizeof(value));
    return GuestStatus::ok;
}

GuestStatus GuestWindow::write_u64(std::uint64_t address, std::uint64_t value) noexcept {
    std::uint8_t* pointer = host_pointer(address, sizeof(std::uint64_t));
    if (pointer == nullptr) {
        return valid() ? GuestStatus::out_of_range : GuestStatus::no_window;
    }
    std::memcpy(pointer, &value, sizeof(value));
    return GuestStatus::ok;
}

GuestStatus GuestWindow::read_cstr(std::uint64_t address, std::size_t max_length,
                                   std::string_view& out) const noexcept {
    if (!valid()) {
        return GuestStatus::no_window;
    }
    const std::uint8_t* pointer = host_pointer(address, 1);
    if (pointer == nullptr) {
        return GuestStatus::out_of_range;
    }
    const std::size_t available =
        size_ - static_cast<std::size_t>(address - guest_base_);
    const std::size_t limit = max_length < available ? max_length : available;
    for (std::size_t index = 0; index < limit; ++index) {
        if (pointer[index] == 0) {
            out = std::string_view(reinterpret_cast<const char*>(pointer), index);
            return GuestStatus::ok;
        }
    }
    return GuestStatus::unterminated_string;
}

}  // namespace sotc
