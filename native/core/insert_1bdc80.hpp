// Native lift of the list-B inserter at RVA `0x1bdc80` (the function immediately after 0x1bdb50).
//
// Body `0x1bdc80..0x1bde20` (ret), 0x1A1 B; canary tail at 0x1bde21 -> 0xf8f3a0.
// Signature recovered from the register use:
//   rdi = container base (list B sentinel is at +0x20)
//   rsi = callable A (0x20 B holder)
//   rdx = callable B (0x20 B holder)
//
// Byte-verified sequence:
//   temp_a = *A ; acquire control                                  ; 0x1bdcab..0x1bdcce
//   temp_b = *B ; acquire control                                  ; 0x1bdcd0..0x1bdcf2
//   temp_a2 = temp_a ; acquire control                             ; 0x1bdcf5..0x1bdd1b
//   temp_b2 = temp_b ; acquire control                             ; 0x1bdd1e..0x1bdd60
//   node = 0x17090(0x50)                                           ; 0x1bdd90..0x1bdd9f
//   node[+0x10] = temp_a2 ; node[+0x30] = temp_b2                  ; 0x1bddb2 / 0x1bddbb
//   node->prev = &container+0x20 ; node->next = old head           ; 0x1bddc0 / 0x1bddce
//   old_head->prev = node ; container[+0x28] = node                ; 0x1bddd7 / 0x1bddda
//   incq [container+0x30]                                          ; 0x1bdddf
//   zero temp_a2/temp_b2 ; 0xf590 each (no-ops)                    ; 0x1bddb7..0x1bddee
//   zero temp_b/temp_a ; 0xf590 each (transfer the refs to node)   ; 0x1bddf3..0x1bde02
//
// The two middle releases are no-ops by construction (the holders are zeroed first), exactly like
// 0x1bdb50. The last two release the caller's temporaries, leaving one reference per holder owned
// by the node. Node layout: prev +0x00, next +0x08, callable A +0x10, callable B +0x30; there is no
// flag word (size 0x50).
//
// NOT modelled: the 0x17090/0x12820 and 0xf590 bodies (hooks), the stack canary, and list B's
// reader/clear (E569). G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

#include "alloc_wrappers_17070.hpp"
#include "insert_1beeb0.hpp"
#include "list_dtor_1bdeb0.hpp"
#include "shared_release_f590.hpp"

namespace sotc {

inline constexpr std::uint64_t kInsert1bdc80Rva = 0x1BDC80;
inline constexpr std::size_t kInsert1bdc80Size = 0x1A1;
inline constexpr std::uint64_t kInsert1bdc80EndRva = 0x1BDE21;
inline constexpr std::uint64_t kInsert1bdc80AllocRva = 0x17090;
inline constexpr std::uint64_t kInsert1bdc80ReleaseRva = 0xF590;
inline constexpr std::size_t kInsert1bdc80NodeSize = 0x50;
inline constexpr std::size_t kInsert1bdc80ListOffset = 0x20;
inline constexpr std::size_t kInsert1bdc80CountOffset = 0x30;

struct Node1bdc80 {
    Node1bdc80* prev;             // +0x00
    Node1bdc80* next;             // +0x08
    Callable1bdeb0 callable_10;   // +0x10
    Callable1bdeb0 callable_30;   // +0x30
};
static_assert(sizeof(Node1bdc80) == kInsert1bdc80NodeSize);
static_assert(offsetof(Node1bdc80, callable_10) == 0x10);
static_assert(offsetof(Node1bdc80, callable_30) == 0x30);

struct List1bdc80 {
    Node1bdc80* prev;      // +0x00 (container +0x20)
    Node1bdc80* next;      // +0x08 (container +0x28)
    std::uint64_t count;   // +0x10 (container +0x30)
    std::uint64_t pad_18;  // container +0x38
};
static_assert(sizeof(List1bdc80) == 0x20);
static_assert(offsetof(List1bdc80, count) == 0x10);

struct Insert1bdc80Hooks {
    AllocDispatchContract alloc{};
    SharedReleaseF590Hooks release{};
};

struct Insert1bdc80Result {
    bool ok = false;
    bool allocated = false;
    int acquires = 0;
    Node1bdc80* node = nullptr;
    std::uint64_t count_after = 0;
    bool refs_restored = false;
};

// Inserts a 0x50 B node at the front of list B (sentinel at container+0x20). `container` is the
// 0x60 B three-list object. Allocation failure restores both acquired references.
[[nodiscard]] Insert1bdc80Result insert_1bdc80(Container1beeb0& container,
                                               const Callable1bdeb0& a,
                                               const Callable1bdeb0& b,
                                               const Insert1bdc80Hooks& hooks) noexcept;

}  // namespace sotc
