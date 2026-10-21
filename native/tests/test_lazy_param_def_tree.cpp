// LazyParamDefTree contract: double-checked build under a borrowed recursive mutex, explicit errors,
// no allocation on the schema_not_available path, retry after failure, release() resets the state.
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <new>
#include <pthread.h>

#include "core/lazy_param_def_tree.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

// Global allocation counter: proves "does not allocate" claims. Counts every operator new in the process.
namespace {
std::atomic<long> g_allocations{0};
}
void* operator new(std::size_t n) {
    g_allocations.fetch_add(1, std::memory_order_relaxed);
    if (void* p = std::malloc(n ? n : 1)) return p;
    throw std::bad_alloc();
}
void* operator new(std::size_t n, const std::nothrow_t&) noexcept {
    g_allocations.fetch_add(1, std::memory_order_relaxed);
    return std::malloc(n ? n : 1);
}
void operator delete(void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }
void operator delete(void* p, const std::nothrow_t&) noexcept { std::free(p); }

namespace {

std::atomic<int> g_build_calls{0};

ParamTreeError build_ok(std::unique_ptr<ParamDefTree>& out) noexcept {
    g_build_calls.fetch_add(1);
    out.reset(new (std::nothrow) ParamDefTree{});
    if (!out) return ParamTreeError::allocation_failed;
    out->node_count = 3;   // test-only content; the real schema is not modelled
    return ParamTreeError::none;
}

ParamTreeError build_unavailable(std::unique_ptr<ParamDefTree>& out) noexcept {
    g_build_calls.fetch_add(1);
    out.reset();
    return ParamTreeError::schema_not_available;
}

ParamTreeError build_success_without_tree(std::unique_ptr<ParamDefTree>& out) noexcept {
    g_build_calls.fetch_add(1);
    out.reset();
    return ParamTreeError::none;   // lying builder: must not be cached as success
}

std::atomic<int> g_fail_first{1};
ParamTreeError build_fail_then_ok(std::unique_ptr<ParamDefTree>& out) noexcept {
    if (g_fail_first.exchange(0) == 1) {
        g_build_calls.fetch_add(1);
        return ParamTreeError::allocation_failed;
    }
    return build_ok(out);
}

struct Racer {
    LazyParamDefTree* tree;
    const ParamDefTree* seen;
    ParamTreeError error;
};
void* race_get(void* raw) {
    auto* r = static_cast<Racer*>(raw);
    const ParamTreeResult res = r->tree->get();
    r->seen = res.tree;
    r->error = res.error;
    return nullptr;
}

}  // namespace

int main() {
    // schema_not_available: explicit error, no tree, no allocation, not cached, repeatable.
    {
        RecursiveMutex mutex;
        CHECK(mutex.initialize() == 0);
        LazyParamDefTree tree(mutex, &build_unavailable);
        g_build_calls = 0;
        const long before = g_allocations.load();
        for (int i = 0; i < 3; ++i) {
            const ParamTreeResult r = tree.get();
            CHECK(r.tree == nullptr && r.error == ParamTreeError::schema_not_available);
        }
        CHECK(g_allocations.load() == before);   // zero heap allocations across three get() calls
        CHECK(g_build_calls.load() == 3);        // failure is not cached; builder retried each time
        CHECK(!tree.built());
    }

    // Null builder behaves as schema_not_available without calling anything.
    {
        RecursiveMutex mutex;
        CHECK(mutex.initialize() == 0);
        LazyParamDefTree tree(mutex, nullptr);
        const long before = g_allocations.load();
        const ParamTreeResult r = tree.get();
        CHECK(r.tree == nullptr && r.error == ParamTreeError::schema_not_available);
        CHECK(g_allocations.load() == before);
    }

    // The mutex must be usable: an uninitialized mutex yields mutex_unavailable, never a silent build.
    {
        RecursiveMutex mutex;   // not initialized
        LazyParamDefTree tree(mutex, &build_ok);
        g_build_calls = 0;
        const ParamTreeResult r = tree.get();
        CHECK(r.tree == nullptr && r.error == ParamTreeError::mutex_unavailable);
        CHECK(g_build_calls.load() == 0);
        CHECK(tree.release() == ParamTreeError::mutex_unavailable);
    }

    // Successful build is cached (builder runs once) and the same pointer is returned.
    {
        RecursiveMutex mutex;
        CHECK(mutex.initialize() == 0);
        LazyParamDefTree tree(mutex, &build_ok);
        g_build_calls = 0;
        const ParamTreeResult a = tree.get();
        CHECK(a.error == ParamTreeError::none && a.tree != nullptr && a.tree->node_count == 3);
        const ParamTreeResult b = tree.get();
        CHECK(b.tree == a.tree && b.error == ParamTreeError::none);
        CHECK(g_build_calls.load() == 1 && tree.built());

        // release() frees the tree and the next get() rebuilds it.
        CHECK(tree.release() == ParamTreeError::none);
        CHECK(!tree.built());
        const ParamTreeResult c = tree.get();
        CHECK(c.error == ParamTreeError::none && c.tree != nullptr);
        CHECK(g_build_calls.load() == 2);
    }

    // release() on a never-built tree is a harmless no-op.
    {
        RecursiveMutex mutex;
        CHECK(mutex.initialize() == 0);
        LazyParamDefTree tree(mutex, &build_unavailable);
        CHECK(tree.release() == ParamTreeError::none && !tree.built());
    }

    // A builder that reports success without a tree is an error, not a cached success.
    {
        RecursiveMutex mutex;
        CHECK(mutex.initialize() == 0);
        LazyParamDefTree tree(mutex, &build_success_without_tree);
        const ParamTreeResult r = tree.get();
        CHECK(r.tree == nullptr && r.error == ParamTreeError::allocation_failed && !tree.built());
    }

    // A failed build leaves no partial state; the retry succeeds.
    {
        RecursiveMutex mutex;
        CHECK(mutex.initialize() == 0);
        LazyParamDefTree tree(mutex, &build_fail_then_ok);
        g_fail_first = 1;
        CHECK(tree.get().error == ParamTreeError::allocation_failed && !tree.built());
        const ParamTreeResult r = tree.get();
        CHECK(r.error == ParamTreeError::none && r.tree != nullptr && tree.built());
    }

    // Concurrent first use builds exactly once and every thread sees the same tree.
    {
        RecursiveMutex mutex;
        CHECK(mutex.initialize() == 0);
        LazyParamDefTree tree(mutex, &build_ok);
        g_build_calls = 0;
        constexpr int kThreads = 8;
        Racer racers[kThreads];
        pthread_t threads[kThreads];
        for (int i = 0; i < kThreads; ++i) {
            racers[i] = {&tree, nullptr, ParamTreeError::schema_not_available};
            CHECK(pthread_create(&threads[i], nullptr, race_get, &racers[i]) == 0);
        }
        for (int i = 0; i < kThreads; ++i) CHECK(pthread_join(threads[i], nullptr) == 0);
        CHECK(g_build_calls.load() == 1);
        for (int i = 0; i < kThreads; ++i) {
            CHECK(racers[i].error == ParamTreeError::none);
            CHECK(racers[i].seen == racers[0].seen && racers[i].seen != nullptr);
        }
    }

    // The mutex is recursive: get() works while the calling thread already holds it.
    {
        RecursiveMutex mutex;
        CHECK(mutex.initialize() == 0);
        LazyParamDefTree tree(mutex, &build_ok);
        CHECK(mutex.lock() == 0);
        const ParamTreeResult r = tree.get();   // recursive: same thread already holds it
        CHECK(r.error == ParamTreeError::none && r.tree != nullptr);
        CHECK(mutex.unlock() == 0);
    }

    std::puts("lazy_param_def_tree: all checks passed");
    return 0;
}
