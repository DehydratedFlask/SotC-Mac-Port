#include "ctor_98a90_3be050.hpp"

#include <cstring>

namespace sotc {
namespace {

void zero_xmm(void* p) noexcept {
    std::memset(p, 0, 16);
}

}  // namespace

Ctor98a90_3be050Result init_98a90_3be050(AllocatorFront& alloc, void* obj,
                                         const Ctor98a90_3be050Hooks& hooks) {
    Ctor98a90_3be050Result out{};
    if (obj == nullptr) {
        return out;
    }
    auto* base = static_cast<std::uint8_t*>(obj);

    // OBS 0x3be064..0x3be06d: vxor; vmovups [r14+8], xmm0
    zero_xmm(base + 8);

    // OBS 0x3be068..0x3be093: alloc 0x48 nested sentinel → store at [obj+8]
    void* nested = alloc.new_(kCtor98a90NestedBytes);
    if (nested == nullptr) {
        return out;
    }
    std::memset(nested, 0, kCtor98a90NestedBytes);
    auto* n = static_cast<std::uint8_t*>(nested);
    // Circular sentinel pointers (OBS mov [rax],rax / +8 / +0x10)
    std::memcpy(n + 0x00, &nested, sizeof(void*));
    std::memcpy(n + 0x08, &nested, sizeof(void*));
    std::memcpy(n + 0x10, &nested, sizeof(void*));
    n[0x18] = 1;
    n[0x19] = 1;
    std::memcpy(base + 8, &nested, sizeof(void*));
    out.nested_48 = nested;

    // OBS 0x3be097..0x3be0b0
    base[0x18] = 0;
    zero_xmm(base + 0x28);
    std::memset(base + 0x38, 0, 8);
    zero_xmm(base + 0x55);
    zero_xmm(base + 0x48);

    // OBS 0x3be0b6: call 0x3be930(obj+0x68)
    if (hooks.sub_3be930 != nullptr) {
        hooks.sub_3be930(base + 0x68);
    }

    // OBS 0x3be0bb..0x3be0c2: lea rdi,[obj+0x98a88]; call 0x323d0
    if (hooks.store_tsc_323d0 != nullptr) {
        hooks.store_tsc_323d0(base + kCtor98a90TscOffset);
    }

    // OBS 0x3be0c7..0x3be0f3: capacity from [+0x38]-[+0x28]>>3; ensure if <=0xff
    std::uint64_t end = 0;
    std::uint64_t begin = 0;
    std::memcpy(&end, base + 0x38, 8);
    std::memcpy(&begin, base + 0x28, 8);
    const std::uint64_t cap = (end - begin) >> 3;
    if (cap <= 0xff) {
        out.ensure_called = true;
        if (hooks.ensure_3c2f70 != nullptr) {
            hooks.ensure_3c2f70(base + 0x20, 0x100);
        }
    }

    // OBS 0x3be0f8..0x3be2c2: remainder (0x80 path) — hook only
    if (hooks.remainder_from_3be0f8 != nullptr) {
        hooks.remainder_from_3be0f8(obj);
    }

    out.ok = (out.nested_48 != nullptr);
    return out;
}

}  // namespace sotc
