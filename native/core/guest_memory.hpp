// Checked guest-memory window used to marshal retail call frames into the native models.
//
// The models take host cells (a `void**` for a vtable pointer, `uint8_t*` for a once flag, ...).
// When a lifted guest body calls one of them, those pointers refer to guest addresses. This module
// maps a guest address window onto host storage and performs every access through a bounds check, so
// a bad address fails visibly instead of touching unrelated memory.
//
// This is a **window**, not the PS4 address space: the caller decides the base and size (a test arena,
// an extracted image region, a mapped guest page).
#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace sotc {

enum class GuestStatus : std::uint8_t {
    ok = 0,
    no_window,
    out_of_range,
    unterminated_string,
};

const char* guest_status_name(GuestStatus status) noexcept;

class GuestWindow {
public:
    GuestWindow() = default;
    GuestWindow(std::uint8_t* base, std::uint64_t guest_base, std::size_t size) noexcept
        : base_(base), guest_base_(guest_base), size_(base == nullptr ? 0 : size) {}

    [[nodiscard]] bool valid() const noexcept { return base_ != nullptr && size_ != 0; }
    [[nodiscard]] std::uint64_t guest_base() const noexcept { return guest_base_; }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }

    // Host pointer for a guest address, or nullptr when the address or the span is outside.
    [[nodiscard]] std::uint8_t* host_pointer(std::uint64_t address, std::size_t bytes) noexcept;
    [[nodiscard]] const std::uint8_t* host_pointer(std::uint64_t address,
                                                   std::size_t bytes) const noexcept;

    [[nodiscard]] GuestStatus read_u8(std::uint64_t address, std::uint8_t& out) const noexcept;
    [[nodiscard]] GuestStatus read_u32(std::uint64_t address, std::uint32_t& out) const noexcept;
    [[nodiscard]] GuestStatus read_u64(std::uint64_t address, std::uint64_t& out) const noexcept;
    [[nodiscard]] GuestStatus write_u8(std::uint64_t address, std::uint8_t value) noexcept;
    [[nodiscard]] GuestStatus write_u32(std::uint64_t address, std::uint32_t value) noexcept;
    [[nodiscard]] GuestStatus write_u64(std::uint64_t address, std::uint64_t value) noexcept;

    // NUL-terminated string starting at `address`, at most `max_length` bytes. The view points into
    // the window, so it is only valid while the window's storage lives.
    [[nodiscard]] GuestStatus read_cstr(std::uint64_t address, std::size_t max_length,
                                        std::string_view& out) const noexcept;

private:
    std::uint8_t* base_ = nullptr;
    std::uint64_t guest_base_ = 0;
    std::size_t size_ = 0;
};

}  // namespace sotc
