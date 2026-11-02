// TLS allocation-site path inside the two allocator-front bodies (`0x11fb0`, `0x12820`).
//
// Pinned guard chain (both bodies, verified by `tools/lift/check_alloc_site_context.py`):
//
//   rax = fs:0 + *(0x15fff98);                 ; fs:0 - 24
//   if (index == 1)                            ; 0x11ff5 / 0x12883  `cmpl $0x1`
//     if (flag != 0)                           ; 0x11ffa  `testb %r14b` (0x12820 has no flag test)
//       rax = *(u64*)rax;                      ; the thread control block
//       if (rax != 0) {
//         count = *(u32*)(rax + 0x400);
//         if (count != 0) {                    ; 0x1200d / 0x12890
//           index = count - 1;                 ; 0x12011 / 0x1289a  `decl` / `leal -1`
//           rax += index * 0x20;
//         } else goto table;                   ; 0x1201c / 0x128a8
//       } else goto table;
//     } else goto table;
//   else goto table;
// table: rax = 0x2dfaab0 + index * 8;        ; the per-index allocator table
//   return *(u64*)rax;                          ; 0x12029 / 0x128b8
//
// So `record.owner` (the first qword of the top TLS record) is the **allocator to use** while a
// site frame is on the stack; otherwise the allocator comes from the global table at `0x2dfaab0`
// indexed by the object's dword. An empty stack is a normal fallback, not an error — the retail
// code branches to the table slot rather than failing.
#pragma once

#include "guest_tls.hpp"

#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kAllocatorTableAddress = 0x2dfaab0;
inline constexpr std::uint64_t kAllocatorInitFlagAddress = 0x2dfaaa0;
inline constexpr std::uint64_t kAllocatorInitCacheAddress = 0x2dfaab8;
inline constexpr std::int32_t kAllocatorTlsIndex = 1;

enum class AllocatorSource : std::uint8_t {
    table = 0,  // the per-index table at 0x2dfaab0
    tls_site,   // the top allocation-site record's owner field
};

const char* allocator_source_name(AllocatorSource source) noexcept;

struct AllocatorChoice {
    AllocatorSource source = AllocatorSource::table;
    std::uint64_t allocator = 0;
    std::uint64_t table_address = 0;   // the slot the table path would read
    bool tls_guard_passed = false;     // index == 1 (and the flag, where the body tests one)
    bool tls_stack_empty = false;      // guard passed but the thread had no site frame
};

// Mirrors the guard chain above. `index` is the object's dword at +0 and `flag` the byte some
// callers pass (`0x11fb0`'s `%r14b`); `0x12820` has no flag test, which `flag = true` expresses.
[[nodiscard]] AllocatorChoice allocator_for_thread(const GuestThreads& threads,
                                                   std::uint32_t thread_id, std::int32_t index,
                                                   bool flag, std::uint64_t fallback) noexcept;

}  // namespace sotc
