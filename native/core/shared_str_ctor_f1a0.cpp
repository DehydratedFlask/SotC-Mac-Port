#include "shared_str_ctor_f1a0.hpp"

#include <atomic>
#include <cstdint>
#include <cstring>

namespace sotc {

bool init_shared_str_f1a0(SharedStr& out, const char* src,
                          const SharedStrCtorHooks& hooks) noexcept {
    const std::size_t size = src == nullptr ? 0 : std::strlen(src);
    if (size == 0) {
        out = empty_string_f730();
        return true;
    }
    if (hooks.alloc.allocate == nullptr) {
        return false;
    }

    const std::size_t alloc = (size + 4) & ~static_cast<std::size_t>(3);
    auto* const data = static_cast<std::uint8_t*>(hooks.alloc.allocate(alloc, /*tag*/ 0, /*align*/ 8,
                                                                      /*flags*/ 0));
    if (data == nullptr) {
        return false;
    }
    std::memcpy(data, src, size);
    data[size] = 0;

    auto* const control = static_cast<std::int64_t*>(hooks.alloc.allocate(8, /*tag*/ 1, /*align*/ 8,
                                                                         /*flags*/ 1));
    if (control == nullptr) {
        if (hooks.alloc.deallocate != nullptr) {
            (void)hooks.alloc.deallocate(data, 0);
        }
        return false;
    }
    std::atomic_ref<std::int64_t>(*control).store(1, std::memory_order_seq_cst);

    out.data = data;
    out.size = static_cast<std::int64_t>(size);
    out.aux = static_cast<std::int64_t>(alloc - 1);
    out.ctl = control;
    return true;
}

}  // namespace sotc
