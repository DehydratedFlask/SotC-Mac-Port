// CPU-01-AOT-CFG-SCALE-01-BATCH19: native model of the id + string entry constructor 0xa59620 (249 B, FDE 249; ELF sha256
// d0723afb...5861d3, body sha 82dd6511...c59195f) and of the doubling pointer-vector push it carries inline (shared by 0xa42960).
// Called 37 times by startup slot 428 (0x52ec10), one 0x28 B entry per call. Byte-pinned by tools/lift/check_helper_a59620.py.
// Nothing here executes or calls an original RVA: the allocator pair is the project's AllocDispatchContract hook
// (0x17090 / 0x170c0 -> tags 1 / 2, flags 2), the empty-string initialiser is the native init_slot_f180 and the shared release is the
// native inert 0xf590.
//
//   0xa59620(rdi = entry, esi = id, rdx = const SharedStr* src)
//     init_slot_f180(entry + 8)                      ; the string starts as the empty view
//     entry->id = id                                  ; mov [r14], r12d      (the four bytes at +4 are never written)
//     if (entry + 8 != src) {                         ; cmp r15, rbx
//         0xf590(entry + 8)                           ; always the inert release: the string was emptied one instruction before
//         entry->str = *src                           ; 16 B {data,size}, aux, ctl      (lock inc qword [ctl] when ctl != 0)
//     }
//     push entry onto the process vector V1 {begin 0x32f8dc8, end 0x32f8dd0, cap 0x32f8dd8}:
//         if (end <  cap)  { *end++ = entry }                                          ; cmp / jae
//         else             { n = end != begin ? (end - begin) >> 2 : 1                 ; elements = 2 * size, or 1 when empty
//                            buf = n != 0 ? 0x17090(n * 8) : 0 ; memmove(buf, begin, end - begin) ; buf[size] = entry
//                            if (begin) 0x170c0(begin) ; begin = buf ; end = buf + size + 1 ; cap = buf + n }
//   so capacities from empty run 1, 2, 4, 8, 16, 32, 64 (a pure doubling, no minimum, no overflow check).
//
// Refusals: the original has no allocator or null check (a null block faults at the memmove). A missing hook or a null block here
// is reported and leaves the vector untouched (the entry itself is already written, exactly as in the original which writes it
// first). NOT modelled: the meaning of the id / string pair (INFERENCE only: an id -> name table), the exit-time teardown of V1.
// FIXTURE_VERIFIED + STATIC_ANALYSIS_VERIFIED only.
#pragma once

#include <cstddef>
#include <cstdint>

#include "alloc_wrappers_17070.hpp"
#include "hashed_string_14c3a0.hpp"

namespace sotc {

inline constexpr std::uint64_t kIdEntryCtorRva = 0xa59620;
inline constexpr std::size_t kIdEntryCtorSize = 249;
inline constexpr std::uint64_t kIdEntryVecBeginRva = 0x32f8dc8;     // V1 begin; end at +8, cap at +0x10
inline constexpr std::size_t kIdEntryStride = 0x28;

// A pointer vector as the original lays it out in .bss: three consecutive qwords {begin, end, cap}.
struct PtrVec24 {
    void** begin;     // +0
    void** end;       // +8
    void** cap;       // +0x10
};
static_assert(sizeof(PtrVec24) == 0x18);

enum class PtrVecRefusal : std::uint8_t {
    none,
    no_allocator,        // growth needed and the allocate hook is missing (or no deallocate hook for the old block)
    allocation_failed,   // the allocate hook returned null
    free_failed,         // the old block's deallocate hook reported failure (state updated, block leaked)
};

struct PtrVecPush {
    bool ok = false;
    PtrVecRefusal refusal = PtrVecRefusal::none;
    bool grew = false;
    std::size_t new_capacity = 0;     // elements, valid when grew
};

// The inline push of 0xa59620 and 0xa42960 (identical bytes): 0x17090 allocate, 0x170c0 free (see the header comment).
[[nodiscard]] PtrVecPush push_ptr_doubling(PtrVec24& vec, void* element, const AllocDispatchContract& alloc) noexcept;

// `lock inc qword [ctl]` when ctl != 0, then the 32 B view copy {data,size}, aux, ctl: the copy both constructors do.
void copy_shared_str_inc(SharedStr& dst, const SharedStr& src) noexcept;

// The 0x28 B entry at the call sites (stride 0x28): {u32 id, 4 bytes never written, SharedStr}.
struct IdEntry {
    std::uint32_t id;             // +0x00
    std::uint8_t gap04[4];        // never written
    SharedStr str;                // +0x08 .. +0x27
};
static_assert(sizeof(IdEntry) == kIdEntryStride && offsetof(IdEntry, id) == 0 && offsetof(IdEntry, str) == 8);

struct IdEntryRegistration {
    bool ok = false;
    PtrVecRefusal refusal = PtrVecRefusal::none;
    bool aliased = false;           // entry + 8 == src: the copy is skipped (src was just emptied by the same call)
    bool release_inert = false;     // the 0xf590 on the freshly emptied string was an exact no-op
    bool pushed = false;
    bool grew = false;
    std::size_t new_capacity = 0;
};

// 0xa59620. The entry is always written before the push can fail.
[[nodiscard]] IdEntryRegistration register_id_entry_a59620(PtrVec24& vec, IdEntry& entry, std::uint32_t id,
                                                           const SharedStr& src, const AllocDispatchContract& alloc) noexcept;

}  // namespace sotc
