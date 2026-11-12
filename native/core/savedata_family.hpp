// Native models for the five bodies around `0xa54920`:
//
//   `0xa55050..0xa551ab` (347 B, sha256 062b16fd…a7456e) — slot allocator. FULL LIFT.
//   `0xa57c10..0xa57d3f` (303 B, sha256 6c03fcb0…fd029c) — sentinel cleanup. Reachable path full
//      lift; it calls the real `0xa58490` model. The in-order traversal after `0xa57c4e` is dead
//      at the byte level (see below).
//   `0xa57d40..0xa581f1` (1,201 B, sha256 4f54c1a6…b3f2e) — red-black erase. FULL LIFT through the
//      identical `0x482a90` body (see the equivalence note).
//   `0xa58490..0xa584dc` (76 B, sha256 7003816a…43c24b) — recursive right-then-left node release.
//      FULL LIFT.
//   `0xa586a0..0xa5880f` (367 B, sha256 723d9b30…5b791f) — block-map growth. FULL LIFT.
//
// Extents are FDE-verified (`local/validation/registry-analysis-agent/fde_ranges.json`): the next
// FDE starts at `0xa551b0`, `0xa57d40`, `0xa581f1` (then `0xa58490`), `0xa584dc` and `0xa58810`;
// the bytes between are padding.
//
// `0xa55050` reads the signed dword cursor at `0x32f88a0`, tests the eight `0xb8`-byte slots at
// `parent+0x28+idx*0xb8` (empty marker at `+0x14`), returns the first empty slot, and stores
// `idx+1` in the cursor. If all eight are occupied it returns `parent+0x28` without changing the
// cursor. It makes no calls and writes only the cursor (2 RIP reads/writes, 8 `je`, 1 store).
//
// `0xa57c10` loads `header = this+0x8`, then `rbx = [header]` and compares `rbx` with `[header]`
// (`a57c37..a57c3f`). That comparison is always equal, so the only reachable path is the empty
// cleanup at `a57ceb..a57d11`: call `0xa58490(this, [header+0x8])`, then set
// `[header+0x8] = [header] = [header+0x10] = header` and `[this+0x10] = 0`, then fall into the
// shared canary/return at `a57d1a..a57d38`. The second compare and the traversal at
// `a57c45..a57cea` are unreachable from this entry: a full disassembly scan finds no call or
// branch into `0xa57c45..0xa57cff` from any other body. That traversal contains the only call to
// `0xa57d40` in this function and is not modelled as if it ran; it is dead code, not a hook.
// The release is the real `0xa58490` model, not a host callback.
//
// `0xa57d40` is byte-identical to `0x482a90` once relative displacements are normalized: all 340
// instructions match with 0 normalized differences, both direct calls resolve to `0x170b0` and
// `0xf8fc20`, and the only absolute reference that differs is the assert string
// (`0x12c4741` here vs `0xffb153`), whose bytes are the same `"invalid map/set<T> iterator"`.
// `savedata_tree_erase_a57d40` therefore delegates to the already-admitted `rb_erase_482a90`
// model rather than duplicating a 340-instruction rebalance. The RB node layout used by both is
// `+0x00 left`, `+0x08 parent`, `+0x10 right`, `+0x18 colour`, `+0x19 nil`; the header root is
// `header+0x08`.
//
// `0xa58490` is a 26-instruction recursive release: reject the sentinel (`+0x19 != 0`), otherwise
// recurse into `+0x10`, free the node through `0x170b0`, then iterate the `+0x00` link and repeat.
// It is modelled exactly, with the allocator pair supplied by `AllocDispatchContract`.
//
// `0xa586a0` is a full 108-instruction growth lift for the **libstdc++ deque map** at
// `a54920+0x5e8`. The 0x50 B object is the deque's `_M_impl` map/iterator fragment:
//
//   +0x00 map base (a586c2 load, a587c4 store)
//   +0x08 map capacity in eight-byte node slots (a586c5 load, a587c7 store)
//   +0x10 start.cur (not read or written by this body)
//   +0x18 start.first (a587db store: the block pointer at start.node)
//   +0x20 start.last, start.first + 0x100 (a587e5 store)
//   +0x28 start.node (a586ba load, a587cf store)
//   +0x30 finish.cur (not read or written by this body)
//   +0x38 finish.first (a587f2 store)
//   +0x40 finish.last, finish.first + 0x100 (a587fc store)
//   +0x48 finish.node (a586be load, a587e9 store)
//
// The only caller, `0xa54920`, passes `parent+0x5e8` at `a54fbc` and reads `finish.cur` at
// `parent+0x618` (+0x30), `finish.first` at `parent+0x620` (+0x38), `finish.last` at
// `parent+0x628` (+0x40), and `finish.node` at `parent+0x630` (+0x48). Those are the same object:
// the body updates the two node pointers and the four block bounds, while the untouched current
// pointers remain valid offsets into the unchanged blocks. The 0x100-byte allocation in the caller
// at `a54fd9` is a deque block of node pointers, not a descriptor. A block itself is 0x100 B
// (`addq $0x100` at a587df/a587f6).
//
// Mode 1 prefers front growth: if `start.node` has at least `requested` node slots before it, the
// used node pointers move left by `max(requested, front/2)`. Mode 0 prefers back growth: if the
// free node slots after the used region are at least `requested`, they move right by
// `max(requested, free/2)`. Otherwise the body allocates
// `(capacity + max(capacity, requested) + 2) * 8` bytes through `0x17090`, places `start.node` at
// `front + (mode == 0 ? requested : 0)`, copies the used node pointers, frees the old map base
// through `0x170c0`, and commits the new base/capacity. It then rewrites +0x28/+0x48 and the
// +0x18/+0x20/+0x38/+0x40 block bounds. The model refuses an inconsistent map instead of reading
// out of bounds, leaves the map unchanged when allocation fails, and reports a failed host free.
//
// The four callees are **reused, not reimplemented**: `0x17090` (20 B) and `0x170c0` (10 B) are the
// already-modelled `alloc_17090`/`free_170c0` wrappers over `0x12820`/`0x12af0`; `0xf8f470` (6 B)
// and `0xf8f480` (6 B) are the `memcpy`/`memmove` PLT imports (GOT `0x1600028`/`0x1600030`, NIDs
// `Q3VBxCXhUHs`/`+P6FRGH4LfA` checked by the oracle), modelled with the ISO C routines. Their
// exact bytes, sizes and SHA-256 pins are below.
//
// G1 PARTIAL: static and fixture evidence only. No guest byte is executed.
#pragma once

#include <cstddef>
#include <cstdint>

#include "alloc_wrappers_17070.hpp"
#include "rb_tree_482a90.hpp"

namespace sotc {

inline constexpr std::uint64_t kSavedataSlotA55050Rva = 0xA55050;
inline constexpr std::size_t kSavedataSlotA55050Size = 347;
inline constexpr std::uint64_t kSavedataSlotA55050EndRva = 0xA551AB;
inline constexpr char kSavedataSlotA55050Sha256[] =
    "062b16fd7c32ca8f25c760e91c7f90e1855cd0ada958128706fee4e140a7456e";

inline constexpr std::uint64_t kSavedataTreeClearA57C10Rva = 0xA57C10;
inline constexpr std::size_t kSavedataTreeClearA57C10Size = 303;
inline constexpr std::uint64_t kSavedataTreeClearA57C10EndRva = 0xA57D3F;
inline constexpr char kSavedataTreeClearA57C10Sha256[] =
    "6c03fcb0da5ce003b10b190f91c46e6c1e49e899c38d156920c3f64f9dfd029c";

inline constexpr std::uint64_t kSavedataTreeEraseA57D40Rva = 0xA57D40;
inline constexpr std::size_t kSavedataTreeEraseA57D40Size = 1201;
inline constexpr std::uint64_t kSavedataTreeEraseA57D40EndRva = 0xA581F1;
inline constexpr char kSavedataTreeEraseA57D40Sha256[] =
    "4f54c1a69b06da222423c7f9ce724823be6698d3fa8acbb699e2ab611f1b3f2e";

inline constexpr std::uint64_t kSavedataTreeReleaseA58490Rva = 0xA58490;
inline constexpr std::size_t kSavedataTreeReleaseA58490Size = 76;
inline constexpr std::uint64_t kSavedataTreeReleaseA58490EndRva = 0xA584DC;
inline constexpr char kSavedataTreeReleaseA58490Sha256[] =
    "7003816aa67ab301f6d65354bbc84182d442f44743ab3316123002631943c24b";

// The already-admitted body this erase is byte-identical to (normalized). Kept as a pin so the
// delegation cannot silently drift to a different routine.
inline constexpr std::uint64_t kSavedataRbEraseDuplicateA482A90Rva = 0x482A90;
inline constexpr std::size_t kSavedataRbEraseDuplicateSize = 1201;

inline constexpr std::uint64_t kSavedataGrowA586A0Rva = 0xA586A0;
inline constexpr std::size_t kSavedataGrowA586A0Size = 367;
inline constexpr std::uint64_t kSavedataGrowA586A0EndRva = 0xA5880F;
inline constexpr char kSavedataGrowA586A0Sha256[] =
    "723d9b307cdcc377080ddb78944172777def53b65e3df917ceacbc7a015b791f";

// 0xa586a0 callees. The first two are reused native models; the last two are PLT import thunks
// whose ISO C operations are modelled directly.
inline constexpr std::uint64_t kSavedataGrowAlloc17090Rva = 0x17090;
inline constexpr std::size_t kSavedataGrowAlloc17090Size = 20;
inline constexpr char kSavedataGrowAlloc17090Sha256[] =
    "f26d9b99d1ad4395bc0dd389d33874229cce9e8ad4ec0dd359fabf714b97ad11";

inline constexpr std::uint64_t kSavedataGrowFree170C0Rva = 0x170C0;
inline constexpr std::size_t kSavedataGrowFree170C0Size = 10;
inline constexpr char kSavedataGrowFree170C0Sha256[] =
    "39cd562d64a27bc6e432aaf827156a2b3551b6d1ae1c44831c2a2f5feeace372";

inline constexpr std::uint64_t kSavedataGrowMemcpyF8F470Rva = 0xF8F470;
inline constexpr std::size_t kSavedataGrowMemcpyF8F470Size = 6;
inline constexpr std::uint64_t kSavedataGrowMemcpyF8F470GotRva = 0x1600028;
inline constexpr char kSavedataGrowMemcpyF8F470Sha256[] =
    "bd719a31f451cadad5ba339791da6db9d6ead137e50a1a512755992099dadf75";

inline constexpr std::uint64_t kSavedataGrowMemmoveF8F480Rva = 0xF8F480;
inline constexpr std::size_t kSavedataGrowMemmoveF8F480Size = 6;
inline constexpr std::uint64_t kSavedataGrowMemmoveF8F480GotRva = 0x1600030;
inline constexpr char kSavedataGrowMemmoveF8F480Sha256[] =
    "02101fcd50d9d2961c75acefd486bc3315a09ea38f7008eedef496da47a7cc51";

inline constexpr std::uint64_t kSavedataSlotCursorRva = 0x32F88A0;
inline constexpr std::size_t kSavedataSlotCount = 8;
inline constexpr std::size_t kSavedataSlotStride = 0xB8;
inline constexpr std::size_t kSavedataSlotBaseOffset = 0x28;
inline constexpr std::size_t kSavedataSlotStatusOffset = 0x14;

inline constexpr std::size_t kSavedataTreeHeaderOffset = 0x8;
// header+0x8, the word `0xa57c10` passes to `0xa58490` and later resets.
inline constexpr std::size_t kSavedataTreeFreeSlotOffset = 0x8;
// this+0x10 is the same cell as header+0x8: the body's last write zeroes it after the sentinel
// reset. It is not a separate size field.
inline constexpr std::size_t kSavedataTreeSizeOffset = 0x10;
inline constexpr std::size_t kSavedataTreeNodeFlagOffset = 0x19;

inline constexpr std::size_t kSavedataBlockSize = 0x100;   // addq $0x100 at a587df / a587f6
inline constexpr std::size_t kSavedataMapBaseOffset = 0x00;
inline constexpr std::size_t kSavedataMapCapacityOffset = 0x08;
inline constexpr std::size_t kSavedataMapStartCurOffset = 0x10;
inline constexpr std::size_t kSavedataMapStartFirstOffset = 0x18;
inline constexpr std::size_t kSavedataMapStartLastOffset = 0x20;
inline constexpr std::size_t kSavedataMapStartNodeOffset = 0x28;
inline constexpr std::size_t kSavedataMapFinishCurOffset = 0x30;
inline constexpr std::size_t kSavedataMapFinishFirstOffset = 0x38;
inline constexpr std::size_t kSavedataMapFinishLastOffset = 0x40;
inline constexpr std::size_t kSavedataMapFinishNodeOffset = 0x48;

// The 0x50-byte libstdc++ deque map fragment read and written by 0xa586a0. `start.node` and
// `finish.node` point into the node-slot array at `map`; `start.first/last` and
// `finish.first/last` are the 0x100-byte block bounds. `start.cur` and `finish.cur` are not
// touched by the growth body and are retained at their real offsets.
struct SavedataDequeMap {
    std::uint8_t** map;             // +0x00 node-slot array base
    std::size_t map_capacity;       // +0x08 capacity in node slots
    std::uint8_t* start_cur;        // +0x10 start.cur
    std::uint8_t* start_first;      // +0x18 start.first
    std::uint8_t* start_last;       // +0x20 start.last
    std::uint8_t** start_node;      // +0x28 start.node
    std::uint8_t* finish_cur;       // +0x30 finish.cur
    std::uint8_t* finish_first;     // +0x38 finish.first
    std::uint8_t* finish_last;      // +0x40 finish.last
    std::uint8_t** finish_node;     // +0x48 finish.node
};
static_assert(sizeof(SavedataDequeMap) == 0x50 &&
              offsetof(SavedataDequeMap, map) == kSavedataMapBaseOffset &&
              offsetof(SavedataDequeMap, map_capacity) == kSavedataMapCapacityOffset &&
              offsetof(SavedataDequeMap, start_cur) == kSavedataMapStartCurOffset &&
              offsetof(SavedataDequeMap, start_first) == kSavedataMapStartFirstOffset &&
              offsetof(SavedataDequeMap, start_last) == kSavedataMapStartLastOffset &&
              offsetof(SavedataDequeMap, start_node) == kSavedataMapStartNodeOffset &&
              offsetof(SavedataDequeMap, finish_cur) == kSavedataMapFinishCurOffset &&
              offsetof(SavedataDequeMap, finish_first) == kSavedataMapFinishFirstOffset &&
              offsetof(SavedataDequeMap, finish_last) == kSavedataMapFinishLastOffset &&
              offsetof(SavedataDequeMap, finish_node) == kSavedataMapFinishNodeOffset);

enum class SavedataFamilyRefusal : std::uint8_t {
    none,
    null_argument,
    null_subtree,      // 0xa57c10 would pass null to 0xa58490 and fault
    invalid_layout,    // 0xa586a0 would read outside the map
    allocation_failed, // 0x17090 returned null
    hook_missing,
    hook_rejected,
};

struct SavedataSlotAllocResult {
    bool ok = false;
    SavedataFamilyRefusal refusal = SavedataFamilyRefusal::none;
    std::uint8_t* slot = nullptr;   // parent+0x28+index*0xb8, or parent+0x28 when full
    std::uint32_t index = 0;
    bool found = false;
    std::uint32_t cursor_before = 0;
    std::uint32_t cursor_after = 0;
};

// Full model of `0xa55050`; `parent` must point at the eight-slot block and `cursor` at the
// dword cell (normally 0x32f88a0). The signed original computes `(cursor+i) % 8`; callers keep
// the cursor non-negative (it starts at zero and only ever stores idx+1).
[[nodiscard]] SavedataSlotAllocResult savedata_slot_alloc_a55050(std::uint8_t* parent,
                                                                 std::uint32_t* cursor) noexcept;

struct SavedataTreeReleaseResult {
    bool ok = false;
    SavedataFamilyRefusal refusal = SavedataFamilyRefusal::none;
    std::size_t released = 0;
};

// Full model of `0xa58490(self, node)`: `self` is threaded through the recursion (the original
// passes it in `rdi`) but only the node is freed. A sentinel returns before any allocator use;
// otherwise recurse into right, free the node through `0x170b0`, and continue at left. `released`
// counts successful frees; on a failing deallocation the walk stops immediately with
// `hook_rejected` and reports the partial count.
[[nodiscard]] SavedataTreeReleaseResult savedata_tree_release_a58490(
    void* self, RbEraseNode* node, const AllocDispatchContract& contract) noexcept;

// Full model of `0xa57d40`, delegated to the byte-identical `0x482a90` erase. Returns the
// successor computed before unlinking; a sentinel argument returns nullptr and mutates nothing.
[[nodiscard]] RbEraseNode* savedata_tree_erase_a57d40(
    RbEraseTree& tree, RbEraseNode* node, const RbEraseHooks& hooks) noexcept;

struct SavedataTreeClearResult {
    bool ok = false;
    SavedataFamilyRefusal refusal = SavedataFamilyRefusal::none;
    int release_calls = 0;                 // expect 1: the a57cf2 call to 0xa58490
    std::size_t released_nodes = 0;
    bool sentinel_reset = false;
    bool dead_traversal_unreachable = true;   // a57c45..a57cea is skipped by the tautological
                                               // `cmp [header],rbx` at a57c3b/a57c3f
};

// Models the reachable byte path of `0xa57c10`: `[self+0x8]` is the header pointer (a57c32),
// `[header+0x8]` is the root passed to `0xa58490` (a57ceb), and `[self+0x10]` is the cell zeroed
// last. The root is released through the real `0xa58490(self, root, ...)` model, then the header
// links are reset to the header. A null header/root would be dereferenced by the original and is
// refused instead of faulting.
[[nodiscard]] SavedataTreeClearResult savedata_tree_clear_a57c10(
    void* self, const AllocDispatchContract& contract = {}) noexcept;

struct SavedataGrowResult {
    bool ok = false;
    SavedataFamilyRefusal refusal = SavedataFamilyRefusal::none;
    bool moved = false;          // memmove path (0xf8f480)
    bool allocated = false;      // new node-slot array (0x17090)
    bool copied = false;         // memcpy path (0xf8f470)
    bool freed_old = false;      // old map base released (0x170c0)
    std::size_t used_nodes = 0;
    std::size_t node_bytes_moved = 0;
    std::size_t new_capacity = 0;
};

// Full model of `0xa586a0(map, requested, mode)`: mode 1 grows at the front, mode 0 at the back,
// any other mode takes the allocation path. `requested` is the incoming rsi and `mode` the
// incoming edx (the a54920 append calls pass 1 and 1). An inconsistent map is refused before any
// read; a failed allocation or host free leaves the map unchanged and reports the failure. The
// model moves the 8-byte node pointers, not the 0x100-byte blocks, and preserves start.cur /
// finish.cur exactly as the body does.
[[nodiscard]] SavedataGrowResult savedata_grow_a586a0(
    SavedataDequeMap* map, std::size_t requested, int mode,
    const AllocDispatchContract& contract = {}) noexcept;

}  // namespace sotc
