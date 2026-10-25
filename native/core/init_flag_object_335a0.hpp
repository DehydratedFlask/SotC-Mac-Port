// Native form of original helper RVA 0x335a0 (34 B; ELF sha256 d0723afb…861d3):
//   lea rax, [0x15954d0]; add rax, 0x10; mov [rdi], rax;  // vtable AP 0x15954e0
//   mov qword [rdi+0x10], 0;
//   xor ecx/eax; xchg dword [rdi+0x20], ecx; xchg qword [rdi+0x30], rax; ret
//
// Called from prologue 0x255180 after alloc 0x40. Native stores a null pointer at +0
// (no guest vptr dispatch — destroy uses injectable hooks) and clears +0x10/+0x20/+0x30.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::size_t kFlagObject335a0Bytes = 0x40;
inline constexpr std::uint64_t kFlagObject335a0VtableAp = 0x15954e0;  // OBS only; not stored

void init_flag_object_335a0(void* obj) noexcept;

}  // namespace sotc
