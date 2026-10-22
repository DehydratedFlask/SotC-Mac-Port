// App vslot `0x533100` tail call-site OBS `0x53403c..0x534052` (22 B):
//   mov esi, 9; mov rdi, r15 (= self+0x3cc8); call 0x140d90
//   lea rdi, [rbp-0x70]; call 0xf590
//   // stop before stack-check / epilogue @ 0x534052
//
// Always-on after E443 cp3shadowprecache attach. Prefer over success atexit
// (already skipped in attach) and over deepen 0x14c2f0 / 0x6a5370 (already
// hooked earlier). Prefer over full body of 0x140d90 (143 B; mutex/atexit).
// Prefer over empty-path migrate/non-null.
//
// Claimed: call-site control flow + arg pin (slot=self+0x3cc8, esi=9) + f590.
// NOT modelled: body of 0x140d90; stack-check / ret epilogue; bodies of
// 0x14c2f0 / 0x6a5370. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVslot40533100Tail140d90Rva = 0x53403c;
inline constexpr std::size_t kVslot40533100Tail140d90Size = 22;
inline constexpr std::uint64_t kVslot40Tail140d90EndRva = 0x534052;
inline constexpr std::uint64_t kVslot40Tail140d90SlotOff = 0x3cc8;
inline constexpr int kVslot40Tail140d90Arg = 9;

struct Vslot40_533100Tail140d90Hooks {
    // Models 0x140d90(slot_obj, arg). Null → no-op.
    void (*call_140d90)(void* slot_obj, int arg) = nullptr;
    // Models 0xf590 path dtor. Null → no-op.
    void (*dtor_f590)(void* obj) = nullptr;
};

struct Vslot40_533100Tail140d90Result {
    bool ok = false;
    void* slot_obj = nullptr;
    int arg = 0;
    int call_140d90_count = 0;
    int dtor_f590_count = 0;
};

[[nodiscard]] Vslot40_533100Tail140d90Result vslot_40_533100_tail_140d90(
    void* self, const Vslot40_533100Tail140d90Hooks& hooks = {}) noexcept;

}  // namespace sotc
