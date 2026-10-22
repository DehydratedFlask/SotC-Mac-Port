#include "hashed_string_14c3a0.hpp"

#include <atomic>

#include "fnv_span_161f80.hpp"

namespace sotc {
namespace {

void inc_ctl(std::int64_t* ctl) noexcept {
    // `test rax,rax ; je skip ; lock inc qword [rax]`
    if (ctl != nullptr) (void)std::atomic_ref<std::int64_t>(*ctl).fetch_add(1, std::memory_order_seq_cst);
}

struct EmptyStorage {
    SharedStr str;                   // original 0x2dd9f50
    std::uint64_t guard_slot;        // original 0x2dd9f70 (guard byte); unused here, the compiler owns the guard
    std::uint8_t nul[8];             // original 0x2dd9f78 (target of ptr)
};
static_assert(offsetof(EmptyStorage, nul) == kEmptyStringBufferOffsetF730);

}  // namespace

void copy_hashed_14c3a0(HashedStr& dst, const HashedStr& src) noexcept {
    dst.hash = src.hash;                  // mov eax,[rsi] ; mov [rdi],eax
    dst.str.data = src.str.data;          // vmovups xmm0,[rsi+8] ; vmovups [rdi+8],xmm0
    dst.str.size = src.str.size;
    dst.str.aux = src.str.aux;            // mov rax,[rsi+0x18] ; mov [rdi+0x18],rax
    dst.str.ctl = src.str.ctl;            // mov rax,[rsi+0x20] ; mov [rdi+0x20],rax
    inc_ctl(dst.str.ctl);
}

void make_hashed_14c4a0(HashedStr& dst, const SharedStr& src) noexcept {
    dst.str.data = src.data;              // vmovups xmm0,[rdx] ; vmovups [rbx+8],xmm0
    dst.str.size = src.size;
    dst.str.aux = src.aux;                // mov rax,[rdx+0x10] ; mov [rbx+0x18],rax
    dst.str.ctl = src.ctl;                // mov rax,[rdx+0x18] ; mov [rbx+0x20],rax
    inc_ctl(dst.str.ctl);
    const ByteSpan span{dst.str.data, dst.str.size};   // rdi = this+8 : the {ptr,len} pair
    dst.hash = fnv_span_161f80(span, kFnvOffsetBasis161f80);   // mov esi,0x811c9dc5 ; call 0x161f80 ; mov [rbx],eax
}

const SharedStr& empty_string_f730() noexcept {
    static EmptyStorage storage{};        // zero-initialised like BSS: len = aux = ctl = 0, nul byte = 0
    static const bool once = (storage.str.data = storage.nul, true);   // guard ; ptr store (len/aux/ctl already 0)
    (void)once;
    return storage.str;
}

}  // namespace sotc
