// Statically resolved vtable call sites (CPU-01-AOT-CFG-SCALE-01).
//
// Every row was resolved end to end without running anything:
//     `call *slot(%reg)` with `%reg` from a RIP-relative table address
//  -> the qword at `vtable + slot` in the pinned image
//  -> a type-8 dynamic relocation at that same address whose addend is the same value
//  -> a target that is an FDE function start.
//
// These are entries, not implementations: the table records which guest function each call site
// reaches. Only `0x20d60` has a body model today (`vtable_1594dc0_slot20_20d60`); the others are
// entry records awaiting their own body lift.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kResolvedVtableTarget_187c0Rva = 0x187c0;
inline constexpr std::uint64_t kResolvedVtableTarget_13d20Rva = 0x13d20;
inline constexpr std::uint64_t kResolvedVtableTarget_20d60Rva = 0x20d60;
inline constexpr std::uint64_t kResolvedVtableTarget_c740Rva = 0xc740;
inline constexpr std::uint64_t kResolvedVtableTarget_358c0Rva = 0x358c0;

inline constexpr std::uint64_t kResolvedVtable_15949a0 = 0x15949a0;
inline constexpr std::uint64_t kResolvedVtable_1594ae8 = 0x1594ae8;
inline constexpr std::uint64_t kResolvedVtable_1594dc0 = 0x1594dc0;
inline constexpr std::uint64_t kResolvedVtable_15955c0 = 0x15955c0;
inline constexpr std::uint64_t kResolvedVtable_15d1d80 = 0x15d1d80;

struct ResolvedVtableCallSite {
    std::uint64_t site;    // guest address of the indirect call
    std::uint64_t vtable;  // RIP-relative table base
    std::uint32_t slot;    // byte offset of the slot
    std::uint64_t target;  // resolved guest function entry
};

// Sorted by `site`.
inline constexpr ResolvedVtableCallSite kResolvedVtableCallSites[] = {
    {0x1254d, kResolvedVtable_15949a0, 0x88, kResolvedVtableTarget_187c0Rva},
    {0x13620, kResolvedVtable_1594ae8, 0x28, kResolvedVtableTarget_13d20Rva},
    {0x28471, kResolvedVtable_1594dc0, 0x20, kResolvedVtableTarget_20d60Rva},
    {0x35165, kResolvedVtable_15955c0, 0x08, kResolvedVtableTarget_358c0Rva},
    {0xa54b7b, kResolvedVtable_15d1d80, 0x08, kResolvedVtableTarget_c740Rva},
    {0xa54c92, kResolvedVtable_15d1d80, 0x08, kResolvedVtableTarget_c740Rva},
};

inline constexpr std::size_t kResolvedVtableCallSiteCount =
    sizeof(kResolvedVtableCallSites) / sizeof(kResolvedVtableCallSites[0]);

// Returns the row for `site`, or nullptr when the site is not statically resolved.
[[nodiscard]] const ResolvedVtableCallSite* find_resolved_vtable_call_site(
    std::uint64_t site) noexcept;

// Number of distinct resolved targets (the table hits `0xc740` twice).
[[nodiscard]] std::size_t resolved_vtable_target_count() noexcept;

}  // namespace sotc
