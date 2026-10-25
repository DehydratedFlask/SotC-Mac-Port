// Common native model of the nine app-vslot `0x533100` attach blocks (E593/E595).
//
// Byte-verified order (sites 1-7 identical; 8-9 use the inlined slot variant):
//   K   = 0x14c2f0(literal)                       0x533461  key, control 1
//   ref = GetResource(K)                          0x533470  hit +1 / miss new wrapper refs 1
//   AddReference(ref.ptr)                         0x53347a  0x14ccb0
//   if (ref.owned) { RequestState(13); used=1 }   0x53347f  dead: GetResource sets owned=0
//   slot.assign(ref.ptr)                          0x5334f9  owned release, store, AddRef, RemoveRef(old)
//   if (used) { Lock(slot); ReleaseDependency(W) } 0x533535 dead
//   RemoveReference(W)                            0x53354a
//   if (ref.owned) ReleaseDependency(W)           0x533552  dead
//   RemoveReference(W)                            0x533565
//   release K                                     0x53356e..0x53357d 0xf590
//   Lock(slot, 13)                                0x53358c  sites 1-8
//   Lock(slot, 9)                                 0x53403c  site 9 tail (0x140d90)
//
// The previous fragment model treated `[out+8]` (the GetResource `owned` byte) as a caller
// "success flag" and `[out]` as a caller `loaded` pointer, then called an invented notify. E595
// corrects that: this block owns the fetch, the join, the cleanup and the lock. A missing pool or
// slot is a visible failure, because without the persistent slot the last RemoveReference would
// destroy the wrapper the site just stored.
#pragma once

#include <cstddef>
#include <cstdint>

#include "platform_tag_147e20.hpp"
#include "resource_pool.hpp"
#include "shared_release_f590.hpp"
#include "shared_str_ctor_f1a0.hpp"

namespace sotc {

struct Vslot40AttachCommonHooks {
    // Base of the persistent slot array: the f08 singleton for sites 1-7, the app object for 8-9.
    void* slot_base = nullptr;
    ResourcePool* pool = nullptr;
    ResourcePoolHooks pool_hooks{};
    std::uint32_t lock_priority = kResourceWrapperLockPriority13;
    // Site 9 locks later at 0x53403c through the 0x140d90 tail; the other eight lock in the block.
    bool lock_after_cleanup = true;
    SharedStrCtorHooks key_alloc{};
    std::uint32_t (*platform_tag)(std::uint32_t fourcc) = platform_tag_147e20_default;
    SharedReleaseF590Hooks release{};
};

struct Vslot40AttachCommonResult {
    bool ok = false;
    const char* error = "";
    bool key_built = false;
    bool get_ok = false;
    bool slot_assigned = false;
    bool lock_ok = false;
    ResourceWrapper* wrapper = nullptr;
    ResourceWrapper* prev_slot = nullptr;
    int add_reference_count = 0;
    int remove_reference_count = 0;
    SharedReleaseF590Result key_release{};
    std::int64_t key_size = 0;
    std::int64_t key_capacity = 0;
    std::uint32_t key_hash = 0;
    int dtor_f590_count = 0;
    std::uint32_t refs_after = 0;
    std::uint32_t deps_after = 0;
    std::uint32_t state_after = 0;
};

// Host initialization of the nine persistent site slots. The original app constructor that writes
// `{null_resource_wrapper(), false}` into this array is not lifted, so the native path performs it
// explicitly. Idempotent; a null base is skipped.
void vslot_40_533100_init_slots(void* f08, void* self) noexcept;

[[nodiscard]] Vslot40AttachCommonResult vslot_40_533100_attach_common(
    const char* literal, std::uint64_t slot_off,
    const Vslot40AttachCommonHooks& hooks) noexcept;

}  // namespace sotc
