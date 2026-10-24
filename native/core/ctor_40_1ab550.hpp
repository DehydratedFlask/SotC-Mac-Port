// Native lift for prologue-tail 0x40 ctor OBS `0x1ab550` (body through jmp 0xf901c0).
//
// Full pin span to ret@0x1ab580 is 49 B (sha a81f2e5d…); honest exit is jmp at 0x1ab577
// (44 B through jmp; sha 92367169…). Called from prologue 0x255180 after alloc 0x40.
//
// OBS:
//   call 0x1b9250(obj)           // 69 B base init
//   [obj] = lea(0x1598ad0)+0x10  // derived vtable 0x1598ae0
//   call 0xf901a0                // PLT — NOT modelled (hook)
//   jmp  0xf901c0                // PLT — NOT modelled (hook)
//
// Base 0x1b9250:
//   [obj] = lea(0x1598c20)+0x10  // 0x1598c30 (overwritten by derived)
//   bytes +8/+9=0, +0xa=1, +0xb=0; dword +0xc=1; dword +0x10=0
//   zero xmm +0x20 and +0x2c; store obj → cell 0x2ee6778
//
// Prefer as next measured Init edge after E415. No leaf-vec; not idx 95; PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kCtor40_1ab550Rva = 0x1ab550;
inline constexpr std::size_t kCtor40_1ab550Size = 49;       // through ret after nops
inline constexpr std::size_t kCtor40_1ab550BodySize = 44;   // through jmp 0xf901c0
inline constexpr std::uint64_t kCtor40Base1b9250Rva = 0x1b9250;
inline constexpr std::size_t kCtor40Base1b9250Size = 69;
inline constexpr std::size_t kCtor40Bytes = 0x40;
inline constexpr std::uint64_t kCtor40DerivedVtableRva = 0x1598ae0;
inline constexpr std::uint64_t kCtor40BaseVtableRva = 0x1598c30;
inline constexpr std::uint64_t kCtor40CellRva = 0x2ee6778;

struct Ctor40_1ab550Hooks {
    void (*plt_f901a0)() = nullptr;  // null → no-op
    void (*plt_f901c0)() = nullptr;  // null → no-op (OBS jmp)
};

// Host stand-ins for OBS vtable addresses (lea targets +0x10).
[[nodiscard]] void* ctor40_base_vtable_1598c30() noexcept;
[[nodiscard]] void* ctor40_derived_vtable_1598ae0() noexcept;

// Process-wide cell modelling OBS qword @ 0x2ee6778 (base stores self).
[[nodiscard]] void*& ctor40_cell_2ee6778() noexcept;

// Models 0x1b9250(rdi=obj).
void init_base_1b9250(void* obj) noexcept;

// Models 0x1ab550(rdi=obj). Requires writable 0x40 bytes.
void init_ctor_40_1ab550(void* obj, const Ctor40_1ab550Hooks& hooks = {}) noexcept;

}  // namespace sotc
