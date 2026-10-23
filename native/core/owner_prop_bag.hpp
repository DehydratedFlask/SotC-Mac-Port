// Native reduction of the owner property bag (`[this+0x3b48]`, 0x178 B, E576).
//
// Recovered:
//   - bag factory `0xa2b960` (`new(0x178)` + `0xa2afb0`), base ctor `0xac4de0`, vptr
//     `0x15d1058`; the construction republishes the object to `*(0x3305568)` (last wins).
//   - four custom red-black trees; the property writers use tree 2 (A) and tree 3 (B), handles
//     at `+0x48` and `+0x60`, sentinels at `+0x50`/`+0x68`, counts at `+0x58`/`+0x70`.
//   - `0xac5260`/`0xac5320` are `insert_equal` (allocate first, no equality exit, duplicates
//     allowed), fixups `0xac8a90`/`0xac8d40`; key is a u32 enum immediate.
//   - payload A `{i32 i, f32 f = 1.0, u32 tag}`; payload B `{f32 lo, f32 hi, f32 v, u32 tag}`.
//
// The native model uses ordered multimaps, which preserve duplicate keys and their insertion
// order, and exposes lookup by key. It is a behaviour model, not the guest RB-tree ABI: trees 0/1,
// the exact balancing/fixup order, the node layout and the `*(0x3305568)` alias remain contracts.
// No guest code reads this memory.
//
// NOT modelled: `0xac4de0`/`0xa2afb0`/`0xa2b960` bodies, trees 0/1, erase (`0xac5aa0`), and the
// scatter `0xac62e0`/`0xac6360`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>
#include <map>

#include "owner_properties_2569d0.hpp"

namespace sotc {

inline constexpr std::uint64_t kOwnerPropBagFactoryRva = 0xA2B960;
inline constexpr std::uint64_t kOwnerPropBagBaseCtorRva = 0xAC4DE0;
inline constexpr std::uint64_t kOwnerPropBagDerivedCtorRva = 0xA2AFB0;
inline constexpr std::uint64_t kOwnerPropBagInsertARva = 0xAC5260;
inline constexpr std::uint64_t kOwnerPropBagInsertBRva = 0xAC5320;
inline constexpr std::uint64_t kOwnerPropBagFixupARva = 0xAC8A90;
inline constexpr std::uint64_t kOwnerPropBagFixupBRva = 0xAC8D40;
inline constexpr std::size_t kOwnerPropBagSize = 0x178;
inline constexpr std::size_t kOwnerPropBagTreeAOffset = 0x48;
inline constexpr std::size_t kOwnerPropBagTreeBOffset = 0x60;

struct OwnerValueA {
    std::int32_t i;
    float f;  // 1.0f in every recovered row
    std::uint32_t tag;
};

struct OwnerValueB {
    float lo;
    float hi;
    float v;
    std::uint32_t tag;
};

// `0xa2afb0` input-device user selection: one unconditional initial-user device plus up to three
// login-id devices. Negative ids are skipped, only the initial user is deduped, and the `r13 <= 2` cap
// drops the last of four distinct login ids. `login_list_ok == false` leaves the initial device.
struct BagUserSelection {
    int count = 0;
    int user_ids[4] = {-1, -1, -1, -1};
    bool login_list_ok = false;
};

[[nodiscard]] BagUserSelection select_bag_users(int initial_user, const int login_ids[4],
                                                bool login_list_ok) noexcept;

class OwnerPropBag;

struct OwnerBagConstructHooks {
    // 0xf90d70 scePadInit(); the original calls it before the user queries.
    bool (*sce_pad_init)() = nullptr;
    // 0xf901e0 sceUserServiceGetInitialUser(&int); -1 when no user is logged in.
    int (*initial_user)() = nullptr;
    // 0xf91300 sceSystemGestureInitializePrimitiveTouchRecognizer(0).
    bool (*gesture_init)() = nullptr;
    // 0xf90df0 sceUserServiceGetLoginUserIdList(&int[4]); false on failure.
    bool (*login_user_ids)(int out[4]) = nullptr;
    // *(0x3305568) = bag (last construction wins).
    void (*publish)(OwnerPropBag& bag) = nullptr;
};

struct OwnerBagConstructResult {
    bool ok = false;
    BagUserSelection users{};
    int tables_created = 0;
    bool pad_init_called = false;
    bool gesture_init_called = false;
    bool published = false;
};

class OwnerPropBag {
public:
    using TreeA = std::multimap<std::uint32_t, OwnerValueA>;
    using TreeB = std::multimap<std::uint32_t, OwnerValueB>;

    // `0xac5260` / `0xac5320`: insert_equal, duplicates preserved.
    void insert_a(std::uint32_t key, const OwnerValueA& value);
    void insert_b(std::uint32_t key, const OwnerValueB& value);

    [[nodiscard]] const TreeA& tree_a() const noexcept { return tree_a_; }
    [[nodiscard]] const TreeB& tree_b() const noexcept { return tree_b_; }
    // `+0x10`: base ctor writes 0; the derived ctor writes 1 last on every path.
    [[nodiscard]] int mode_10() const noexcept { return mode_10_; }
    void set_mode_10(int mode) noexcept { mode_10_ = mode; }

    [[nodiscard]] std::size_t size_a() const noexcept { return tree_a_.size(); }
    [[nodiscard]] std::size_t size_b() const noexcept { return tree_b_.size(); }
    [[nodiscard]] std::size_t total() const noexcept { return tree_a_.size() + tree_b_.size(); }

private:
    TreeA tree_a_;
    TreeB tree_b_;
    int mode_10_ = 0;
};

// `0xa2afb0` observable construction: pad/user/gesture hooks, the 1..4 user selection, the final
// `+0x10 = 1`, and the global republish. The four 0xc88 input devices are opaque and null-safe for
// every analyzed reader, so the native model records the selection but does not allocate them.
[[nodiscard]] OwnerBagConstructResult owner_bag_construct(
    OwnerPropBag& bag, const OwnerBagConstructHooks& hooks) noexcept;

// Adapter that lets `apply_owner_properties` feed the bag directly.
class OwnerPropBagSink final : public OwnerPropertySink {
public:
    explicit OwnerPropBagSink(OwnerPropBag& bag) noexcept : bag_(bag) {}

    bool insert_a(std::uint16_t key, std::int32_t i, float f, std::uint32_t tag) override;
    bool insert_b(std::uint16_t key, float lo, float hi, float v,
                  std::uint32_t tag) override;

private:
    OwnerPropBag& bag_;
};

}  // namespace sotc
