// CPU-01-AOT-CFG-SCALE-01-BATCH26 (RB erase 0x482a90): first-class native model of the
// 1,201-byte red-black erase reached from the tree destructor 0x47e0f0 through the clear body
// 0x482960. Original ELF sha256 d0723afb...5861d3.
//
// ---- decoded layout (every offset below comes from the bytes) --------------------------------
// Node (0x20 B):
//   +0x00 left, +0x08 parent, +0x10 right        (0x482aaf `movq 0x10(%rdi)`, 0x482b08
//                                                `movq (%rdi)`, 0x482bac `movq 0x8(%rdi)`;
//                                                the 0xa57c10 successor walk proves the same)
//   +0x18 colour (1 = black, 0 = red)          (0x482c27 `cmpb $1,0x18(%rdi)`)
//   +0x19 nil sentinel flag                     (0x482aa5 `cmpb $0,0x19(%rdi)`)
// Tree (0x18 B):
//   +0x00 unread by every body below
//   +0x08 header pointer                        (0x482987 `movq 0x8(%r12),%r13`)
//   +0x10 element count                         (0x482a61 `movq $0,0x10(%r12)`)
// Header node: +0x00 leftmost, +0x08 root, +0x10 rightmost, colour = nil = 1; the empty state is
// every link pointing at the header itself (0x482a4c/0x482a55/0x482a5d). The root is therefore
// `header->parent` (not `header->right`); the rightmost link is `header->right`. This corrects the
// earlier mirrored field labels; the 1,201-byte body at 0xa57d40 is byte-identical apart from three
// relocations and is covered by the same model.
//
// ---- erase ----------------------------------------------------------------------------------
// 0x482a90 is `_Rb_tree_rebalance_for_erase` with explicit nil nodes: the unlinking prologue
// (0x482ab7..0x482c23) is the libstdc++ transplant, the loop 0x482c27..0x482f04 is the four
// rebalance cases, and 0x482f08 releases the removed node through 0x170b0 after decrementing the
// element count. The successor is written into the returned 24-byte struct at +0x10 (0x482ae1).
// The original ABI is `sret(rdi) + tree(rsi) + a 24-byte stack value whose third word is the
// node (arg9)`; the native signature below is the clean `(tree, node) -> successor` model. The
// ABI mapping is recorded, not reproduced.
//
// 0x482a90 rejects a nil argument by loading "invalid map/set<T> iterator" (0xffb153) and calling
// 0xf8fc20 (`__assert_fail`-style abort). The native model returns nullptr for that case.
//
// ---- clear and the reachability fact --------------------------------------------------------
// 0x482960 (304 B) opens with `movq 0x8(%r12),%r13; movq (%r13),%rbx; cmpq (%r13),%rbx;
// je 0x482a3b` — a comparison of the header's left link against *itself* (both decoders agree on
// the bytes 49 8b 5d 00 / 49 3b 5d 00). The branch is always taken, so the erase loop at
// 0x4829b0..0x482a28 (the only call site of 0x482a90) is unreachable in this image; the taken
// path calls 0x483240 on the root, resets the three header links and zeroes the count.
// `rb_clear_482960` therefore models the taken path (post-order release + reset) and
// `rb_erase_all_loop` models the loop the dead branch contains, so the erase is exercised and the
// reachability fact is explicit rather than assumed.
//
// 0x483240 itself is pinned as a callee (4 call sites: 0x47e572, 0x47e5b4, 0x482a42, 0x483267).
// Its recursion edge is +0x10 and its iteration edge is +0x00; with the erase's +0x10 = parent
// this does not match a plain left/right post-order, so the destroy walk is modelled as the
// equivalent post-order release and labelled INFERENCE where it cannot be observed (the slot's
// three trees are empty at construction, so the walk is never entered by slot 454).
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kRbEraseRva = 0x482a90;
inline constexpr std::size_t kRbEraseSize = 0x4b1;              // 1,201 B, ends 0x482f41
inline constexpr std::uint64_t kRbEraseNextRva = 0x482f50;      // 15 pad nops 0x482f41..0x482f50
inline constexpr std::uint64_t kRbClearRva = 0x482960;
inline constexpr std::size_t kRbClearSize = 0x130;              // 304 B, ends 0x482a90
inline constexpr std::uint64_t kRbDestroyRva = 0x483240;
inline constexpr std::uint64_t kRbEraseNodeReleaseRva = 0x170b0;     // operator delete
inline constexpr std::uint64_t kRbAssertFailRva = 0xf8fc20;
inline constexpr std::uint64_t kRbAssertTextRva = 0xffb153;     // "invalid map/set<T> iterator"
inline constexpr std::uint64_t kRbTreeDtorA = 0x47e0f0;         // clear + delete header (0x38)
inline constexpr std::uint64_t kRbTreeDtorB = 0x482f50;         // payload destroy + delete header

inline constexpr std::uint8_t kRbRed = 0;
inline constexpr std::uint8_t kRbBlack = 1;

struct RbEraseNode {
    RbEraseNode* left;       // +0x00
    RbEraseNode* parent;     // +0x08
    RbEraseNode* right;      // +0x10
    std::uint8_t color; // +0x18
    std::uint8_t nil;   // +0x19
    std::uint8_t pad1a[6];
};
static_assert(sizeof(RbEraseNode) == 0x20 && offsetof(RbEraseNode, left) == 0x00 &&
              offsetof(RbEraseNode, parent) == 0x08 && offsetof(RbEraseNode, right) == 0x10 &&
              offsetof(RbEraseNode, color) == 0x18 && offsetof(RbEraseNode, nil) == 0x19);

struct RbEraseTree {
    std::uint64_t tag;  // +0x00 unread by the erase/clear bodies
    RbEraseNode* header;     // +0x08
    std::size_t size;   // +0x10
};
static_assert(sizeof(RbEraseTree) == 0x18 && offsetof(RbEraseTree, header) == 0x08 &&
              offsetof(RbEraseTree, size) == 0x10);

// The two allocation/release entries the original uses: 0x17070 (`operator new`) and 0x170b0
// (`operator delete`). A null entry falls back to non-throwing ::operator new/delete.
struct RbEraseHooks {
    void* (*allocate)(std::size_t) noexcept = nullptr;
    void (*release)(void*) noexcept = nullptr;
};

// Allocates `header_bytes` (0x38 for the two small trees, 0x1a8 for the payload tree) and makes it
// a self-linked black nil header with count 0, exactly as 0x4842b1/0x4842fd/0x48437d do.
[[nodiscard]] bool rb_tree_init(RbEraseTree& tree, std::size_t header_bytes, const RbEraseHooks& hooks) noexcept;

// clear + release the header (the 0x47e0f0 path minus the destructor tag).
void rb_tree_release(RbEraseTree& tree, const RbEraseHooks& hooks) noexcept;

// 0x482a90: unlinks `z`, rebalances, decrements the count and releases `z`; returns the successor
// computed before unlinking (the header when `z` is last). `z` with nil == 1 returns nullptr and
// mutates nothing.
[[nodiscard]] RbEraseNode* rb_erase_482a90(RbEraseTree& tree, RbEraseNode* z, const RbEraseHooks& hooks) noexcept;

// 0x482960 taken path: post-order release of the whole tree, then header links = self and count 0.
// Returns the number of released nodes.
[[nodiscard]] std::size_t rb_clear_482960(RbEraseTree& tree, const RbEraseHooks& hooks) noexcept;

// The loop the always-taken guard at 0x48298b hides: erase from the leftmost until empty. Same
// final state as rb_clear, but every node goes through rb_erase_482a90.
[[nodiscard]] std::size_t rb_erase_all_loop(RbEraseTree& tree, const RbEraseHooks& hooks) noexcept;

// In-order successor; the header when there is none.
[[nodiscard]] RbEraseNode* rb_next(const RbEraseTree& tree, RbEraseNode* z) noexcept;

}  // namespace sotc
