// Native model of the list-A half of the `0x1bdf30` collection path.
//
// `0x1bdf30` is not only a delegate dispatcher. After the list-C loop it:
//   1. optionally inserts two borrowed strings into list B through `0x1bdc80` (0x50 B nodes),
//   2. serializes every list-A entry into a report object at `0x1be14d..0x1be222`,
//   3. deletes every list-A node whose `dword [node+0x50] == 1` at `0x1be3f4..0x1be456`,
//   4. clears list B.
//
// This module models steps 2 and 3, the part the native app-info registry needs. The report type is
// not recovered, so the visitor receives the two recovered 0x20 B string holders instead of a
// fabricated FieldEntry. The visitor must not mutate the registry.
//
// Byte-verified drain order (`0x1be400`): walk from `[sentinel+0x00]` and follow `[node+0x00]`;
// when `flag == 1`, advance first, recover the victim as `prev->next`, unlink it, release `+0x30`
// then `+0x10` through `0xf590`, free through `0x170c0` (tag 2), and decrement the 64-bit count at
// `sentinel+0x10`. Nodes with any other flag are left for the `0x1bdeb0` purge (which deletes
// `flag == 0`).
//
// NOT modelled here: the report/FieldEntry construction and list B (see `appinfo_listb`), and the
// `snapshotBugData` gate at `0x25db56` (the only caller path). G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

#include "app_info_registry.hpp"
#include "shared_release_f590.hpp"

namespace sotc {

inline constexpr std::uint64_t kAppInfoCollectReaderRva = 0x1BE14D;
inline constexpr std::uint64_t kAppInfoCollectReaderEndRva = 0x1BE222;
inline constexpr std::uint64_t kAppInfoDrainRva = 0x1BE3F4;
inline constexpr std::uint64_t kAppInfoDrainEndRva = 0x1BE456;
inline constexpr std::uint64_t kAppInfoDrainFreeRva = 0x170C0;
inline constexpr std::uint64_t kAppInfoDrainReleaseRva = 0xF590;
inline constexpr std::uint32_t kAppInfoDrainFlag = 1;

// Receives one entry. `name`/`value` are the 0x20 B holder contents as views; a holder with a null
// data pointer yields an empty view. The text may be owned by the node or borrowed; either way it
// is valid only for the duration of the call.
using AppInfoVisitor = void (*)(void* ctx, std::string_view name, std::string_view value);

struct AppInfoCollectHooks {
    SharedReleaseF590Hooks release{};
    void (*free_tag2)(void* node) = nullptr;
};

struct AppInfoCollectResult {
    bool ok = false;
    int visited = 0;
    int drained = 0;
    int releases = 0;          // two per drained node
    int release_failures = 0;
    std::uint32_t count_after = 0;
};

struct AppInfoDrainResult {
    bool ok = false;
    int drained = 0;
    int releases = 0;          // two per drained node
    int release_failures = 0;
    std::uint32_t count_after = 0;
};

// Visits every list-A entry in the recovered traversal order, then drains every `flag == 1` node.
// A null visitor or free hook is a visible failure and leaves the list untouched. A release failure
// is reported but does not stop the drain, matching the original's unconditional unlink/free.
[[nodiscard]] AppInfoCollectResult collect_app_info(AppInfoRegistry& registry, AppInfoVisitor visitor,
                                                    void* ctx,
                                                    const AppInfoCollectHooks& hooks) noexcept;

// 0x1be3f4..0x1be456 only: delete every list-A node whose flag is exactly 1, without visiting.
// A null free hook is a visible failure and leaves the list untouched.
[[nodiscard]] AppInfoDrainResult drain_app_info(AppInfoRegistry& registry,
                                                const AppInfoCollectHooks& hooks) noexcept;

}  // namespace sotc
