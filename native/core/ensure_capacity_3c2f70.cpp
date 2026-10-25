#include "ensure_capacity_3c2f70.hpp"

#include <cstring>

namespace sotc {
namespace {

[[nodiscard]] std::uint8_t* load_ptr(std::uint8_t* base, std::size_t off) noexcept {
    void* p = nullptr;
    std::memcpy(&p, base + off, sizeof(void*));
    return static_cast<std::uint8_t*>(p);
}

void store_ptr(std::uint8_t* base, std::size_t off, void* p) noexcept {
    std::memcpy(base + off, &p, sizeof(void*));
}

}  // namespace

EnsureCapacity3c2f70Result ensure_capacity_3c2f70(AllocatorFront& alloc, void* vec,
                                                 std::uint64_t n,
                                                 const EnsureCapacity3c2f70Hooks& hooks) noexcept {
    EnsureCapacity3c2f70Result out{};
    if (vec == nullptr) {
        return out;
    }
    auto* base = static_cast<std::uint8_t*>(vec);

    // OBS: test n; je skip_alloc (rax=0). ja abort if n > max.
    void* neu = nullptr;
    if (n != 0) {
        if (n > kEnsure3c2f70MaxElems) {
            return out;  // OBS abort path NOT run
        }
        const std::size_t bytes = static_cast<std::size_t>(n * kEnsure3c2f70ElemBytes);
        neu = alloc.new_(bytes);
        if (neu == nullptr) {
            return out;
        }
        std::memset(neu, 0, bytes);
        out.allocated = true;
        out.buf = neu;
    }

    std::uint8_t* begin = load_ptr(base, kEnsure3c2f70OffBegin);
    std::uint8_t* end = load_ptr(base, kEnsure3c2f70OffEnd);

    std::uint64_t count = 0;
    if (begin != end) {
        // Migrate 0xa8-stride elems into neu (OBS copy +0 / call 0x1da30 / copy +0xa0).
        auto* d = static_cast<std::uint8_t*>(neu);
        auto* s = begin;
        while (s != end) {
            if (d != nullptr) {
                std::memcpy(d, s, 8);
                if (hooks.elem_move_1da30 != nullptr) {
                    hooks.elem_move_1da30(d + 8, s + 8);
                }
                std::memcpy(d + 0xa0, s + 0xa0, 8);
            }
            d += kEnsure3c2f70ElemBytes;
            s += kEnsure3c2f70ElemBytes;
            ++count;
        }
        // Destroy old range then delete_
        s = begin;
        while (s != end) {
            void* inner = nullptr;
            std::memcpy(&inner, s + 0x38, sizeof(void*));
            void* inner_end = nullptr;
            std::memcpy(&inner_end, s + 0x58, sizeof(void*));
            if (inner != nullptr && inner != inner_end) {
                if (hooks.elem_inner_free_170c0 != nullptr) {
                    hooks.elem_inner_free_170c0(inner);
                }
            }
            if (hooks.elem_dtor_f590 != nullptr) {
                hooks.elem_dtor_f590(s + 0x10);  // OBS lea rdi,[r13+0x10]; call 0xf590
            }
            s += kEnsure3c2f70ElemBytes;
        }
        if (begin != nullptr) {
            alloc.delete_(begin);
        }
    } else if (begin != nullptr) {
        // begin==end non-null: still delete_ old (OBS)
        alloc.delete_(begin);
        count = 0;
    }

    // OBS install: cap=new+n*0xa8, end=new+count*0xa8, begin=new
    auto* nb = static_cast<std::uint8_t*>(neu);
    store_ptr(base, kEnsure3c2f70OffBegin, neu);
    store_ptr(base, kEnsure3c2f70OffEnd, nb + count * kEnsure3c2f70ElemBytes);
    store_ptr(base, kEnsure3c2f70OffCap, nb + n * kEnsure3c2f70ElemBytes);
    out.ok = true;
    return out;
}

}  // namespace sotc
