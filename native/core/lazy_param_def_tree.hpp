// Lazily built parameter-definition ("schema") tree guarded by a recursive mutex.
//
// Original contract (CPU-01-CTOR11-REFLECTION-SCOPE-01, class ClothingPhysicalMeshParameters):
//   built flag 0x3356070 (plain byte), mutex handle 0x3356078, table pointer 0x3356080,
//   getters 0xf06c80 / 0xf08bc0 (double-checked: test flag, lock 0xb309e0, re-test, build via
//   0xf06cd0, unlock 0xb30a00, return the table), free routine 0xf08d30 (destroy nodes, clear flag).
//
// Native model: ordinary C++. The mutex is borrowed from the owning DescriptorRegistration (that is
// the original mutex handle), the built flag is a std::atomic<bool> with release/acquire ordering and
// the tree is owned by a unique_ptr. The builder is injected; if the schema data is not supplied the
// builder reports ParamTreeError::schema_not_available and nothing is allocated, cached or faked.
#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>

#include "recursive_mutex.hpp"

namespace sotc {

enum class ParamTreeError : std::uint8_t {
    none = 0,
    schema_not_available = 1,   // the 74-record schema data has not been supplied (policy decision pending)
    allocation_failed = 2,      // the builder could not allocate, or reported success without a tree
    mutex_unavailable = 3,      // the owning mutex is not initialized (descriptor not published yet)
};

// Placeholder root of the parameter-definition tree. Deliberately carries no node layout: the original
// 74 x 0x98-byte records and their names are not modelled (schema provenance is an open principal
// decision). Nothing in the native tree may be invented to make get() succeed.
struct ParamDefTree {
    std::size_t node_count = 0;
};

struct ParamTreeResult {
    const ParamDefTree* tree;   // null whenever error != none
    ParamTreeError error;
};

class LazyParamDefTree final {
public:
    // Builder contract: on success set `out` to the freshly built tree and return none. On failure
    // return the error and leave `out` empty. Must not throw.
    using BuildFn = ParamTreeError (*)(std::unique_ptr<ParamDefTree>& out) noexcept;

    // `mutex` must outlive this object. A null `build` behaves as schema_not_available.
    LazyParamDefTree(RecursiveMutex& mutex, BuildFn build) noexcept : mutex_(mutex), build_(build) {}
    ~LazyParamDefTree() noexcept;

    LazyParamDefTree(const LazyParamDefTree&) = delete;
    LazyParamDefTree& operator=(const LazyParamDefTree&) = delete;
    LazyParamDefTree(LazyParamDefTree&&) = delete;
    LazyParamDefTree& operator=(LazyParamDefTree&&) = delete;

    // Original getters 0xf06c80 / 0xf08bc0. Thread-safe. Never returns an empty-but-successful tree.
    // A failed build is not cached: the next call retries the builder.
    [[nodiscard]] ParamTreeResult get() noexcept;

    // Original free routine 0xf08d30. The caller guarantees no thread still uses a tree pointer
    // returned by get(); a concurrent get() is serialized by the mutex but earlier pointers dangle.
    // Returns the mutex error if it could not lock (the tree is then left untouched).
    [[nodiscard]] ParamTreeError release() noexcept;

    [[nodiscard]] bool built() const noexcept { return built_.load(std::memory_order_acquire); }

private:
    RecursiveMutex& mutex_;
    BuildFn build_;
    std::atomic<bool> built_{false};
    std::unique_ptr<ParamDefTree> tree_;
};

}  // namespace sotc
