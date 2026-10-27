// Native recovery of the debug-menu "named item" constructor 0x26db0 (0xef bytes, code
// [0x26db0, 0x26e9f); original ELF sha256 d0723afb...5861d3) used by startup slots 322, 331 and 332.
// Task L3-SLOTS-322-334. Offline analysis only; no original code is executed. Byte-pinned by
// tools/lift/check_ctor_slot_322.py (and the sibling checkers, which reuse the same pins).
//
// It is the sibling of 0x26ea0 (debug_menu_item.hpp): the same 0xa4-byte item, the same 268b0 tail
// registration and the same virtual-slot range narrowing, but the caption comes from the TARGET and
// there is one view argument instead of two.
//
//   this=rdi, parent=rsi (SharedStr view), target=rdx, flags=ecx
//   [this]=0; [this+8]=this; [this+0x10]=this; [this+0x18]=0; byte [this+0x20]=0
//   [this+0x28..0x48) = *parent (+ `lock inc [ctl]` when parent.ctl != 0)         ; view A
//   dword [this+0x48] = 2                                                         ; kind
//   [this+0x50..0x70) = *(target+0x20)   (the target's ConCmdBase name, +0x20..0x3f; + `lock inc`)
//   0xf180(this+0x70)                                                             ; empty view C
//   [this+0x90]=target; dword [0x98]=1.0f; dword [0x9c]=10.0f; dword [0xa0]=flags
//   if (target->vptr[3]() == 1 && target->vptr[6]() == 3) { [0x98]=0.1f; [0x9c]=0.5f }
//   tail-jump 0x268b0(this, [0x2dfd4b8])
//
// Differences from 0x26ea0, all read from the bytes: 0x26ea0 takes (label, text, target, flags) and
// copies `text` into +0x50 from its own argument; 0x26db0 copies the target's own name view.
// The two virtual calls are replaced by the lifted slot bodies selected by the target's vptr tag
// (DebugMenuTarget); an unlifted class fails visibly. Registration (0x268b0) is a separate native
// call (register_menu_item_268b0) exactly as for the other constructors.
#pragma once

#include <cstdint>

#include "concmd_base.hpp"
#include "debug_menu_item.hpp"

namespace sotc {

inline constexpr std::uint64_t kMenuNamedItemCtorRva = 0x26db0;
inline constexpr std::uint64_t kMenuNamedItemCtorEndRva = 0x26e9f;   // exclusive: tail `jmp 0x268b0` ends 0x26e9f
inline constexpr std::size_t kMenuNamedItemCtorSize = 0xef;

// `target_base` is the target's ConCmdBase (its name view at +0x20 is the caption). `target` supplies
// the two virtual-slot results. Fails (false, item untouched) when `target.known` is false.
[[nodiscard]] bool init_debug_menu_item_26db0(DebugMenuItem& item, const SharedStr& parent,
                                              const ConCmdBase& target_base, const DebugMenuTarget& target,
                                              std::uint32_t flags) noexcept;

}  // namespace sotc
