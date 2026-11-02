// CPU-01-CONCMD-MANAGER-210B0-01 — native `0x210b0` = the ConCmd manager constructor + pending drain.
//
// Identity is behavioural, not symbolic: `0x210b0` carries no assert string of its own, but it is the
// only callee of the only non-zero store to `[0x2dfcbc8]` — `0x243f5` inside `0x242c0`, which
// allocates 0x60 B (`0x243a1`), calls `0x210b0` on it (`0x243b1`) and installs it — and the 0x60-byte
// object it builds is exactly the one the two already-native bodies read: `0x21620`
// (`bpe::ConCmdManager::FindConCmd`) reads the header end at `+0x08`, the root at `+0x18` and the
// CritSec handle at `+0x58`; `0x20d80` (`bpe::ConCmdManager::Register`) reads the root at `+0x18`,
// the size cell at `+0x28` and locks `+0x58`; the teardown `0x240f0` destroys `+0x58` (`0x241ba`),
// frees `[+0x38]` (`0x241bf`) and destroys the tree at `+0x18` (`0x241cd`). `concmd_base.hpp` already
// records the class-name inference (`0xf95c3e` / `0xf95c8e`) and the installer.
//
// ============================== OBSERVED (pinned bytes; tools/lift/check_concmd_manager_210b0.py) ===
// Body `0x210b0..0x211c1` = 273 B, FDE `(0x210b0, 273)`, sha256 619fba45…17ff40. 273 instructions in
// one Ghidra block (the loop is a `do/while` with a short back edge), 4 branches, 4 relative calls to
// 4 distinct targets, no `e9`, no indirect transfer. The body never locks or unlocks: `0x3b390` /
// `0x3b3c0` do not appear in it. The calls are, in address order:
//
//   0x210f2  call 0x3b310   CritSec init on `manager+0x58` (retail discards the return code)
//   0x21108  call 0xf8f3d0  __cxa_guard_acquire(&0x2dfce60)
//   0x21131  call 0xf8f3e0  __cxa_guard_release(&0x2dfce60)
//   0x21166  call 0x20d80   Register(cmd, manager) — the register *body*, never 0x20d60's guard
//
// 1) `0x210ba..0x210f2` — eight stores and nothing else. `new(0x60)` memory is NOT otherwise touched:
//
//      +0x08 = manager + 8      (0x210c9)      +0x18 = 0             (0x210d1, the lookup's root)
//      +0x10 = manager + 8      (0x210cd)      +0x20..+0x2f = 0      (0x210d9, vmovdqu)
//      +0x20 = 0 (byte)         (0x210df)      +0x38..+0x47 = 0      (0x210e4, vmovdqu)
//      +0x48 = 0                (0x210ea)
//
//    The byte store at `+0x20` is redundant with the 16-byte span that precedes it: two adjacent
//    zero-initialisations were merged into the vector store and one of them is a one-byte field.
//    `+0x00..+0x07`, `+0x30..+0x37` and `+0x50..+0x57` are NOT written here. `+0x08`/`+0x10` hold
//    `manager+8`, which is why `0x21695` calls `manager+8` the collection end; `+0x28` (inside the
//    zeroed span) is the size cell `0x212c0` tests; `+0x38` is the pointer `0x241bf` frees.
//
// 2) `0x210f7..0x21135` — the guarded function-local static `0x2dfce50` / guard `0x2dfce60`, byte-for-
//    byte the same 63-byte construct as the register body's (`0x20e7f..0x20ebd`) and the lookup's
//    (`0x2171b..0x21759`): test the guard byte, `__cxa_guard_acquire`, and — only when acquire
//    returns 1 — `head = {&head, &head}` then release. All three blocks differ in their seven
//    RIP-relative displacements and in nothing else. This is exactly `ensure_concmd_pending_list`
//    (concmd_base), which the model therefore shares rather than duplicating.
//
// 3) `0x21136..0x2117d` — the drain, FIFO, straight off the head's forward chain:
//
//      0x21136  rax = [0x2dfce50]                  link = head.next
//      0x21144  rbx = rax - 8                      element = link - 8   (0x2114b cmove 0 on null)
//      0x2113d  r15 = 0x2dfce48                    the sentinel element  (&head - 8)
//      0x2114f  if (element == sentinel) skip
//      0x21160  Register(element, manager)         0x21166, rdi = element, rsi = manager
//      0x2116b  rax = [element + 8]                link = element->next
//      0x2116f  rbx = rax - 8; 0x21176 cmove 0 on null; 0x2117a back edge while != sentinel
//
//    It calls the register **body** `0x20d80`, not the guard `0x20d60`. That distinction is load-
//    bearing: `0x20d60` sets `+0x18 = 1` and then tail-jumps to `0x20d80`, so every queued command
//    already carries `registered == 1`; a drain through `0x20d60` would therefore return at its
//    `cmpb [rdi+0x18],0` guard and insert nothing, while `0x210b0` inserts every queued command.
//    The register body does not unlink the node it registers, so the list is intact for the whole
//    walk and every element is visited exactly once, in queue order.
//
// 4) `0x2117f..0x211af` — the re-termination, which is what re-empties the list:
//
//      0x2117f  rax = [0x2dfce50]                  head.next, re-read after the drain
//      0x21186  rcx = rax - 8                       first element (0x21194 cmove 0 on null)
//      0x21198  rax = [rcx + 0x10]                  first->prev   -> the predecessor *link*
//      0x2119c  rcx = rax - 8                       tail element  (0x211a3 cmove 0 on null)
//      0x211a7  [rcx + 8] = &head                   tail->next = &head
//      0x211ab  rcx += 8
//      0x211af  [0x2dfce58] = rcx                   head.prev = tail link
//
//    `0x20d80`'s pending path writes `cmd->prev = old head.prev` and `head.prev = &cmd->pending`, so
//    for any list built by that body the first element's `prev` is the head *link* `0x2dfce50`, the
//    predecessor element is the sentinel `&head - 8`, and the two stores land on `head.next` and
//    `head.prev` themselves: the object ends with `head = {&head, &head}`, i.e. empty. The nodes keep
//    their own links (the body never unlinks them) but the head no longer reaches them, so a second
//    drain visits nothing. The general form is modelled as written, not specialised to the empty
//    result: a list whose first element's `prev` points at the last node re-terminates without
//    emptying, which is a byte consequence of this code and cannot occur on the 0x20d80 path.
//
// ============================== HOST MODEL =====================================================
// `construct_concmd_manager_210b0` writes the eight stores above, initialises the CritSec through the
// reviewed `critsec_3b310` model, runs the shared guarded pending-list init, walks the caller's list
// and calls the native register body `register_concmd_body_20d80` for each element. It reads and
// writes caller memory at the pinned byte offsets only; it allocates nothing, owns nothing and copies
// no command. The manager object is a 0x60-byte block supplied by the caller (`new(0x60)` in retail,
// `ConCmdManagerStorage` for a host).
//
// Because the drain must reach the register *body*, `concmd_base` now exposes
// `register_concmd_body_20d80` and `register_concmd_20d60` is its guard plus that body. The observable
// behaviour of `register_concmd_20d60` is unchanged (same paths, same results); the split only makes
// the body callable the way `0x210b0` calls it.
//
// The non-null manager path of the register body needs `ConCmdManagerContract::insert`, which still
// has no default: with a queue and no `insert`, every drained command records a failure and the
// constructor reports `ok == false`. Nothing here invents a map, a node or a registry entry.
//
// HOST-ONLY ADDITIONS (never retail error semantics; the retail body has no failure branch):
//   * a null manager block, a null head link, a null queued link and a null predecessor link fail
//     visibly instead of faulting (retail would write or read through a null pointer);
//   * a non-zero CritSec init code is reported (`mutex_error`) and makes `ok` false while the drain
//     still runs, because retail continues;
//   * `list_initialised`, `drained`, `register_failures`, `list_emptied` and the `fail` enum are
//     host observations, not retail state.
//
// NOT CLAIMED: what the container at `+0x08..+0x2f` is (a red-black map header is consistent with
// every observable offset and with `0x211d0`/`0x22b20`, but the container type is not proved); what
// `+0x30..+0x37` and `+0x50..+0x57` hold (this body does not write them, and whether the allocator at
// `0x17070` zero-fills is not established); what `+0x38` points to; any run-time behaviour, engine
// readiness, or whether the manager exists at any point during a real boot.
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "critsec_3b310.hpp"

namespace sotc {

inline constexpr std::uint64_t kConCmdManager210b0Rva = 0x210b0;  // the installer 0x243b1 calls it
inline constexpr std::size_t kConCmdManager210b0Bytes = 273;      // FDE length, 0x210b0..0x211c1
inline constexpr char kConCmdManager210b0Sha256[] =
    "619fba45d65cab01645eb1f8f8015c7585aeec31d4afe7f3b35ff2422e17ff40";

// The manager object: 0x60 B, `new(0x60)` at 0x243a1.
inline constexpr std::size_t kConCmdManager210b0StorageBytes = 0x60;
inline constexpr std::size_t kConCmdManager210b0SelfLinkAOffset = 0x08;  // qword = manager + 8
inline constexpr std::size_t kConCmdManager210b0SelfLinkBOffset = 0x10;  // qword = manager + 8
inline constexpr std::size_t kConCmdManager210b0RootOffset = 0x18;       // qword 0; the tree root
inline constexpr std::size_t kConCmdManager210b0ZeroSpanAOffset = 0x20;  // 16 B zero
inline constexpr std::size_t kConCmdManager210b0ZeroByteOffset = 0x20;   // one more byte store
inline constexpr std::size_t kConCmdManager210b0SizeOffset = 0x28;       // inside span A (0x212c0)
inline constexpr std::size_t kConCmdManager210b0ZeroSpanBOffset = 0x38;  // 16 B zero
inline constexpr std::size_t kConCmdManager210b0OwnedPointerOffset = 0x38;  // freed by 0x241bf
inline constexpr std::size_t kConCmdManager210b0ZeroQwordOffset = 0x48;  // qword 0
inline constexpr std::size_t kConCmdManager210b0MutexOffset = 0x58;      // 8-byte CritSec handle
inline constexpr std::size_t kConCmdManager210b0ZeroSpanBytes = 16;
// The sentinel "element" the drain compares against is `&head - 8`: `ConCmdBase::pending` sits at
// +0x08, so `element = link - 8` and the head — which is a bare link pair at 0x2dfce50 — plays the
// role of the element at 0x2dfce48. Consequently sentinel+0x08 IS head.next and sentinel+0x10 IS
// head.prev; the model expresses those two accesses as head reads/writes instead of an out-of-bounds
// pointer (host-only, semantically identical).
inline constexpr std::ptrdiff_t kConCmdManager210b0SentinelDelta = -8;

// Guard-block structure shared by 0x210f7, 0x20e7f and 0x2171b: 63 B that differ only in seven
// RIP-relative displacements. Pinned by the checker, not used at run time.
inline constexpr std::size_t kConCmdManager210b0GuardBlockBytes = 63;

// A caller-owned 0x60-byte block for the manager. The constructor writes only the pinned bytes; the
// value-initialisation here zeroes the whole block because a host wants determinism, and the fixture
// proves separately which bytes the constructor itself leaves alone.
struct alignas(8) ConCmdManagerStorage {
    std::uint8_t bytes[kConCmdManager210b0StorageBytes]{};

    [[nodiscard]] void* address() noexcept { return bytes; }
    [[nodiscard]] const void* address() const noexcept { return bytes; }
};

enum class ConCmdManagerCtorFail : std::uint8_t {
    none,
    null_manager,        // host guard: no 0x60-byte block was supplied
    mutex_init,          // 0x210f2 returned non-zero (retail discards it)
    null_head_link,      // host guard: [0x2dfce50] was null; 0x21136/0x2117f would fault
    null_drained_link,   // host guard: a queued element's `next` link was null (0x2116b)
    null_tail_link,      // host guard: [0x21198]+0x10 was null; 0x211a7 would store at +8
    register_refused,    // 0x20d80 reported ok == false (missing insert contract, see header)
};

struct ConCmdManagerCtorResult {
    bool ok = false;                   // no host guard trip, mutex 0, every register call ok
    bool list_initialised = false;     // this call ran the guarded `head = {&head, &head}` init
    bool list_emptied = false;         // observation: head == {&head, &head} after the re-termination
    std::size_t drained = 0;           // register-body calls made by the 0x21160 loop
    std::size_t register_failures = 0; // of those, how many reported ok == false
    int mutex_error = 0;               // 0x210f2's return code, 0 when the handle was built
    ConCmdManagerCtorFail fail = ConCmdManagerCtorFail::none;
};

// Optional overrides. Null members select the native implementations, so an empty hook set is
// complete except for the register body's own `insert` contract, which has no default.
struct ConCmdManagerCtorHooks {
    void* host = nullptr;
    int (*critsec_init)(void* host, void* handle8) noexcept = nullptr;  // 0x3b310
    // 0x20d80. The default is `register_concmd_body_20d80`, i.e. the native body *without* the
    // 0x20d60 once-flag guard.
    ConCmdRegisterResult (*register_body)(void* host, ConCmdBase& cmd,
                                          ConCmdRegistry& registry) noexcept = nullptr;
};

// 0x210b0. `manager` is the caller's 0x60-byte object; `registry` supplies the pending list and the
// register contract. Never locks the manager mutex (the body does not) and never allocates.
[[nodiscard]] ConCmdManagerCtorResult construct_concmd_manager_210b0(
    void* manager, ConCmdRegistry& registry, const ConCmdManagerCtorHooks& hooks = {}) noexcept;

// Reading the object back for hosts and fixtures: the value of the qword at `offset` (pinned
// offsets only) and a convenience predicate for the empty-list state the re-termination produces.
[[nodiscard]] std::uint64_t concmd_manager_qword(const void* manager, std::size_t offset) noexcept;

}  // namespace sotc
