#include "atomic_add_ref_14ccb0.hpp"

namespace sotc {

void add_ref_14ccb0(std::atomic<std::int32_t>& field) noexcept {
    // `lock inc` is a full-barrier read-modify-write; fetch_add on signed atomics wraps (C++20).
    (void)field.fetch_add(1, std::memory_order_seq_cst);
}

void add_ref_14ccb0(void* object) noexcept {
    auto* cell = reinterpret_cast<std::int32_t*>(static_cast<unsigned char*>(object) + kAddRef14ccb0Offset);
    std::atomic_ref<std::int32_t>(*cell).fetch_add(1, std::memory_order_seq_cst);
}

}  // namespace sotc
