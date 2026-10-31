#include "init_531d60_post_base_a431e0_u1.hpp"

#include <cstring>

namespace sotc {
namespace {

std::uint8_t flag_from(const std::uint8_t* byte) noexcept {
    if (byte == nullptr) {
        return 0;
    }
    return (*byte == 0) ? 1 : 0;
}

void* load_ptr(const void* base, std::size_t offset) noexcept {
    void* value = nullptr;
    if (base == nullptr) {
        return nullptr;
    }
    std::memcpy(&value, static_cast<const std::uint8_t*>(base) + offset, sizeof(value));
    return value;
}

const std::uint8_t* flag_byte_from_demo(const void* demo_object) noexcept {
    const void* bound = load_ptr(demo_object, kLazyBind91210BoundOffset);
    if (bound == nullptr) {
        return nullptr;
    }
    return static_cast<const std::uint8_t*>(load_ptr(bound, 0x60));
}

const std::uint8_t* resolve_flag(const Init531d60PostBaseA431e0U1Hooks& hooks,
                                 const std::uint8_t* supplied) noexcept {
    if (supplied != nullptr) {
        return supplied;
    }
    if (!hooks.use_native_lazy_bind_91210) {
        return nullptr;
    }
    return flag_byte_from_demo(hooks.demo_object);
}

}  // namespace

Init531d60PostBaseA431e0U1Result init_531d60_post_base_a431e0_u1(
    const Init531d60PostBaseA431e0U1Hooks& hooks) noexcept {
    Init531d60PostBaseA431e0U1Result out{};

    const void* rbx = (hooks.cell_32f6d18 != nullptr) ? *hooks.cell_32f6d18 : nullptr;

    if (hooks.use_native_lazy_bind_91210) {
        const LazyBind91210Result bound = run_lazy_bind_91210(hooks.demo_object, hooks.lazy_bind);
        ++out.lazy_bind_calls;
        out.lazy_bind_ok = bound.ok;
        if (!bound.ok) {
            return out;
        }
    } else if (hooks.vcall_slot0 != nullptr) {
        hooks.vcall_slot0(const_cast<void*>(hooks.cell_2efe318 != nullptr
                                                ? static_cast<const void*>(hooks.cell_2efe318)
                                                : nullptr),
                          0);
    }
    ++out.vcall_count;

    const std::uint8_t* flag2 = resolve_flag(hooks, hooks.flag_byte_iter2);
    const std::uint8_t sil = flag_from(flag2);
    out.flag2_known = (flag2 != nullptr);
    out.sil_iter2 = sil;
    out.rdi_arg = const_cast<void*>(rbx);
    out.esi_arg = sil;

    if (hooks.call_a431e0 != nullptr) {
        hooks.call_a431e0(out.rdi_arg, sil);
    }
    ++out.call_a431e0_count;

    out.ok = (out.call_a431e0_count == 1) && (out.vcall_count == 1);
    return out;
}

}  // namespace sotc
