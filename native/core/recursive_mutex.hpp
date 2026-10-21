#pragma once

#include <pthread.h>

namespace sotc {

// Lifecycle calls require external synchronization; mutex operations may be shared after initialization.
// Call close() to observe destroy errors; the destructor terminates if cleanup fails.
class RecursiveMutex final {
public:
    RecursiveMutex() noexcept = default;
    ~RecursiveMutex() noexcept;

    RecursiveMutex(const RecursiveMutex&) = delete;
    RecursiveMutex& operator=(const RecursiveMutex&) = delete;
    RecursiveMutex(RecursiveMutex&&) = delete;
    RecursiveMutex& operator=(RecursiveMutex&&) = delete;

    [[nodiscard]] int initialize() noexcept;
    [[nodiscard]] int lock() noexcept;
    [[nodiscard]] int try_lock() noexcept;
    [[nodiscard]] int unlock() noexcept;
    [[nodiscard]] int close() noexcept;

private:
    pthread_mutex_t mutex_{};
    bool initialized_{false};
};

}  // namespace sotc
