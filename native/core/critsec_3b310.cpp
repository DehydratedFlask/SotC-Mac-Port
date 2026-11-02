#include "critsec_3b310.hpp"

#include <cerrno>
#include <cstring>
#include <memory>
#include <mutex>
#include <new>
#include <pthread.h>
#include <unordered_map>

namespace sotc {
namespace {

// The game-facing object contains only a token. pthread_mutex_t is initialized in its final,
// stable allocation and is never copied or moved after pthread_mutex_init.
struct Sidecar {
    pthread_mutex_t mutex{};
    bool initialized = false;
    bool closing = false;
    std::size_t active_leases = 0;
};

std::mutex g_registry_mutex;
std::unordered_map<std::uint64_t, std::unique_ptr<Sidecar>> g_sidecars;
std::uint64_t g_next_token = 1;

struct Lease {
    std::uint64_t token = 0;
    Sidecar* sidecar = nullptr;
};

std::uint64_t read_token(const void* handle_bytes) noexcept {
    std::uint64_t token = 0;
    std::memcpy(&token, handle_bytes, sizeof(token));
    return token;
}

bool acquire_lease(const void* handle_bytes, Lease& lease) noexcept {
    if (handle_bytes == nullptr) return false;

    std::lock_guard<std::mutex> lock(g_registry_mutex);
    const std::uint64_t token = read_token(handle_bytes);
    if (token == 0) return false;

    const auto it = g_sidecars.find(token);
    if (it == g_sidecars.end() || it->second == nullptr ||
        !it->second->initialized || it->second->closing) {
        return false;
    }
    if (it->second->active_leases == static_cast<std::size_t>(-1)) return false;

    ++it->second->active_leases;
    lease.token = token;
    lease.sidecar = it->second.get();
    return true;
}

void release_lease(Sidecar* sidecar) noexcept {
    std::lock_guard<std::mutex> lock(g_registry_mutex);
    if (sidecar != nullptr && sidecar->active_leases != 0) --sidecar->active_leases;
}

void reopen_after_failed_destroy(std::uint64_t token, Sidecar* sidecar) noexcept {
    std::lock_guard<std::mutex> lock(g_registry_mutex);
    const auto it = g_sidecars.find(token);
    if (it != g_sidecars.end() && it->second.get() == sidecar) sidecar->closing = false;
}

std::uint64_t allocate_token_locked() noexcept {
    // Tokens are process-local identities. Skip zero and any live token after wraparound.
    const std::uint64_t first = g_next_token;
    do {
        std::uint64_t candidate = g_next_token++;
        if (candidate == 0) candidate = g_next_token++;
        if (candidate != 0 && g_sidecars.find(candidate) == g_sidecars.end()) return candidate;
    } while (g_next_token != first);
    return 0;
}

}  // namespace

int critsec_init_at(void* handle_bytes) noexcept {
    if (handle_bytes == nullptr) return EINVAL;

    std::unique_ptr<Sidecar> sidecar(new (std::nothrow) Sidecar{});
    if (sidecar == nullptr) return ENOMEM;

    pthread_mutexattr_t attr;
    int rc = pthread_mutexattr_init(&attr);
    if (rc != 0) return rc;

    rc = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    if (rc != 0) {
        (void)pthread_mutexattr_destroy(&attr);
        return rc;
    }

    rc = pthread_mutex_init(&sidecar->mutex, &attr);
    const int attr_destroy_rc = pthread_mutexattr_destroy(&attr);
    if (rc != 0) return rc;
    if (attr_destroy_rc != 0) {
        (void)pthread_mutex_destroy(&sidecar->mutex);
        return attr_destroy_rc;
    }
    sidecar->initialized = true;

    try {
        std::lock_guard<std::mutex> lock(g_registry_mutex);
        // Recheck while serialized in case another initializer received the same byte buffer. The retail
        // shim never reads the field - it hands it to scePthreadMutexInit, which overwrites it - so only a
        // token that is still live in this registry can mean "already initialised". Arbitrary contents
        // (a fresh, unzeroed allocation) are overwritten, exactly like the imported call would.
        const std::uint64_t existing = read_token(handle_bytes);
        if (existing != 0 && g_sidecars.find(existing) != g_sidecars.end()) {
            (void)pthread_mutex_destroy(&sidecar->mutex);
            return EALREADY;
        }

        const std::uint64_t token = allocate_token_locked();
        if (token == 0) {
            (void)pthread_mutex_destroy(&sidecar->mutex);
            return ENOSPC;
        }

        const auto [it, inserted] = g_sidecars.try_emplace(token, nullptr);
        if (!inserted) {
            (void)pthread_mutex_destroy(&sidecar->mutex);
            return EAGAIN;
        }
        it->second = std::move(sidecar);
        std::memcpy(handle_bytes, &token, sizeof(token));
    } catch (...) {
        // unordered_map allocation can throw; pthread APIs and this exported API use errno-style
        // results, so no exception may escape the noexcept boundary.
        if (sidecar != nullptr && sidecar->initialized) {
            (void)pthread_mutex_destroy(&sidecar->mutex);
            sidecar->initialized = false;
        }
        return ENOMEM;
    }
    return 0;
}

int critsec_destroy_at(void* handle_bytes) noexcept {
    if (handle_bytes == nullptr) return EINVAL;

    Lease lease{};
    if (!acquire_lease(handle_bytes, lease)) return EINVAL;

    // A nonblocking probe lets a held mutex (including a recursive lock owned by this thread)
    // remain registered. No registry lock is held while touching pthread state.
    int rc = pthread_mutex_trylock(&lease.sidecar->mutex);
    if (rc != 0) {
        release_lease(lease.sidecar);
        return rc;
    }

    bool may_destroy = false;
    {
        std::lock_guard<std::mutex> lock(g_registry_mutex);
        const auto it = g_sidecars.find(lease.token);
        if (it != g_sidecars.end() && it->second.get() == lease.sidecar &&
            lease.sidecar->initialized && !lease.sidecar->closing &&
            lease.sidecar->active_leases == 1) {
            // This blocks new leases only after proving no other operation is already using or
            // waiting on the mutex. Operations that raced before this point force EBUSY below.
            lease.sidecar->closing = true;
            may_destroy = true;
        }
    }

    if (!may_destroy) {
        (void)pthread_mutex_unlock(&lease.sidecar->mutex);
        release_lease(lease.sidecar);
        return EBUSY;
    }

    rc = pthread_mutex_unlock(&lease.sidecar->mutex);
    if (rc != 0) {
        reopen_after_failed_destroy(lease.token, lease.sidecar);
        release_lease(lease.sidecar);
        return rc;
    }

    rc = pthread_mutex_destroy(&lease.sidecar->mutex);
    if (rc != 0) {
        reopen_after_failed_destroy(lease.token, lease.sidecar);
        release_lease(lease.sidecar);
        return rc;
    }

    {
        std::lock_guard<std::mutex> lock(g_registry_mutex);
        const auto it = g_sidecars.find(lease.token);
        if (it == g_sidecars.end() || it->second.get() != lease.sidecar ||
            lease.sidecar->active_leases != 1 || !lease.sidecar->closing) {
            // Invariants make this path unreachable: closing excludes new leases and the one
            // remaining lease is this destroy operation. Keep the byte handle intact on anomaly.
            return EINVAL;
        }
        lease.sidecar->initialized = false;
        --lease.sidecar->active_leases;
        g_sidecars.erase(it);
        const std::uint64_t empty = 0;
        std::memcpy(handle_bytes, &empty, sizeof(empty));
    }
    return 0;
}

std::size_t critsec_live_sidecars() noexcept {
    std::lock_guard<std::mutex> lock(g_registry_mutex);
    return g_sidecars.size();
}

int critsec_lock_at(const void* handle_bytes) noexcept {
    Lease lease{};
    if (!acquire_lease(handle_bytes, lease)) return EINVAL;

    // This may block; the operation lease keeps the stable sidecar alive while the registry lock
    // is free so the current owner can enter unlock_at.
    const int rc = pthread_mutex_lock(&lease.sidecar->mutex);
    release_lease(lease.sidecar);
    return rc;
}

int critsec_unlock_at(const void* handle_bytes) noexcept {
    Lease lease{};
    if (!acquire_lease(handle_bytes, lease)) return EINVAL;

    // Unlock is also outside the registry lock. The lease protects storage through the call.
    const int rc = pthread_mutex_unlock(&lease.sidecar->mutex);
    release_lease(lease.sidecar);
    return rc;
}

namespace {

bool critsec_init_hook(void* object) noexcept { return critsec_init_at(object) == 0; }

bool critsec_destroy_hook(void* object) noexcept { return critsec_destroy_at(object) == 0; }

}  // namespace

Slot414Hooks default_critsec_slot414_hooks() noexcept {
    Slot414Hooks hooks{};
    hooks.mutex_init_3b310 = critsec_init_hook;
    hooks.mutex_destroy_3b380 = critsec_destroy_hook;
    return hooks;
}

}  // namespace sotc
