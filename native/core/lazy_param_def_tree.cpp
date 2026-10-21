#include "lazy_param_def_tree.hpp"

namespace sotc {

namespace {

// Holds the lock only if lock() succeeded; unlock errors cannot be recovered from here.
struct ScopedLock {
    explicit ScopedLock(RecursiveMutex& m) noexcept : mutex(m), rc(m.lock()) {}
    ~ScopedLock() noexcept {
        if (rc == 0) (void)mutex.unlock();
    }
    RecursiveMutex& mutex;
    int rc;
};

}  // namespace

LazyParamDefTree::~LazyParamDefTree() noexcept = default;

ParamTreeResult LazyParamDefTree::get() noexcept {
    if (built_.load(std::memory_order_acquire)) return {tree_.get(), ParamTreeError::none};

    ScopedLock guard(mutex_);
    if (guard.rc != 0) return {nullptr, ParamTreeError::mutex_unavailable};

    if (built_.load(std::memory_order_relaxed)) return {tree_.get(), ParamTreeError::none};

    if (build_ == nullptr) return {nullptr, ParamTreeError::schema_not_available};

    std::unique_ptr<ParamDefTree> fresh;
    const ParamTreeError error = build_(fresh);
    if (error != ParamTreeError::none) return {nullptr, error};
    if (!fresh) return {nullptr, ParamTreeError::allocation_failed};

    tree_ = std::move(fresh);
    built_.store(true, std::memory_order_release);
    return {tree_.get(), ParamTreeError::none};
}

ParamTreeError LazyParamDefTree::release() noexcept {
    ScopedLock guard(mutex_);
    if (guard.rc != 0) return ParamTreeError::mutex_unavailable;
    built_.store(false, std::memory_order_release);
    tree_.reset();
    return ParamTreeError::none;
}

}  // namespace sotc
