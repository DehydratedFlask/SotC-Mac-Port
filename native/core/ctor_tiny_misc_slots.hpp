// CPU-01-TINY-MISC-SLOTS-01 (lane L1-TINY): native recovery of twelve original constructor-table slots
// whose body is at most 0x60 bytes but is not the plain "zero cell + atexit" idiom (that family is
// core/ctor_tiny_atexit_slots). ELF sha256 d0723afb...5861d3; offline analysis only. NOTHING here executes,
// loads or calls an original RVA: destructor RVAs, the dso handle and every callee RVA below are IDENTITY
// TAGS; destructors are recorded in the StartupAtexitLog and never run; unlifted callees are HOOKS and
// the fragment fails closed (ok == false, named `blocked` stage, no write when the hook is absent).
//
//   404 0x5e7c10  memset(0x30061a0, 0, 0x51f8)                                  (PLT 0xf8f4f0 = memset)
//   406 0x5dbe10  twice: 0x8bd90(obj) then atexit(0x8bdb0, obj); objs 0x30015c8 / 0x30015f0 (0x28 B);
//                 0x8bd90 (native here): qword [obj+0x10] = 0; 16 B constant at [obj+0x18]
//   409 0x5b50f0  zero 16 B at obj+8; node = alloc 0x40 (0x17070, HOOK); node.{left,parent,right} = node,
//                 node.color = node.isnil = 1; obj.head(+8) = node; atexit(0x5b3d10, obj 0x2fda208)
//   414 0x57b9b0  mutex init 0x3b310(obj 0x2f35898) (HOOK); atexit(0x3b380, obj); u32 [obj+8] = 0;
//                 16 B zero at obj+0x10
//   494 0x26f7c0  0x26e340(obj0 0x2eef038) (HOOK); atexit(0x269590, obj0); 0x26e770(obj1 0x2eef058)
//                 (HOOK); atexit(0x269610, obj1)
//   517 0x1aa510  cell 0x2ee6720 = 0; atexit(0x1aa070, cell); then 0x326a0(rec 0x2ee6728, 0): byte
//                 rec.flag = 0, TSC stamp rec.tsc (the guard/base cells 0x2ea4700/0x2ea4708 shared with
//                 core/store_tsc_323d0), qword rec.zero = 0
//   519 0x19d2d0  zero 24 B at obj 0x2ee4548; atexit(0x19a7b0, obj); memset(0x2ee4568, 0, 0x2000);
//                 xchg qword [0x2ee6570], 0
//   535 0x138e20  mutex init 0x3b310(m 0x2ee14f0) (HOOK); atexit(0x3b380, m); zero [obj+8, obj+0x20) of
//                 obj 0x2ee14f8; atexit(0x138370, obj)
//   541 0x0fd210  u32 [0x2edef68] = fnv_path(0xfa503b, seed 0x811c9dc5), u32 [0x2edef6c] = fnv_path(0xfa5040,
//                 seed) with 0x162020 (native fnv_path_cstr_162020); the two names are "wind" and
//                 "mesh_instance" (4 and 13 ASCII bytes, pinned by the checker)
//   553 0x08bd40  struct at 0x2ec0120: [+0x00] = [+0x08] = [+0x20] = &struct[+0x28]; [+0x10] = 0x2ed1f48
//                 (external object, stand-in here); atexit(0x8a620, struct)
//   562 0x036100  cell 0x2ea4718 = 0; atexit(0x33280, cell); xchg dword [0x2ea4720], 6
//   563 0x032c70  the TSC store of 0x323d0 inlined (qword guard 0x2ea4700, base 0x2ea4708), stamp at
//                 0x2ea4710 (core/store_tsc_323d0 cells, shared process-wide)
//
// Byte-level contract: every write is confined to the documented ranges and every other byte of the caller
// buffer is left alone (tests fill the state with 0xA5 guard bytes). Objects are caller-owned buffers; the
// original object addresses are identity only. The ordering of observable effects equals the original.
//
// Shared cells (not aliased by this module): 0x2ea4700/0x2ea4708 are the process-wide TSC cells of
// core/store_tsc_323d0 and ARE shared; the other objects are state-owned stand-ins.
//
// Slots 414 and 535 register the critical-section destroy `0x3b380` with `__cxa_atexit` and never call it.
// `teardown_slot414_57b9b0` / `teardown_slot535_138e20` are host-only opt-ins that run that registered
// identity (hook present, log entry still present, hook succeeds) and leave the handle intact on every
// rejection. They are not wired into any production path; see core/critsec_3b310 for the 8-byte layout.
#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "ctor_tiny_atexit_slots.hpp"
#include "store_tsc_323d0.hpp"

namespace sotc {

struct TinyMiscResult {
    bool ok = false;
    const char* blocked = nullptr;   // named stage when ok == false
};

// ----------------------------------------------------------------------------------------------- 404
inline constexpr std::uint64_t kSlot404Rva = 0x5e7c10;
inline constexpr std::uint32_t kSlot404Size = 19;
inline constexpr std::uint64_t kSlot404BufferRva = 0x30061a0;
inline constexpr std::size_t kSlot404BufferSize = 0x51f8;
struct Slot404Buffer {
    std::array<std::uint8_t, kSlot404BufferSize> bytes{};
};
void init_slot404_5e7c10(Slot404Buffer& buffer) noexcept;

// ----------------------------------------------------------------------------------------------- 406
inline constexpr std::uint64_t kSlot406Rva = 0x5dbe10;
inline constexpr std::uint32_t kSlot406Size = 92;
inline constexpr std::uint64_t kSlot406Obj0Rva = 0x30015c8;
inline constexpr std::uint64_t kSlot406Obj1Rva = 0x30015f0;
inline constexpr std::uint64_t kSlot406InitRva = 0x8bd90;     // native below (identity tag)
inline constexpr std::uint64_t kSlot406DtorRva = 0x8bdb0;     // recorded, never run
inline constexpr std::uint64_t kSlot406ConstRva = 0xf9ebd0;   // the 16 B constant's rodata address
inline constexpr std::uint64_t kSlot406ConstLo = 0x0000001d00000000ull;
inline constexpr std::uint64_t kSlot406ConstHi = 0xffffffff00000000ull;
inline constexpr std::size_t kSlot406ObjectSize = 0x28;
struct Slot406State {
    std::array<std::array<std::uint8_t, kSlot406ObjectSize>, 2> obj{};
};
[[nodiscard]] TinyMiscResult init_slot406_5dbe10(Slot406State& state, StartupAtexitLog& log) noexcept;

// ----------------------------------------------------------------------------------------------- 409
inline constexpr std::uint64_t kSlot409Rva = 0x5b50f0;
inline constexpr std::uint32_t kSlot409Size = 89;
inline constexpr std::uint64_t kSlot409ObjRva = 0x2fda208;
inline constexpr std::uint64_t kSlot409DtorRva = 0x5b3d10;
inline constexpr std::uint64_t kSlot409AllocRva = 0x17070;    // allocator wrapper, HOOK
inline constexpr std::size_t kSlot409NodeSize = 0x40;
inline constexpr std::size_t kSlot409NodeColorOffset = 0x18;
inline constexpr std::size_t kSlot409NodeIsNilOffset = 0x19;
struct Slot409Object {
    std::uint64_t unwritten0 = 0;          // +0x00 never written by the constructor
    void* head = nullptr;                  // +0x08 -> sentinel node
    std::uint64_t size = 0;                // +0x10 zero
};
static_assert(sizeof(Slot409Object) == 0x18);
struct Slot409Hooks {
    // Models 0x17070(size): a 0x40-byte block; its bytes are NOT initialised by the original except the
    // five fields above. Null hook (or a null result) fails closed.
    void* (*alloc_17070)(std::size_t size) noexcept = nullptr;
};
struct Slot409State {
    Slot409Object obj{};
    void* node = nullptr;                  // the block returned by the allocator hook (caller-owned)
};
// Host stand-in allocator (std::malloc), NOT the engine allocator; release with std::free.
[[nodiscard]] Slot409Hooks default_slot409_hooks() noexcept;
[[nodiscard]] TinyMiscResult init_slot409_5b50f0(Slot409State& state, const Slot409Hooks& hooks,
                                                 StartupAtexitLog& log) noexcept;

// ----------------------------------------------------------------------------------------------- 414
inline constexpr std::uint64_t kSlot414Rva = 0x57b9b0;
inline constexpr std::uint32_t kSlot414Size = 72;
inline constexpr std::uint64_t kSlot414ObjRva = 0x2f35898;
inline constexpr std::uint64_t kSlot414MutexInitRva = 0x3b310;   // HOOK
inline constexpr std::uint64_t kSlot414DtorRva = 0x3b380;        // recorded, never run
inline constexpr std::size_t kSlot414ObjectSize = 0x20;
struct Slot414State {
    std::array<std::uint8_t, kSlot414ObjectSize> obj{};   // +0 mutex handle (hook), +8 u32 0, +0x10 16 B 0
};
struct Slot414Hooks {
    // Models 0x3b310(obj): initialises the 8-byte mutex handle at obj+0. Null or false fails closed.
    bool (*mutex_init_3b310)(void* obj) noexcept = nullptr;
    // Models 0x3b380(obj): optional explicit destroy (atexit identity in the original). Null skips.
    bool (*mutex_destroy_3b380)(void* obj) noexcept = nullptr;
};
[[nodiscard]] TinyMiscResult init_slot414_57b9b0(Slot414State& state, const Slot414Hooks& hooks,
                                                 StartupAtexitLog& log) noexcept;

// Host-only teardown of the identity that 0x57b9b0 registered. OBSERVED: the constructor only records
// `__cxa_atexit(0x3b380, obj)`; it never calls 0x3b380 itself, and nothing in this project's production
// path drains StartupAtexitLog (E1050), so no caller is implied. These helpers are the explicit opt-in
// that makes the registered identity executable by a host that owns the drain decision.
//
// Contract: the destroy hook must be present, the log must still hold that exact (identity, object) pair,
// and the hook must succeed. Every rejection is visible and leaves the 8-byte handle untouched, so a
// caller can retry. Retail contention/teardown ordering is not established and is not claimed here.
[[nodiscard]] TinyMiscResult teardown_slot414_57b9b0(Slot414State& state, const Slot414Hooks& hooks,
                                                     const StartupAtexitLog& log) noexcept;

// ----------------------------------------------------------------------------------------------- 494
inline constexpr std::uint64_t kSlot494Rva = 0x26f7c0;
inline constexpr std::uint32_t kSlot494Size = 84;
inline constexpr std::uint64_t kSlot494Obj0Rva = 0x2eef038;
inline constexpr std::uint64_t kSlot494Obj1Rva = 0x2eef058;
inline constexpr std::uint64_t kSlot494Ctor0Rva = 0x26e340;      // HOOK (not lifted)
inline constexpr std::uint64_t kSlot494Ctor1Rva = 0x26e770;      // HOOK (not lifted)
inline constexpr std::uint64_t kSlot494Dtor0Rva = 0x269590;
inline constexpr std::uint64_t kSlot494Dtor1Rva = 0x269610;
inline constexpr std::size_t kSlot494ObjectSize = 0x20;
struct Slot494State {
    std::array<std::array<std::uint8_t, kSlot494ObjectSize>, 2> obj{};
};
struct Slot494Hooks {
    // 0x26e340(obj0) and 0x26e770(obj1): the bodies allocate and fill 0x1000-element arrays and are not
    // lifted. Null or false fails closed at the named stage.
    bool (*ctor_26e340)(void* obj) noexcept = nullptr;
    bool (*ctor_26e770)(void* obj) noexcept = nullptr;
};
[[nodiscard]] TinyMiscResult init_slot494_26f7c0(Slot494State& state, const Slot494Hooks& hooks,
                                                 StartupAtexitLog& log) noexcept;

// ----------------------------------------------------------------------------------------------- 517
inline constexpr std::uint64_t kSlot517Rva = 0x1aa510;
inline constexpr std::uint32_t kSlot517Size = 56;
inline constexpr std::uint64_t kSlot517CellRva = 0x2ee6720;
inline constexpr std::uint64_t kSlot517RecordRva = 0x2ee6728;
inline constexpr std::uint64_t kSlot517DtorRva = 0x1aa070;
inline constexpr std::uint64_t kSlot517StampCalleeRva = 0x326a0;
struct Slot517Record {
    std::uint8_t flag = 0;                 // +0x00 byte (sil == 0)
    std::uint64_t tsc = 0;                 // +0x08 stamp
    std::uint64_t zero = 0;                // +0x10
};
struct Slot517State {
    std::uint64_t cell = 0;                // 0x2ee6720
    Slot517Record record{};                // 0x2ee6728
};
[[nodiscard]] TinyMiscResult init_slot517_1aa510(Slot517State& state, StartupAtexitLog& log,
                                                 const StoreTsc323d0Hooks& tsc_hooks = {}) noexcept;

// ----------------------------------------------------------------------------------------------- 519
inline constexpr std::uint64_t kSlot519Rva = 0x19d2d0;
inline constexpr std::uint32_t kSlot519Size = 83;
inline constexpr std::uint64_t kSlot519ObjRva = 0x2ee4548;
inline constexpr std::uint64_t kSlot519BufferRva = 0x2ee4568;
inline constexpr std::uint64_t kSlot519CellRva = 0x2ee6570;
inline constexpr std::uint64_t kSlot519DtorRva = 0x19a7b0;
inline constexpr std::size_t kSlot519ObjectSize = 0x18;
inline constexpr std::size_t kSlot519BufferSize = 0x2000;
struct Slot519State {
    std::array<std::uint8_t, kSlot519ObjectSize> obj{};
    std::array<std::uint8_t, kSlot519BufferSize> buffer{};
    std::atomic<std::uint64_t> cell{0};
};
[[nodiscard]] TinyMiscResult init_slot519_19d2d0(Slot519State& state, StartupAtexitLog& log) noexcept;

// ----------------------------------------------------------------------------------------------- 535
inline constexpr std::uint64_t kSlot535Rva = 0x138e20;
inline constexpr std::uint32_t kSlot535Size = 96;
inline constexpr std::uint64_t kSlot535MutexRva = 0x2ee14f0;
inline constexpr std::uint64_t kSlot535ObjRva = 0x2ee14f8;
inline constexpr std::uint64_t kSlot535MutexDtorRva = 0x3b380;
inline constexpr std::uint64_t kSlot535ObjDtorRva = 0x138370;
inline constexpr std::size_t kSlot535MutexSize = 8;
inline constexpr std::size_t kSlot535ObjectSize = 0x20;
struct Slot535State {
    std::array<std::uint8_t, kSlot535MutexSize> mutex{};
    std::array<std::uint8_t, kSlot535ObjectSize> obj{};   // zero [+8, +0x20)
};
[[nodiscard]] TinyMiscResult init_slot535_138e20(Slot535State& state, const Slot414Hooks& hooks,
                                                 StartupAtexitLog& log) noexcept;

// Host-only teardown of the mutex identity that 0x138e20 registered (`__cxa_atexit(0x3b380, mutex)`).
// Same contract as teardown_slot414_57b9b0. OBSERVED ordering fact, deliberately NOT executed here: the
// second record is `__cxa_atexit(0x138370, obj)`, so a real reverse-order drain would run the object
// destructor 0x138370 before this mutex destroy. Whether to drain at all is still an open host decision.
[[nodiscard]] TinyMiscResult teardown_slot535_138e20(Slot535State& state, const Slot414Hooks& hooks,
                                                     const StartupAtexitLog& log) noexcept;

// ----------------------------------------------------------------------------------------------- 541
inline constexpr std::uint64_t kSlot541Rva = 0xfd210;
inline constexpr std::uint32_t kSlot541Size = 52;
inline constexpr std::uint64_t kSlot541CellRva = 0x2edef68;
inline constexpr std::uint64_t kSlot541Name0Rva = 0xfa503b;
inline constexpr std::uint64_t kSlot541Name1Rva = 0xfa5040;
inline constexpr char kSlot541Name0[] = "wind";
inline constexpr char kSlot541Name1[] = "mesh_instance";
struct Slot541State {
    std::uint32_t hash0 = 0;               // 0x2edef68
    std::uint32_t hash1 = 0;               // 0x2edef6c
};
void init_slot541_fd210(Slot541State& state) noexcept;

// ----------------------------------------------------------------------------------------------- 553
inline constexpr std::uint64_t kSlot553Rva = 0x8bd40;
inline constexpr std::uint32_t kSlot553Size = 72;
inline constexpr std::uint64_t kSlot553ObjRva = 0x2ec0120;
inline constexpr std::uint64_t kSlot553SentinelOffset = 0x28;      // 0x2ec0148 - 0x2ec0120
inline constexpr std::uint64_t kSlot553ExternalRva = 0x2ed1f48;
inline constexpr std::uint64_t kSlot553DtorRva = 0x8a620;
inline constexpr std::size_t kSlot553ObjectSize = 0x30;
struct Slot553State {
    alignas(8) std::array<std::uint8_t, kSlot553ObjectSize> obj{};
    alignas(8) std::array<std::uint8_t, 0x10> external{};   // stand-in for the object at 0x2ed1f48
};
[[nodiscard]] TinyMiscResult init_slot553_8bd40(Slot553State& state, StartupAtexitLog& log) noexcept;

// ----------------------------------------------------------------------------------------------- 562
inline constexpr std::uint64_t kSlot562Rva = 0x36100;
inline constexpr std::uint32_t kSlot562Size = 50;
inline constexpr std::uint64_t kSlot562CellRva = 0x2ea4718;
inline constexpr std::uint64_t kSlot562FlagRva = 0x2ea4720;
inline constexpr std::uint64_t kSlot562DtorRva = 0x33280;
inline constexpr std::uint32_t kSlot562FlagValue = 6;
struct Slot562State {
    std::uint64_t cell = 0;
    std::atomic<std::uint32_t> flag{0};
};
[[nodiscard]] TinyMiscResult init_slot562_36100(Slot562State& state, StartupAtexitLog& log) noexcept;

// ----------------------------------------------------------------------------------------------- 563
inline constexpr std::uint64_t kSlot563Rva = 0x32c70;
inline constexpr std::uint32_t kSlot563Size = 72;
inline constexpr std::uint64_t kSlot563StampRva = 0x2ea4710;
struct Slot563Stamp {
    std::uint64_t tsc = 0;
};
void init_slot563_32c70(Slot563Stamp& stamp, const StoreTsc323d0Hooks& tsc_hooks = {}) noexcept;

}  // namespace sotc
