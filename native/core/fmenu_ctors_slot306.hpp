// CPU-01-SLOT306-MENU-CTORS-01: native recovery of the three debug-menu item constructors that
// startup slot 306 (0xa633b0, the `sndx_` block) uses and that were not native yet, plus the
// complete registration helper 0x268b0 including its kind-1 branch.
//
// Original ELF sha256 d0723afb...5861d3. All bodies were read end to end from the pinned bytes
// (xcrun llvm-objdump -d) and are byte-pinned by tools/lift/check_fmenu_ctors_slot306.py. Nothing
// here executes an original RVA: vptr-like values are not involved (these items have no vtable), and
// every un-lifted callee is an explicit hook or a pinned constant.
//
// Item layout (shared with debug_menu_item.hpp, 0xa4 bytes; offsets verified in all five ctors):
//   +0x00 link{self,prev,next,owner}  +0x20 registered   +0x28 view A (parent path)
//   +0x48 kind (dword)                +0x50 view B (caption)
//   +0x70 view C                      +0x90 target ptr   +0x98 f32 +0x9c f32   +0xa0 flags
// debug_menu_item.hpp names view A/B/C `label`/`text`/`empty`; the call sites show A is the parent
// path ("" for the top group, "Sound" for its children) and B the caption, so those names are
// historic. The structure is the same type and is reused unchanged.
//
// Lifted bodies:
//   0x26c10 (0xc2 B)  group item.   this=rdi, parent=rsi, caption=rdx, flags=ecx.
//        kind = 1; view B = *caption (+ lock inc ctl); view C = *caption AGAIN (+ lock inc ctl),
//        i.e. a group repeats its caption in +0x70 and holds two references; target = 0;
//        range {1.0, 10.0}; flags; tail-calls 0x268b0(this, [0x2dfd4b8]).
//   0x26b60 (0xa6 B)  separator.    this=rdi, parent=rsi, (rdx unused), flags=ecx.
//        kind = 3; view B = SharedStr("MenuSeparator") built by 0xf1a0(this+0x50, 0xf96530)
//        (native: shared_str_ctor_f1a0, allocator = hook); view C = 0xf180 (empty); target = 0;
//        range {1.0, 10.0}; flags; tail-calls 0x268b0. The only item shape that allocates.
//   0x26f90 (0xc0 B)  command item. this=rdi, parent=rsi, caption=rdx, target=rcx, command=r8,
//        flags=r9d. kind = 2 (same kind as the toggle/slider items); view B = *caption,
//        view C = *command name (the console command the item invokes, e.g. "sndx_reloadData");
//        target = rcx (an opaque pointer, 0x2dfcbd0 at the only call site); range {1.0, 10.0}; flags;
//        tail-calls 0x268b0.
//   0x268b0 (0x2ac B incl. 1 pad byte; code ends 0x26b5b)  registration. Complete control flow:
//        registered != 0                                   -> return (0x268d4)
//        flags & 0xc0 != 0                    [site A]     -> engine-state-gated log 0x3e7e0, line 0xf3
//        kind == 0                                         -> skip B and the kind-1 check
//        flags & 0xf00 != 0                   [site B]     -> gated log, line 0xf8
//        kind == 1 && (flags & 0x2c) != 0x20  [site C]     -> gated log, line 0xfe  (kind-1 branch)
//        kind 2 and kind 3 have NO branch of their own: kind 3 (separator) takes exactly the kind-2 path.
//        parent != null -> 0x27470(parent, &item+0x28, &item+0x48); registered = 1
//        parent == null -> lazy root init (guard 0x2dfd4e8, atexit 0x27280) + tail append, owner=root.
//        registered stays 0 on the root path (only the parent path sets +0x20).
//      The "gated log" condition is `[0x2ea5170+0x22b0] == [0x2ea5170+0x22b8] || *[[..22b0]+0x2b8]`:
//      engine state that is not native. It is a hook (DebugMenuAssertHooks); with no hook the call
//      FAILS CLOSED (ok == false, nothing linked). With flags 0x30 (every slot-306 item) no site is
//      reachable: 0x30 & 0xc0 == 0, 0x30 & 0xf00 == 0 and 0x30 & 0x2c == 0x20, so the kind-1 group
//      items are registered without any log, and no hook is consulted.
//
// NOT modelled: the log function 0x3e7e0 (hook only), the parent-insert body 0x27470 (existing
// DebugMenuParentContract), the item destructor 0x272d0 (pinned and recorded with __cxa_atexit by
// the call sites; it logs when an unregistered item dies, unlinks, and releases views C, B, A).
#pragma once

#include <cstddef>
#include <cstdint>

#include "debug_menu_item.hpp"
#include "shared_str_ctor_f1a0.hpp"

namespace sotc {

inline constexpr std::uint64_t kMenuGroupCtorRva = 0x26c10;
inline constexpr std::size_t kMenuGroupCtorSize = 0xc2;
inline constexpr std::uint64_t kMenuSeparatorCtorRva = 0x26b60;
inline constexpr std::size_t kMenuSeparatorCtorSize = 0xa6;
inline constexpr std::uint64_t kMenuCommandCtorRva = 0x26f90;
inline constexpr std::size_t kMenuCommandCtorSize = 0xc0;
inline constexpr std::size_t kMenuRegisterSize = 0x2ac;        // as pinned by check_fctor_slots_298_299_301.py
inline constexpr std::uint64_t kMenuRegisterCodeEndRva = 0x26b5b;
inline constexpr std::uint64_t kMenuSeparatorTextRva = 0xf96530;   // "MenuSeparator"
inline constexpr char kMenuSeparatorText[] = "MenuSeparator";

inline constexpr std::uint32_t kMenuKindGroup = 1;       // 0x26c10
inline constexpr std::uint32_t kMenuKindCommand = 2;     // 0x26f90 (also 0x26ea0 / 0x27050)
inline constexpr std::uint32_t kMenuKindSeparator = 3;   // 0x26b60
inline constexpr std::uint32_t kMenuSlot306Flags = 0x30;

inline constexpr std::uint32_t kMenuAssertFlagsMaskA = 0xc0;    // test byte [flags], 0xc0
inline constexpr std::uint32_t kMenuAssertFlagsMaskB = 0xf00;   // test byte [flags+1], 0xf
inline constexpr std::uint32_t kMenuKind1FlagsMask = 0x2c;      // and eax, 0x2c ; cmp eax, 0x20
inline constexpr std::uint32_t kMenuKind1FlagsOk = 0x20;
inline constexpr std::uint32_t kMenuAssertLineA = 0xf3;
inline constexpr std::uint32_t kMenuAssertLineB = 0xf8;
inline constexpr std::uint32_t kMenuAssertLineC = 0xfe;
inline constexpr std::uint64_t kMenuAssertTextRvaA = 0xf9665d;
inline constexpr std::uint64_t kMenuAssertTextRvaB = 0xf9668d;
inline constexpr std::uint64_t kMenuAssertTextRvaC = 0xf966dc;

// Group (kind 1). Copies `caption` into views B and C and acquires each control counter.
[[nodiscard]] bool init_menu_group_26c10(DebugMenuItem& item, const SharedStr& parent,
                                         const SharedStr& caption, std::uint32_t flags) noexcept;

// Separator (kind 3). Builds "MenuSeparator" through the native 0xf1a0 with `hooks.alloc`. Fail-closed:
// on allocation failure nothing in `item` is modified and no counter is acquired (the original would
// have faulted; the native refuses before the first store).
[[nodiscard]] bool init_menu_separator_26b60(DebugMenuItem& item, const SharedStr& parent,
                                             std::uint32_t flags, const SharedStrCtorHooks& hooks) noexcept;

// Command item (kind 2). `target` is stored verbatim and never dereferenced.
[[nodiscard]] bool init_menu_command_26f90(DebugMenuItem& item, const SharedStr& parent,
                                           const SharedStr& caption, const void* target,
                                           const SharedStr& command, std::uint32_t flags) noexcept;

enum class MenuAssertSite : std::uint8_t {
    none,
    a_flags_c0,     // line 0xf3
    b_flags_f00,    // line 0xf8
    c_kind1_flags,  // line 0xfe
};

struct MenuAssertEvent {
    MenuAssertSite site = MenuAssertSite::none;
    std::uint32_t line = 0;                // the dword the original stores at [rbp-0x30/-0x50/-0x70]
    std::uint64_t message_rva = 0;         // r9 format string
    const DebugMenuItem* item = nullptr;   // the formatted argument is item->text.data
};

// Hooks for the engine-state gate and the 0x3e7e0 log. `log_active` stands for
// `begin == end || *(byte*)(*(void**)(begin + 0x2b8))` at 0x2ea5170+0x22b0/0x22b8. When `log_active`
// is null a reachable site fails closed. `emit` must return true when the log completed.
struct DebugMenuAssertHooks {
    bool (*log_active)() noexcept = nullptr;
    bool (*emit)(const MenuAssertEvent& event) noexcept = nullptr;
};

struct MenuRegisterResult {
    bool ok = false;
    DebugMenuRegisterPath path = DebugMenuRegisterPath::already_registered;
    MenuAssertSite blocked_site = MenuAssertSite::none;   // set when a reachable site had no usable hook
    std::uint8_t asserts_emitted = 0;                     // number of 0x3e7e0 events delivered to the hook
    bool root_initialised = false;
    bool atexit_recorded = false;
};

// 0x268b0 for every kind (0..3). Same linking as register_debug_menu_item_268b0 for kinds 0/2/3 and
// additionally the kind-1 check. Never changes anything when it returns ok == false before the
// link step (hooks first, then insertion, exactly the original order).
[[nodiscard]] MenuRegisterResult register_menu_item_268b0(DebugMenuItem& item, DebugMenuRegistry& menu,
                                                          StartupAtexitLog& atexit,
                                                          const DebugMenuAssertHooks& hooks = {}) noexcept;

// Call-site tail common to every slot-306 item: `__cxa_atexit(0x272d0, item, dso)`.
[[nodiscard]] bool record_menu_item_dtor_272d0(StartupAtexitLog& atexit, const DebugMenuItem& item) noexcept;

}  // namespace sotc
