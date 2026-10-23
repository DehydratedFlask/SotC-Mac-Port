// App vslot `0x533100` shadowbanks attach OBS `0x5338d3..0x5339e9` (278 B):
//   cmp success-flag [out+8]; je fail → r12=0  (measured default)
//   // success path (atexit/tag) NOT modelled when flag false
//   join @ 0x53394d: install loaded into *[f08+0x40] (replace + refcount)
//   flag byte at f08+0x48; optional notify if success; release temps; 0xf590
//   // stop before shadowstreams @ 0x5339e9 (see vslot_40_533100_shadowstreams.*)
//
// Always-on after E432 shadowbanks load-attempt. Prefer over success atexit
// (flag-true only), over jump to shadowstreams, and over deepen 0x14c2f0 /
// 0x6a5370 (already hooked). Prefer over empty-path migrate/non-null.
//
// Claimed: fail-path + always-on install/cleanup control flow into f08+0x40.
// NOT modelled: success atexit cluster (0xf8f3d0/0xf730/0x14c4a0/…);
// bodies of 0x14d880/0x14d2d0/0x14ccc0/0x95580; shadowstreams. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

#include "platform_tag_147e20.hpp"
#include "shared_release_f590.hpp"
#include "shared_str_ctor_f1a0.hpp"

namespace sotc {

inline constexpr std::uint64_t kVslot40533100BanksAttachRva = 0x5338d3;
inline constexpr std::size_t kVslot40533100BanksAttachSize = 278;
inline constexpr std::uint64_t kVslot40BanksAttachEndRva = 0x5339e9;
inline constexpr std::uint64_t kVslot40BanksAttachSlotOff = 0x40;  // *[f08+0x40]
inline constexpr std::uint64_t kVslot40BanksAttachFlagOff = 0x48;  // byte [f08+0x48]

// 0x14c2f0 literal (E582 §5.1); `****` is replaced by the runtime platform tag before hashing.
inline constexpr const char kBanksKeyLiteral[] = "$/sound/shadow/****/shadowbanks.crmp";
static_assert(sizeof(kBanksKeyLiteral) - 1 == 36, "0x14c2f0 literal length (E582 §5.1)");

struct Vslot40_533100BanksAttachHooks {
    void* (*sentinel_14d880)() = nullptr;
    void (*release_14ccc0)(void* obj) = nullptr;
    void (*release_14d2d0)(void* obj) = nullptr;
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

struct Vslot40_533100BanksAttachResult {
    bool ok = false;
    bool took_fail_path = false;
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

[[nodiscard]] Vslot40_533100BanksAttachResult vslot_40_533100_banks_attach(
    void* loaded, bool success_flag,
    const Vslot40_533100BanksAttachHooks& hooks = {}) noexcept;

}  // namespace sotc
