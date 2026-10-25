// Native lift of the list-B phases of `0x1bdf30` (E575).
//
// Reader `0x1be2f7..0x1be3e5` (239 B, sha256 `af6ee6af…`): FIFO from `container+0x20`
// (`sentinel.next`) following `node+0x00` (`node.next`). For every entry it allocates a 0x1a0
// `"Attachment"` report node and a 0xd0 value object (kind 4), copies the value text from
// `node+0x30`, sets the property `"name" = node+0x10`, links the value under the node and the node
// under the report root. The count at `container+0x30` is never read; the walk terminates at the
// sentinel.
//
// Clear `0x1be458..0x1be49c` (69 B, sha256 `a2399c53…`): per node release the value (`+0x30`) then
// the name (`+0x10`) through `0xf590`, free the node through `0x170c0` (tag 2); then reset both
// sentinel links and store 0 to the full 8-byte count at `+0x30` (`0x1be4a4..0x1be4b6`). The reset
// runs even when the list is empty, so the operation is idempotent.
//
// Ownership: the reader copies every string; the source shared strings are released by the clear,
// so a borrowing model would dangle. `insert_1bdc80` gives the node one reference per holder and
// the clear consumes it. A null free hook is a visible failure that leaves the list untouched.
//
// NOT modelled: the screenshot query `0x961a0` that decides whether list B is populated, the report
// root adoption `0xa70c30`/`0xa70d10`, and the allocator bodies. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

#include "insert_1bdc80.hpp"
#include "report_tree.hpp"
#include "shared_release_f590.hpp"

namespace sotc {

inline constexpr std::uint64_t kAppInfoListBReaderRva = 0x1BE2F7;
inline constexpr std::size_t kAppInfoListBReaderSize = 239;
inline constexpr std::uint64_t kAppInfoListBReaderEndRva = 0x1BE3E6;
inline constexpr std::uint64_t kAppInfoListBClearRva = 0x1BE458;
inline constexpr std::size_t kAppInfoListBClearSize = 69;
inline constexpr std::uint64_t kAppInfoListBClearEndRva = 0x1BE49D;
inline constexpr std::uint64_t kAppInfoListBResetRva = 0x1BE4A4;
inline constexpr std::size_t kAppInfoListBResetSize = 19;
inline constexpr std::uint64_t kAppInfoListBResetEndRva = 0x1BE4B7;
inline constexpr std::uint64_t kAppInfoListBFreeRva = 0x170C0;
inline constexpr std::uint64_t kAppInfoListBAttachmentLiteralRva = 0xFB3E4C;
inline constexpr std::uint64_t kAppInfoListBNameLiteralRva = 0xFB3E34;
inline constexpr const char kAppInfoListBAttachmentType[] = "Attachment";
inline constexpr const char kAppInfoListBNameKey[] = "name";
inline constexpr std::int32_t kAppInfoListBValueKind = 4;

struct AppInfoListBSerializeResult {
    bool ok = false;
    int entries = 0;  // list-B nodes visited
    int nodes = 0;    // "Attachment" report nodes appended
    int values = 0;   // value children appended
};

struct AppInfoListBClearHooks {
    SharedReleaseF590Hooks release{};
    // 0x170c0(node) — tag-2 deallocate. Null => visible failure before the list is touched.
    void (*free_tag2)(void* node) = nullptr;
};

struct AppInfoListBClearResult {
    bool ok = false;
    int freed = 0;
    int releases = 0;          // two per freed node
    int release_failures = 0;  // a release returned ok == false
    std::uint64_t count_after = 0;
};

// Appends one `"Attachment"` node per list-B entry under `root`, in insertion order. `root` must be
// a report node; a value root is a visible failure and leaves it untouched. Text is copied.
// Allocation failure propagates as `std::bad_alloc`, matching the original throwing allocator.
[[nodiscard]] AppInfoListBSerializeResult appinfo_listb_serialize(
    const Container1beeb0& container, ReportItem& root);

// Releases and frees every list-B node, then resets the list. Idempotent on an empty list.
[[nodiscard]] AppInfoListBClearResult appinfo_listb_clear(Container1beeb0& container,
                                                          const AppInfoListBClearHooks& hooks) noexcept;

}  // namespace sotc
