// Owner vtable slot +0x58 (`0xb11210`): the RetailBuild-gated dev property insert.
//
// Naming: the file name follows the `owner_ctor_<hex>` convention for this lane, but `0xb11210` is
// NOT a constructor and is NOT in `kOwnerMemberRows`. It is the concrete implementation of virtual
// slot `+0x58` of the two owner vtables (`0x15c8cd8`, `0x15d5b80`); `0x2569d0`, which is slot
// `+0x40` of the second table, calls it at exit (`0x257068`). The body never reads `rdi`.
//
// Decoded body (105 B, byte-verified, compiler stack canary aside):
//   gate = *(u8*)(*(void**)(0x2dfce70 + 0x60));        0x2570a9 uses the same byte as "RetailBuild"
//   if (gate != 0) return;                              retail builds take no dev property
//   bag = *(void**)(0x3305568);                         0xac4f33 republishes this global
//   rec = {i32 0x42, f32 1.0f, u32 tag 2};              0x3f80000000000042 at [rbp-0x20]
//   0xac53e0(bag, key 4, &rec);                         tree 0 insert (handle bag+0x18)
//
// `0xac53e0` is the ordered-tree insert for tree 0 (`handle +0x18`, sentinel `+0x20`, 0x30 B nodes,
// rebalance `0xac8ff0`). It is a different sink from map A (`0xac5260`, handle `+0x48`) and map B
// (`0xac5320`, handle `+0x60`); it stays a hook here. A missing hook, a null bag and a rejecting
// sink all report `ok == false`; nothing is silently skipped.
// G1 PARTIAL: tree 0 storage itself is not modelled.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kOwnerSlot58Rva = 0xB11210;
inline constexpr std::size_t kOwnerSlot58Size = 0x69;
inline constexpr std::uint64_t kOwnerRetailGateRva = 0x2DFCE70;
inline constexpr std::size_t kOwnerRetailGateFieldOffset = 0x60;
inline constexpr std::uint64_t kOwnerBagGlobalRva = 0x3305568;
inline constexpr std::uint64_t kOwnerTree0InsertRva = 0xAC53E0;
inline constexpr std::size_t kOwnerTree0InsertSize = 0xB2;
inline constexpr std::size_t kOwnerBagTree0HandleOffset = 0x18;
inline constexpr std::size_t kOwnerBagTree0SentinelOffset = 0x20;
inline constexpr std::int32_t kOwnerSlot58Key = 0x4;
inline constexpr std::int32_t kOwnerSlot58ValueInt = 0x42;
inline constexpr float kOwnerSlot58ValueFloat = 1.0F;
inline constexpr std::uint32_t kOwnerSlot58ValueFloatBits = 0x3F800000;
inline constexpr std::uint32_t kOwnerSlot58Tag = 2;

struct OwnerSlot58Hooks {
    // The byte at `*(void**)(0x2dfce70+0x60)`; `true` means non-zero ("RetailBuild").
    bool (*retail_build)() = nullptr;
    // `*(void**)(0x3305568)`, the most recently constructed owner bag.
    void* (*bag_singleton)() = nullptr;
    // 0xac53e0(bag, key, rec). The original returns void; the bool is a host contract so a sink
    // that cannot complete is visible instead of silently dropped.
    bool (*insert_tree0)(void* bag, std::int32_t key, std::int32_t value_int, float value_float,
                         std::uint32_t tag) = nullptr;
};

struct OwnerSlot58Result {
    bool ok = false;
    bool gated = false;     // gate != 0: retail build, the insert was skipped
    bool inserted = false;  // the sink accepted key 4
};

// 0xb11210: insert key 4 (`i = 0x42`, `f = 1.0f`, tag 2) into tree 0 only when the gate byte is 0.
[[nodiscard]] OwnerSlot58Result owner_slot58_run(const OwnerSlot58Hooks& hooks) noexcept;

}  // namespace sotc
