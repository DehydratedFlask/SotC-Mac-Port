// Native recovery of the debug-menu item constructors and the null-parent registration used by
// startup slot 299 (original ELF sha256 d0723afb...5861d3). Names are ours; the image carries only
// the assert text `void bpe::DebugMenuItemC...` (0xf9661d) and the file path of
// `...\Code\BPE\Source\...` (0xf96543), which is why the class is called a debug-menu item. The
// constructors are 110 / 505 image callers each (the common slider / toggle item shapes).
//
// Lifted bodies (byte-pinned by tools/lift/check_fctor_slots_298_299_301.py):
//   0x27050 (0xd0 B)  item ctor with explicit float range
//        this=rdi, label=rsi, text=rdx, target=rcx, flags=r8d, min=xmm0, max=xmm1; tail-calls
//        0x268b0(this, [0x2dfd4b8]).
//   0x26ea0 (0xed B)  item ctor with default range {1.0, 10.0}; if the target's virtual slot 3
//        returns 1 AND slot 6 returns 3, the range becomes {0.1, 0.5}; tail-calls 0x268b0.
//        The two virtual calls are replaced by the lifted slot bodies of the one class this slot
//        ever targets (bool variable class 0x1594218: slot 3 = 0xb890 `mov eax,1; ret`, slot 6 =
//        0xb930 `xor eax,eax; ret`), selected by the target's vptr tag. Any other tag is an
//        unlifted contract and fails visibly.
//   0x268b0 (0x2a7 B) registration. Modelled exactly for the observed shapes (kind 2,
//        flags & 0xc0 == 0, flags & 0xf00 == 0). The assert/log branches (0x3e7e0 with the
//        `Menu Items should never ...` texts, guarded by an engine-state compare) and the kind-1
//        block are NOT modelled: a request that would take them returns a visible failure rather
//        than silently skipping a log.
//          parent == null  -> append to the process-wide root list 0x2dfd4c0 (lazy, guarded by
//                             0x2dfd4e8, with __cxa_atexit(0x27280, root))        [NATIVE]
//          parent != null  -> 0x27470(parent, &item.name, &item.kind)            [CONTRACT hook]
//
// Why the parent is null on the startup path: the parent cell `[0x2dfd4b8]` is zero `.bss`; its
// writers are the runtime prologue 0x255180 (native elsewhere as prologue_a30_cell_2dfd4b8) and
// constructor-table index 565 (0x2fcf0), which only zeroes it. No code that runs at indices 0..299
// stores to it. Startup keeps its own copy (DebugMenuRegistry::parent) instead of the prologue's
// process global, because the prologue runs after construction.
//
// Layout of the 0xa4-byte item (all offsets verified in the bodies above):
//   +0x00 link {self, prev, next, owner}   +0x20 registered (byte)   +0x28 label view (0x20 B)
//   +0x48 kind (dword = 2)                 +0x50 text view (0x20 B)  +0x70 empty view (0xf180)
//   +0x90 target pointer                   +0x98 range min (f32)     +0x9c range max (f32)
//   +0xa0 flags (dword)
// Destructors 0x272d0 (item) and 0x27280 (root) are pinned, never executed.
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "concmd_vars.hpp"

namespace sotc {

inline constexpr std::uint64_t kDebugMenuItemFloatCtorRva = 0x27050;
inline constexpr std::size_t kDebugMenuItemFloatCtorSize = 0xd0;
inline constexpr std::uint64_t kDebugMenuItemToggleCtorRva = 0x26ea0;
inline constexpr std::size_t kDebugMenuItemToggleCtorSize = 0xed;
inline constexpr std::uint64_t kDebugMenuRegisterRva = 0x268b0;
inline constexpr std::uint64_t kDebugMenuParentInsertRva = 0x27470;   // contract
inline constexpr std::uint64_t kDebugMenuParentCellRva = 0x2dfd4b8;
inline constexpr std::uint64_t kDebugMenuRootRva = 0x2dfd4c0;
inline constexpr std::uint64_t kDebugMenuRootGuardRva = 0x2dfd4e8;
inline constexpr std::uint64_t kDebugMenuRootDtorRva = 0x27280;       // pinned, not executed
inline constexpr std::uint64_t kDebugMenuItemDtorRva = 0x272d0;       // pinned, not executed
inline constexpr std::uint32_t kDebugMenuItemKind = 2;
inline constexpr std::uint64_t kBoolVarSlot3Rva = 0xb890;             // mov eax,1; ret
inline constexpr std::uint64_t kBoolVarSlot6Rva = 0xb930;             // xor eax,eax; ret
inline constexpr std::uint32_t kBoolVarSlot3Result = 1;
inline constexpr std::uint32_t kBoolVarSlot6Result = 0;

struct MenuLink {
    MenuLink* self;     // +0x00  0 at the root, `this` once an item is linked
    MenuLink* prev;     // +0x08
    MenuLink* next;     // +0x10
    void* owner;        // +0x18  the root once linked
};
static_assert(sizeof(MenuLink) == 0x20);

struct DebugMenuItem {
    MenuLink link;              // +0x00
    std::uint8_t registered;    // +0x20
    std::uint8_t gap21[7];      // +0x21 never written
    SharedStr label;            // +0x28
    std::uint32_t kind;         // +0x48
    std::uint8_t gap4c[4];      // +0x4c never written
    SharedStr text;             // +0x50
    SharedStr empty;            // +0x70 (0xf180)
    const void* target;         // +0x90
    float range_min;            // +0x98
    float range_max;            // +0x9c
    std::uint32_t flags;        // +0xa0
};
static_assert(offsetof(DebugMenuItem, registered) == 0x20 && offsetof(DebugMenuItem, label) == 0x28 &&
              offsetof(DebugMenuItem, kind) == 0x48 && offsetof(DebugMenuItem, text) == 0x50 &&
              offsetof(DebugMenuItem, empty) == 0x70 && offsetof(DebugMenuItem, target) == 0x90 &&
              offsetof(DebugMenuItem, range_min) == 0x98 && offsetof(DebugMenuItem, range_max) == 0x9c &&
              offsetof(DebugMenuItem, flags) == 0xa0);

// The root list object at 0x2dfd4c0 (header words as MenuLink; count dword at +0x20).
struct DebugMenuRoot {
    MenuLink link{};
    std::uint32_t count = 0;
};

// Contract for the non-null parent path (0x27470). No default.
struct DebugMenuParentContract {
    void* parent = nullptr;   // `[0x2dfd4b8]`; null on the whole startup path
    bool (*insert)(void* parent, DebugMenuItem& item) noexcept = nullptr;
};

struct DebugMenuRegistry {
    DebugMenuRoot root{};
    bool root_guard_done = false;    // 0x2dfd4e8 != 0 after the lazy initialisation
    DebugMenuParentContract parent{};
};

enum class DebugMenuRegisterPath : std::uint8_t {
    already_registered,   // 0x268d4: `cmp byte [rbx+0x20],0; jne end`
    root_list,
    parent,
    unmodelled_assert,    // flags / kind would take an assert or kind-1 branch that is not lifted
};

struct DebugMenuRegisterResult {
    bool ok = false;
    DebugMenuRegisterPath path = DebugMenuRegisterPath::already_registered;
    bool root_initialised = false;   // this call ran the guarded root initialisation
    bool atexit_recorded = false;    // the root's destructor registration was recorded
};

// Identity of the one virtual class a toggle item targets. Built from the bool variable's vptr tag.
struct DebugMenuTarget {
    const void* object = nullptr;
    std::uint32_t slot3 = 0;     // virtual call at [vptr + 0x18]
    std::uint32_t slot6 = 0;     // virtual call at [vptr + 0x30]
    bool known = false;          // false => the vptr tag is not a lifted class
};
[[nodiscard]] DebugMenuTarget debug_menu_target_for(const ConCmdBoolVar& var) noexcept;

// 0x268b0(item, parent) for the observed shapes.
[[nodiscard]] DebugMenuRegisterResult register_debug_menu_item_268b0(DebugMenuItem& item,
                                                                     DebugMenuRegistry& menu,
                                                                     StartupAtexitLog& atexit) noexcept;

// 0x27050 followed by its tail call into 0x268b0 and (at the call sites) the atexit record of
// 0x272d0. `label` and `text` are copied with `lock inc` when they carry a control counter.
[[nodiscard]] bool init_debug_menu_item_27050(DebugMenuItem& item, const SharedStr& label,
                                              const SharedStr& text, const void* target,
                                              std::uint32_t flags, float range_min, float range_max) noexcept;

// 0x26ea0. `target` supplies the two virtual-slot results (see header).
[[nodiscard]] bool init_debug_menu_item_26ea0(DebugMenuItem& item, const SharedStr& label,
                                              const SharedStr& text, const DebugMenuTarget& target,
                                              std::uint32_t flags) noexcept;

}  // namespace sotc
