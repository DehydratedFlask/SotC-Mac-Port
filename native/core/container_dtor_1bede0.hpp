// Native lift of the 0x60 B three-list container destructor at RVA `0x1bede0`.
//
// ELF sha256 d0723afb…861d3. Body `0x1bede0..0x1beeb0` = 208 B, three sequential loops:
//   list C (+0x40, 0x60 B callable nodes, newest? no: walk from prev):
//     rax = node[0x50] (vtable 0x1598ce0) ; rdi = node+0x30 ; call [rax+0x18]
//     free node through 0x170c0 (tag 2)
//   list B (+0x20) and list A (+0x00), same shape:
//     rdi = node+0x30 ; call 0xf590
//     rdi = node+0x10 ; call 0xf590
//     free node through 0x170c0 (tag 2)
//
// The list-C vtable slot 3 was resolved from the image: `[0x1598cf8] = 0x19f0`, the bare `ret` of
// the four-entry helper table {0x19a0,0x19c0,0x19e0,0x19f0}. The list-C payload destroys nothing,
// which is why that loop has no 0xf590 calls. A native override hook is still exposed so a host can
// model a non-empty payload without this module pretending the original did more.
//
// Order: C, then B, then A. The original does not check any free result; a missing native free hook
// is a visible failure and stops before mutating the first list. A failed release (last reference
// with a missing destroy/free contract) is reported but the node is still freed, matching the
// original's unconditional free after the calls.
//
// NOT modelled: the allocator body (0x170c0/0x12af0, hooks), the object destructors of nodes in
// lists A/B beyond the two releases, and any effect of the container's neighbours. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

#include "insert_1beeb0.hpp"
#include "list_dtor_1bdeb0.hpp"
#include "shared_release_f590.hpp"

namespace sotc {

inline constexpr std::uint64_t kContainerDtor1bede0Rva = 0x1BEDE0;
inline constexpr std::size_t kContainerDtor1bede0Size = 208;
inline constexpr std::uint64_t kContainerDtor1bede0EndRva = 0x1BEEB0;
inline constexpr std::uint64_t kContainerDtor1bede0FreeRva = 0x170C0;
inline constexpr std::uint64_t kContainerDtor1bede0ReleaseRva = 0xF590;
inline constexpr std::uint64_t kContainerDtor1bede0ListCVtable = 0x1598CE0;
inline constexpr std::uint64_t kContainerDtor1bede0ListCSlot3 = 0x19F0;

struct ContainerDtor1bede0Hooks {
    // Original list-C call [vtable+0x18] with rdi = node+0x30. The recovered target is a bare ret;
    // null keeps that exact no-op. A host may supply a native payload destructor instead.
    void (*destroy_list_c_payload)(void* storage_30) = nullptr;
    // 0xf590 on node+0x30 then node+0x10 for lists B and A.
    SharedReleaseF590Hooks release{};
    // 0x170c0(node) — tag-2 deallocate. Null => visible failure before any list is touched.
    void (*free_tag2)(void* node) = nullptr;
};

struct ContainerDtor1bede0Result {
    bool ok = false;
    int freed_c = 0;
    int freed_b = 0;
    int freed_a = 0;
    int releases = 0;          // two per list A/B node
    int payload_destroys = 0;  // list-C override calls
    int release_failures = 0;  // a release returned ok == false
};

// Runs the recovered teardown over the 0x60 B container. The three sentinels are reset and all
// counts zeroed after their loops, so the container is reusable afterwards.
[[nodiscard]] ContainerDtor1bede0Result container_dtor_1bede0(
    Container1beeb0& container, const ContainerDtor1bede0Hooks& hooks) noexcept;

}  // namespace sotc
