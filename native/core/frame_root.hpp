// Native model of the bug-report Frame root (`0xa70c30` / `0xa70d10`, E588).
//
// Recovered from `local/re/evidence/snapshotbugdata-gate/report.md` §4:
//   - the holder at `ctx+0x48` is 24 B `{node +0x00, ctrl +0x08, owner +0x10}`; an all-zero holder
//     means "no frame yet";
//   - `0xa70c30` (220 B, sha256 `33074135…`) lazily creates a `"Frame"` report node and stores the
//     integer property `"frameNum"` from `[owner+0x70]` (or -1 when the owner is null);
//   - `0xa70d10` (32 B) calls `0xa70c30`, then tail-jumps to `0xab0d40` to append the root under
//     the Frame node; ownership transfers, no copy.
//
// The native model owns the Frame node with `unique_ptr` and appends children with the existing
// report tree. The holder's 0x20 B `ctrl` object (vtable `0x15d22f0`) and the live frame counter
// are contracts; the caller supplies `frame_num`.
//
// NOT modelled: `0x15d22f0`, `[owner+0x70]`, and the `0xa70400`/`0xa70d30` sibling paths.
// G1 PARTIAL.
#pragma once

#include <cstdint>
#include <memory>

#include "report_tree.hpp"

namespace sotc {

inline constexpr std::uint64_t kFrameRootGetRva = 0xA70C30;
inline constexpr std::size_t kFrameRootGetSize = 220;
inline constexpr std::uint64_t kFrameRootAdoptRva = 0xA70D10;
inline constexpr std::size_t kFrameRootAdoptSize = 32;
inline constexpr std::uint64_t kFrameRootAppendRva = 0xAB0D40;
inline constexpr const char kFrameRootType[] = "Frame";
inline constexpr const char kFrameRootNumKey[] = "frameNum";

class FrameRoot {
public:
    FrameRoot() = default;
    FrameRoot(const FrameRoot&) = delete;
    FrameRoot& operator=(const FrameRoot&) = delete;

    // `0xa70c30`: creates the "Frame" node on first use with `frame_num`; later calls return the
    // same node and ignore `frame_num` (the original only reads `[owner+0x70]` on creation).
    [[nodiscard]] ReportItem* node(std::int32_t frame_num);
    [[nodiscard]] const ReportItem* get() const noexcept { return node_.get(); }

    // `0xa70d10`: ensure the Frame node, then append `root` under it. Returns the raw child. A
    // null root is rejected before creating the node (native guard; not an original input).
    [[nodiscard]] ReportItem* adopt(std::unique_ptr<ReportItem> root, std::int32_t frame_num);

private:
    std::unique_ptr<ReportItem> node_;
};

}  // namespace sotc
