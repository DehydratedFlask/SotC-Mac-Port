#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <thread>

#include "core/recursive_mutex.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

struct CompetingTry {
    int try_rc{EINVAL};
    int unlock_rc{EINVAL};
};

static CompetingTry try_from_another_thread(sotc::RecursiveMutex& mutex) {
    CompetingTry result;
    std::thread competitor([&] {
        result.try_rc = mutex.try_lock();
        if (result.try_rc == 0) result.unlock_rc = mutex.unlock();
    });
    competitor.join();
    return result;
}

static int unlock_from_another_thread(sotc::RecursiveMutex& mutex) {
    int unlock_rc = EINVAL;
    std::thread competitor([&] { unlock_rc = mutex.unlock(); });
    competitor.join();
    return unlock_rc;
}

int main() {
    using sotc::RecursiveMutex;

    RecursiveMutex mutex;
    CHECK(mutex.lock() == EINVAL);
    CHECK(mutex.try_lock() == EINVAL);
    CHECK(mutex.unlock() == EINVAL);
    CHECK(mutex.close() == EINVAL);

    CHECK(mutex.initialize() == 0);
    CHECK(mutex.initialize() == EALREADY);
    CHECK(mutex.lock() == 0);
    CHECK(mutex.lock() == 0);
    CHECK(unlock_from_another_thread(mutex) == EPERM);

    const CompetingTry while_fully_held = try_from_another_thread(mutex);
    CHECK(while_fully_held.try_rc == EBUSY);
    CHECK(while_fully_held.unlock_rc == EINVAL);

    CHECK(mutex.unlock() == 0);
    const CompetingTry while_recursion_remains = try_from_another_thread(mutex);
    CHECK(while_recursion_remains.try_rc == EBUSY);
    CHECK(while_recursion_remains.unlock_rc == EINVAL);

    CHECK(mutex.close() == EBUSY);
    CHECK(mutex.unlock() == 0);  // failed close kept ownership initialized

    const CompetingTry after_final_unlock = try_from_another_thread(mutex);
    CHECK(after_final_unlock.try_rc == 0);
    CHECK(after_final_unlock.unlock_rc == 0);
    CHECK(mutex.close() == 0);
    CHECK(mutex.lock() == EINVAL);
    CHECK(mutex.try_lock() == EINVAL);
    CHECK(mutex.unlock() == EINVAL);

    CHECK(mutex.initialize() == 0);
    CHECK(mutex.close() == 0);
    CHECK(mutex.initialize() == 0);
    CHECK(mutex.close() == 0);

    {
        RecursiveMutex destructor_owned;
        CHECK(destructor_owned.initialize() == 0);
    }

    std::puts("recursive_mutex: all checks passed");
    return 0;
}
