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
inline constexpr std::uint64_t kResolvedVtableTarget_e24900Rva = 0xe24900;
inline constexpr std::uint64_t kResolvedVtableTarget_13d20Rva = 0x13d20;
inline constexpr std::uint64_t kResolvedVtableTarget_20d60Rva = 0x20d60;
inline constexpr std::uint64_t kResolvedVtableTarget_c740Rva = 0xc740;
inline constexpr std::uint64_t kResolvedVtableTarget_358c0Rva = 0x358c0;

inline constexpr std::uint64_t kResolvedVtableTarget_2687b0Rva = 0x2687b0;
inline constexpr std::uint64_t kResolvedVtableTarget_c2e0Rva = 0xc2e0;
inline constexpr std::uint64_t kResolvedVtableTarget_a40e20Rva = 0xa40e20;
inline constexpr std::uint64_t kResolvedVtableTarget_a460f0Rva = 0xa460f0;
inline constexpr std::uint64_t kResolvedVtableTarget_1bf710Rva = 0x1bf710;
inline constexpr std::uint64_t kResolvedVtableTarget_ddf4b0Rva = 0xddf4b0;
inline constexpr std::uint64_t kResolvedVtableTarget_e21420Rva = 0xe21420;
inline constexpr std::uint64_t kResolvedVtableTarget_e1a5c0Rva = 0xe1a5c0;
inline constexpr std::uint64_t kResolvedVtableTarget_e19980Rva = 0xe19980;
inline constexpr std::uint64_t kResolvedVtableTarget_e16a00Rva = 0xe16a00;

inline constexpr std::uint64_t kResolvedVtable_15949a0 = 0x15949a0;
inline constexpr std::uint64_t kResolvedVtable_1594ae8 = 0x1594ae8;
inline constexpr std::uint64_t kResolvedVtable_1594dc0 = 0x1594dc0;
inline constexpr std::uint64_t kResolvedVtable_15955c0 = 0x15955c0;
inline constexpr std::uint64_t kResolvedVtable_15d1d80 = 0x15d1d80;
inline constexpr std::uint64_t kResolvedVtable_15e2d60 = 0x15e2d60;
inline constexpr std::uint64_t kResolvedVtable_159c908 = 0x159c908;
inline constexpr std::uint64_t kResolvedVtable_15d1678 = 0x15d1678;
inline constexpr std::uint64_t kResolvedVtable_1598e10 = 0x1598e10;
inline constexpr std::uint64_t kResolvedVtable_15e0b48 = 0x15e0b48;
inline constexpr std::uint64_t kResolvedVtable_15e2028 = 0x15e2028;
inline constexpr std::uint64_t kResolvedVtable_15e1ee0 = 0x15e1ee0;
inline constexpr std::uint64_t kResolvedVtable_15f2f08 = 0x15f2f08;


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
    {0x47571, kResolvedVtable_1594dc0, 0x20, kResolvedVtableTarget_20d60Rva},
    {0x14dc19, kResolvedVtable_1594dc0, 0x20, kResolvedVtableTarget_20d60Rva},
    {0x257068, kResolvedVtable_159c908, 0x58, kResolvedVtableTarget_2687b0Rva},
    {0x4f0d41, kResolvedVtable_1594dc0, 0x20, kResolvedVtableTarget_20d60Rva},
    {0xa3df9c, kResolvedVtable_15d1678, 0xe8, kResolvedVtableTarget_c2e0Rva},
    {0xa3dfda, kResolvedVtable_15d1678, 0x68, kResolvedVtableTarget_a40e20Rva},
    {0xa3dff7, kResolvedVtable_15d1678, 0x280, kResolvedVtableTarget_a460f0Rva},
    {0xa54b7b, kResolvedVtable_15d1d80, 0x08, kResolvedVtableTarget_c740Rva},
    {0xa54c92, kResolvedVtable_15d1d80, 0x08, kResolvedVtableTarget_c740Rva},
    {0xa70f4d, kResolvedVtable_1598e10, 0x18, kResolvedVtableTarget_1bf710Rva},
    {0xddf017, kResolvedVtable_15e0b48, 0x28, kResolvedVtableTarget_ddf4b0Rva},
    {0xe119be, kResolvedVtable_15e2028, 0x2a0, kResolvedVtableTarget_e21420Rva},
    {0xe119d3, kResolvedVtable_15e2028, 0x70, kResolvedVtableTarget_e1a5c0Rva},
    {0xe11c64, kResolvedVtable_15e1ee0, 0xb0, kResolvedVtableTarget_e19980Rva},
    {0xe1ee0d, kResolvedVtable_15e2d60, 0xd0, kResolvedVtableTarget_e24900Rva},
    {0xe1ee31, kResolvedVtable_15e2d60, 0xd0, kResolvedVtableTarget_e24900Rva},
    {0xed171c, kResolvedVtable_15f2f08, 0x38, kResolvedVtableTarget_e16a00Rva},
    {0xed1733, kResolvedVtable_15f2f08, 0x38, kResolvedVtableTarget_e16a00Rva},
};

inline constexpr std::size_t kResolvedVtableCallSiteCount =
    sizeof(kResolvedVtableCallSites) / sizeof(kResolvedVtableCallSites[0]);

// Returns the row for `site`, or nullptr when the site is not statically resolved.
[[nodiscard]] const ResolvedVtableCallSite* find_resolved_vtable_call_site(
    std::uint64_t site) noexcept;

// Number of distinct resolved targets (the table hits `0xc740` twice).
[[nodiscard]] std::size_t resolved_vtable_target_count() noexcept;

}  // namespace sotc
