// Owner vtable slot +0x48 (`0xb111e0`) and the two bodies it drives (`0x256970`, `0xa46dc0`).
//
// Naming: the file name follows the `owner_ctor_<hex>` convention for this lane, but `0xb111e0` is
// NOT a constructor and is NOT in `kOwnerMemberRows`. It is the concrete implementation of virtual
// slot `+0x48` of two owner vtables (`0x15c8cd8` and `0x15d5b80`; both also carry the same body at
// `+0x58`, see `owner_ctor_b11210_slot58`). `0x2569d0` is slot `+0x40` of the second table, and it
// calls slot `+0x48` at entry (`0x256a0d`) and slot `+0x58` at exit (`0x257068`). The row table's
// `this` fields are `+0x3b48/+0x3b50/+0x3b68/+0x3b70`; `+0x48`/`+0x58` are vtable slots, not fields.
//
// Decoded bodies (offline, byte-verified):
//   - `0xb111e0` (25 B): call `0x256970`; load `*(0x32f79a8)`; tail-jump `0xa46dc0(object)`.
//     It never reads `rdi`, so it does not touch its own `this`.
//   - `0x256970` (81 B): `new(0x210)` (`0x17070`), ctor `0xa46c70`, then replace the global cell
//     `0x32f79a8`: if the previous pointer differs and is non-null, `0xa46d10(previous)` then
//     `0x170b0(previous)`; finally store the new pointer. The same cell is the `this` of
//     `0xa473b0` (capture/streaming enable-state), so the singleton is the capture manager.
//   - `0xa46dc0` (101 B): `dword local = 0x2bc`; `0xf90e00(&local)` (`sceUserServiceInitialize`,
//     NID `j3YMu1MVNNo`); `0xf91360(1, 0x20000, 0x2bc, 0x3f)` (NID `j7DlalBzHh8`, the shadPS4
//     candidate name for that NID does not match this call shape, so it stays unnamed); then
//     `byte [object+0x178] = 1`; both return values are ignored (`mov al,1` at `0xa46e15`).
//
// Nothing here executes a guest address. The `0x32e79a8` cell is host storage, the allocator, the
// ctor/dtor pair and the two Sony imports are hooks, and a missing hook is a visible `false`.
// G1 PARTIAL: the singleton's own 0x210-byte layout (ctor `0xa46c70`) is not modelled.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kOwnerSlot48Rva = 0xB111E0;
inline constexpr std::size_t kOwnerSlot48Size = 0x19;
inline constexpr std::uint64_t kOwnerSingletonEnsureRva = 0x256970;
inline constexpr std::size_t kOwnerSingletonEnsureSize = 0x51;
inline constexpr std::uint64_t kOwnerSingletonGlobalRva = 0x32F79A8;
inline constexpr std::size_t kOwnerSingletonObjectSize = 0x210;
inline constexpr std::uint64_t kOwnerSingletonActivateRva = 0xA46DC0;
inline constexpr std::size_t kOwnerSingletonActivateSize = 0x65;
inline constexpr std::size_t kOwnerSingletonFlagOffset = 0x178;
inline constexpr std::int32_t kOwnerUserServicePriority = 0x2BC;
inline constexpr std::int32_t kOwnerShareUtilityArg0 = 1;
inline constexpr std::int32_t kOwnerShareUtilityArg1 = 0x20000;
inline constexpr std::int32_t kOwnerShareUtilityArg2 = 0x2BC;
inline constexpr std::int32_t kOwnerShareUtilityArg3 = 0x3F;

// Host storage for the guest global `*(0x32e79a8)`. The address is evidence only.
struct OwnerSingletonCell {
    void* object = nullptr;
};

struct OwnerSlot48Hooks {
    // 0x17070(size). Null => visible failure.
    void* (*allocate)(std::size_t size) = nullptr;
    // 0x170b0(ptr). Null => visible failure (it is also the failure-path release).
    void (*deallocate)(void* object) = nullptr;
    // 0xa46c70(new object). Null or false => visible failure; the new object is released.
    bool (*construct)(void* object) = nullptr;
    // 0xa46d10(previous object). Required only when the cell already holds a different pointer.
    void (*destroy)(void* object) = nullptr;
    // 0xf90e00(&{priority}) = sceUserServiceInitialize(0x2bc). Null => visible failure.
    void (*user_service_init)(std::int32_t priority) = nullptr;
    // 0xf91360(1, 0x20000, 0x2bc, 0x3f). Null => visible failure.
    void (*share_utility_call)(std::int32_t a1, std::int32_t a2, std::int32_t a3,
                               std::int32_t a4) = nullptr;
};

// 0x256970: allocate, construct, then replace the cell (destroy + free the previous object).
// The original cannot fail; the native model fails visibly on a missing hook or allocation.
[[nodiscard]] bool owner_singleton_ensure(OwnerSingletonCell& cell,
                                          const OwnerSlot48Hooks& hooks) noexcept;

// 0xa46dc0: user-service init, the NID `j7DlalBzHh8` call with the four observed constants, then
// `+0x178 = 1`. Both import results are ignored by the original; this returns false only when a
// hook is missing or `object` is null.
[[nodiscard]] bool owner_singleton_activate(void* object, const OwnerSlot48Hooks& hooks) noexcept;

// 0xb111e0: vtable slot `+0x48`. Ensure the singleton, then activate the cell's current object.
[[nodiscard]] bool owner_slot48_run(OwnerSingletonCell& cell,
                                    const OwnerSlot48Hooks& hooks) noexcept;

}  // namespace sotc
