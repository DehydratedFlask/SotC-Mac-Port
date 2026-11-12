// CPU-01-SLOT408-PREREQS: native model of the 116 byte ring destructor 0x354470 (ELF body pinned per instruction by
// tools/lift/check_helper_354470.py). It is the atexit callback startup slot 473 registers for the object 0x2ef2ba8 that
// 0x1f220 (helper_1f220_ring_header) initialises: [+0] begin, [+8] end, [+0x20] the inline storage pointer.
//
//   for e in [begin, end) step 0x98:
//       p = [e+0x30] ; if (p && p != [e+0x50]) free_type2(p)     ; a small-buffer string/vector (inline at +0x50 not freed)
//       shared_release_f590(holder at e+8)                        ; the 0x20 B holder, control at e+0x20
//   b = [obj] (reloaded) ; if (b && b != [obj+0x20]) free_type2(b) ; the element buffer unless it is the inline storage
//
// The free is a HOOK (0x170c0 = `mov esi,2 ; jmp 0x12af0`); the release is the native 0xf590 model with its own hooks.
// Elements are raw 0x98 B blobs; only the three named qwords are read. Pointers are real host pointers.
#pragma once

#include <cstddef>
#include <cstdint>

#include "core/shared_release_f590.hpp"

namespace sotc {

inline constexpr std::uint64_t kRingDtor354470Rva = 0x354470;
inline constexpr std::size_t kRingDtor354470Size = 116;
inline constexpr std::size_t kRingDtorElementSize = 0x98;
inline constexpr std::size_t kRingDtorElementHolderOffset = 0x08;
inline constexpr std::size_t kRingDtorElementBufferOffset = 0x30;
inline constexpr std::size_t kRingDtorElementInlineOffset = 0x50;
inline constexpr std::size_t kRingDtorInlineStorageOffset = 0x20;

struct RingDtorHooks {
    void (*free_type2)(void* pointer) = nullptr;       // 0x170c0
    SharedReleaseF590Hooks release;                    // 0xf590 destroy / free callees
};

struct RingDtorResult {
    bool ok = false;
    std::size_t elements = 0;
    std::size_t buffers_freed = 0;       // element buffers freed (excluding the ring buffer)
    std::size_t releases_last = 0;       // element holders whose release was the last reference
    bool ring_buffer_freed = false;
};

[[nodiscard]] RingDtorResult ring_dtor_354470(void* object, const RingDtorHooks& hooks) noexcept;

}  // namespace sotc
