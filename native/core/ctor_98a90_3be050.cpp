#include "ctor_98a90_3be050.hpp"

#include <cstring>

namespace sotc {
namespace {

void* g_self_cell_2ef63f8 = nullptr;
void* g_list_head_cell_2ef6400 = nullptr;

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

// OBS 0x3be1ce..0x3be1e5: store obj → 0x2ef63f8; head=*0x2ef6400; je skip if null.
void list_walk_head(void* obj, const Ctor98a90_3be050Hooks& hooks,
                    Ctor98a90_3be050Result& out) noexcept {
    g_self_cell_2ef63f8 = obj;
    out.list_self_stored = true;
    void* head = g_list_head_cell_2ef6400;
    out.list_head_seen = head;
    if (head == nullptr) {
        out.list_walk_skipped_null = true;
        return;
    }
    if (hooks.list_walk_nonnull_body != nullptr) {
        hooks.list_walk_nonnull_body(obj, head);
    }
}

}  // namespace

void*& ctor_98a90_self_cell_2ef63f8() noexcept { return g_self_cell_2ef63f8; }
void*& ctor_98a90_list_head_cell_2ef6400() noexcept { return g_list_head_cell_2ef6400; }

Ctor98a90_3be050Result init_98a90_3be050(AllocatorFront& alloc, void* obj,
                                         const Ctor98a90_3be050Hooks& hooks) {
    Ctor98a90_3be050Result out{};
    if (obj == nullptr) {
        return out;
    }
    auto* base = static_cast<std::uint8_t*>(obj);

    zero_xmm(base + 8);

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

    base[0x18] = 0;
    zero_xmm(base + 0x28);
    std::memset(base + 0x38, 0, 8);
    zero_xmm(base + 0x55);
    zero_xmm(base + 0x48);

    if (hooks.sub_3be930 != nullptr) {
        hooks.sub_3be930(base + 0x68);
    }
    // OBS 0x3be0bb..0x3be0c2: lea rdi,[obj+0x98a88]; call 0x323d0 (lifted; overrideable)
    if (hooks.store_tsc_323d0 != nullptr) {
        hooks.store_tsc_323d0(base + kCtor98a90TscOffset);
    } else {
        store_tsc_323d0(base + kCtor98a90TscOffset, hooks.tsc_detail);
    }
    std::memcpy(&out.tsc_value, base + kCtor98a90TscOffset, 8);
    out.tsc_stored = (out.tsc_value != 0);

    // OBS 0x3be0c7..0x3be0f3: capacity = ([+0x38]-[+0x28])/0xa8; ensure if <=0xff
    std::uint64_t cap_ptr = 0;
    std::uint64_t begin_ptr = 0;
    std::memcpy(&cap_ptr, base + 0x38, 8);
    std::memcpy(&begin_ptr, base + 0x28, 8);
    if (ensure_3c2f70_elem_count(begin_ptr, cap_ptr) <= 0xff) {
        out.ensure_called = true;
        if (hooks.ensure_3c2f70 != nullptr) {
            hooks.ensure_3c2f70(base + kCtor98a90EnsureVec, 0x100);
        } else {
            EnsureCapacity3c2f70Hooks eh = hooks.ensure_detail;
            if (eh.elem_dtor_f590 == nullptr) {
                eh.elem_dtor_f590 = hooks.elem_dtor_f590;
            }
            const auto er =
                ensure_capacity_3c2f70(alloc, base + kCtor98a90EnsureVec, 0x100, eh);
            if (!er.ok) {
                return out;
            }
            out.ensure_allocated = er.allocated;
            out.ensure_buf = er.buf;
        }
    }

    void* buf = nullptr;
    if (!grow_vec_buf_80(alloc, base, hooks, buf)) {
        return out;
    }
    if (buf != nullptr) {
        out.buf_grown = true;
        out.buf_80 = buf;
    }

    // OBS 0x3be1ce..: measured null-head early-out; non-null body hooked
    list_walk_head(obj, hooks, out);

    out.ok = (out.nested_48 != nullptr) && out.list_self_stored && out.tsc_stored;
    return out;
}

}  // namespace sotc
