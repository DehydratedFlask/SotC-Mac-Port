#include "registry_object_14c9a0.hpp"

#include <cstdio>
#include <cstdlib>

#include "atomic_add_ref_14ccb0.hpp"

namespace sotc {
namespace {

[[noreturn]] void unsupported_parent_14c9a0() noexcept {
    std::fputs("sotc: ctor_14c9a0 (RVA 0x14c9a0) with parent != nullptr is UNSUPPORTED: the original copies the "
               "parent's string through 0xf590 and tail-jumps via a manager vtable slot; that path is not lifted\n",
               stderr);
    std::abort();
}

}  // namespace

const HashedStr& static_empty_hashed_2ed3640() noexcept {
    static HashedStr value{};   // BSS: the four bytes at +4 stay zero
    static const bool once = (make_hashed_14c4a0(value, empty_string_f730()), true);   // guard ; 0xf730 ; 0x14c4a0
    (void)once;                                                                       // atexit(0x95610) omitted
    return value;
}

void ctor_14c9a0(Object14c9a0& self, Object14c9a0* parent) noexcept {
    if (parent != nullptr) unsupported_parent_14c9a0();   // before any write
    self.q00 = 0;
    self.self08 = &self;
    self.self10 = &self;
    self.q18 = 0;
    self.parent = parent;                                  // nullptr here
    (void)std::atomic_ref<std::int32_t>(self.a30).exchange(0, std::memory_order_seq_cst);   // xchg [rbx+0x30],eax
    (void)std::atomic_ref<std::int32_t>(self.a40).exchange(0, std::memory_order_seq_cst);   // xchg [rbx+0x40],ecx
    (void)std::atomic_ref<std::int64_t>(self.a50).exchange(0, std::memory_order_seq_cst);   // xchg [rbx+0x50],rax
    self.d60 = 0;
    self.q68 = 0;
    copy_hashed_14c3a0(self.s70, static_empty_hashed_2ed3640());
    self.d98 = kInitialD98;
    for (auto& b : self.zero_a8) b = 0;                    // vmovups [rbx+0xa8],ymm0 (zero)
    copy_hashed_14c3a0(self.sc8, static_empty_hashed_2ed3640());
    self.df0 = 0;
    self.df4 = kInitialD98;
    self.bf8 = 0;
    // parent == nullptr: `test r14,r14 ; je ret`.
}

Object14c9a0& static_object_2ee3b10() noexcept {
    static Object14c9a0 object{};   // zero-initialised (BSS), no guard
    return object;
}

std::atomic<Object14c9a0*>& registry_cell_2ee3b00() noexcept {
    static std::atomic<Object14c9a0*> cell{nullptr};
    return cell;
}

Object14c9a0* getter_14d880() noexcept {
    auto& cell = registry_cell_2ee3b00();
    if (Object14c9a0* existing = cell.load(std::memory_order_relaxed)) return existing;   // mov rax,[cell] ; test ; jne ret
    Object14c9a0& object = static_object_2ee3b10();
    static const bool once = (ctor_14c9a0(object, nullptr), true);   // guard ; 0x14c9a0(obj, 0) ; atexit(0x14cb70) OMITTED
    (void)once;
    cell.store(&object, std::memory_order_relaxed);                  // mov [cell],rax
    add_ref_14ccb0(static_cast<void*>(&object));                     // lock inc dword [obj+0x30]
    return &object;
}

void init_ctor_b25470(std::array<Slot118, kSlotCountB25470>& slots) noexcept {
    for (Slot118& s : slots) {
        Object14c9a0* p = getter_14d880();       // call 0x14d880
        s.node = p;                              // mov [rbx+r14-8],rax
        s.flag110 = 0;                           // mov byte [rbx+r14],0
        add_ref_14ccb0(static_cast<void*>(p));   // call 0x14ccb0 (rdi = rax)
        s.flag00 = 0;                            // mov byte [rbx+r14-0x110],0
    }
    // tail: __cxa_atexit(0xb24be0, 0, dso) OMITTED
}

}  // namespace sotc
