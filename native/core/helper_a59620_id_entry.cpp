#include "helper_a59620_id_entry.hpp"

#include <atomic>
#include <cstring>

#include "concmd_base.hpp"
#include "init_sub_3be930.hpp"

namespace sotc {

void copy_shared_str_inc(SharedStr& dst, const SharedStr& src) noexcept {
    dst.data = src.data;                       // vmovups xmm0,[src] ; vmovups [dst],xmm0
    dst.size = src.size;
    dst.aux = src.aux;                         // mov rax,[src+0x10] ; mov [dst+0x10],rax
    dst.ctl = src.ctl;                         // mov rax,[src+0x18] ; mov [dst+0x18],rax
    if (dst.ctl != nullptr) {                  // test rax,rax ; je ; lock inc qword [rax]
        (void)std::atomic_ref<std::int64_t>(*dst.ctl).fetch_add(1, std::memory_order_seq_cst);
    }
}

PtrVecPush push_ptr_doubling(PtrVec24& vec, void* element, const AllocDispatchContract& alloc) noexcept {
    PtrVecPush out{};
    if (vec.end < vec.cap) {                   // cmp rbx,[cap] ; jae grow
        *vec.end = element;                    // mov [rbx], r14
        ++vec.end;                             // lea rax,[rbx+8] ; mov [end],rax
        out.ok = true;
        return out;
    }
    const std::size_t bytes = static_cast<std::size_t>(reinterpret_cast<const char*>(vec.end) -
                                                       reinterpret_cast<const char*>(vec.begin));
    // r13 = 1 ; rax = bytes >> 2 (sar) ; cmovne r13, rax when end != begin
    std::size_t n = bytes != 0 ? static_cast<std::size_t>(static_cast<std::int64_t>(bytes) >> 2) : 1;
    // Every refusal is decided before the first write, so a refusal leaves the vector untouched.
    if (n != 0 && alloc.allocate == nullptr) {
        out.refusal = PtrVecRefusal::no_allocator;
        return out;
    }
    if (vec.begin != nullptr && alloc.deallocate == nullptr) {
        out.refusal = PtrVecRefusal::no_allocator;
        return out;
    }
    void** buffer = nullptr;                   // xor r12d,r12d ; test r13 ; je
    if (n != 0) {
        const AllocWrapperResult block = alloc_17090(n * sizeof(void*), alloc);   // lea rdi,[r13*8] ; call 0x17090
        if (!block.ok || block.pointer == nullptr) {
            out.refusal = PtrVecRefusal::allocation_failed;
            return out;
        }
        buffer = static_cast<void**>(block.pointer);
    }
    if (bytes != 0) std::memmove(static_cast<void*>(buffer), static_cast<const void*>(vec.begin), bytes);   // call memmove
    char* const raw = reinterpret_cast<char*>(buffer);
    *reinterpret_cast<void**>(raw + bytes) = element;                              // mov [r12+rbx], r14
    void** const new_end = reinterpret_cast<void**>(raw + bytes + sizeof(void*));  // lea rbx,[r12+rbx+8]
    bool free_ok = true;
    if (vec.begin != nullptr) free_ok = free_170c0(vec.begin, alloc).ok;           // test r15 ; je ; call 0x170c0
    vec.begin = buffer;                        // mov [V.begin], r12
    vec.end = new_end;                         // mov [V.end], rbx
    vec.cap = buffer + n;                      // lea rax,[r12+r13*8] ; mov [V.cap], rax
    out.ok = free_ok;
    out.refusal = free_ok ? PtrVecRefusal::none : PtrVecRefusal::free_failed;
    out.grew = true;
    out.new_capacity = n;
    return out;
}

IdEntryRegistration register_id_entry_a59620(PtrVec24& vec, IdEntry& entry, std::uint32_t id, const SharedStr& src,
                                             const AllocDispatchContract& alloc) noexcept {
    IdEntryRegistration out{};
    init_slot_f180(&entry.str);                // call 0xf180 (rdi = entry + 8)
    entry.id = id;                             // mov [r14], r12d
    out.release_inert = true;
    if (&entry.str == &src) {                  // cmp r15, rbx ; je skip
        out.aliased = true;
    } else {
        out.release_inert = release_inert_view_f590(entry.str);   // call 0xf590 on the string 0xf180 just emptied
        if (!out.release_inert) return out;                       // not reachable: ctl is zero after 0xf180
        copy_shared_str_inc(entry.str, src);
    }
    const PtrVecPush pushed = push_ptr_doubling(vec, &entry, alloc);
    out.refusal = pushed.refusal;
    out.grew = pushed.grew;
    out.new_capacity = pushed.new_capacity;
    out.pushed = pushed.ok || pushed.refusal == PtrVecRefusal::free_failed;
    out.ok = pushed.ok;
    return out;
}

}  // namespace sotc
