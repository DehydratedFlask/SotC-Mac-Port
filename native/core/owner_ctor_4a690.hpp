// Row-21 owner constructor `0x4a690` (`kOwnerMemberRows[21]`, 0x90 B heap object, pointer slot
// `0x2ea8978`).
//
// Characterization: `local/re/evidence/owner-row21-4a690/report.md` (offline static decode only).
// Body `0x4a690..0x4a824`, size `0x194` (404 B), sha256
// `c0771efb29d101f447dbdbb97a7e2a40871e46e04cab6391f76c5363792d4d24`, 104 decoded instructions
// (100 real + 4 inter-block `nop` alignment pads), eight `call` sites with six distinct targets,
// zero indirect calls, no stack canary.
//
// Corrections to the row-21 summary in `local/re/evidence/owner-ctors-next/report.md` §5, which
// described "a global red-black tree walk ... 6 direct calls":
//   - eight call sites, not six; `0xf180` (SharedStr ctor) and `0xf8f500` (the `strcmp` PLT) are
//     each called twice;
//   - the allocator is the plain `0x17070` thunk with the immediate `0x48`, not `0x17090`
//     (`0x17090` is not called from this body at all);
//   - `*(0x2ea8980)` is **not** a tree: it is the head of an intrusive singly-linked list of
//     0x30 B entries (`+0x08` value, `+0x28` next). The constructor **drains** it and republishes
//     the head after every entry, so the global ends up null;
//   - the *destination* (`object+0x80`) is an ordered tree, but the constructor never links a node
//     itself: it calls `0x4b3f0` (`insert_unique`, no hint) which allocates the 0x48 B node, copies
//     the key, runs the fixup through `0x4c500` and returns `&node->mapped`.
//
// Object layout (`this`), byte-exact; the holes stay untouched:
//   +0x00 16 B zero (the destructor frees +0x00/+0x08/+0x10 as pointers, so they start null)
//   +0x10 qword 0
//   +0x18 0x20 B SharedStr  (`0xf180`, already native as `init_slot_f180`)
//   +0x38 0x20 B SharedStr  (`0xf180`)
//   +0x58 qword stamp.first   (from `0x511f0`)
//   +0x60 qword stamp.second  (from `0x511f0`)
//   +0x68 u32   0x7d03        (+0x6c..+0x70 is a hole)
//   +0x70 0x10 B critical-section sub-object (`0x3b310`, hook-owned, never written here)
//   +0x78 tree handle: +0x00 never read or written here, +0x08 anchor pointer, +0x10 node count
//   +0x80 anchor pointer (== tree handle + 0x08)
//   +0x88 node count     (== tree handle + 0x10)
//
// Tree node (`0x48` B, MSVC-style `_Tree_node` base): left +0x00, parent +0x08, right +0x10,
// colour +0x18 (0 red, 1 black), is_anchor +0x19, value +0x20. An *empty* link is the anchor node
// itself (self-linked on creation), not null: `anchor->parent` holds the root, `anchor->left` the
// leftmost node, `anchor->right` the rightmost, exactly as `0x4c500` and `0x4b3f0` read them.
// The tree value at +0x20 is 0x20 B (`+0x00` key text pointer, `+0x18` refcounted control pointer)
// and the mapped slot at +0x40 receives the drained queue entry.
//
// What is native here: the whole constructor body - the head/store sequence, both `0xf180` slots,
// the stamp/tag block, the anchor allocation and its self-link/colour stores, the global-list
// drain, the per-entry lower_bound descent and equivalent-key test, and the `0x4b3f0`
// `insert_unique` primitive (`_Lower_bound` + `_Insert_new_node_at` + red-black fixup, size
// increment) that the body calls.
//
// The body never frees anything, not even on a failure path: a node allocation that fails leaves
// the previous prefix (and the anchor) installed, exactly like the original would before faulting.
// Teardown is the destructor's contract (report §7). The native `lock incq` model uses
// `std::atomic_ref`, so the key's control pointer must be 8-byte aligned; the guest instruction
// tolerates any alignment (report §10.4).
//
// What stays a hook: `0xf8f500` (`strcmp`, the key order), `0x17070` (allocate),
// `0x511f0` (unnamed 16 B stamp provider), `0x3b310` (critical-section/mutex init at +0x70: import
// sequence attr-init, attr-settype(2), init(object, attr, "CritSec"), attr-destroy) and the two
// `*(0x2ea8980)` queue accessors.
//
// Not modelled: the destructor `0x4bde0` (size `0xc9`, `kDtorThenFree` pairing) is a recorded
// contract only (report §7): `0x4ac80(this)`, release+free `+0x10`, `0x4beb0(this+0x78)`,
// free `[+0x80]`, `0x3b380(this+0x70)`, `0xf590(+0x38)`, `0xf590(+0x18)`, then `+0x10`, `+0x08`
// and `+0x00` (destroy `0x4de20`, free).
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kOwnerCtor4A690Rva = 0x4A690;
inline constexpr std::size_t kOwnerCtor4A690Size = 0x194;
inline constexpr std::size_t kOwnerCtor4A690InsnCount = 104;
inline constexpr std::size_t kOwnerCtor4A690PadNopCount = 4;
inline constexpr std::size_t kOwnerCtor4A690ObjSize = 0x90;
inline constexpr std::uint64_t kOwnerCtor4A690SlotRva = 0x2EA8978;       // row-21 destination cell
inline constexpr std::uint64_t kOwnerCtor4A690QueueHeadRva = 0x2EA8980;  // *(0x2ea8980) = head
inline constexpr std::uint32_t kOwnerCtor4A690Tag = 0x7D03;
inline constexpr std::size_t kOwnerMemberRow21 = 21;

// Destructor contract only (report §7): `0x4bde0..0x4bea9`, 0xc9 B; the row teardown is dtor then
// `0x170b0`. Deliberately not modelled.
inline constexpr std::uint64_t kOwnerDtor4BDE0Rva = 0x4BDE0;
inline constexpr std::size_t kOwnerDtor4BDE0Size = 0xC9;

// Callees of `0x4a690`, in call order (see report §3). The last two are modelled natively.
inline constexpr std::uint64_t kOwnerRow21SharedStrRva = 0xF180;
inline constexpr std::uint64_t kOwnerRow21StampRva = 0x511F0;
inline constexpr std::uint64_t kOwnerRow21MutexInitRva = 0x3B310;
inline constexpr std::uint64_t kOwnerRow21AllocRva = 0x17070;
inline constexpr std::uint64_t kOwnerRow21FreeRva = 0x170B0;
inline constexpr std::uint64_t kOwnerRow21CompareRva = 0xF8F500;   // strcmp PLT
inline constexpr std::uint64_t kOwnerRow21InsertRva = 0x4B3F0;     // insert_unique(value)
inline constexpr std::uint64_t kOwnerRow21RebalanceRva = 0x4C500;  // _Insert_new_node_at

// Object offsets.
inline constexpr std::size_t kOwnerRow21HeadOffset = 0x00;     // 16 B zero
inline constexpr std::size_t kOwnerRow21SpareOffset = 0x10;    // qword 0
inline constexpr std::size_t kOwnerRow21Shared0Offset = 0x18;  // 0xf180
inline constexpr std::size_t kOwnerRow21Shared1Offset = 0x38;  // 0xf180
inline constexpr std::size_t kOwnerRow21Stamp0Offset = 0x58;
inline constexpr std::size_t kOwnerRow21Stamp1Offset = 0x60;
inline constexpr std::size_t kOwnerRow21TagOffset = 0x68;    // u32; +0x6c..+0x70 hole
inline constexpr std::size_t kOwnerRow21MutexOffset = 0x70;  // 0x3b310, hook-owned
inline constexpr std::size_t kOwnerRow21TreeOffset = 0x78;   // tree handle
inline constexpr std::size_t kOwnerRow21AnchorOffset = 0x80;  // tree handle + 0x08
inline constexpr std::size_t kOwnerRow21SizeOffset = 0x88;    // tree handle + 0x10

// Tree node layout.
inline constexpr std::size_t kOwnerRow21NodeSize = 0x48;
inline constexpr std::size_t kOwnerRow21KeyOffset = 0x20;
inline constexpr std::size_t kOwnerRow21KeyBytes = 0x20;
inline constexpr std::size_t kOwnerRow21KeyTextOffset = 0x00;     // inside the key
inline constexpr std::size_t kOwnerRow21KeyControlOffset = 0x18;  // inside the key
inline constexpr std::size_t kOwnerRow21MappedOffset = 0x40;
inline constexpr std::uint8_t kOwnerRow21Red = 0;
inline constexpr std::uint8_t kOwnerRow21Black = 1;

// Global queue entry layout (`*(0x2ea8980)`): 0x30 B, owned by the host.
inline constexpr std::size_t kOwnerRow21QueueNodeSize = 0x30;
inline constexpr std::size_t kOwnerRow21QueueValueOffset = 0x08;
inline constexpr std::size_t kOwnerRow21QueueNextOffset = 0x28;

// Guest-layout tree node. Only the fields the constructor and the insert primitive touch are
// named; a fresh node gets `left = parent = right = anchor`, `colour = red`, `is_anchor = 0`,
// the 0x20 B value copy and `mapped = nullptr`.
struct OwnerRow21Node {
    OwnerRow21Node* left;    // +0x00
    OwnerRow21Node* parent;  // +0x08 (anchor: root)
    OwnerRow21Node* right;   // +0x10 (anchor: rightmost)
    std::uint8_t colour;     // +0x18
    std::uint8_t is_anchor;  // +0x19
    std::uint8_t alignment[6];              // +0x1a..0x20, never named by the body
    std::uint8_t key[kOwnerRow21KeyBytes];  // +0x20
    void* mapped;                           // +0x40
};

static_assert(sizeof(OwnerRow21Node) == kOwnerRow21NodeSize);
static_assert(offsetof(OwnerRow21Node, left) == 0x00);
static_assert(offsetof(OwnerRow21Node, parent) == 0x08);
static_assert(offsetof(OwnerRow21Node, right) == 0x10);
static_assert(offsetof(OwnerRow21Node, colour) == 0x18);
static_assert(offsetof(OwnerRow21Node, is_anchor) == 0x19);
static_assert(offsetof(OwnerRow21Node, key) == kOwnerRow21KeyOffset);
static_assert(offsetof(OwnerRow21Node, mapped) == kOwnerRow21MappedOffset);

// Global queue entry. `reserved` is never read by `0x4a690`; `value` is copied verbatim by
// `0x4b3f0` (0x20 B, key text pointer at +0x00 and the refcounted control pointer at +0x18).
struct OwnerRow21QueueNode {
    std::uint8_t reserved[kOwnerRow21QueueValueOffset];  // +0x00
    std::uint8_t value[kOwnerRow21KeyBytes];             // +0x08
    OwnerRow21QueueNode* next;                           // +0x28
};

static_assert(sizeof(OwnerRow21QueueNode) == kOwnerRow21QueueNodeSize);
static_assert(offsetof(OwnerRow21QueueNode, value) == kOwnerRow21QueueValueOffset);
static_assert(offsetof(OwnerRow21QueueNode, next) == kOwnerRow21QueueNextOffset);

struct OwnerRow21Hooks {
    // 0xf8f500: the strcmp PLT. Called as compare(stored, probe) during a descent and
    // compare(probe, stored) for the equivalent-key test, exactly as the body does. Null fails at
    // "compare" on the first comparison the body needs.
    int (*compare)(const char* stored, const char* probe) = nullptr;
    // 0x17070: operator new. Always called with 0x48 (anchor, then one node per inserted entry).
    // Null or a null result fails at "anchor_alloc" / "node_alloc".
    void* (*allocate)(std::size_t size) = nullptr;
    // 0x511f0: unnamed 16 B provider (out param filled by PLT 0xf8fd60, unwound into rax/rdx).
    // Null or false fails at "stamp" before any of +0x58/+0x60/+0x68 is written.
    bool (*stamp)(std::uint64_t& first, std::uint64_t& second) = nullptr;
    // 0x3b310: critical-section/mutex init at `object + 0x70` (import sequence: attr init,
    // attr settype 2, init(object, attr, "CritSec"), attr destroy). Null or false fails at
    // "mutex_init"; the body never writes +0x70..+0x80 itself.
    bool (*mutex_init)(void* at) = nullptr;
    // *(0x2ea8980): the global queue head. Read before the first entry and re-read after every
    // insert (0x4a72d / 0x4a802). Null fails at "queue".
    const OwnerRow21QueueNode* (*queue_head)() = nullptr;
    // *(0x2ea8980) = next: the drain republishes the head after every entry (0x4a809).
    void (*set_queue_head)(const OwnerRow21QueueNode* next) = nullptr;
};

struct OwnerRow21Result {
    bool ok;
    const char* failed_at;  // "null-object", "object-align", "stamp", "mutex_init",
                            // "anchor_alloc", "queue", "compare", "null-key", "node_alloc"
    std::size_t inserted;    // nodes created by this call
    std::size_t duplicates;  // drained entries whose key was already present (first one wins)
};

// Builds the 0x90 B object at `object` and drains `*(0x2ea8980)` into its tree. `object` must be
// writable for 0x90 B and 8-byte aligned. `ok == false` stops at the first failure and keeps the
// already-written prefix (the original would have written it before faulting).
//
// Hook requirements: the anchor allocation (`allocate`) and the queue accessors are needed on
// every call; `compare` only once a comparison is due, i.e. not for an empty queue or a tree that
// is still empty. A missing hook is a named `failed_at`, never a silent success.
[[nodiscard]] OwnerRow21Result owner_ctor_4a690(void* object,
                                                const OwnerRow21Hooks& hooks) noexcept;

// Adapter for `owner_members`' row driver (`bool (*)(row, object, arg)`, E579). That signature
// carries no context, so the host installs one hook table before `init_owner_members` runs; the
// adapter ignores `arg` (row 21's ctor takes no argument) and reports false for any other row.
// Single-threaded startup use only.
void owner_row21_set_hooks(const OwnerRow21Hooks& hooks) noexcept;
[[nodiscard]] bool owner_row21_member_ctor(std::size_t row, void* object, void* arg) noexcept;
// `failed_at` of the most recent adapter call (nullptr when it succeeded), so the driver's
// generic "row N ctor" stop stays diagnosable.
[[nodiscard]] const char* owner_row21_last_failed_at() noexcept;

}  // namespace sotc
