// Per-thread record stack: `0x12300` (push) and `0x12350` (pop).
//
// Pinned bodies (verified by `tools/lift/check_tls_record_stack.py`):
//
//   0x12300, 77 B: tcb = *(fs:0 + *(0x15fff98));                  ; thread control block
//                  ; 0x15fff98 holds -24 in the file (relocated at load), i.e. the body reads the
//                  ; self-pointer slot one pointer below the thread pointer.
//                  count = *(u32*)(tcb + 0x400);
//                  *(u32*)(tcb + 0x400) = count + 1;              ; post-increment, stored first
//                  record = tcb + count * 0x20;
//                  record.q0     = rdi;
//                  record.q8     = *(16 bytes at rsi);
//                  record.dw0x18 = *(u32*)(rsi + 0x10);
//   0x12350, 31 B: tcb = *(fs:0 + *(0x15fff98)); --*(u32*)(tcb + 0x400);
//
// So the TLS window holds a 0x20-byte record array from the control block base with a u32 count at
// +0x400, which caps the array at **32** records (0x400 / 0x20).
//
// The retail push has **no bound check**: at count = 32 it increments the count, then writes the
// record at +0x400 — over the count itself. That path is memory corruption, not behaviour worth
// copying, so the model refuses it (`capacity`) and says so instead of reproducing it.
//
// Modelled: the record layout, the count update order (count first, then the record), and pop.
// NOT modelled: the real `fs:0 - 24` self-pointer indirection and whatever the loader writes into
// `0x15fff98` (the host model treats the thread's TLS block as the control block), and any consumer
// of these records.
#pragma once

#include "guest_tls.hpp"

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kTlsRecordPush18000Rva = 0x12300;
inline constexpr std::size_t kTlsRecordPush18000Size = 0x4d;
inline constexpr std::uint64_t kTlsRecordPop18000Rva = 0x12350;
inline constexpr std::size_t kTlsRecordPop18000Size = 0x1f;
inline constexpr std::uint32_t kTlsRecordCountOffset = 0x400;
inline constexpr std::uint32_t kTlsRecordStride = 0x20;
inline constexpr std::uint32_t kTlsRecordCapacity = kTlsRecordCountOffset / kTlsRecordStride;
inline constexpr std::uint32_t kTlsRecordField0x18Offset = 0x18;
inline constexpr std::uint32_t kTlsRecordBlobOffset = 0x08;
inline constexpr std::uint32_t kTlsRecordBlobBytes = 0x10;
// The link-time offset the retail body adds to `fs:0` before dereferencing the control block.
inline constexpr std::uint64_t kTlsRecordControlBlockOffsetCell = 0x15fff98;

enum class TlsRecordStatus : std::uint8_t {
    ok = 0,
    no_thread,
    out_of_range,
    capacity,          // retail would overwrite the count at +0x400; the model refuses
    thread_pointer,    // the TLS model could not resolve `fs:0`
    empty,             // the consumer path requires a non-zero count
};

const char* tls_record_status_name(TlsRecordStatus status) noexcept;

struct TlsRecordPush {
    std::uint32_t index = 0;      // the count value used for this record
    std::uint32_t count_after = 0;
};

struct TlsRecordPushResult {
    TlsRecordStatus status = TlsRecordStatus::no_thread;
    bool ok = false;
    TlsRecordPush push{};
};

// `blob` is the 16 bytes the retail body copies from `rsi`; `field` is the dword at `rsi+0x10`.
[[nodiscard]] TlsRecordPushResult tls_record_push(GuestThreads& threads, std::uint32_t thread_id,
                                                  std::uint64_t arg0, const std::uint8_t blob[16],
                                                  std::uint32_t field) noexcept;

struct TlsRecordPopResult {
    TlsRecordStatus status = TlsRecordStatus::no_thread;
    bool ok = false;
    std::uint32_t count_after = 0;
};

[[nodiscard]] TlsRecordPopResult tls_record_pop(GuestThreads& threads,
                                                std::uint32_t thread_id) noexcept;

// Reads the record `index` back (q0, the blob, the field), for fixtures and for a future consumer.
struct TlsRecordRead {
    TlsRecordStatus status = TlsRecordStatus::no_thread;
    bool ok = false;
    std::uint64_t arg0 = 0;
    std::uint8_t blob[16] = {};
    std::uint32_t field = 0;
};

// The consumers' idiom: `count = *(u32*)(tcb+0x400); if (!count) skip; record = tcb + (count-1)*0x20`.
// Both allocator-front bodies (`0x11fb0` at 0x12007, `0x12820` at 0x12890) read the last pushed
// record this way, so the records behave as a LIFO override stack for the current thread.
[[nodiscard]] TlsRecordRead tls_record_peek_top(const GuestThreads& threads,
                                               std::uint32_t thread_id) noexcept;


[[nodiscard]] TlsRecordRead tls_record_read(const GuestThreads& threads, std::uint32_t thread_id,
                                            std::uint32_t index) noexcept;

}  // namespace sotc
