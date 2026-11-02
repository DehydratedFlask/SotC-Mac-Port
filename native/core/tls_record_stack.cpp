#include "tls_record_stack.hpp"

#include <cstring>

namespace sotc {

const char* tls_record_status_name(TlsRecordStatus status) noexcept {
    switch (status) {
        case TlsRecordStatus::ok:
            return "ok";
        case TlsRecordStatus::no_thread:
            return "no_thread";
        case TlsRecordStatus::out_of_range:
            return "out_of_range";
        case TlsRecordStatus::capacity:
            return "capacity";
        case TlsRecordStatus::thread_pointer:
            return "thread_pointer";
    }
    return "unknown";
}

namespace {

// Resolves the thread's control block. The host model treats the TLS block itself as the control
// block: the retail body's `fs:0 + *(0x15fff98)` indirection has no host analogue yet.
TlsRecordStatus control_block(GuestThreads& threads, std::uint32_t thread_id, std::uint32_t& count) {
    std::uint64_t pointer = 0;
    if (threads.thread_pointer(thread_id, pointer) != TlsStatus::ok) {
        return TlsRecordStatus::no_thread;
    }
    (void)pointer;
    return threads.read_u32(thread_id, kTlsRecordCountOffset, count) == TlsStatus::ok
               ? TlsRecordStatus::ok
               : TlsRecordStatus::thread_pointer;
}

}  // namespace

TlsRecordPushResult tls_record_push(GuestThreads& threads, std::uint32_t thread_id,
                                   std::uint64_t arg0, const std::uint8_t blob[16],
                                   std::uint32_t field) noexcept {
    TlsRecordPushResult out{};
    std::uint32_t count = 0;
    const TlsRecordStatus status = control_block(threads, thread_id, count);
    if (status != TlsRecordStatus::ok) {
        out.status = status;
        return out;
    }
    if (count >= kTlsRecordCapacity) {
        // The retail body would increment the count and then write the record over it.
        out.status = TlsRecordStatus::capacity;
        return out;
    }

    const std::uint32_t index = count;
    const std::uint32_t base = index * kTlsRecordStride;
    // Retail order: the count is stored first, then the record is written.
    if (threads.write_u32(thread_id, kTlsRecordCountOffset, index + 1) != TlsStatus::ok ||
        threads.write_u64(thread_id, base, arg0) != TlsStatus::ok ||
        threads.write_u32(thread_id, base + kTlsRecordField0x18Offset, field) != TlsStatus::ok) {
        out.status = TlsRecordStatus::out_of_range;
        return out;
    }
    for (std::uint32_t byte = 0; byte < kTlsRecordBlobBytes; ++byte) {
        if (threads.write_u8(thread_id, base + kTlsRecordBlobOffset + byte, blob[byte]) !=
            TlsStatus::ok) {
            out.status = TlsRecordStatus::out_of_range;
            return out;
        }
    }
    out.status = TlsRecordStatus::ok;
    out.ok = true;
    out.push.index = index;
    out.push.count_after = index + 1;
    return out;
}

TlsRecordPopResult tls_record_pop(GuestThreads& threads, std::uint32_t thread_id) noexcept {
    TlsRecordPopResult out{};
    std::uint32_t count = 0;
    const TlsRecordStatus status = control_block(threads, thread_id, count);
    if (status != TlsRecordStatus::ok) {
        out.status = status;
        return out;
    }
    if (count == 0) {
        // `decl` on zero would wrap to 0xffffffff; the model reports it instead.
        out.status = TlsRecordStatus::out_of_range;
        return out;
    }
    const std::uint32_t after = count - 1;
    if (threads.write_u32(thread_id, kTlsRecordCountOffset, after) != TlsStatus::ok) {
        out.status = TlsRecordStatus::out_of_range;
        return out;
    }
    out.status = TlsRecordStatus::ok;
    out.ok = true;
    out.count_after = after;
    return out;
}

TlsRecordRead tls_record_read(const GuestThreads& threads, std::uint32_t thread_id,
                              std::uint32_t index) noexcept {
    TlsRecordRead out{};
    if (!threads.alive(thread_id)) {
        out.status = TlsRecordStatus::no_thread;
        return out;
    }
    if (index >= kTlsRecordCapacity) {
        out.status = TlsRecordStatus::out_of_range;
        return out;
    }
    const std::uint32_t base = index * kTlsRecordStride;
    if (threads.read_u64(thread_id, base, out.arg0) != TlsStatus::ok ||
        threads.read_u32(thread_id, base + kTlsRecordField0x18Offset, out.field) != TlsStatus::ok) {
        out.status = TlsRecordStatus::out_of_range;
        return out;
    }
    for (std::uint32_t byte = 0; byte < kTlsRecordBlobBytes; ++byte) {
        if (threads.read_u8(thread_id, base + kTlsRecordBlobOffset + byte, out.blob[byte]) !=
            TlsStatus::ok) {
            out.status = TlsRecordStatus::out_of_range;
            return out;
        }
    }
    out.status = TlsRecordStatus::ok;
    out.ok = true;
    return out;
}

}  // namespace sotc
