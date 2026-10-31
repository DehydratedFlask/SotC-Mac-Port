// Native recovery of the shared console-command base used by the startup ConCmd constructors
// (original ELF sha256 d0723afb...5861d3). Names are ours; the image has no symbols. "ConCmd" is
// supported only by the assert text `void bpe::ConCmdManager::...` (0xf95c3e / 0xf95c8e) that the
// register/unregister bodies carry; the exact class names are inference.
//
// Lifted bodies (all byte-pinned by tools/lift/check_fctor_slots_298_299_301.py):
//   0x20af0 (90 B)  base constructor  ConCmdBase(this=rdi, name=rsi, description=rdx, flags=ecx)
//   0x20d60 (26 B)  register-once guard: `if (this->registered) return; registered = 1;
//                   jmp 0x20d80(this, *[0x2dfcbc8])`
//   0x20d80 (392 B) register body. Two paths on the manager cell `[0x2dfcbc8]`:
//        null     -> link `this+8` at the TAIL of the global pending list `0x2dfce50/58`
//                    (lazily initialised under the Itanium guard byte `0x2dfce60`)  [NATIVE]
//        non-null -> lock `manager+0x58` (0x3b390), `std::map<const char*, ConCmd*>` insert keyed by
//                    the name pointer under the `strcasecmp`-labeled import (NID-name inference),
//                    unlock (0x3b3c0)
//                    [CONTRACT: hook `ConCmdManagerContract::insert`]
//
// Why the null path is the honest startup path: the manager cell is zero-fill `.bss`. Its only
// non-zero store is `mov [r15], r14` at 0x243f5 inside 0x242c0 (0x149 B), which allocates 0x60 B
// (0x17070), runs the manager constructor 0x210b0 on it and installs it. 0x242c0 is not a
// constructor-table entry and has exactly one caller (0x255119, in the engine boot sequence that
// follows static initialisation). The manager constructor 0x210b0 (0x111 B) then DRAINS the pending
// list FIFO by calling 0x20d80(node-8, manager) for each queued command. The only other write is
// constructor-table index 567 (`0x235d0`), which stores 0 (`mov qword [r14],0`) before registering
// the `executeCmd` command, and the teardown 0x240f0 stores 0 at 0x241e1. The census of the 28
// image references to the cell (CPU-01-CONCMD-298-299-301-01) finds only `lea` forms, and the
// stores through those registers are exactly these three (two zero, one install). Hence every ConCmd constructor at
// indices 0..566 sees a null manager and queues itself. Nothing here creates, owns or drains a
// manager.
//
// NOT modelled: the unregister/destructor chain (0x20b50 base, 0x20b0 / 0xb0cb40 / 0x20f0 /
// 0x272d0 class destructors). They are registered with `__cxa_atexit` and are recorded in the
// StartupAtexitLog; they are pinned, never executed (same convention as slot 297's 0xb24be0).
// Vtable pointers are IDENTITY TAGS (the original vtable address point); no native code dispatches
// through them.
#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "hashed_string_14c3a0.hpp"
#include "shared_release_f590.hpp"

namespace sotc {

inline constexpr std::uint64_t kConCmdBaseCtorRva = 0x20af0;
inline constexpr std::size_t kConCmdBaseCtorSize = 0x5a;
inline constexpr std::uint64_t kConCmdRegisterRva = 0x20d60;
inline constexpr std::size_t kConCmdRegisterSize = 0x1a;
inline constexpr std::uint64_t kConCmdRegisterBodyRva = 0x20d80;
inline constexpr std::size_t kConCmdRegisterBodySize = 0x188;
inline constexpr std::uint64_t kConCmdBaseDtorRva = 0x20b50;       // pinned, not executed
inline constexpr std::size_t kConCmdBaseDtorSize = 0x155;
inline constexpr std::uint64_t kConCmdManagerCellRva = 0x2dfcbc8;  // `ConCmdManager*`, null until the engine boot installer 0x242c0
inline constexpr std::uint64_t kConCmdPendingHeadRva = 0x2dfce50;  // {next, prev}
inline constexpr std::uint64_t kConCmdPendingGuardRva = 0x2dfce60; // Itanium guard byte
inline constexpr std::uint64_t kConCmdManagerZeroIndex = 567;      // 0x235d0 stores 0 into the cell
inline constexpr std::uint64_t kConCmdManagerCtorRva = 0x210b0;    // builds the manager, drains pending
inline constexpr std::uint64_t kConCmdManagerInstallRva = 0x243f5; // the only non-zero store (in 0x242c0)
inline constexpr std::uint64_t kConCmdManagerInstallerRva = 0x242c0;
inline constexpr std::uint64_t kConCmdBaseVptr = 0x1594d90;        // 0x1594d80 + 0x10, identity tag
inline constexpr std::uint64_t kStartupAtexitRva = 0xf8f390;       // __cxa_atexit PLT
inline constexpr std::uint64_t kStartupDsoRva = 0x1604218;

// `lock inc qword [ctl]` on a SharedStr control counter, exactly as 0x20af0 does for each view.
void concmd_acquire_view(const SharedStr& view) noexcept;

// Intrusive list link. The original embeds it at `ConCmdBase + 8` (next) / `+ 0x10` (prev); the list
// head is the same two words at `0x2dfce50` / `0x2dfce58`.
struct ConCmdListNode {
    ConCmdListNode* next;   // +0x00
    ConCmdListNode* prev;   // +0x08
};
static_assert(sizeof(ConCmdListNode) == 0x10);

// The 0x60-byte record built by 0x20af0.
struct ConCmdBase {
    std::uint64_t vptr;          // +0x00  class identity tag (original vtable address point)
    ConCmdListNode pending;      // +0x08  pending-list link (next @+8, prev @+0x10)
    std::uint8_t registered;     // +0x18  register-once flag
    std::uint8_t gap19[3];       // +0x19  never touched
    std::uint32_t flags;         // +0x1c  ecx of the constructor (1 at the startup variables, 2 at resource_dump)
    SharedStr name;              // +0x20  lookup key: manager comparator import is labeled strcasecmp (inference)
    SharedStr description;       // +0x40  second view (empty at every observed site; role inferred)
};
static_assert(sizeof(ConCmdBase) == 0x60);
static_assert(offsetof(ConCmdBase, pending) == 0x08 && offsetof(ConCmdBase, registered) == 0x18 &&
              offsetof(ConCmdBase, flags) == 0x1c && offsetof(ConCmdBase, name) == 0x20 &&
              offsetof(ConCmdBase, description) == 0x40);

// 0x20af0: vptr = 0x1594d90; registered = 0; flags = ecx; name/description copied 32 bytes each
// with `lock inc qword [ctl]` when the view carries a control counter. Bytes +0x08..+0x17 and
// +0x19..+0x1b are NOT written (the list link is only meaningful once queued).
void init_concmd_base_20af0(ConCmdBase& cmd, const SharedStr& name, const SharedStr& description,
                            std::uint32_t flags) noexcept;

// `__cxa_atexit(dtor, object, &__dso_handle)` observations. The destructors are pinned, not run.
struct StartupAtexitEntry {
    std::uint64_t dtor_rva;
    const void* object;
};

class StartupAtexitLog {
public:
    // Records one registration. False only when the host cannot allocate (visible to the caller).
    [[nodiscard]] bool record(std::uint64_t dtor_rva, const void* object) noexcept;
    [[nodiscard]] std::size_t size() const noexcept { return entries_.size(); }
    [[nodiscard]] const StartupAtexitEntry& at(std::size_t i) const noexcept { return entries_[i]; }

private:
    std::vector<StartupAtexitEntry> entries_;
};

// Global pending list `0x2dfce50` (head) with its Itanium guard byte `0x2dfce60`.
struct ConCmdPendingList {
    ConCmdListNode head{nullptr, nullptr};   // 0x2dfce50: first (next), 0x2dfce58: last (prev)
    bool guard_done = false;                 // 0x2dfce60 != 0 after the first lazy initialisation

    [[nodiscard]] std::size_t size() const noexcept;                 // walks head.next
    [[nodiscard]] const ConCmdListNode* first() const noexcept { return head.next; }
};

// Contract for the non-null manager path of 0x20d80. There is no default insert.
struct ConCmdManagerContract {
    void* manager = nullptr;   // `[0x2dfcbc8]`; null on the whole startup path (see header)
    // Under the manager mutex: `map[cmd.name.data] = &cmd`. Null with a non-null manager is a
    // visible failure, never a silent success.
    bool (*insert)(void* manager, ConCmdBase& cmd) noexcept = nullptr;
};

// Everything the startup ConCmd constructors share: the pending list, the manager cell and the
// atexit log. Owned by StartupState.
struct ConCmdRegistry {
    ConCmdPendingList pending{};
    ConCmdManagerContract manager{};
    StartupAtexitLog atexit_log{};
};

enum class ConCmdRegisterPath : std::uint8_t {
    already_registered,   // 0x20d60: `cmp byte [rdi+0x18],0; jne ret`
    pending_list,         // 0x20e7f..0x20edf: queued at the tail of the global pending list
    manager,              // 0x20dae..0x20e76: manager insert (hook)
};

struct ConCmdRegisterResult {
    bool ok = false;
    ConCmdRegisterPath path = ConCmdRegisterPath::already_registered;
    bool list_initialised = false;   // this call ran the guarded `head = {&head, &head}` init
};

// 0x20d60 / 0x20d80.
[[nodiscard]] ConCmdRegisterResult register_concmd_20d60(ConCmdBase& cmd,
                                                         ConCmdRegistry& registry) noexcept;

// `call 0xf590` on a stack view built by the constructors: the temporary never carries a control
// counter, so the lifted release is an exact no-op. Returns false if a counter were present (the
// native model would then be wrong about the original).
[[nodiscard]] bool release_inert_view_f590(const SharedStr& view) noexcept;

}  // namespace sotc
