// CPU-01-AOT-CFG-SCALE-01-BATCH20 (slot 494): native model of startup constructor slot 494 (RVA 0x26f7c0, 84 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 494. Every body fact below was read
// end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_494.py. NOTHING here executes,
// loads or calls an original RVA: destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run by the
// startup runner.
//
// Shape (the body): builder 0x26e340 on the zero-initialised 0x20-byte registry R0 (0x2eef038), __cxa_atexit(0x269590,
// R0, &dso); builder 0x26e770 on R1 (0x2eef058), tail-call __cxa_atexit(0x269610, R1, &dso).
//
// What a builder does (0x26e340 with 0x18-byte nodes, 0x26e770 with 0x20-byte nodes; the rest is identical):
//   R+0x08..0x20 = {0,0,0}                                    (registry vector<Pool*>: begin / end / cap; R+0x00 is never written)
//   P = allocate(0x28)  [0x17070]; P+0x08..0x20 = {0,0,0}     (pool block; P+0x00 is never written -- indeterminate)
//   reserve the pool's node vector P+0x08 to 0x1000 nodes     (0x26e5f0 for 0x18-byte nodes, 0x26ea10 for 0x20-byte nodes)
//   value-initialise the 0x1000 nodes to {0, 0xffffffff@+8, 0@+0x10 [, 0xffffffff@+0x18]}; the dword at +0xc is never written
//   P+0x20 = 0; for i = 0xfff .. 0: node[i].dword@8 = 0; node[i].next = node[i + 1] (node[0xfff].next = 0);  P+0x20 = node[0]
//   push_back(P) onto R's pointer vector (0x26e4d0 / 0x26e8f0 grow it: new capacity = max(size + 1, cap + cap / 2) pointers;
//   the aliasing branches of the push compare against a stack slot and are not reachable for a heap vector)
// Allocator-side callees are HOOKS (0x17070 = tagged allocate, 0x170b0 = tagged free; both tail-jump the TLS allocator
// dispatch, not lifted); 0xf8f480 is the memmove import.  The two length / bad-allocation throw paths (0x26e5d0 /
// 0x26e750, 0xf8f5f0) are unreachable for these sizes and are modelled as failure (the model returns false).
//
// The destructors 0x269590 / 0x269610 (identical bodies): for each non-null pool: free its node vector (if non-null), free the
// pool; then end = begin; if begin is non-null free it and zero begin / end / cap.  They are modelled here and exercised by the
// test; the startup runner only RECORDS them in the atexit log (process exit never runs inside the port).
#pragma once

#include <cstddef>
#include <cstdint>

#include "ctor_shapes_343_356.hpp"

namespace sotc {

// The hook-based identity model of the same slot in ctor_tiny_misc_slots.hpp (builders 0x26e340 / 0x26e770 as HOOKS) is superseded
// in the startup run by this model, which lifts the builders; the names below carry the "Pools" tag to coexist with it.
inline constexpr std::uint64_t kSlot494PoolsRva = 0x26f7c0;
inline constexpr std::size_t kSlot494PoolsSize = 84;
inline constexpr std::uint64_t kSlot494PoolsObj0Rva = 0x2eef038;
inline constexpr std::uint64_t kSlot494PoolsObj1Rva = 0x2eef058;
inline constexpr std::uint64_t kSlot494PoolsCtor0Rva = 0x26e340;   // builder, lifted here (0x18-byte nodes)
inline constexpr std::uint64_t kSlot494PoolsCtor1Rva = 0x26e770;   // builder, lifted here (0x20-byte nodes)
inline constexpr std::uint64_t kSlot494PoolsDtor0Rva = 0x269590;
inline constexpr std::uint64_t kSlot494PoolsDtor1Rva = 0x269610;
inline constexpr std::uint64_t kSlot494PoolReserve0Rva = 0x26e5f0;
inline constexpr std::uint64_t kSlot494PoolReserve1Rva = 0x26ea10;
inline constexpr std::uint64_t kSlot494PtrGrow0Rva = 0x26e4d0;
inline constexpr std::uint64_t kSlot494PtrGrow1Rva = 0x26e8f0;
inline constexpr std::size_t kSlot494PoolBlockSize = 0x28;
inline constexpr std::size_t kSlot494NodeCount = 0x1000;
inline constexpr std::size_t kSlot494Node0Size = 0x18;
inline constexpr std::size_t kSlot494Node1Size = 0x20;
inline constexpr std::uint32_t kSlot494NodeStateInit = 0xffffffffu;

// A pool block (0x28 B, from the allocator). `word0` and the dword at node +0xc are never written by the original.
struct Slot494PoolBlock {
    std::uint64_t word0;              // +0x00 never written
    std::byte* nodes_begin;           // +0x08
    std::byte* nodes_end;             // +0x10
    std::byte* nodes_cap;             // +0x18
    std::byte* free_head;             // +0x20
};
static_assert(sizeof(Slot494PoolBlock) == 0x28);

// The registry static (0x20 B, zero-initialised .bss): a pointer vector of pool blocks at +0x08.
struct Slot494Registry {
    std::uint64_t word0;              // +0x00 never written
    Slot494PoolBlock** begin;         // +0x08
    Slot494PoolBlock** end;           // +0x10
    Slot494PoolBlock** cap;           // +0x18
};
static_assert(sizeof(Slot494Registry) == 0x20);

struct Slot494Node0 {                 // 0x18 B
    Slot494Node0* next;               // +0x00
    std::uint32_t state;              // +0x08
    std::uint32_t word_0c;            // +0x0c never written
    std::uint64_t aux;                // +0x10
};
static_assert(sizeof(Slot494Node0) == kSlot494Node0Size);

struct Slot494Node1 {                 // 0x20 B
    Slot494Node1* next;               // +0x00
    std::uint32_t state;              // +0x08
    std::uint32_t word_0c;            // +0x0c never written
    std::uint64_t aux;                // +0x10
    std::uint32_t state2;             // +0x18
    std::uint32_t word_1c;            // +0x1c never written
};
static_assert(sizeof(Slot494Node1) == kSlot494Node1Size);

// Host allocator hooks. Both are required by every reachable path; a missing or failing hook makes the model return false
// and perform nothing past the missing call.
struct Slot494PoolHooks {
    void* (*allocate)(std::size_t size) = nullptr;     // 0x17070 (size, alignment 8, type 1)
    void (*release)(void* pointer) = nullptr;          // 0x170b0 (type 1)
};

// Heap-backed hooks (std::malloc / std::free) for hosts that have no tagged allocator of their own.
[[nodiscard]] Slot494PoolHooks slot494_default_pool_hooks() noexcept;

struct Slot494Objects {
    Slot494Registry r0;               // 0x2eef038
    Slot494Registry r1;               // 0x2eef058
    Slot494PoolHooks hooks{};
};

struct Slot494Result {
    bool ok = false;
    bool built0 = false;
    bool atexit0 = false;
    bool built1 = false;
    bool atexit1 = false;
};

// 0x26e4d0 / 0x26e8f0 / 0x26e5f0 / 0x26ea10: "make room for `count` more elements" on a begin / end / cap triple. False for the
// length_error / bad_alloc throw paths and for a missing or failing allocator hook (nothing past that call is performed).
[[nodiscard]] bool slot494_reserve_more(std::byte*& begin, std::byte*& end, std::byte*& cap, std::size_t element_size,
                                        std::size_t max_elements, std::size_t count, const Slot494PoolHooks& hooks) noexcept;
// 0x26e340 / 0x26e770 with the node size and the matching initialiser (0 = 0x18-byte nodes, 1 = 0x20-byte nodes).
[[nodiscard]] bool slot494_build_registry(Slot494Registry& registry, int node_kind, const Slot494PoolHooks& hooks) noexcept;
// 0x269590 / 0x269610 (identical bodies).
[[nodiscard]] bool slot494_destroy_registry(Slot494Registry& registry, const Slot494PoolHooks& hooks) noexcept;

[[nodiscard]] Slot494Result init_slot494_pools_26f7c0(Slot494Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
