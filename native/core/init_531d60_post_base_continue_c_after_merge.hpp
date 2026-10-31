// Module init_531d60_post_base_continue_c_after_merge — merge fall-through after continue C.
//
// First instruction after continue C's pinned span (`kInit531d60PostBaseContinueCEndRva`):
//   0x53299d  lea rax,[0x32f79a8]   (7 B)  — post-end lea head verified by continue-C checker
//   … same four units as batch D (E535); execution uses `init_531d60_post_base_continue_d`.
//
// Claimed: entry/end RVAs, span size, and chaining from continue C's merge `je 0x5329ae` boundary
// into the fall-through block when the branch is not taken (or outcome unmodelled, matching slice).
// NOT modelled: continue E (`0x5329ae` taken path), `0xa473b0` body unless capture hooks supplied,
// cell value at `0x32f79a8`, merge flag provenance.
#pragma once

#include "init_531d60_post_base_continue_c.hpp"
#include "init_531d60_post_base_continue_d.hpp"

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseContinueCAfterMergeRva = 0x53299d;
inline constexpr std::size_t kInit531d60PostBaseContinueCAfterMergeSize = 17;
inline constexpr std::uint64_t kInit531d60PostBaseContinueCAfterMergeEndRva = 0x5329ae;
inline constexpr std::uint64_t kInit531d60PostBaseContinueCAfterMergeCellRva = 0x32f79a8;
inline constexpr std::uint64_t kInit531d60PostBaseContinueCAfterMergeCallRva = 0xa473b0;

static_assert(kInit531d60PostBaseContinueCAfterMergeRva ==
                  kInit531d60PostBaseContinueCEndRva,
              "continue C end must equal merge fall-through entry");
static_assert(kInit531d60PostBaseContinueCAfterMergeRva ==
                  kInit531d60PostBaseContinueDRva,
              "after-merge entry matches batch D");
static_assert(kInit531d60PostBaseContinueCAfterMergeEndRva ==
                  kInit531d60PostBaseContinueDEndRva,
              "after-merge end matches batch D");

struct InitContinueCAfterMergeResult {
    Init531d60PostBaseContinueCResult cont_c{};
    Init531d60PostBaseContinueDResult fallthrough{};
    bool ran_fallthrough = false;
};

// Runs continue C, then batch D when the merge branch is not taken (or je outcome unmodelled),
// matching `run_appmain_init_ctor_98a90_slice` fall-through policy.
[[nodiscard]] InitContinueCAfterMergeResult run_init_continue_c_after_merge(
    const Init531d60PostBaseContinueCHooks& c_hooks,
    const Init531d60PostBaseContinueDHooks& d_hooks = {}) noexcept;

}  // namespace sotc
