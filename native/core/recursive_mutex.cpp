#include "recursive_mutex.hpp"

#include <cerrno>
#include <exception>

namespace sotc {

RecursiveMutex::~RecursiveMutex() noexcept {
    if (initialized_ && close() != 0) std::terminate();
}

int RecursiveMutex::initialize() noexcept {
    if (initialized_) return EALREADY;

    pthread_mutexattr_t attr;
    int rc = pthread_mutexattr_init(&attr);
    if (rc != 0) return rc;

    const auto destroy_attr = [&]() noexcept {
        // Attribute cleanup is an invariant; do not silently discard its error.
        if (pthread_mutexattr_destroy(&attr) != 0) std::terminate();
    };

    rc = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    if (rc != 0) {
        destroy_attr();
        return rc;
    }

    rc = pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);
    if (rc != 0) {
        destroy_attr();
        return rc;
    }

    rc = pthread_mutex_init(&mutex_, &attr);
    if (rc == 0) initialized_ = true;
    destroy_attr();
    return rc;
}

int RecursiveMutex::lock() noexcept {
    return initialized_ ? pthread_mutex_lock(&mutex_) : EINVAL;
}

int RecursiveMutex::try_lock() noexcept {
    return initialized_ ? pthread_mutex_trylock(&mutex_) : EINVAL;
}

int RecursiveMutex::unlock() noexcept {
    return initialized_ ? pthread_mutex_unlock(&mutex_) : EINVAL;
}

int RecursiveMutex::close() noexcept {
    if (!initialized_) return EINVAL;

    const int rc = pthread_mutex_destroy(&mutex_);
    if (rc == 0) initialized_ = false;
    return rc;
}

}  // namespace sotc
