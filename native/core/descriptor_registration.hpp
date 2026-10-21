// Native lifetime model shared by the 34 "descriptor + heap mutex + atexit" constructors
// (constructor-table indices 10-19 and 24-47; see docs/CTOR_CLASSES.md).
//
// Original contract, byte-verified per slot:
//   1. publish one or two adjacent .bss globals (descriptor address point, then mutex pointer),
//   2. malloc a 16-byte block (size word at 0x12d2d1c) and initialize a recursive +
//      priority-inherit pthread mutex in it through 0xb30950,
//   3. register the shared destroy+free destructor 0xe44a10 with __cxa_atexit(dtor, &global,
//      0x1604218).
//
// Native model: this is the settled StartupState ownership decision already used for slot 10.
// The descriptor is an ordinary pointer, the mutex is owned by value, and cleanup is deterministic
// RAII instead of atexit. The original's heap block, its 16-byte size word and the atexit ordering
// are NOT reproduced; nothing here is binary compatible with the original object layout.
#pragma once

#include <cstdint>

#include "recursive_mutex.hpp"

namespace sotc {

enum class DescriptorPublishError : std::uint8_t {
    none = 0,
    null_descriptor = 1,     // the original never publishes a null address point
    already_published = 2,   // one-shot: the original's guard cell makes re-publication a no-op
    mutex_init_failed = 3,   // the original cannot report this; it would leave an unusable mutex
};

// Registers one published descriptor and owns the mutex paired with it.
// Lifecycle calls require external synchronization; the mutex may be used once published.
class DescriptorRegistration final {
public:
    DescriptorRegistration() noexcept = default;
    ~DescriptorRegistration() noexcept = default;

    DescriptorRegistration(const DescriptorRegistration&) = delete;
    DescriptorRegistration& operator=(const DescriptorRegistration&) = delete;
    DescriptorRegistration(DescriptorRegistration&&) = delete;
    DescriptorRegistration& operator=(DescriptorRegistration&&) = delete;

    // Initializes the owned mutex and then records the descriptor. Returns the first error and, on
    // error, leaves the registration unpublished with the previous descriptor untouched.
    [[nodiscard]] DescriptorPublishError publish(const void* descriptor) noexcept;

    [[nodiscard]] bool published() const noexcept { return published_; }
    [[nodiscard]] const void* descriptor() const noexcept { return descriptor_; }

    // Valid only after a successful publish(); the original also exposes the mutex indirectly
    // through the published global.
    [[nodiscard]] RecursiveMutex& mutex() noexcept { return mutex_; }

private:
    const void* descriptor_ = nullptr;
    RecursiveMutex mutex_{};
    bool published_ = false;
};

}  // namespace sotc
