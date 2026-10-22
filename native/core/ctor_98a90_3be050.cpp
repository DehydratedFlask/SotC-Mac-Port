#include "ctor_98a90_3be050.hpp"

#include <cstring>

namespace sotc {
namespace {

void zero_xmm(void* p) noexcept { std::memset(p, 0, 16); }

[[nodiscard]] std::uint8_t* load_ptr(std::uint8_t* base, std::size_t off) noexcept {
    void* p = nullptr;
    std::memcpy(&p, base + off, sizeof(void*));
    return static_cast<std::uint8_t*>(p);
}

void store_ptr(std::uint8_t* base, std::size_t off, void* p) noexcept {
    std::memcpy(base + off, &p, sizeof(void*));
}

// OBS 0x3be0f8..0x3be1cb: grow 0x20-stride vector at +0x48/+0x50/+0x58 to 4 slots.
[[nodiscard]] bool grow_vec_buf_80(AllocatorFront& alloc, std::uint8_t* base,
                                   const Ctor98a90_3be050Hooks& hooks, void*& out_buf) noexcept {
    std::uint8_t* begin = load_ptr(base, kCtor98a90VecBegin);
    std::uint8_t* end = load_ptr(base, kCtor98a90VecEnd);
    std::uint8_t* cap = load_ptr(base, kCtor98a90VecCap);

    const std::uint64_t capacity_elems =
        (reinterpret_cast<std::uintptr_t>(cap) - reinterpret_cast<std::uintptr_t>(begin)) >> 5;
    if (capacity_elems > 3) {
        return true;  // OBS ja 0x3be1ce
    }

    void* neu = alloc.new_(kCtor98a90BufBytes);
    if (neu == nullptr) {
        return false;
    }
    std::memset(neu, 0, kCtor98a90BufBytes);
    auto* dst = static_cast<std::uint8_t*>(neu);
    const std::uint64_t count =
        (begin != nullptr && end != nullptr)
            ? ((reinterpret_cast<std::uintptr_t>(end) - reinterpret_cast<std::uintptr_t>(begin)) >>
               5)
            : 0;

    // OBS: copy 0x20 elems begin..end into neu (vxor clear src); then dtor+delete_ if begin!=0
    if (begin != nullptr) {
        if (begin != end) {
            auto* d = dst;
            auto* s = begin;
            while (s != end) {
                std::memcpy(d, s, kCtor98a90ElemBytes);
                std::memset(s, 0, kCtor98a90ElemBytes);
                d += kCtor98a90ElemBytes;
                s += kCtor98a90ElemBytes;
            }
            s = begin;
            while (s != end) {
                if (hooks.elem_dtor_f590 != nullptr) {
                    hooks.elem_dtor_f590(s);
                }
                s += kCtor98a90ElemBytes;
            }
        }
        alloc.delete_(begin);
    }

    store_ptr(base, kCtor98a90VecBegin, neu);
    store_ptr(base, kCtor98a90VecEnd,
              static_cast<std::uint8_t*>(neu) + (count * kCtor98a90ElemBytes));
    store_ptr(base, kCtor98a90VecCap, static_cast<std::uint8_t*>(neu) + kCtor98a90BufBytes);
    out_buf = neu;
    return true;
}

}  // namespace

Ctor98a90_3be050Result init_98a90_3be050(AllocatorFront& alloc, void* obj,
                                         const Ctor98a90_3be050Hooks& hooks) {
    Ctor98a90_3be050Result out{};
    if (obj == nullptr) {
        return out;
    }
    auto* base = static_cast<std::uint8_t*>(obj);

    // OBS 0x3be064..0x3be06d
    zero_xmm(base + 8);

    // OBS 0x3be068..0x3be093: nested 0x48 sentinel
    void* nested = alloc.new_(kCtor98a90NestedBytes);
    if (nested == nullptr) {
        return out;
    }
    std::memset(nested, 0, kCtor98a90NestedBytes);
    auto* n = static_cast<std::uint8_t*>(nested);
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

    if (hooks.sub_3be930 != nullptr) {
        hooks.sub_3be930(base + 0x68);
    }
    if (hooks.store_tsc_323d0 != nullptr) {
        hooks.store_tsc_323d0(base + kCtor98a90TscOffset);
    }

    // OBS 0x3be0c7..0x3be0f3
    std::uint64_t end28 = 0;
    std::uint64_t begin28 = 0;
    std::memcpy(&end28, base + 0x38, 8);
    std::memcpy(&begin28, base + 0x28, 8);
    if (((end28 - begin28) >> 3) <= 0xff) {
        out.ensure_called = true;
        if (hooks.ensure_3c2f70 != nullptr) {
            hooks.ensure_3c2f70(base + 0x20, 0x100);
        }
    }

    // OBS 0x3be0f8..0x3be1cb: buffer grow (measured empty path always takes this)
    void* buf = nullptr;
    if (!grow_vec_buf_80(alloc, base, hooks, buf)) {
        return out;
    }
    if (buf != nullptr) {
        out.buf_grown = true;
        out.buf_80 = buf;
    }

    // OBS 0x3be1ce..0x3be2c2: global list walk — NOT modelled
    if (hooks.list_walk_from_3be1ce != nullptr) {
        hooks.list_walk_from_3be1ce(obj);
    }

    out.ok = (out.nested_48 != nullptr);
    return out;
}

}  // namespace sotc
