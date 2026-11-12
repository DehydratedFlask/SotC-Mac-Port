// CPU-01-SLOT408-PREREQS: native models of the startup-slot-451 holder destructors (ELF bodies pinned per instruction by
// tools/lift/check_helper_499e90.py). The three allocator-side callees are HOOKS: 0x170b0 / 0x170c0 are `mov esi,1/2 ;
// jmp 0x12af0` (free type 1 / 2, TLS allocator dispatch) and 0x3b380 is `jmp 0xf8f8c0` (critical-section destroy).
//
//   0x3b380 (5 B)   jmp 0xf8f8c0                                       -> critsec_destroy(object)
//   0x499e50 (5 B)  jmp 0x49c9e0                                       -> six_buffer_dtor_49c9e0
//   0x499e60 (37 B) p=[o+8]; if p free2(p); critsec_destroy(o)
//   0x499e90 (74 B) n=[*slot]; if n { c=[n+0x28]; free2 if c; c=[n+8]; free2 if c; critsec_destroy(n); free1(n); *slot=0 }
//   0x49c9e0 (113 B) free2 of each non-null [o+0xb8,0x98,0x78,0x58,0x38,0x18] in that order; critsec_destroy(o)
//
// Buffers are the stored pointers themselves (real host pointers). A missing hook for a reachable call makes the model
// return false and perform nothing past the missing call, instead of pretending the free happened.
#pragma once

#include <cstddef>
#include <cstdint>
#include <array>

namespace sotc {

inline constexpr std::uint64_t kCritsecDtor3b380Rva = 0x3b380;
inline constexpr std::uint64_t kTailDtor499e50Rva = 0x499e50;
inline constexpr std::uint64_t kBufferDtor499e60Rva = 0x499e60;
inline constexpr std::uint64_t kNodeDtor499e90Rva = 0x499e90;
inline constexpr std::uint64_t kSixBufferDtor49c9e0Rva = 0x49c9e0;
inline constexpr std::size_t kNodeDtor499e90OuterOffset = 0x28;
inline constexpr std::size_t kNodeDtor499e90InnerOffset = 0x08;
inline constexpr std::size_t kBufferDtor499e60BufferOffset = 0x08;
inline constexpr std::array<std::size_t, 6> kSixBufferOffsets49c9e0 = {0xb8, 0x98, 0x78, 0x58, 0x38, 0x18};

struct Slot451DtorHooks {
    void (*free_type1)(void* pointer) = nullptr;        // 0x170b0
    void (*free_type2)(void* pointer) = nullptr;        // 0x170c0
    void (*critsec_destroy)(void* object) = nullptr;    // 0x3b380 -> 0xf8f8c0
};

[[nodiscard]] bool critsec_dtor_3b380(void* object, const Slot451DtorHooks& hooks) noexcept;
[[nodiscard]] bool tail_dtor_499e50(void* object, const Slot451DtorHooks& hooks) noexcept;
[[nodiscard]] bool buffer_dtor_499e60(void* object, const Slot451DtorHooks& hooks) noexcept;
[[nodiscard]] bool node_dtor_499e90(void** slot, const Slot451DtorHooks& hooks) noexcept;
[[nodiscard]] bool six_buffer_dtor_49c9e0(void* object, const Slot451DtorHooks& hooks) noexcept;

}  // namespace sotc
