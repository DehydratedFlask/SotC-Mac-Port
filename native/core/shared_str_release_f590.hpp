// Native semantic recovery of original helper 0xf590; see check_ff590.py.
#pragma once

#include <cstdint>

#include "hashed_string_14c3a0.hpp"

namespace sotc {

enum class SharedStrDeallocationKindF590 : std::uint32_t {
    data = 0,
    control = 1,
};

// Required allocator dependency. The original wrapper 0x170b0 forwards to
// 0x12af0; that allocator body is unresolved. There is no default allocator.
class SharedStrDeallocatorF590 {
public:
    virtual ~SharedStrDeallocatorF590() = default;
    virtual void deallocate(const void* allocation,
                            SharedStrDeallocationKindF590 kind) noexcept = 0;
};

// ctl must designate a live, suitably aligned int64_t counter when nonnull.
// All concurrent counter accesses must be atomic. Concurrent mutation of the
// SharedStr fields is unsupported; the data callback may synchronously change
// ctl, which is reloaded before the control callback. No fields are cleared.
//
// Ownership: each callback receives one allocation to release; the helper never
// frees either allocation itself. A null `str.data` is forwarded unchanged. The
// helper clears no fields, so `str` is dead once the last reference is released.
// Failure: there is no default or no-op allocator and no status result; a
// deallocator that cannot free an allocation is a contract violation, and
// `noexcept` means throwing terminates the process.
void release_shared_str_f590(SharedStr& str,
                             SharedStrDeallocatorF590& deallocator) noexcept;

}  // namespace sotc
