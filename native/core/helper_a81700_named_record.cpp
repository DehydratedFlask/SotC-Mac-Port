#include "helper_a81700_named_record.hpp"

#include <atomic>
#include <cstring>

namespace sotc {
namespace {

void inc_ctl(std::int64_t* ctl) noexcept {
    // `mov rax,[rsi+0x18] ; test rax,rax ; je skip ; lock inc qword [rax]`
    if (ctl != nullptr) (void)std::atomic_ref<std::int64_t>(*ctl).fetch_add(1, std::memory_order_seq_cst);
}

}  // namespace

void construct_record_registry_a81b50(RecordRegistry32fd398& registry) noexcept {
    registry.first.begin = nullptr;       // vmovups [rdi+8], xmm0 (16 zero bytes: begin, end)
    registry.first.end = nullptr;
    registry.first.cap = nullptr;         // mov qword [rdi+0x18], 0
    registry.d50 = 0;                     // mov dword [rdi+0x50], 0
    registry.second.begin = nullptr;      // vmovups [rdi+0x28], ymm0 (32 zero bytes: begin, end, cap, q40)
    registry.second.end = nullptr;
    registry.second.cap = nullptr;
    registry.q40 = 0;
}

VecReserveResult vec_reserve_a82050(RecordPtrVec& vec, std::size_t n, const AllocDispatchContract& alloc) noexcept {
    VecReserveResult out{};
    const std::size_t room = static_cast<std::size_t>(vec.cap - vec.end);            // (cap - end) >> 3 ; cmp rsi ; jae ret
    if (room >= n) {
        out.ok = true;
        return out;
    }
    const std::size_t size = static_cast<std::size_t>(vec.end - vec.begin);
    if (kVecMaxElements - size < n) {                                                  // jb 0xa82134 -> 0xa82150 (throws)
        out.refusal = RecordRegistryRefusal::length_error;
        return out;
    }
    const std::size_t want = size + n;
    const std::size_t capacity = static_cast<std::size_t>(vec.cap - vec.begin);
    std::size_t grown = capacity >> 1;                                                 // shr rbx
    const std::size_t headroom = kVecMaxElements - grown;
    grown += capacity;                                                                 // add rbx, rcx
    if (headroom < capacity) grown = 0;                                                // cmovb rbx, r12 (= 0)
    if (grown < want) grown = want;                                                    // cmovb rbx, rdx
    if (grown > kVecMaxElements) {                                                     // ja 0xa8213e -> 0xf8f5f0
        out.refusal = RecordRegistryRefusal::bad_alloc;
        return out;
    }
    // Everything that can be refused is decided before the first write, so a refusal leaves the vector untouched.
    NamedRecord** buffer = nullptr;
    if (grown != 0) {                                                                  // test rbx ; je skip
        if (alloc.allocate == nullptr) {
            out.refusal = RecordRegistryRefusal::no_allocator;
            return out;
        }
    }
    if (vec.begin != nullptr && alloc.deallocate == nullptr) {
        out.refusal = RecordRegistryRefusal::no_allocator;
        return out;
    }
    if (grown != 0) {
        const AllocWrapperResult block = alloc_17070(grown * sizeof(void*), alloc);   // call 0x17070(new * 8)
        if (!block.ok) {
            out.refusal = RecordRegistryRefusal::allocation_failed;
            return out;
        }
        buffer = static_cast<NamedRecord**>(block.pointer);
    }
    if (size != 0) std::memmove(static_cast<void*>(buffer), static_cast<const void*>(vec.begin), size * sizeof(void*));   // call memmove
    bool free_ok = true;
    if (vec.begin != nullptr) free_ok = free_170b0(vec.begin, alloc).ok;               // test r15 ; je ; call 0x170b0
    vec.begin = buffer;                                                                // mov [r14+8], r12
    vec.end = buffer + size;                                                           // mov [r14+0x10], r13*8 + r12
    vec.cap = buffer + grown;                                                          // mov [r14+0x18], r12 + rbx*8
    out.ok = free_ok;
    out.refusal = free_ok ? RecordRegistryRefusal::none : RecordRegistryRefusal::free_failed;
    out.grew = true;
    out.new_capacity = grown;
    return out;
}

namespace {

// One vector's half of 0xa81840: `cmp end, cap ; je reserve(1)` then `*end = record ; end += 8`.
VecReserveResult push_back_a81840(RecordPtrVec& vec, NamedRecord* record, const AllocDispatchContract& alloc) noexcept {
    VecReserveResult reserved{};
    reserved.ok = true;
    if (vec.end == vec.cap) {
        reserved = vec_reserve_a82050(vec, 1, alloc);                                  // mov esi,1 ; call 0xa82050
        if (reserved.refusal != RecordRegistryRefusal::none && reserved.refusal != RecordRegistryRefusal::free_failed) {
            return reserved;
        }
    }
    *vec.end = record;                                                                 // mov [rax], rcx
    ++vec.end;                                                                         // add qword [rbx+0x10], 8
    return reserved;
}

}  // namespace

NamedRecordRegistration register_named_record_a81700(RecordRegistryCell& cell, NamedRecord& record, const SharedStr& name,
                                                     std::uint32_t kind, std::uint8_t flag,
                                                     const AllocDispatchContract& alloc) noexcept {
    NamedRecordRegistration out{};
    record.name.data = name.data;          // vmovups xmm0,[rsi] ; vmovups [rbx],xmm0
    record.name.size = name.size;
    record.name.aux = name.aux;            // mov rax,[rsi+0x10] ; mov [rbx+0x10],rax
    record.name.ctl = name.ctl;            // mov rax,[rsi+0x18] ; mov [rbx+0x18],rax
    inc_ctl(record.name.ctl);              // test rax,rax ; je ; lock inc qword [rax]
    record.kind = kind;                    // mov [rbx+0x20], edx
    record.q28 = 0;                        // mov qword [rbx+0x28], 0
    record.b30 = 0;                        // mov byte [rbx+0x30], 0
    record.flag = flag;                    // mov [rbx+0x31], cl
    if (cell.guard == 0) {                 // cmp byte [0x33053f0],0 ; jne ; __cxa_guard_acquire ; test eax ; je
        construct_record_registry_a81b50(cell.registry);   // the inline zeroing (same stores as 0xa81b50)
        out.registry_initialised_now = true;               // __cxa_atexit(0xa81b80, &registry, DSO) is pending
        cell.guard = 1;                                    // __cxa_guard_release
    }
    const VecReserveResult first = push_back_a81840(cell.registry.first, &record, alloc);
    if (first.grew) ++out.grows;
    if (first.refusal != RecordRegistryRefusal::none && first.refusal != RecordRegistryRefusal::free_failed) {
        out.refusal = first.refusal;
        return out;
    }
    out.pushed_first = true;
    const VecReserveResult second = push_back_a81840(cell.registry.second, &record, alloc);
    if (second.grew) ++out.grows;
    if (second.refusal != RecordRegistryRefusal::none && second.refusal != RecordRegistryRefusal::free_failed) {
        out.refusal = second.refusal;
        return out;
    }
    out.pushed_second = true;
    if (first.refusal == RecordRegistryRefusal::free_failed) out.refusal = RecordRegistryRefusal::free_failed;
    else if (second.refusal == RecordRegistryRefusal::free_failed) out.refusal = RecordRegistryRefusal::free_failed;
    out.ok = out.refusal == RecordRegistryRefusal::none;
    return out;
}

RecordRegistryCell& record_registry_cell_32fd398() noexcept {
    static RecordRegistryCell cell{};      // BSS: zero, guard clear
    return cell;
}

}  // namespace sotc
