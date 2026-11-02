// CPU-01-INIT-21620-LOOKUP-01 — native `0x21620` = `bpe::ConCmdManager::FindConCmd`.
//
// Identity is not inferred from layout: the body's own assert metadata names it.
//   0xf95d1f  "static bpe::IConBase *bpe::ConCmdManager::FindConCmd(const bpe::String &)"
//   0xf95c06  "d:\\dev\\cp10\\Main\\Code\\BPE\\Source\\Core\\System\\ConVar.cpp"   (0x2165d/0x2173c)
//   0xf95c3e  "static void bpe::ConCmdManager::Register(bpe::IConBase *, bpe::ConCmdManager *)"
//             (0x20d80, the writer of both collections this function reads)
//   0xf95d69  "bpe::ECmdExecuteResult bpe::ConCmdManager::ExecuteCmd(const bpe::Command &)"
//             (0x217f0, the next FDE — so 0x21620 ends here)
// The return value is therefore an `IConBase*`; the argument is a `bpe::String`, i.e. the 0x20-byte
// SharedStr already modelled in hashed_string_14c3a0.hpp {data@0, size@8, aux@0x10, ctl@0x18}; and the
// two collections are the ConCmd registries already modelled in concmd_base.{hpp,cpp}
// (manager cell `[0x2dfcbc8]`, pending list head `0x2dfce50`, guard byte `0x2dfce60`).
//
// ============================== OBSERVED (pinned bytes; see tools/lift/check_lookup_21620.py) ======
// Body `0x21620..0x217f0`: 464 B pinned by the spec (`0x1d0`, sha256 a7ba043b…0413bd), of which the
// FDE is 463 B `[0x21620,0x217ef)`; `0x217ef` is the alignment nop that the 464-byte pin also covers.
// Two paths, selected by `[0x2dfcbc8]` (0x2164a / 0x21650):
//
//   manager != null  ->  0x21656..0x21716 (locked map lookup, "primary")
//       0x21689  0x3b390(manager+0x58, &{file,func,line 0x9b})   = scePthreadMutexLock
//       0x21691  root = [manager+0x18]; 0x21695 end = manager+0x8
//       0x216b3  strcasecmp(node+0x20 /*name*/, key->data)
//       0x216c1  cmp >= 0 -> candidate = node, descend node+0x08 ("right")
//       0x216c3  cmp <  0 -> descend node+0x00 ("left"); a null left ends with the candidate
//       0x216e5  strcasecmp(key->data, candidate+0x20), 0x216f3 `js` -> miss (so equality only)
//       0x216fa  result = [candidate+0x40]      (the map value: an IConBase*)
//       0x21701 / 0x2170e  0x3b3c0(manager+0x58) = scePthreadMutexUnlock
//     The key *length* is never read on this path, and the tree is ordered so that a
//     smaller name lies in the greater name's right subtree — the same polarity the writer
//     0x20d80/0x211d0 uses, so the search is self-consistent, not ascending.
//
//   manager == null  ->  0x2171b..0x217bf (linear scan of the pending list, "fallback")
//       0x2171b  if ([0x2dfce60] == 0) { 0x2172c __cxa_guard_acquire(0x2dfce60);
//                0x21735 head = {0x2dfce50, 0x2dfce50} (self-links); 0x21755 guard_release }
//       0x2175a  link = [0x2dfce50]; element = link - 8; end = 0x2dfce48
//       0x21790  element->+0x28 == key->size   (the second key qword, exact)
//       0x2179d  strcasecmp(element->+0x20, key->data) == 0
//       0x217bc  result = element                (the ConCmdBase itself, not a +0x40 value)
//       0x217aa  next = element->+0x08; element = next - 8; stop when element == end
//     No lock is taken on this path; the guarded static is its only synchronisation.
//
//   Both paths return a borrowed pointer that the retail callers dereference (vtable +0x18/+0x30)
//   only *after* the unlock. This model reproduces that ordering and adds no ownership.
//
// The second key qword is the name length, byte-verified at BOTH call sites:
//   0x53327d  stack key {data=0x1014e56 "r_fps", size=5, 0, 0}   (0x53320e/0x53323b/0x533257)
//   0x91227   key = this+8 = 0x2efe320 {data=0x1014ca3 "shadowDemoMode", size=14, aux=0, ctl=0}
//             (static initialiser 0x53899d) — i.e. the `bpe::String` member of a ConCmdBase-like
//             object whose bound slot is +0x28 = 0x2efe340.
//
// ============================== HOST MODEL =====================================================
// The model is byte-faithful over caller-supplied collections; it never invents registry content.
//   * `ConCmdRegistry` is the host registry the register path already uses (concmd_base.hpp). Its
//     `manager.manager` is `[0x2dfcbc8]`; its `pending.head` stands in for `0x2dfce50`.
//   * The manager object is read, never constructed, here: root at +0x18, map end at +0x08, and the
//     8-byte CritSec handle at +0x58 used through the reviewed critsec_3b310 model.
//   * Map nodes are read at the pinned offsets (left +0x00, right +0x08, key SharedStr +0x20 with the
//     name at +0x20 and the size at +0x28, value +0x40, node size 0x48 as allocated by 0x211d0).
//   * The comparator is `strcasecmp` (import 0xf8f500, NID AV6ipCNa4Rw, libc). Only its sign and its
//     zero-ness are observable, so `concmd_strcasecmp` pins exactly those.
//
// HOST-ONLY ADDITIONS (never retail error semantics; the retail body has no failure branch):
//   * a lock/unlock error fails closed (the retail body discards both return values);
//   * a null key, a null node/entry name or a null list link reports `failed` instead of faulting;
//   * `install_concmd_registry` / `uninstall_concmd_registry` are scaffolding for the vslot hook slot,
//     which has no host parameter. They are not retail state.
//
// NOT CLAIMED: whether `FindConCmd` returns non-null for "shadowDemoMode", "r_fps" or any other name
// in a live engine; whether the manager exists at any point in a real boot; engine readiness.
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "critsec_3b310.hpp"
#include "hashed_string_14c3a0.hpp"

namespace sotc {

inline constexpr std::uint64_t kLookup21620Rva = 0x21620;
// Hot span through `ret` at 0x217e0; the checker also pins the reachable cold block and the tail.
inline constexpr std::size_t kLookup21620HotSize = 449;
// Spec pin: the whole `0x21620..0x217f0` range including the cold block 0x217e1 and the stack-guard
// tail 0x217e9 (and the trailing nop at 0x217ef).
inline constexpr std::size_t kLookup21620BodySize = 0x1d0;
inline constexpr char kLookup21620HotSha256[] =
    "787bf4ada236589505be9d753be8317445d253e8f43a2db55f102b0cf77b5185";
inline constexpr char kLookup21620BodySha256[] =
    "a7ba043b102574823a8381f36c654b1bf30f6ef812b64e209e9b438d670413bd";
// Unwind range of the body: 0x21620 + 463 = 0x217ef; the next FDE starts at 0x217f0 (ExecuteCmd).
inline constexpr std::size_t kLookup21620FdeLength = 463;

// Key: the 0x20-byte bpe::String / SharedStr. Only data (+0x00) and size (+0x08) are read here.
inline constexpr std::size_t kLookup21620KeyDataOffset = 0x00;
inline constexpr std::size_t kLookup21620KeySizeOffset = 0x08;

// Manager object (`0x60` B, built by 0x210b0, allocated at 0x243a1).
inline constexpr std::size_t kLookup21620ManagerBytes = 0x60;
inline constexpr std::size_t kLookup21620ManagerEndOffset = 0x08;   // compared against, never read
inline constexpr std::size_t kLookup21620ManagerRootOffset = 0x18;  // map/set root node pointer
inline constexpr std::size_t kLookup21620ManagerMutexOffset = 0x58; // 8-byte CritSec handle

// Map node (0x48 B; 0x211d0 allocates it and returns &node+0x40 as the value slot).
inline constexpr std::size_t kLookup21620NodeBytes = 0x48;
inline constexpr std::size_t kLookup21620NodeLeftOffset = 0x00;
inline constexpr std::size_t kLookup21620NodeRightOffset = 0x08;
inline constexpr std::size_t kLookup21620NodeKeyOffset = 0x20;  // copied SharedStr: data, size, aux, ctl
inline constexpr std::size_t kLookup21620NodeValueOffset = 0x40;

// Pending (fallback) list: the sentinel "element" is the head node minus its 8-byte vptr slot, the
// link fields are `pending` (+0x08 next / +0x10 prev) and the key sits at +0x20 exactly as on a map
// node, because ConCmdBase::name is at +0x20 too.
inline constexpr std::uint64_t kLookup21620PendingHeadRva = 0x2dfce50;
inline constexpr std::uint64_t kLookup21620PendingSentinelRva = 0x2dfce48;
inline constexpr std::uint64_t kLookup21620PendingGuardRva = 0x2dfce60;
inline constexpr std::size_t kLookup21620PendingNameOffset = 0x20;
inline constexpr std::size_t kLookup21620PendingSizeOffset = 0x28;
inline constexpr std::size_t kLookup21620PendingNextOffset = 0x08;

// Callees (all byte-pinned in the checker).
inline constexpr std::uint64_t kLookup21620LockBeginRva = 0x3b390;   // jmp scePthreadMutexLock
inline constexpr std::uint64_t kLookup21620LockEndRva = 0x3b3c0;     // jmp scePthreadMutexUnlock
inline constexpr std::uint64_t kLookup21620CompareRva = 0xf8f500;    // strcasecmp (libc)
inline constexpr std::uint64_t kLookup21620GuardAcquireRva = 0xf8f3d0;
inline constexpr std::uint64_t kLookup21620GuardReleaseRva = 0xf8f3e0;
inline constexpr std::uint64_t kLookup21620StackChkFailRva = 0xf8f3a0;

// `strcasecmp` shape used by both paths: ASCII case folding, only the sign and zero-ness are pinned.
[[nodiscard]] int concmd_strcasecmp(const char* lhs, const char* rhs) noexcept;

enum class FindConCmdPath : std::uint8_t {
    none,          // no path ran (failed before the branch)
    manager_map,   // 0x21656..0x21716, locked
    pending_list,  // 0x2171b..0x217bf, unlocked
};

enum class FindConCmdStatus : std::uint8_t {
    hit,
    miss,
    failed,        // host-only: missing/corrupt dependency, or a lock that could not be taken
};

struct FindConCmdResult {
    FindConCmdStatus status = FindConCmdStatus::failed;
    FindConCmdPath path = FindConCmdPath::none;
    ConCmdBase* cmd = nullptr;      // borrowed; never owned, never refcounted
    bool locked = false;            // the manager mutex was taken by this call
    bool list_initialised = false;  // this call ran the guarded `head = {&head, &head}` init
    int lock_error = 0;             // errno when status == failed because the lock was refused
};

// Optional overrides. Null members select the native defaults (concmd_strcasecmp and the reviewed
// critsec_3b310 lock/unlock on the manager's own 8-byte handle), so an empty hook set is complete.
struct FindConCmdHooks {
    void* host = nullptr;
    int (*compare)(void* host, const char* lhs, const char* rhs) = nullptr;  // 0xf8f500
    int (*lock)(void* host, void* handle8) = nullptr;                        // 0x3b390
    int (*unlock)(void* host, void* handle8) = nullptr;                      // 0x3b3c0
};

// 0x21620. Reads `name` (a bpe::String view) out of `registry`; never mutates either collection
// except for the guarded pending-list initialisation that the original also performs.
[[nodiscard]] FindConCmdResult find_concmd_21620(const SharedStr& name, ConCmdRegistry& registry,
                                                 const FindConCmdHooks& hooks = {}) noexcept;

// Context for the `LazyBind91210Hooks::lookup_21620` slot (0x91227), whose `host` is opaque.
struct FindConCmdContext {
    ConCmdRegistry* registry = nullptr;
    FindConCmdHooks hooks{};
};

// Adapter with the exact `void*(void* host, void* key)` shape of that slot: `host` is a
// FindConCmdContext*, `key` points at the caller's 0x20-byte SharedStr (0x91227 passes `this+8`).
// Returns the raw pointer (null for miss or failure), so it also stays usable from the
// `Vslot40_533100MissHooks::find_21620` slot via the process-wide install below.
[[nodiscard]] void* find_concmd_descriptor_hosted(void* host, void* key_descriptor) noexcept;

// ---- process-wide adapter for the vslot-miss slot (0x53327d), which has no host parameter --------
// The retail counterpart is process-wide state too (`[0x2dfcbc8]` / `0x2dfce50`), so a single
// installed context is the honest shape. Host scaffolding only: install once, before lookups.
[[nodiscard]] bool install_concmd_registry(ConCmdRegistry& registry,
                                           const FindConCmdHooks& hooks = {}) noexcept;
void uninstall_concmd_registry() noexcept;
[[nodiscard]] bool concmd_registry_installed() noexcept;

// `void*(void* key)` adapter over the installed context. Null when nothing is installed (fail
// closed) or on miss/failure.
[[nodiscard]] void* find_concmd_descriptor(void* key_descriptor) noexcept;

}  // namespace sotc
