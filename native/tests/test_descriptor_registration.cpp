// Lifetime contract for the 34 descriptor+mutex+atexit constructors (docs/CTOR_CLASSES.md).
// Tests the native one-shot publication model and mutex ownership; they do not exercise the
// original heap block, the atexit path or any registration in startup_ctors.
#include <cstdio>
#include <cstdlib>
#include <pthread.h>

#include "core/descriptor_registration.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {

struct ThreadArgs {
    RecursiveMutex* mutex;
    int locks;
    int rc;
};

void* lock_unlock_twice(void* raw) {
    auto* args = static_cast<ThreadArgs*>(raw);
    for (int i = 0; i < args->locks; ++i) {
        args->rc = args->mutex->lock();
        if (args->rc != 0) return nullptr;
        args->rc = args->mutex->unlock();
        if (args->rc != 0) return nullptr;
    }
    return nullptr;
}

}  // namespace

int main() {
    // Default state: nothing published, no descriptor.
    {
        DescriptorRegistration registration;
        CHECK(!registration.published());
        CHECK(registration.descriptor() == nullptr);
    }

    // Null descriptors are refused and leave the registration untouched.
    {
        DescriptorRegistration registration;
        CHECK(registration.publish(nullptr) == DescriptorPublishError::null_descriptor);
        CHECK(!registration.published());
        CHECK(registration.descriptor() == nullptr);
    }

    // One-shot publication; a second publish must not overwrite the first descriptor.
    {
        static int first_cell = 1;
        static int second_cell = 2;
        DescriptorRegistration registration;
        CHECK(registration.publish(&first_cell) == DescriptorPublishError::none);
        CHECK(registration.published());
        CHECK(registration.descriptor() == &first_cell);
        CHECK(registration.publish(&second_cell) == DescriptorPublishError::already_published);
        CHECK(registration.descriptor() == &first_cell);
    }

    // Independent registrations do not share state.
    {
        static int cell_a = 1;
        static int cell_b = 2;
        DescriptorRegistration a;
        DescriptorRegistration b;
        CHECK(a.publish(&cell_a) == DescriptorPublishError::none);
        CHECK(b.publish(&cell_b) == DescriptorPublishError::none);
        CHECK(a.descriptor() != b.descriptor());
        CHECK(a.published() && b.published());
    }

    // The owned mutex is recursive and usable after publication.
    {
        static int cell = 3;
        DescriptorRegistration registration;
        CHECK(registration.publish(&cell) == DescriptorPublishError::none);
        RecursiveMutex& mutex = registration.mutex();
        CHECK(mutex.lock() == 0);
        CHECK(mutex.lock() == 0);   // recursive: same thread may re-enter
        CHECK(mutex.unlock() == 0);
        CHECK(mutex.unlock() == 0);
    }

    // The mutex provides real exclusion for another thread.
    {
        static int cell = 4;
        DescriptorRegistration registration;
        CHECK(registration.publish(&cell) == DescriptorPublishError::none);
        RecursiveMutex& mutex = registration.mutex();
        CHECK(mutex.lock() == 0);

        ThreadArgs args{&mutex, 1, 0};
        pthread_t thread;
        CHECK(pthread_create(&thread, nullptr, lock_unlock_twice, &args) == 0);
        // Give the helper a chance to block on the held mutex; it must not complete before unlock.
        pthread_yield_np();
        CHECK(mutex.unlock() == 0);
        CHECK(pthread_join(thread, nullptr) == 0);
        CHECK(args.rc == 0);
    }

    std::puts("descriptor_registration: all checks passed");
    return 0;
}
