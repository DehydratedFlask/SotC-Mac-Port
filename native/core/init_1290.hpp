// Native lift of original function RVA `0x1290` (Init 0x531d60 callee at 0x531e03).
//
// ELF sha256 d0723afb…861d3. Body `0x1290..0x1353` = 195 B, fn sha256 185f03d3…
// (E516). Byte-verified shape: straight line, 5 blocks, one `ret` at 0x134d,
// stack-check fail edge `jne 0x134e → 0xf8f3a0`; ABI `void f(void)` (no incoming
// register read; success leaves the canary in rax, which is not a result).
//
// Observed effects, in order:
//   A = {ctx=&self, invoke=0x1a00, 0, fn=0xb10}, A+0x40 = table 0x1594060
//   call 0x1bde30(owner=*(0x2ee68c8), &A)      ; insert into list-3 (E522: copies 0x20 B)
//   call *[A+0x20][0x18]                        ; table 0x1594060 slot 3 = 0x19f0, a bare `ret`
//   B = {ctx=&self, invoke=0x1a10, 0, fn=0xbb0}
//   call 0x21cd0(owner=*(0x2dfcbc8), &B)
//
// The `__stack_chk_guard` cell 0x15fe518 is read-only (two reads, no writes), so the
// guard check is deliberately NOT modelled: it can only abort, never change the
// observable effect sequence, and modelling it as a fake success would be a lie.
//
// Optional real path (E532): when `container_a` and `alloc_a` are supplied, the A insert runs
// the lifted 0x1beeb0 body (0x20 B copy + list link + count) instead of the hook. When
// `container_b` is supplied, the B insert runs the lifted 0x21cd0 body (bump-allocator
// 0x20 B copy + self-link) instead of the hook.
//
// NOT modelled: the two singleton owners (their allocation lives in 0x533100/0x2569d0
// and cannot be ordered against Init statically), the bodies of 0x1bde30/0x21cd0/0xb10/
// 0xbb0, the 0x1594060 slot-3 target beyond "bare ret", and any effect on Init's result.
// G1 PARTIAL — fixture-level model only, nothing executed against the game.
#pragma once

#include <cstddef>
#include <cstdint>

#include "insert_1bde30.hpp"
#include "insert_1beeb0.hpp"
#include "insert_21cd0.hpp"

namespace sotc {

inline constexpr std::uint64_t kFn1290Rva = 0x1290;
inline constexpr std::size_t kFn1290Size = 195;
inline constexpr std::uint64_t kFn1290EndRva = 0x1353;
inline constexpr std::uint64_t kFn1290RegAInvoke = 0x1a00;
inline constexpr std::uint64_t kFn1290RegBInvoke = 0x1a10;
inline constexpr std::uint64_t kFn1290RegAFn = 0xb10;
inline constexpr std::uint64_t kFn1290RegBFn = 0xbb0;
inline constexpr std::uintptr_t kFn1290Table = 0x1594060;
inline constexpr std::size_t kFn1290EntrySize = 0x20;
inline constexpr std::size_t kFn1290SelfArgOffset = 0x20;
inline constexpr std::size_t kFn1290TableOffset = 0x40;

// Descriptor actually copied by the insert path: `vmovups` pair moves exactly
// 0x20 B to node+0x10 (E522). `ctx` becomes the node's self link when fn != 0.
struct HandlerEntry1290 {
    std::uintptr_t ctx;       // +0x00  &self in the original
    std::uintptr_t invoke;    // +0x08  trampoline 0x1a00 / 0x1a10
    std::uintptr_t reserved;  // +0x10  stored 0
    std::uintptr_t fn;        // +0x18  callback 0xb10 / 0xbb0
};
static_assert(sizeof(HandlerEntry1290) == kFn1290EntrySize);

// Original stack aggregate (A / B). Only A carries the +0x20 vcall argument slot,
// the +0x40 table and the +0x48 zero; the insert path reads them as arguments and
// never bulk-copies them (E522).
struct HandlerRegistration1290 {
    HandlerEntry1290 entry;
    void* self_arg[4];        // +0x20..+0x40  vcall receives &self_arg[0] == this+0x20
    std::uintptr_t table;     // +0x40
    std::uintptr_t zero_48;   // +0x48
};
static_assert(offsetof(HandlerRegistration1290, self_arg) == kFn1290SelfArgOffset);
static_assert(offsetof(HandlerRegistration1290, table) == kFn1290TableOffset);

struct Init1290Hooks {
    // 0x1bde30(owner=*(0x2ee68c8), &A). Null means the subsystem is unavailable: the
    // model reports ok=false instead of pretending success.
    void (*insert_1bde30)(void* owner, const HandlerRegistration1290& reg) = nullptr;
    // 0x1594060 slot 3 (0x19f0, bare ret) called with rdi = &A + 0x20.
    void (*vcall_1594060_slot3)(void* self_arg) = nullptr;
    // 0x21cd0(owner=*(0x2dfcbc8), &B). Its body is NOT lifted; the hook stays necessary.
    void (*insert_21cd0)(void* owner, const HandlerRegistration1290& reg) = nullptr;

    // Optional real path for the A insert: when `container_a` and `alloc_a.alloc` are set,
    // the lifted 0x1beeb0 body executes the 0x20 B copy and list link instead of the
    // `insert_1bde30` hook.
    Container1beeb0* container_a = nullptr;
    Insert1beeb0Alloc alloc_a{};

    // Optional real path for the B insert: when `container_b` is set, the lifted 0x21cd0
    // body executes the bump-allocator insert instead of the `insert_21cd0` hook. The
    // container must have `data`/`count`/`capacity` initialised and `sync_bump_end()` called.
    Container21cd0* container_b = nullptr;
    Insert21cd0Hooks insert_b_hooks{};
};

struct Init1290Result {
    bool ok = false;
    int insert_1bde30_count = 0;
    int vcall_count = 0;
    int insert_21cd0_count = 0;
    HandlerRegistration1290 reg_a{};
    HandlerRegistration1290 reg_b{};
    void* vcall_self_arg = nullptr;
    // True when the A insert ran the lifted 0x1beeb0 body (container + allocator path).
    bool insert_a_executed = false;
    Insert1beeb0Result inserted_a{};
    // True when the B insert ran the lifted 0x21cd0 body (bump-allocator path).
    bool insert_b_executed = false;
    Insert21cd0Result inserted_b{};
};

// Runs the recovered effect sequence. `self` is the object whose address is written
// into both descriptors' ctx field. Effect order is asserted by the unit test.
[[nodiscard]] Init1290Result init_1290(void* self, const Init1290Hooks& hooks) noexcept;

}  // namespace sotc
