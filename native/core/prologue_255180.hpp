// Early fragment of base-Initialize prologue `0x255180` (448 B body; first honest open
// from E407). Models OBS 0x25519b..0x25525f only:
//   alloc 0xa30 → ctor 0x264e50 (hook) → install cell 0x2dfd4b8 (dtor vslot+0x08 hook)
//   call 0x11e70 / 0x3df40 (hooks; default no-op — 0x11e70 is null-gate; 0x3df40 large)
//   alloc 0x40 → init_flag_object_335a0 → install cell 0x2ea4718
//   apply 0x335d0 (hook) → xchg_store_u32_357f0(flag, 1) → tls 0x35770 (hook)
//
// NOT modelled: remainder 0x255264.. (0x98a90 alloc, app+0x3b58/+0x3b40, debug gates).
// PR #5 latch at 0x2dfaaa0 cited only via 0x11e70 null-gate behavior. No leaf-vec; not idx 95.
#pragma once

#include "allocator_front.hpp"
#include "base_initialize_255130.hpp"
#include "init_flag_object_335a0.hpp"
#include "xchg_store_u32_357f0.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kPrologue255180Rva = 0x255180;
inline constexpr std::size_t kPrologue255180Size = 448;
inline constexpr std::size_t kPrologueA30Bytes = 0xa30;
inline constexpr std::uint64_t kPrologueA30CellRva = 0x2dfd4b8;

// OBS stack config blob fed to 0x335d0 (ymm from 0xfc3220 + trailing qwords).
struct PrologueFlagConfig255180 {
    std::array<std::uint64_t, 4> ymm_words{{0x800, 0x400, 0x80, 0x20}};  // LE qwords @ 0xfc3220
    std::uint64_t qword_4000 = 0x4000;
    std::uint64_t qword_10000 = 0x10000;
    std::uint64_t qword_pack = 0xfffffffe0000001eULL;
};

struct Prologue255180Hooks {
    void (*ctor_a30_264e50)(void* obj) = nullptr;           // required
    void (*dtor_vslot08)(void* obj) = nullptr;              // destroy previous cell objs
    void (*call_11e70)() = nullptr;                         // null → no-op (OBS null-gate)
    void (*call_3df40)() = nullptr;                         // null → no-op
    void (*apply_335d0)(void* flag_obj, const PrologueFlagConfig255180& cfg) = nullptr;
    void (*tls_35770)(void* flag_obj, std::uint8_t v) = nullptr;  // null → no-op
};

struct Prologue255180Result {
    bool ok = false;
    void* a30_obj = nullptr;
    void* flag_obj = nullptr;
    std::uint32_t flag_word_20 = 0;  // expect 1 after success
};

// Process-wide cell modelling original qword @ 0x2dfd4b8.
[[nodiscard]] void*& prologue_a30_cell_2dfd4b8() noexcept;

// Early prologue fragment via tip AllocatorFront. On failure leaves cells unchanged
// when possible; freshly allocated objs are destroyed via dtor hook + delete_.
[[nodiscard]] Prologue255180Result run_prologue_255180_early(
    AllocatorFront& alloc, const Prologue255180Hooks& hooks);

}  // namespace sotc
