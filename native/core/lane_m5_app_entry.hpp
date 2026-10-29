// CPU-01-STARTUP-SLOTS-M5 (lane M5): shared model of the "named delegate entry" shape that startup constructor
// slots 492 (0x278640) and 543 (0xfae50) both build. Original ELF sha256 d0723afb...5861d3. Every body fact below
// was read from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_492.py / _543.py. NOTHING here
// executes, loads or calls an original RVA: the handler address, the invoker 0x4cca0 and the entry destructor
// 0x4b660 are IDENTITY TAGS (recorded, never run).
//
// Entry layout (0x50 B, a global in .bss):
//   +0x00 registered byte (written 0 by the constructor, set to 1 by the queue insert 0x4b560)
//   +0x01..+0x07 never written
//   +0x08 SharedStr name (0x20 B: {data, size, aux, ctl}; the stack view built by 0xf2f0 is copied, with
//         `lock inc [ctl]` when ctl != 0 -- never at these sites, the view is borrowed)
//   +0x28 next pointer (zeroed; 0x4b560 overwrites it when the entry is pushed on the queue)
//   +0x30 delegate {self, invoker, adj, handler}: self = &entry+0x30 (stored only when the handler word is
//         non-zero), invoker = the relocated cell 0x15fe800 (= 0x4cca0: mov rax,rdi; mov rdi,rsi; jmp [rax+0x18]),
//         adj = 0, handler = the identity tag.
// Sequence: 0xf2f0 builds the name view; entry stores; call 0x4b560(entry) (register-once + queue); 0xf590 on the
// stack view (inert); __cxa_atexit(0x4b660, entry, &dso). 0x4b660 is `add rdi,8; jmp 0xf590` (releases +0x08).
//
// 0x4b560 (233 B) is modelled for the path the startup sequence takes: `cmp byte [entry],0; jne ret`, then
// `[entry] = 1`, then it reads the singleton cell [0x2ea8978]. NULL singleton (OBSERVATION of the code; that the
// cell is still null while static constructors run is INFERENCE: its only store is the row-21 owner constructor
// 0x4a690, which drains this same queue later): `entry+0x28 = [0x2ea8980]; [0x2ea8980] = entry` (push front).
// NON-NULL singleton: 0x4b560 takes a tree lookup / insert_unique branch that is NOT modelled; the model sets the
// registered byte exactly as the original does and then reports ok == false (fail closed, never a silent success).
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "fconcmd_object_slot306.hpp"
#include "hashed_string_14c3a0.hpp"

namespace sotc::lane_m5 {

inline constexpr std::uint64_t kAppInfoSingletonCellRva = 0x2ea8978;
inline constexpr std::uint64_t kAppInfoQueueHeadCellRva = 0x2ea8980;
inline constexpr std::uint64_t kEntryRegisterRva = 0x4b560;
inline constexpr std::size_t kEntryRegisterSize = 233;
inline constexpr std::uint64_t kEntryDtorRva = 0x4b660;
inline constexpr std::uint64_t kDelegateInvokerRva = 0x4cca0;
inline constexpr std::uint64_t kDelegateInvokerCellRva = 0x15fe800;
inline constexpr std::size_t kEntrySize = 0x50;

struct AppInfoQueueEntry {
    std::uint8_t registered;         // +0x00
    std::uint8_t gap01[7];           // +0x01 never written
    SharedStr name;                  // +0x08
    AppInfoQueueEntry* next;         // +0x28
    FConCmdDelegate delegate;        // +0x30
};
static_assert(sizeof(AppInfoQueueEntry) == kEntrySize && offsetof(AppInfoQueueEntry, name) == 0x08 &&
              offsetof(AppInfoQueueEntry, next) == 0x28 && offsetof(AppInfoQueueEntry, delegate) == 0x30);

// The two cells 0x4b560 reads: the singleton pointer and the queue head. Owned by StartupState.
struct AppInfoQueue {
    const void* singleton = nullptr;            // [0x2ea8978]; null while the startup constructors run
    AppInfoQueueEntry* head = nullptr;          // [0x2ea8980]
};

enum class EntryRegisterPath : std::uint8_t {
    already_registered,   // `cmp byte [entry],0; jne` : nothing else happens
    queued,               // null singleton: push front on the queue
    singleton_present,    // tree branch, NOT modelled (ok == false)
};

struct EntryRegisterResult {
    bool ok = false;
    EntryRegisterPath path = EntryRegisterPath::already_registered;
};

[[nodiscard]] EntryRegisterResult register_entry_4b560(AppInfoQueueEntry& entry, AppInfoQueue& queue) noexcept;

struct NamedDelegateSpec {
    SharedStr name;                 // borrowed view built by 0xf2f0 (data, strlen, 0, 0)
    std::uint64_t handler_tag;      // the handler RVA tag (must be non-zero: the self-pointer rule)
};

struct NamedDelegateResult {
    bool ok = false;
    EntryRegisterResult registration{};
    bool view_inert = false;        // 0xf590 on the stack view was the exact no-op
    bool atexit_recorded = false;   // __cxa_atexit(0x4b660, entry)
};

// The shared sequence of slots 492 and 543 (see header). Stops at the first failing step.
[[nodiscard]] NamedDelegateResult init_named_delegate_entry(AppInfoQueueEntry& entry, const NamedDelegateSpec& spec,
                                                            AppInfoQueue& queue, StartupAtexitLog& atexit_log) noexcept;

}  // namespace sotc::lane_m5
