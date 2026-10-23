// Native lift of original function RVA `0x1beeb0` — the list-3 insert body reached by the
// 18 B tail-call remarshal `0x1bde30` (E522). This is the "descriptor copy" that makes the
// insert in `0x1290` an executed effect rather than an observed hook.
//
// ELF sha256 d0723afb…861d3. Body `0x1beeb0..0x1befcc` = 284 B (0x11c), fn sha256
// 9a05e72e…97af. Byte-verified (replayed against `local/re/eboot.elf`, not inferred):
//
//   entry contract (from 0x1bde30):  rdi = &container[0x40] (list-3 sentinel),
//                                    rsi = same sentinel, rdx = &descriptor
//   prologue: r12 = &descriptor, r15 = sentinel, [rbp-0x90] = sentinel, stack canary
//             read from guard cell 0x15fe518 (two-level: [0x15fe518] then [rax]).
//   1. edi = 0x60; call operator new (0x17090) -> rbx = node
//   2. node[0x50] = vtable 0x1598ce0
//   3. node[0x40] = 0
//   4. node[0x58] = 0            (overwritten by step 8; net effect is step 8 only)
//   5. vmovups ymm0,[descriptor] ; vmovups [node+0x10],ymm0  -> 0x20 B bulk copy
//   6. vextractf128/vpextrq extract `fn` (descriptor[0x18]); if fn != 0:
//        node[0x10] = &node[0x10]  (self-link)
//   7. (unmodelled vcalls) descriptor[0x20]/[0x40] drive copy/move/destroy through
//        vtable 0x1594060 / node vtable 0x1598ce0 — see NOT modelled below.
//   8. node[0x58] = descriptor[0x48]   ("swap into node[0x58]": the original also reads the
//        old node[0x58] into a dead stack slot; no observable effect beyond the store)
//   9. list-3 push-front after the sentinel:
//        node[0x00] = sentinel; node[0x08] = sentinel->next; old_head->prev = node;
//        sentinel->next = node
//  10. incq [sentinel+0x10]  ==  ++container[0x50]   (list-3 count)
//   epilogue: canary compare, jne 0x1befcc -> __stack_chk_fail (0xf8f3a0). Return value is
//        the canary in rax (an artifact, NOT a result) — the original returns void.
//
// CORRECTION to the task brief (E522 is right here): node[0x58] receives the descriptor's
// `+0x48` field, NOT its `+0x40` field. `descriptor[0x40]` is the *vtable pointer* used by
// the unmodelled vcalls (call [0x40+0x08] at 0x1bef44); it is never stored into the node.
//
// NOT modelled: the copy/move/destroy vcall sequence (descriptor[0x20] context copied into
// node[0x30] via table 0x1594060 / node vtable 0x1598ce0 slots 0x19a0/0x19c0/0x19e0/0x19f0);
// the stack-canary fail path (read-only guard, can only abort); the 0x1bdb10 ctor and
// 0x1bdae0 dtor / 0x1bdeb0 cleanup that own the container; node[0x30..0x40) and node[0x48]
// (never written by this function). G1 PARTIAL — fixture-level model only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInsert1beeb0Rva = 0x1beeb0;
inline constexpr std::size_t kInsert1beeb0Size = 284;
inline constexpr std::uint64_t kInsert1beeb0EndRva = 0x1befcc;
inline constexpr std::uint64_t kInsert1beeb0NodeVtable = 0x1598ce0;       // node[0x50]
inline constexpr std::uint64_t kInsert1beeb0DescriptorVtable = 0x1594060; // descriptor[0x40]
inline constexpr std::uint64_t kInsert1beeb0OperatorNew = 0x17090;        // alloc thunk
inline constexpr std::size_t kInsert1beeb0NodeSize = 0x60;
inline constexpr std::size_t kInsert1beeb0CopySize = 0x20;    // vmovups pair width
inline constexpr std::size_t kInsert1beeb0CopyOffset = 0x10;  // copy destination node+0x10
inline constexpr std::size_t kInsert1beeb0List3Offset = 0x40; // container list-3 sentinel
inline constexpr std::size_t kInsert1beeb0Count3Offset = 0x50; // container count for list-3

// 0x60 B node. Only the fields written by this function are meaningful here; the others are
// left as allocator garbage in the original (or filled by the unmodelled vcall copy sequence).
struct Node1beeb0 {
    Node1beeb0* prev;         // 0x00  list-3 prev link (sentinel)
    Node1beeb0* next;         // 0x08  list-3 next link (old head)
    std::uintptr_t obj;       // 0x10  descriptor[0x00]; self-linked to &obj iff fn != 0
    std::uintptr_t invoke;    // 0x18  descriptor[0x08]
    std::uintptr_t offset;    // 0x20  descriptor[0x10] (init_1290 stores 0 here)
    std::uintptr_t fn;        // 0x28  descriptor[0x18]
    std::uintptr_t ctx_30;    // 0x30  NOT written by insert (vcall copy dst)
    std::uintptr_t ctx_38;    // 0x38  NOT written by insert
    std::uintptr_t zero_40;   // 0x40  = 0
    std::uintptr_t uninit_48; // 0x48  NOT written by insert
    std::uintptr_t vtable;    // 0x50  = 0x1598ce0
    std::uintptr_t slot_58;   // 0x58  = descriptor[0x48]
};
static_assert(sizeof(Node1beeb0) == kInsert1beeb0NodeSize);
static_assert(offsetof(Node1beeb0, obj) == 0x10);
static_assert(offsetof(Node1beeb0, fn) == 0x28);
static_assert(offsetof(Node1beeb0, zero_40) == 0x40);
static_assert(offsetof(Node1beeb0, vtable) == 0x50);
static_assert(offsetof(Node1beeb0, slot_58) == 0x58);

// 0x50 B descriptor. The insert reads [0x00..0x20] as the copied callable, [0x18] as `fn`,
// [0x40] as the vtable for the vcalls, and [0x48] as the value moved to node[0x58].
struct Descriptor1beeb0 {
    std::uintptr_t ctx;        // 0x00
    std::uintptr_t invoke;     // 0x08
    std::uintptr_t offset;     // 0x10
    std::uintptr_t fn;         // 0x18
    std::uintptr_t context[4]; // 0x20..0x40  source for the unmodelled vcall copy
    std::uintptr_t vtable;     // 0x40  0x1594060 (vcall table, NOT stored into node)
    std::uintptr_t slot_48;    // 0x48  moved to node[0x58]
};
static_assert(sizeof(Descriptor1beeb0) == 0x50);
static_assert(offsetof(Descriptor1beeb0, fn) == 0x18);
static_assert(offsetof(Descriptor1beeb0, vtable) == 0x40);
static_assert(offsetof(Descriptor1beeb0, slot_48) == 0x48);

// 0x60 B three-list container (ctor 0x1bdb10, E522). Only list-3 (+0x40 sentinel, +0x50 count)
// is touched by this insert; the other two lists exist to match the object size/offsets. The
// ctor 0x1bdb10 leaves the +0x18/+0x38/+0x58 qwords unwritten (padding/other-purpose).
struct Container1beeb0 {
    Node1beeb0* list1_prev;    // 0x00
    Node1beeb0* list1_next;    // 0x08
    std::uint64_t count1;      // 0x10
    std::uint64_t pad1;        // 0x18  (not written by ctor 0x1bdb10)
    Node1beeb0* list2_prev;    // 0x20
    Node1beeb0* list2_next;    // 0x28
    std::uint64_t count2;      // 0x30
    std::uint64_t pad2;        // 0x38  (not written by ctor 0x1bdb10)
    Node1beeb0* list3_prev;    // 0x40  sentinel prev
    Node1beeb0* list3_next;    // 0x48  sentinel next
    std::uint64_t count3;      // 0x50  list-3 count (incremented)
    std::uint64_t pad3;        // 0x58  (not written by ctor 0x1bdb10)
};
static_assert(sizeof(Container1beeb0) == kInsert1beeb0NodeSize);
static_assert(offsetof(Container1beeb0, list3_prev) == kInsert1beeb0List3Offset);
static_assert(offsetof(Container1beeb0, list3_next) == 0x48);
static_assert(offsetof(Container1beeb0, count3) == kInsert1beeb0Count3Offset);

// Explicit allocation seam for the 0x60 B node (the original's throwing operator new). The
// caller supplies both allocation and free; a test may back this with a fixed-capacity pool.
// `alloc` returning nullptr (or a null `alloc`) is a *visible* failure, never a fake success.
struct Insert1beeb0Alloc {
    void* (*alloc)(std::size_t bytes) = nullptr;  // operator new(0x60)
    void  (*dealloc)(void* ptr) = nullptr;        // not used by insert; for caller/test cleanup
};

struct Insert1beeb0Result {
    bool ok = false;              // false iff allocation failed (list and count untouched)
    Node1beeb0* node = nullptr;   // created node (null on failure)
    std::uintptr_t copied_obj = 0;    // descriptor[0x00] (raw copy, before self-link)
    std::uintptr_t copied_invoke = 0; // descriptor[0x08]
    std::uintptr_t copied_offset = 0; // descriptor[0x10]
    std::uintptr_t copied_fn = 0;     // descriptor[0x18]
    bool self_linked = false;         // fn != 0
    Node1beeb0* prev = nullptr;       // node->prev (sentinel)
    Node1beeb0* next = nullptr;       // node->next (old head)
    std::uint64_t count = 0;          // container.count3 after the increment
};

// Runs the recovered insert. `container` is the 0x60 B three-list object; the list-3 sentinel
// is `&container + 0x40`. Allocation happens first and any failure leaves the container
// untouched. The original returns void; the result struct is native observability only.
[[nodiscard]] Insert1beeb0Result insert_1beeb0(Container1beeb0& container,
                                               const Descriptor1beeb0& descriptor,
                                               const Insert1beeb0Alloc& alloc) noexcept;

}  // namespace sotc
