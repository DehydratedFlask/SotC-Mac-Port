#include <cerrno>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <future>
#include <thread>

#include "core/critsec_3b310.hpp"
#include "core/ctor_tiny_misc_slots.hpp"

#define CHECK(c)                                                                                   \
    do {                                                                                           \
        if (!(c)) {                                                                                \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);                       \
            std::exit(1);                                                                          \
        }                                                                                          \
    } while (0)

int main() {
    using sotc::CritSecHandle;
    using sotc::critsec_destroy_at;
    using sotc::critsec_init_at;
    using sotc::critsec_lock_at;
    using sotc::critsec_unlock_at;

    CHECK(sotc::kCritSecCtorRva == 0x3b310 && sotc::kCritSecDtorRva == 0x3b380);
    CHECK(sotc::kCritSecHandleBytes == 8);

    CritSecHandle h{};
    CHECK(critsec_init_at(nullptr) == EINVAL);
    CHECK(h.initialize() == 0);
    CHECK(h.initialize() == EALREADY);
    CHECK(h.lock() == 0 && h.lock() == 0 && h.unlock() == 0 && h.unlock() == 0);
    CHECK(h.close() == 0);
    CHECK(h.lock() == EINVAL);

    CritSecHandle h2{};
    CHECK(h2.initialize() == 0);
    CHECK(h2.lock() == 0);
    CHECK(h2.destroy() == EBUSY);
    CHECK(h2.unlock() == 0);
    CHECK(h2.destroy() == 0);

    // Deterministic owner/waiter fixture. While the waiter is blocked in pthread_mutex_lock, the
    // owner must still be able to enter unlock_at. Every wait is bounded; a registry-lock deadlock
    // exits the fixture instead of hanging CTest indefinitely.
    using namespace std::chrono_literals;
    CritSecHandle contended{};
    CHECK(contended.initialize() == 0);

    std::promise<int> owner_lock_promise;
    std::future<int> owner_lock = owner_lock_promise.get_future();
    std::promise<void> allow_owner_unlock_promise;
    std::shared_future<void> allow_owner_unlock = allow_owner_unlock_promise.get_future().share();
    std::promise<int> owner_unlock_promise;
    std::future<int> owner_unlock = owner_unlock_promise.get_future();
    std::thread owner([&] {
        const int lock_rc = contended.lock();
        owner_lock_promise.set_value(lock_rc);
        if (lock_rc != 0) return;
        if (allow_owner_unlock.wait_for(3s) != std::future_status::ready) {
            owner_unlock_promise.set_value(ETIMEDOUT);
            return;
        }
        owner_unlock_promise.set_value(contended.unlock());
    });

    CHECK(owner_lock.wait_for(3s) == std::future_status::ready);
    CHECK(owner_lock.get() == 0);

    std::promise<void> waiter_started_promise;
    std::future<void> waiter_started = waiter_started_promise.get_future();
    std::promise<int> waiter_lock_promise;
    std::future<int> waiter_lock = waiter_lock_promise.get_future();
    std::promise<int> waiter_unlock_promise;
    std::future<int> waiter_unlock = waiter_unlock_promise.get_future();
    std::thread waiter([&] {
        waiter_started_promise.set_value();
        const int lock_rc = contended.lock();
        waiter_lock_promise.set_value(lock_rc);
        if (lock_rc == 0) waiter_unlock_promise.set_value(contended.unlock());
    });

    CHECK(waiter_started.wait_for(3s) == std::future_status::ready);
    CHECK(waiter_lock.wait_for(100ms) == std::future_status::timeout);
    allow_owner_unlock_promise.set_value();

    CHECK(owner_unlock.wait_for(3s) == std::future_status::ready);
    CHECK(owner_unlock.get() == 0);
    CHECK(waiter_lock.wait_for(3s) == std::future_status::ready);
    CHECK(waiter_lock.get() == 0);
    CHECK(waiter_unlock.wait_for(3s) == std::future_status::ready);
    CHECK(waiter_unlock.get() == 0);
    owner.join();
    waiter.join();
    CHECK(contended.destroy() == 0);

    const sotc::Slot414Hooks hooks = sotc::default_critsec_slot414_hooks();
    CHECK(hooks.mutex_init_3b310 != nullptr && hooks.mutex_destroy_3b380 != nullptr);
    std::uint8_t obj[sotc::kSlot414ObjectSize]{};
    CHECK(hooks.mutex_init_3b310(obj));
    CHECK(hooks.mutex_destroy_3b380(obj));

    std::puts("OK test_critsec_3b310");
    return 0;
}
