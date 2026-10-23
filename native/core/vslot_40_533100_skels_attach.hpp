// App vslot `0x533100` shadowskels attach OBS `0x5335e1..0x5336f7` (278 B):
//   cmp success-flag [out+8]; je fail → r12=0  (measured default)
//   // success path (atexit/tag) NOT modelled when flag false
//   join @ 0x53365e: install loaded into *[f08+0x10] (replace + refcount)
//   flag byte at f08+0x18; optional notify if success; release temps; 0xf590
//   // stop before shadowanims @ 0x5336f7 (see vslot_40_533100_shadowanims.*)
//
// Always-on after E428 shadowskels load-attempt. Prefer over success atexit
// (flag-true only) and over deepen 0x14c2f0 / 0x6a5370 (already hooked).
// Prefer over empty-path migrate/non-null.
//
// Claimed: fail-path + always-on install/cleanup control flow into f08+0x10.
// NOT modelled: success atexit cluster (0xf8f3d0/0xf730/0x14c4a0/…);
// bodies of 0x14d880/0x14d2d0/0x14ccc0/0x95580. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

#include "platform_tag_147e20.hpp"
#include "shared_release_f590.hpp"
#include "shared_str_ctor_f1a0.hpp"

namespace sotc {

inline constexpr std::uint64_t kVslot40533100SkelsAttachRva = 0x5335e1;
inline constexpr std::size_t kVslot40533100SkelsAttachSize = 278;
inline constexpr std::uint64_t kVslot40SkelsAttachEndRva = 0x5336f7;
inline constexpr std::uint64_t kVslot40SkelsAttachSlotOff = 0x10;  // *[f08+0x10]
inline constexpr std::uint64_t kVslot40SkelsAttachFlagOff = 0x18;  // byte [f08+0x18]

// 0x14c2f0 literal (E582 §5.1); `****` is replaced by the runtime platform tag before hashing.
inline constexpr const char kSkelsKeyLiteral[] = "$/Models/****/shadowskels.crmp";
static_assert(sizeof(kSkelsKeyLiteral) - 1 == 30, "0x14c2f0 literal length (E582 §5.1)");

struct Vslot40_533100SkelsAttachHooks {
    // Models 0x14d880() → sentinel / empty marker. Null → nullptr.
    void* (*sentinel_14d880)() = nullptr;
    // Models 0x14ccc0(obj) release. Null → no-op.
    void (*release_14ccc0)(void* obj) = nullptr;
    // Models 0x14d2d0(obj) strong release. Null → no-op.
    void (*release_14d2d0)(void* obj) = nullptr;
    // Models 0x95580(f08+0x10) notify on success path. Null → no-op.
    void (*notify_95580)(void* slot_obj) = nullptr;
    // 0xf1a0 allocation seam for the key (tag 0 buffer, tag 1 control). A host must supply
    // `key_alloc.alloc.allocate`; a missing allocator or `platform_tag` makes this fragment
    // report ok == false instead of faking a key.
    SharedStrCtorHooks key_alloc{};
    // 0x147e20(*0x2ee2d00, fourcc) → 4-byte platform tag for the `****` rewrite.
    std::uint32_t (*platform_tag)(std::uint32_t fourcc) = platform_tag_147e20_default;
    // 0xf590 callees for the key holder (0x12af0 tag-0 destroy, 0x170b0 control free).
    SharedReleaseF590Hooks release{};
    // ResourcePool::GetResource ([[0x2ee2cf8]]+0x18) takes +1 on a map miss.
    bool resource_pool_miss = false;
};

struct Vslot40_533100SkelsAttachResult {
    bool ok = false;
    bool took_fail_path = false;  // measured default
    bool installed = false;
    void* prev_slot = nullptr;
    void* slot_after = nullptr;
    int release_14ccc0_count = 0;
    // 0xf1a0/0x14c2f0 key values and its 0xf590 release (replaces the path stand-in).
    SharedReleaseF590Result key_release{};
    std::int64_t key_size = 0;
    std::int64_t key_capacity = 0;
    std::uint32_t key_hash = 0;
    int dtor_f590_count = 0;
};

// Models attach from success-flag through cleanup. Stops before next path.
// `loaded` from E428; `success_flag` false on measured path.
[[nodiscard]] Vslot40_533100SkelsAttachResult vslot_40_533100_skels_attach(
    void* loaded, bool success_flag,
    const Vslot40_533100SkelsAttachHooks& hooks = {}) noexcept;

}  // namespace sotc
