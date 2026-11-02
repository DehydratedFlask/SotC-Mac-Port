#include "guest_tls.hpp"

#include <cstring>

namespace sotc {

const char* tls_status_name(TlsStatus status) noexcept {
    switch (status) {
        case TlsStatus::ok:
            return "ok";
        case TlsStatus::no_thread:
            return "no_thread";
        case TlsStatus::out_of_range:
            return "out_of_range";
        case TlsStatus::already_exists:
            return "already_exists";
        case TlsStatus::too_many_threads:
            return "too_many_threads";
    }
    return "unknown";
}

GuestThreads::GuestThreads(std::uint64_t guest_base) noexcept : guest_base_(guest_base) {}

int GuestThreads::index_of(std::uint32_t thread_id) const noexcept {
    for (std::uint32_t i = 0; i < kMaxThreads; ++i) {
        if (used_[i] && ids_[i] == thread_id) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

std::uint8_t* GuestThreads::block(int index) noexcept {
    return storage_[static_cast<std::size_t>(index)];
}

const std::uint8_t* GuestThreads::block(int index) const noexcept {
    return storage_[static_cast<std::size_t>(index)];
}

TlsStatus GuestThreads::create(std::uint32_t thread_id) noexcept {
    if (index_of(thread_id) >= 0) {
        return TlsStatus::already_exists;
    }
    for (std::uint32_t i = 0; i < kMaxThreads; ++i) {
        if (!used_[i]) {
            used_[i] = true;
            ids_[i] = thread_id;
            std::memset(block(static_cast<int>(i)), 0, kBlockSize);
            ++count_;
            return TlsStatus::ok;
        }
    }
    return TlsStatus::too_many_threads;
}

TlsStatus GuestThreads::destroy(std::uint32_t thread_id) noexcept {
    const int index = index_of(thread_id);
    if (index < 0) {
        return TlsStatus::no_thread;
    }
    std::memset(block(index), 0, kBlockSize);
    used_[static_cast<std::size_t>(index)] = false;
    ids_[static_cast<std::size_t>(index)] = 0;
    --count_;
    return TlsStatus::ok;
}

bool GuestThreads::alive(std::uint32_t thread_id) const noexcept { return index_of(thread_id) >= 0; }

TlsStatus GuestThreads::thread_pointer(std::uint32_t thread_id, std::uint64_t& out) const noexcept {
    const int index = index_of(thread_id);
    if (index < 0) {
        return TlsStatus::no_thread;
    }
    out = guest_base_ + static_cast<std::uint64_t>(index) * kBlockSize;
    return TlsStatus::ok;
}

TlsStatus GuestThreads::check(std::uint32_t thread_id, std::uint32_t offset,
                              std::size_t bytes) const noexcept {
    if (index_of(thread_id) < 0) {
        return TlsStatus::no_thread;
    }
    if (offset > kBlockSize || bytes > kBlockSize - static_cast<std::size_t>(offset)) {
        return TlsStatus::out_of_range;
    }
    return TlsStatus::ok;
}

TlsStatus GuestThreads::read_u8(std::uint32_t thread_id, std::uint32_t offset,
                                std::uint8_t& out) const noexcept {
    const TlsStatus status = check(thread_id, offset, sizeof(std::uint8_t));
    if (status != TlsStatus::ok) {
        return status;
    }
    out = block(index_of(thread_id))[offset];
    return TlsStatus::ok;
}

TlsStatus GuestThreads::write_u8(std::uint32_t thread_id, std::uint32_t offset,
                                 std::uint8_t value) noexcept {
    const TlsStatus status = check(thread_id, offset, sizeof(std::uint8_t));
    if (status != TlsStatus::ok) {
        return status;
    }
    block(index_of(thread_id))[offset] = value;
    return TlsStatus::ok;
}

TlsStatus GuestThreads::read_u32(std::uint32_t thread_id, std::uint32_t offset,
                                 std::uint32_t& out) const noexcept {
    const TlsStatus status = check(thread_id, offset, sizeof(std::uint32_t));
    if (status != TlsStatus::ok) {
        return status;
    }
    std::memcpy(&out, block(index_of(thread_id)) + offset, sizeof(out));
    return TlsStatus::ok;
}

TlsStatus GuestThreads::write_u32(std::uint32_t thread_id, std::uint32_t offset,
                                  std::uint32_t value) noexcept {
    const TlsStatus status = check(thread_id, offset, sizeof(std::uint32_t));
    if (status != TlsStatus::ok) {
        return status;
    }
    std::memcpy(block(index_of(thread_id)) + offset, &value, sizeof(value));
    return TlsStatus::ok;
}

TlsStatus GuestThreads::read_u64(std::uint32_t thread_id, std::uint32_t offset,
                                 std::uint64_t& out) const noexcept {
    const TlsStatus status = check(thread_id, offset, sizeof(std::uint64_t));
    if (status != TlsStatus::ok) {
        return status;
    }
    std::memcpy(&out, block(index_of(thread_id)) + offset, sizeof(out));
    return TlsStatus::ok;
}

TlsStatus GuestThreads::write_u64(std::uint32_t thread_id, std::uint32_t offset,
                                  std::uint64_t value) noexcept {
    const TlsStatus status = check(thread_id, offset, sizeof(std::uint64_t));
    if (status != TlsStatus::ok) {
        return status;
    }
    std::memcpy(block(index_of(thread_id)) + offset, &value, sizeof(value));
    return TlsStatus::ok;
}

}  // namespace sotc
