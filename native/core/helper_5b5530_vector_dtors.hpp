// CPU-01-SLOT408-PREREQS: native models of the three vector-style destructors startup slot 408 (0x5c7f50) registers with
// __cxa_atexit (ELF bodies pinned per instruction by tools/lift/check_helper_5b5530.py). Free is a HOOK: 0x170b0 / 0x170c0
// are `mov esi,1/2 ; jmp 0x12af0` (TLS allocator dispatch, an indirect call).
//
//   0x5b5530 (142 B, object 0x2fda980): vector at +8 {begin +8, end +0x10, cap +0x18} of 0x28 B SSO-string elements
//       if (begin == 0) return ; for each element: if ([e+0x20] >= 0x10) free_type1([e+8]) ; [e+0x18] = 0 ; [e+0x20] = 0xf ;
//       [e+8] = 0 (byte) ; then free_type1(begin) (also when begin == end) ; begin = end = cap = 0
//   0x5b55c0 (23 B, object 0x2fda9a0): p = [obj] ; if (p && p != [obj+0x20]) free_type2(p)          -- inline-storage guard
//   0x5c7c00 (127 B, object 0x2ff2af0): vector at +8 of 0x30 B elements holding a buffer at +0x18:
//       if (begin == 0) return ; for each element: if ([e+0x18]) { free_type1([e+0x18]) ; [e+0x18..0x28) = 0 ; [e+0x28] = 0 } ;
//       free_type1(begin) ; begin = end = cap = 0
//
// Element contents other than the named fields are untouched. Pointers are real host pointers. A reachable free with a missing
// hook returns false before that free (and performs nothing past it) instead of claiming it happened.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kSsoVectorDtor5b5530Rva = 0x5b5530;
inline constexpr std::size_t kSsoVectorDtor5b5530Size = 142;
inline constexpr std::uint64_t kInlineGuardDtor5b55c0Rva = 0x5b55c0;
inline constexpr std::size_t kInlineGuardDtor5b55c0Size = 23;
inline constexpr std::uint64_t kVec30Dtor5c7c00Rva = 0x5c7c00;
inline constexpr std::size_t kVec30Dtor5c7c00Size = 127;
inline constexpr std::size_t kSsoElementSize = 0x28;
inline constexpr std::size_t kSsoElementBufferOffset = 0x08;
inline constexpr std::size_t kSsoElementSizeFieldOffset = 0x18;
inline constexpr std::size_t kSsoElementCapacityOffset = 0x20;
inline constexpr std::uint64_t kSsoInlineCapacity = 0xf;
inline constexpr std::size_t kVec30ElementSize = 0x30;
inline constexpr std::size_t kVec30ElementBufferOffset = 0x18;
inline constexpr std::size_t kVectorHeaderOffset = 0x08;   // begin at +8, end at +0x10, capacity end at +0x18

struct VectorDtorHooks {
    void (*free_type1)(void* pointer) = nullptr;   // 0x170b0
    void (*free_type2)(void* pointer) = nullptr;   // 0x170c0
};

[[nodiscard]] bool sso_vector_dtor_5b5530(void* object, const VectorDtorHooks& hooks) noexcept;
[[nodiscard]] bool inline_guard_dtor_5b55c0(void* object, const VectorDtorHooks& hooks) noexcept;
[[nodiscard]] bool vec30_dtor_5c7c00(void* object, const VectorDtorHooks& hooks) noexcept;

}  // namespace sotc
