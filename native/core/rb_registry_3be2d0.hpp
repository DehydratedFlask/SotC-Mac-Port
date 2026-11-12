// CPU-01-AOT-CFG-SCALE-01-BATCH21: native model of the red-black "named registry" find-or-insert used by startup slot 446
// (the call 0x3be2d0) with every callee body read end to end from the pinned ELF and pinned by tools/lift/check_rb_registry_3be2d0.py.
// NOTHING here executes, loads or calls an original RVA; the allocator thunks 0x17070 / 0x170b0 and the libc imports are the only hooks.
//
// Tree layout (Dinkumware-style `_Tree`, 0x18 B object: +0 never touched, +8 head node, +0x10 size):
//   head node (0x30 B from 0x17070): left = leftmost, parent = root, right = rightmost; [0x18] = color (0 red, 1 black); [0x19] = isnil.
//   An empty tree has left = parent = right = head and color = isnil = 1.
//   data node (0x48 B): +0 left, +8 parent, +0x10 right, +0x18 color, +0x19 isnil, +0x20 SharedStr key (data, size, aux, ctl), +0x40 mapped.
//   Ordering is the libc import 0xf8f500 = strcasecmp (NID AV6ipCNa4Rw): case-INSENSITIVE, strcmp-style sign.
//
// Functions lifted (names carry the original RVA):
//   0x3be2d0 (192 B)  register_entry: lower_bound(key at entry+0x60); if absent or mapped == 0: *index(key) = entry.
//   0x3c0840 (368 B)  index: lower_bound; on a miss allocate a 0x48 B node (key SharedStr copied, ctl incremented atomically, mapped = 0),
//                     insert with the lower bound as the hint, return &node->mapped.
//   0x3c31b0 (692 B)  hint insert: empty tree -> insert at head; hint == leftmost / head / interior neighbours decide a direct insert,
//                     otherwise the full unique insert 0x3c3710 runs.
//   0x3c3710 (544 B)  unique insert: descent (dl == 0: key < node.key goes left; dl != 0: node.key >= key goes left), predecessor check,
//                     a duplicate releases the new node (0xf590 on its key, 0x170b0 on the node) and returns the existing one.
//   0x3c3470 (662 B)  link + rebalance: size bound 0x038e38e38e38e38d, head bookkeeping, the textbook red-black fix-up (uncle red:
//                     recolor; otherwise one or two rotations), root forced black.
//   0x3c3930 (32 B)   node destroy (0xf590 on node+0x20, tail 0x170b0).
// The 24-byte result records of 0x3c31b0 / 0x3c3710 / 0x3c3470 carry 16 INDETERMINATE stack bytes at +0 (never read by any caller):
// the model returns only the node and the inserted flag.
//
// Paths modelled as failure (the original throws, via 0xf8fc10, and never returns): the size bound of 0x3c3470 (after the node was
// destroyed by 0x3c3930, as the original does first). One predecessor path of 0x3c3710 / 0x3c31b0 reads the key of the HEAD node
// (uninitialised memory in the original); it is unreachable for a valid tree and reported as a failure (ok == false).
//
// Reuse for the erase 0x482a90 (slot 454): the node layout, the head convention, the isnil/color bytes and the size field at +0x10 are the same
// shape; see docs/checkpoints/2026-11-03-startup-batch-21.md for the feasibility analysis.
#pragma once

#include <cstddef>
#include <cstdint>

#include "alloc_wrappers_17070.hpp"
#include "hashed_string_14c3a0.hpp"

namespace sotc {

inline constexpr std::uint64_t kRbRegisterEntryRva = 0x3be2d0;
inline constexpr std::size_t kRbRegisterEntrySize = 192;
inline constexpr std::uint64_t kRbIndexRva = 0x3c0840;
inline constexpr std::size_t kRbIndexSize = 368;
inline constexpr std::uint64_t kRbHintInsertRva = 0x3c31b0;
inline constexpr std::size_t kRbHintInsertSize = 692;
inline constexpr std::uint64_t kRbUniqueInsertRva = 0x3c3710;
inline constexpr std::size_t kRbUniqueInsertSize = 544;
inline constexpr std::uint64_t kRbInsertAtRva = 0x3c3470;
inline constexpr std::size_t kRbInsertAtSize = 662;
inline constexpr std::uint64_t kRbNodeDestroyRva = 0x3c3930;
inline constexpr std::size_t kRbNodeDestroySize = 32;
inline constexpr std::uint64_t kRbStrcasecmpPltRva = 0xf8f500;
inline constexpr std::size_t kRbHeadNodeBytes = 0x30;
inline constexpr std::size_t kRbDataNodeBytes = 0x48;
inline constexpr std::uint64_t kRbMaxSize = 0x038e38e38e38e38d;
inline constexpr std::size_t kRbEntryKeyOffset = 0x60;   // the SharedStr inside a registered entry (0x3be2ec: lea rsi,[r15+0x60])

struct RbNode {
    RbNode* left;            // +0x00
    RbNode* parent;          // +0x08
    RbNode* right;           // +0x10
    std::uint8_t color;      // +0x18  0 = red, 1 = black
    std::uint8_t isnil;      // +0x19  1 = the head node
    std::uint8_t pad1a[6];   // +0x1a  never written
    SharedStr key;           // +0x20  (data, size, aux, ctl)
    void* mapped;            // +0x40
};
static_assert(sizeof(RbNode) == kRbDataNodeBytes && offsetof(RbNode, color) == 0x18 && offsetof(RbNode, isnil) == 0x19 &&
              offsetof(RbNode, key) == 0x20 && offsetof(RbNode, mapped) == 0x40);

struct RbMap {
    void* word0;             // +0x00 never read or written by the lifted functions
    RbNode* head;            // +0x08
    std::uint64_t size;      // +0x10
};
static_assert(sizeof(RbMap) == 0x18 && offsetof(RbMap, head) == 8 && offsetof(RbMap, size) == 0x10);

struct RbHooks {
    AllocDispatchContract alloc{};                                          // 0x17070 / 0x170b0 (HOOKS: TLS allocator dispatch is not lifted)
    int (*strcasecmp_import)(const char*, const char*) = nullptr;            // 0xf8f500; null selects libc strcasecmp
};

struct RbInsertResult {
    bool ok = false;         // false: allocation failure, the size bound (original throws) or an unreachable path
    RbNode* node = nullptr;  // +0x10 of the original 24-byte record
    bool inserted = false;   // 0x3c3710 byte +0x18 (true: the new node was linked, false: duplicate, new node destroyed)
};

// The empty tree: head = 0x17070(0x30), left = parent = right = head, color = isnil = 1, size = 0 (the 'tree' header rows of other slots).
[[nodiscard]] bool rb_map_init_empty(RbMap& map, const RbHooks& hooks) noexcept;

// 0x3c3930: destroy a node: 0xf590 on node+0x20 (key release), then 0x170b0 on the node.
[[nodiscard]] bool rb_node_destroy_3c3930(RbNode* node, const RbHooks& hooks) noexcept;

// 0x3c3470: link `node` under `where` (add_left selects the side) and rebalance. Destroys `node` and fails at the size bound.
[[nodiscard]] RbInsertResult rb_insert_at_3c3470(RbMap& map, bool add_left, RbNode* where, RbNode* node, const RbHooks& hooks) noexcept;

// 0x3c3710: unique insert (mode == 0 is the only mode the observed caller uses).
[[nodiscard]] RbInsertResult rb_unique_insert_3c3710(RbMap& map, std::uint8_t mode, const SharedStr& key, RbNode* node, const RbHooks& hooks) noexcept;

// 0x3c31b0: insert with a hint node.
[[nodiscard]] RbInsertResult rb_hint_insert_3c31b0(RbMap& map, const SharedStr& key, RbNode* node, RbNode* hint, const RbHooks& hooks) noexcept;

struct RbIndexResult {
    bool ok = false;
    void** slot = nullptr;   // &node->mapped
    bool created = false;
};

// 0x3c0840: operator[] on the key (copied with an atomic ctl increment on a miss).
[[nodiscard]] RbIndexResult rb_index_3c0840(RbMap& map, const SharedStr& key, const RbHooks& hooks) noexcept;

struct RbRegisterResult {
    bool ok = false;
    bool already_registered = false;   // found with mapped != 0
    bool stored = false;
    bool created_node = false;
};

// 0x3be2d0: `entry` is the registered object (an opaque 0x90+ B record); its key is the SharedStr at entry+0x60.
[[nodiscard]] RbRegisterResult rb_register_entry_3be2d0(RbMap& map, void* entry, const RbHooks& hooks) noexcept;

// Test helpers (NOT part of the original): check the red-black invariants of a tree built by the functions above.
struct RbCheck {
    bool ok = false;
    std::uint64_t count = 0;
    int black_height = 0;
};
[[nodiscard]] RbCheck rb_check_invariants(const RbMap& map) noexcept;

}  // namespace sotc
