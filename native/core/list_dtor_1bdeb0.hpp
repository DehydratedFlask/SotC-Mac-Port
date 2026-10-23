// Native lift of the list destructor RVA `0x1bdeb0` (E555 decoded it, E558 resolved its node type).
//
// ELF sha256 d0723afb…861d3. Body `0x1bdeb0..0x1bdf28` = 120 B, sha256 7d64b9d3…ba35, `ret` at
// 0x1bdf27. Its only caller is `0x25dc76`, which passes `*(0x2ee68c8)` — the same singleton `0x1290`
// uses as the A-insert owner.
//
// Node type (E558): the list holds **0x58-byte nodes** built by `0x1bdb50`, not the 0x60-byte
// callable nodes `0x1beeb0` inserts into `container+0x40`:
//   +0x00 prev        +0x08 next
//   +0x10 callable A  (0x20 B, control-block pointer at +0x18)
//   +0x30 callable B  (0x20 B, control-block pointer at +0x18)
//   +0x50 dword flag  (+0x54 padding)
// The list head is the sentinel pair itself: `prev` at +0x00, `next` at +0x08, count at +0x10.
//
// Recovered loop (walk backwards from sentinel.prev):
//   node = [head]                              ; sentinel.prev
//   while node != head:
//     if dword [node+0x50] != 0: node = node->prev; continue      ; skip flagged nodes
//     node = node->prev                        ; advance first
//     victim = node->next                      ; == the node we just left
//     [victim->prev + 0x08] = victim->next     ; unlink
//     [victim->next]        = victim->prev
//     call 0xf590(victim+0x30)                 ; shared-ownership release (E552)
//     call 0xf590(victim+0x10)                 ; shared-ownership release
//     call 0x170c0(victim)                     ; tagged free, tag 2 (E553)
//     dec dword [head+0x10]
//
// What the list holds (E561): an application info registry of named entries ("Game" ->
// "Shadow of the Colossus", "Build Version", "Platform Type" -> "ps4 - NEO"/"ps4 - STA",
// "Is Debug Memory Available" -> "True"/"False"), all inserted with flag = 1 by `0x1bdb50`. The
// purge here therefore targets flag == 0 entries, a state no analysed path produces yet.
//
// NOT modelled: the bodies of the two releases and of the tag-2 free (caller-supplied hooks), the
// singleton at `0x2ee68c8` (still created elsewhere), and whatever the flagged nodes mean (they are
// deliberately skipped by the original). G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

#include "alloc_wrappers_17070.hpp"
#include "shared_release_f590.hpp"

namespace sotc {

inline constexpr std::uint64_t kListDtor1bdeb0Rva = 0x1BDEB0;
inline constexpr std::size_t kListDtor1bdeb0Size = 120;
inline constexpr std::uint64_t kListDtor1bdeb0EndRva = 0x1BDF28;
inline constexpr std::uint64_t kListDtor1bdeb0ReleaseRva = 0xF590;
inline constexpr std::uint64_t kListDtor1bdeb0FreeRva = 0x170C0;
inline constexpr std::size_t kListDtor1bdeb0NodeSize = 0x58;
inline constexpr std::size_t kListDtor1bdeb0FlagOffset = 0x50;
inline constexpr std::size_t kListDtor1bdeb0CountOffset = 0x10;

// 0x20 B type-erased callable storage; the control-block pointer sits at +0x18, which is exactly
// what `shared_release_f590` reads from its holder.
struct Callable1bdeb0 {
    std::uintptr_t value_00;
    std::uintptr_t value_08;
    std::uintptr_t value_10;
    std::atomic<std::uint64_t>* control_18;
};
static_assert(sizeof(Callable1bdeb0) == 0x20);

struct Node1bdeb0 {
    Node1bdeb0* prev;             // +0x00
    Node1bdeb0* next;             // +0x08
    Callable1bdeb0 callable_10;   // +0x10
    Callable1bdeb0 callable_30;   // +0x30
    std::uint32_t flag_50;        // +0x50  non-zero => skipped by the original
    std::uint32_t pad_54;
};
static_assert(sizeof(Node1bdeb0) == kListDtor1bdeb0NodeSize);
static_assert(offsetof(Node1bdeb0, flag_50) == kListDtor1bdeb0FlagOffset);

struct List1bdeb0 {
    Node1bdeb0* prev;          // +0x00 (sentinel.prev)
    Node1bdeb0* next;          // +0x08 (sentinel.next)
    std::uint32_t count;       // +0x10 (decremented per erased node)
    std::uint32_t pad_14;
};
static_assert(offsetof(List1bdeb0, count) == kListDtor1bdeb0CountOffset);

struct ListDtor1bdeb0Hooks {
    // 0xf590(victim+0x30) then 0xf590(victim+0x10).
    SharedReleaseF590Hooks release{};
    // 0x170c0(victim) — tag-2 deallocate. Null => the erase is reported as failed.
    void (*free_tag2)(void* node) = nullptr;
};

struct ListDtor1bdeb0Result {
    bool ok = false;
    int erased = 0;
    int skipped = 0;
    int releases = 0;      // two per erased node
    int frees = 0;
    std::uint32_t count_after = 0;
};

// Runs the recovered teardown on `list`. `list` is the sentinel pair itself (the caller passes the
// sentinel, matching `rdi = *(0x2ee68c8)`). A missing free hook stops the walk and reports
// ok == false after the nodes already erased; the nodes released before the failure are reported.
[[nodiscard]] ListDtor1bdeb0Result list_dtor_1bdeb0(List1bdeb0& list,
                                                   const ListDtor1bdeb0Hooks& hooks) noexcept;

}  // namespace sotc
