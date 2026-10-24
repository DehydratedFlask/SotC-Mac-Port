// Native lift of the `snapshotBugData` BugData block (`0x25d8b0..0x25dcee`, E585).
//
// `0x25db56` is not a function; it is the block reached from `0x25da58` inside `0x25d8b0`
// (1086 B, sha256 `e3eeb549…`). The block:
//   - reads the debug gate byte at `[*(0x2eed670)]` (the `"snapshotBugData"` variable),
//   - clears it with `0xba40` at `0x25db7b` **before** building, so it fires once per arming,
//   - calls `0x1bdf30(*(0x2ee68c8), *(0x2ed3670), "", "")` at `0x25db9e`,
//   - when the screenshot flag is 0, emits `"Command"` + `name="screenshot"` + value
//     `"bug_screenshot"` (`0x1be236..0x1be256`), otherwise inserts list-B
//     `("Screenshot", "bug_screenshot.png")`,
//   - adopts the root through `0xa70d10(&ctx+0x48, root)` (ownership transfer, no copy).
//
// The native seam keeps the gate, the four gate/flag cases and the report composition. The GPU
// readback `0x961a0`, the file sink, `frameNum`, the surface and the `0x25c080` poll frequency are
// caller contracts; a native `query_screenshot` returns true only when it produced a real payload.
//
// `0x25c080` is a per-tick virtual with no `E8` caller; "once per frame" is inferred, not proven.
// G1 PARTIAL.
#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "app_info_run.hpp"
#include "insert_1bdc80.hpp"
#include "screenshot_contract.hpp"

namespace sotc {

inline constexpr std::uint64_t kSnapshotBugDataBodyRva = 0x25D8B0;
inline constexpr std::size_t kSnapshotBugDataBodySize = 1086;
inline constexpr std::uint64_t kSnapshotBugDataGateVarRva = 0x2EED610;
inline constexpr std::uint64_t kSnapshotBugDataGateValueRva = 0x2EED6D0;
inline constexpr std::uint64_t kSnapshotBugDataGateClearRva = 0x25DB7B;
inline constexpr std::uint64_t kSnapshotBugDataCallRva = 0x25DB9E;
inline constexpr std::uint64_t kScreenshotQueryRva = 0x961A0;
inline constexpr std::uint64_t kFrameRootGetRva = 0xA70C30;
inline constexpr std::uint64_t kFrameRootAdoptRva = 0xA70D10;
inline constexpr const char kSyntheticCommandType[] = "Command";
inline constexpr const char kSyntheticCommandKey[] = "name";
inline constexpr const char kSyntheticCommandProp[] = "screenshot";
inline constexpr const char kSyntheticCommandValue[] = "bug_screenshot";
inline constexpr const char kScreenshotListBName[] = "Screenshot";
inline constexpr const char kScreenshotListBValue[] = "bug_screenshot.png";

struct SnapshotBugDataHooks {
    // Gate observed at poll time. The original clears it before the build.
    bool armed = false;
    // Screenshot source presence (`*(0x2ed3670)`). Null is legal and forces the synthetic entry.
    bool arg2_present = false;
    // Preferred E597 seam: the real capture/sink decision table. When non-null it takes precedence
    // over `query_screenshot`; `attached` decides the synthetic entry and the list-B insert.
    const ScreenshotHooks* screenshot = nullptr;
    FrameImage screenshot_frame{};
    // Legacy raw capture seam. True only when a real image payload was produced; the bytes model
    // the file sink, whose result the original discards.
    std::function<bool(std::vector<std::uint8_t>& png)> query_screenshot;
    // Root adoption. Must append the root under the lazily-created "Frame" node and must NOT copy
    // it. Returning nullptr models "no frame root available".
    std::function<ReportItem*(ReportItem* root)> adopt_root;
    // frameNum source for the "Frame" node; the original uses [[holder+0x10]+0x70] else -1.
    std::int32_t frame_num = -1;
    // List-B insert seam for the "Screenshot" attachment (0x1bdc80).
    Insert1bdc80Hooks list_b_insert{};
    // Phase 2/4/5/6 hooks; `synthetic_command` is set by this driver.
    AppInfoSnapshotHooks snapshot{};
};

struct SnapshotBugDataResult {
    bool ok = false;
    bool gate_armed = false;           // observed at poll time
    bool gate_cleared = false;         // always true when armed
    bool built = false;                // the report path completed
    bool screenshot_attached = false;  // list-B "Screenshot" inserted
    bool synthetic_command = false;    // "Command"/"screenshot"/"bug_screenshot" emitted
    bool adopted = false;              // adopt_root returned non-null
    // E597: populated when `hooks.screenshot` is supplied.
    bool screenshot_sent_to_sink = false;
    ScreenshotStatus screenshot_status = ScreenshotStatus::no_surface;
    std::string error;
};

// Runs the block. `armed == false` performs no work at all. When armed, the gate is cleared first,
// the screenshot decision is made, the report is built (with the synthetic entry at the recovered
// point when the flag is 0), and the root is adopted. A list-B allocation failure stops before the
// report build.
[[nodiscard]] SnapshotBugDataResult snapshot_bug_data(RegistryOwner& owner, ReportItem& root,
                                                      const SnapshotBugDataHooks& hooks);

}  // namespace sotc
