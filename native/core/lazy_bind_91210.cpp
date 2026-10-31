#include "lazy_bind_91210.hpp"

#include <cstring>

namespace sotc {
namespace {

[[nodiscard]] void* load_ptr(void* base, std::size_t offset) noexcept {
    void* value = nullptr;
    std::memcpy(&value, static_cast<std::uint8_t*>(base) + offset, sizeof(value));
    return value;
}

void store_ptr(void* base, std::size_t offset, void* value) noexcept {
    std::memcpy(static_cast<std::uint8_t*>(base) + offset, &value, sizeof(value));
}

}  // namespace

LazyBind91210Result run_lazy_bind_91210(void* object, const LazyBind91210Hooks& hooks) noexcept {
    LazyBind91210Result out{};
    if (object == nullptr) {
        return out;
    }

    if (load_ptr(object, kLazyBind91210BoundOffset) != nullptr) {
        out.fast_path = true;
        out.returned = 1;
        out.ok = true;
        return out;
    }
    if (!hooks.complete()) {
        return out;
    }

    void* name_slot = static_cast<std::uint8_t*>(object) + kLazyBind91210NameSlotOffset;
    void* sub = hooks.lookup_21620(hooks.host, name_slot);
    if (sub != nullptr && hooks.vcall_18(hooks.host, sub) == 1) {
        store_ptr(object, kLazyBind91210BoundOffset, sub);
        out.stored = true;
        if (hooks.vcall_30(hooks.host, sub) != 0) {
            store_ptr(object, kLazyBind91210BoundOffset, nullptr);
            out.rolled_back = true;
        }
    }

    out.returned = load_ptr(object, kLazyBind91210BoundOffset) != nullptr ? 1 : 0;
    out.ok = true;
    return out;
}

}  // namespace sotc
