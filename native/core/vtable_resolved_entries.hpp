// Statically resolved vtable call slots (CPU-01-AOT-CFG-SCALE-01).
//
// Two evidence grades live in this table, and every row is labelled with its own (E1066):
//
//   site-provable - `call *slot(%reg)` where the register's NEAREST preceding write is the
//     RIP-relative load of this table (`lea` of the table base, or `mov` of a table-pointer global),
//     with no intervening write the scanner cannot classify. Reading the pinned image then forces the
//     target for that call.
//   table-level - the table side is tied and the site side is NOT proven: the qword at
//     `vtable + slot` is the target, one type-8 dynamic relocation at that address carries the same
//     addend, and the target is an FDE function start; but the call register's nearest definition is
//     a member load with no static source (or a write the scanner cannot classify), so the site may
//     dispatch elsewhere at run time. The row records and models the TARGET, not the call.
//
// Both grades require, for the slot itself: the qword in the pinned image, one type-8 dynamic
// relocation with the same addend, and an FDE function start. Nothing was executed.
// `tools/lift/check_vtable_resolved_entries.py` re-verifies exactly those plus the entry-map record;
// it cannot and does not test dispatch.
//
// These are entries, not implementations: every one of the 37 distinct targets has a native body
// model. Measured split (E1066): 41 site-provable rows, 50 table-level rows. The 50 inherited a
// table-level tie from `indirect_frontier.classify`, whose `rip_global_for` fallback scans *past*
// the nearest definition and can attach a table load the call does not use;
// `docs/INDIRECT_FRONTIER.md` ("Evidence grades") defines both grades and gives the counts.
//
// The bounded store-to-load forwarding pass (`tools/lift/store_to_load_forwarding.py`, 2026-11-01)
// was then run over all 91 rows and promoted **none** of the 50: 40 have no store to the load
// location anywhere in their own function, 9 reach a store that is not a single reaching definition
// in a straight-line region, and 1 loads through an indexed address. No row's grade or value
// changed as a result. `tools/lift/check_evidence_grades.py` re-derives the split from the pinned
// ELF and this header, and fails when `docs/INDIRECT_FRONTIER.md` drifts.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kResolvedVtableTarget_318b0Rva = 0x318b0;
inline constexpr std::uint64_t kResolvedVtableTarget_31870Rva = 0x31870;
inline constexpr std::uint64_t kResolvedVtableTarget_31310Rva = 0x31310;
inline constexpr std::uint64_t kResolvedVtableTarget_4b0Rva = 0x4b0;
inline constexpr std::uint64_t kResolvedVtableTarget_22a60Rva = 0x22a60;
inline constexpr std::uint64_t kResolvedVtableTarget_187c0Rva = 0x187c0;
inline constexpr std::uint64_t kResolvedVtableTarget_e26f90Rva = 0xe26f90;
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

// Vtable-closure sweep (2026-11-01): the remaining slots of the tables already recorded above.
inline constexpr std::uint64_t kResolvedVtableTarget_13d50Rva = 0x13d50;
inline constexpr std::uint64_t kResolvedVtableTarget_13d60Rva = 0x13d60;
inline constexpr std::uint64_t kResolvedVtableTarget_30e50Rva = 0x30e50;
inline constexpr std::uint64_t kResolvedVtableTarget_488d0Rva = 0x488d0;
inline constexpr std::uint64_t kResolvedVtableTarget_c2c0Rva = 0xc2c0;
inline constexpr std::uint64_t kResolvedVtableTarget_e19cb0Rva = 0xe19cb0;
inline constexpr std::uint64_t kResolvedVtableTarget_e19f70Rva = 0xe19f70;
inline constexpr std::uint64_t kResolvedVtableTarget_e1a040Rva = 0xe1a040;
inline constexpr std::uint64_t kResolvedVtableTarget_e19440Rva = 0xe19440;
inline constexpr std::uint64_t kResolvedVtableTarget_e52cb0Rva = 0xe52cb0;
inline constexpr std::uint64_t kResolvedVtableTarget_ed4860Rva = 0xed4860;

// Provenance-audited slice (2026-11-01): sites whose nearest table load is the one the call uses.
inline constexpr std::uint64_t kResolvedVtableTarget_9b7b80Rva = 0x9b7b80;
inline constexpr std::uint64_t kResolvedVtableTarget_d7c4b0Rva = 0xd7c4b0;
inline constexpr std::uint64_t kResolvedVtableTarget_6e92c0Rva = 0x6e92c0;
inline constexpr std::uint64_t kResolvedVtableTarget_9804c0Rva = 0x9804c0;

inline constexpr std::uint64_t kResolvedVtable_15949a0 = 0x15949a0;
inline constexpr std::uint64_t kResolvedVtable_1594ae8 = 0x1594ae8;
inline constexpr std::uint64_t kResolvedVtable_1594dc0 = 0x1594dc0;
inline constexpr std::uint64_t kResolvedVtable_15955c0 = 0x15955c0;
inline constexpr std::uint64_t kResolvedVtable_15953f8 = 0x15953f8;
inline constexpr std::uint64_t kResolvedVtable_15d1d80 = 0x15d1d80;
inline constexpr std::uint64_t kResolvedVtable_15ed2a0 = 0x15ed2a0;
inline constexpr std::uint64_t kResolvedVtable_15e2d60 = 0x15e2d60;
inline constexpr std::uint64_t kResolvedVtable_159c908 = 0x159c908;
inline constexpr std::uint64_t kResolvedVtable_15d1678 = 0x15d1678;
inline constexpr std::uint64_t kResolvedVtable_1598e10 = 0x1598e10;
inline constexpr std::uint64_t kResolvedVtable_15e0b48 = 0x15e0b48;
inline constexpr std::uint64_t kResolvedVtable_15e2028 = 0x15e2028;
inline constexpr std::uint64_t kResolvedVtable_15e1ee0 = 0x15e1ee0;
inline constexpr std::uint64_t kResolvedVtable_15f2f08 = 0x15f2f08;
inline constexpr std::uint64_t kResolvedVtable_1595b60 = 0x1595b60;
inline constexpr std::uint64_t kResolvedVtable_1595320 = 0x1595320;
inline constexpr std::uint64_t kResolvedVtable_15cf248 = 0x15cf248;
inline constexpr std::uint64_t kResolvedVtable_15de228 = 0x15de228;
inline constexpr std::uint64_t kResolvedVtable_2dcb170 = 0x2dcb170;
inline constexpr std::uint64_t kResolvedVtable_2dd09b0 = 0x2dd09b0;


struct ResolvedVtableCallSite {
    std::uint64_t site;    // guest address of the indirect call
    std::uint64_t vtable;  // RIP-relative table base
    std::uint32_t slot;    // byte offset of the slot
    std::uint64_t target;  // resolved guest function entry
};

// Sorted by `site`. The trailing comment on each row is its evidence grade: `site-provable` means the
// call site alone forces the target, `table-level` means only the table slot is tied (file header).
inline constexpr ResolvedVtableCallSite kResolvedVtableCallSites[] = {
    {0x113c5, kResolvedVtable_1595b60, 0x18, kResolvedVtableTarget_488d0Rva},  // table-level
    {0x113d6, kResolvedVtable_1595b60, 0x50, kResolvedVtableTarget_13d60Rva},  // table-level
    {0x11452, kResolvedVtable_1594ae8, 0x28, kResolvedVtableTarget_13d20Rva},  // site-provable
    {0x11462, kResolvedVtable_1594ae8, 0x50, kResolvedVtableTarget_13d60Rva},  // table-level
    {0x1149d, kResolvedVtable_1594ae8, 0x40, kResolvedVtableTarget_13d50Rva},  // table-level
    {0x1254d, kResolvedVtable_15949a0, 0x88, kResolvedVtableTarget_187c0Rva},  // table-level
    {0x13620, kResolvedVtable_1594ae8, 0x28, kResolvedVtableTarget_13d20Rva},  // site-provable
    {0x28471, kResolvedVtable_1594dc0, 0x20, kResolvedVtableTarget_20d60Rva},  // site-provable
    {0x30f56, kResolvedVtable_1595320, 0x10, kResolvedVtableTarget_30e50Rva},  // table-level
    {0x312ce, kResolvedVtable_15953f8, 0xa8, kResolvedVtableTarget_318b0Rva},  // table-level
    {0x312de, kResolvedVtable_15953f8, 0xa0, kResolvedVtableTarget_31870Rva},  // table-level
    {0x312f0, kResolvedVtable_15953f8, 0x18, kResolvedVtableTarget_31310Rva},  // table-level
    {0x3133e, kResolvedVtable_15953f8, 0xa8, kResolvedVtableTarget_318b0Rva},  // table-level
    {0x3134e, kResolvedVtable_15953f8, 0xa0, kResolvedVtableTarget_31870Rva},  // table-level
    {0x31360, kResolvedVtable_15953f8, 0x18, kResolvedVtableTarget_31310Rva},  // table-level
    {0x35165, kResolvedVtable_15955c0, 0x8, kResolvedVtableTarget_358c0Rva},  // table-level
    {0x465eb, kResolvedVtable_15949a0, 0x58, kResolvedVtableTarget_c2c0Rva},  // table-level
    {0x47571, kResolvedVtable_1594dc0, 0x20, kResolvedVtableTarget_20d60Rva},  // site-provable
    {0x4d179, kResolvedVtable_1594dc0, 0x20, kResolvedVtableTarget_20d60Rva},  // site-provable
    {0x8a5bf, kResolvedVtable_1594dc0, 0x20, kResolvedVtableTarget_20d60Rva},  // site-provable
    {0x14a645, kResolvedVtable_1594dc0, 0x20, kResolvedVtableTarget_20d60Rva},  // site-provable
    {0x14dc19, kResolvedVtable_1594dc0, 0x20, kResolvedVtableTarget_20d60Rva},  // site-provable
    {0x14def4, kResolvedVtable_1594dc0, 0x10, kResolvedVtableTarget_4b0Rva},  // table-level
    {0x14df09, kResolvedVtable_1594dc0, 0x18, kResolvedVtableTarget_22a60Rva},  // table-level
    {0x1be855, kResolvedVtable_1598e10, 0x18, kResolvedVtableTarget_1bf710Rva},  // table-level
    {0x1c00b8, kResolvedVtable_1598e10, 0x18, kResolvedVtableTarget_1bf710Rva},  // table-level
    {0x21a695, kResolvedVtable_15cf248, 0x68, kResolvedVtableTarget_9b7b80Rva},  // site-provable
    {0x21ba92, kResolvedVtable_15cf248, 0x68, kResolvedVtableTarget_9b7b80Rva},  // site-provable
    {0x21bb84, kResolvedVtable_15cf248, 0x68, kResolvedVtableTarget_9b7b80Rva},  // site-provable
    {0x21c573, kResolvedVtable_15cf248, 0x68, kResolvedVtableTarget_9b7b80Rva},  // site-provable
    {0x21c725, kResolvedVtable_15cf248, 0x68, kResolvedVtableTarget_9b7b80Rva},  // site-provable
    {0x21d134, kResolvedVtable_15cf248, 0x68, kResolvedVtableTarget_9b7b80Rva},  // site-provable
    {0x21d306, kResolvedVtable_15cf248, 0x68, kResolvedVtableTarget_9b7b80Rva},  // site-provable
    {0x2537e1, kResolvedVtable_1594dc0, 0x20, kResolvedVtableTarget_20d60Rva},  // site-provable
    {0x257068, kResolvedVtable_159c908, 0x58, kResolvedVtableTarget_2687b0Rva},  // table-level
    {0x2f54da, kResolvedVtable_15cf248, 0x68, kResolvedVtableTarget_9b7b80Rva},  // site-provable
    {0x357301, kResolvedVtable_1594dc0, 0x20, kResolvedVtableTarget_20d60Rva},  // site-provable
    {0x3e2d40, kResolvedVtable_1594dc0, 0x20, kResolvedVtableTarget_20d60Rva},  // site-provable
    {0x3e4af3, kResolvedVtable_15cf248, 0x68, kResolvedVtableTarget_9b7b80Rva},  // site-provable
    {0x3e4dc4, kResolvedVtable_15cf248, 0x68, kResolvedVtableTarget_9b7b80Rva},  // site-provable
    {0x3e7237, kResolvedVtable_15cf248, 0x68, kResolvedVtableTarget_9b7b80Rva},  // site-provable
    {0x3edf1a, kResolvedVtable_15cf248, 0x68, kResolvedVtableTarget_9b7b80Rva},  // site-provable
    {0x3ee19f, kResolvedVtable_15cf248, 0x68, kResolvedVtableTarget_9b7b80Rva},  // site-provable
    {0x49e5e0, kResolvedVtable_1594dc0, 0x20, kResolvedVtableTarget_20d60Rva},  // site-provable
    {0x4f0d41, kResolvedVtable_1594dc0, 0x20, kResolvedVtableTarget_20d60Rva},  // site-provable
    {0x6a2dc4, kResolvedVtable_2dcb170, 0x08, kResolvedVtableTarget_6e92c0Rva},  // site-provable
    {0x6a4ada, kResolvedVtable_2dcb170, 0x08, kResolvedVtableTarget_6e92c0Rva},  // site-provable
    {0x6a8eea, kResolvedVtable_1594dc0, 0x20, kResolvedVtableTarget_20d60Rva},  // site-provable
    {0x6b13d1, kResolvedVtable_1594dc0, 0x20, kResolvedVtableTarget_20d60Rva},  // site-provable
    {0x6b4dd1, kResolvedVtable_1594dc0, 0x20, kResolvedVtableTarget_20d60Rva},  // site-provable
    {0x786041, kResolvedVtable_1594dc0, 0x20, kResolvedVtableTarget_20d60Rva},  // site-provable
    {0x787356, kResolvedVtable_1594dc0, 0x20, kResolvedVtableTarget_20d60Rva},  // site-provable
    {0x7d3a86, kResolvedVtable_1594dc0, 0x20, kResolvedVtableTarget_20d60Rva},  // site-provable
    {0x7d7aa3, kResolvedVtable_1594dc0, 0x20, kResolvedVtableTarget_20d60Rva},  // site-provable
    {0x7d91e8, kResolvedVtable_1594dc0, 0x20, kResolvedVtableTarget_20d60Rva},  // site-provable
    {0x7e3356, kResolvedVtable_1594dc0, 0x20, kResolvedVtableTarget_20d60Rva},  // site-provable
    {0x96c533, kResolvedVtable_2dd09b0, 0x08, kResolvedVtableTarget_9804c0Rva},  // site-provable
    {0xa338cb, kResolvedVtable_1594dc0, 0x20, kResolvedVtableTarget_20d60Rva},  // site-provable
    {0xa3df9c, kResolvedVtable_15d1678, 0xe8, kResolvedVtableTarget_c2e0Rva},  // table-level
    {0xa3dfda, kResolvedVtable_15d1678, 0x68, kResolvedVtableTarget_a40e20Rva},  // table-level
    {0xa3dff7, kResolvedVtable_15d1678, 0x280, kResolvedVtableTarget_a460f0Rva},  // table-level
    {0xa448f5, kResolvedVtable_1594dc0, 0x20, kResolvedVtableTarget_20d60Rva},  // site-provable
    {0xa54b7b, kResolvedVtable_15d1d80, 0x8, kResolvedVtableTarget_c740Rva},  // table-level
    {0xa54c92, kResolvedVtable_15d1d80, 0x8, kResolvedVtableTarget_c740Rva},  // table-level
    {0xa70f4d, kResolvedVtable_1598e10, 0x18, kResolvedVtableTarget_1bf710Rva},  // table-level
    {0xab2f6a, kResolvedVtable_1598e10, 0x18, kResolvedVtableTarget_1bf710Rva},  // table-level
    {0xab620a, kResolvedVtable_1598e10, 0x18, kResolvedVtableTarget_1bf710Rva},  // table-level
    {0xbda7e5, kResolvedVtable_15de228, 0x20, kResolvedVtableTarget_d7c4b0Rva},  // site-provable
    {0xddf017, kResolvedVtable_15e0b48, 0x28, kResolvedVtableTarget_ddf4b0Rva},  // table-level
    {0xe119be, kResolvedVtable_15e2028, 0x2a0, kResolvedVtableTarget_e21420Rva},  // table-level
    {0xe119d3, kResolvedVtable_15e2028, 0x70, kResolvedVtableTarget_e1a5c0Rva},  // table-level
    {0xe11c64, kResolvedVtable_15e1ee0, 0xb0, kResolvedVtableTarget_e19980Rva},  // table-level
    {0xe14c26, kResolvedVtable_15e1ee0, 0x10, kResolvedVtableTarget_e19440Rva},  // table-level
    {0xe1b1f1, kResolvedVtable_15e2028, 0x40, kResolvedVtableTarget_e1a040Rva},  // table-level
    {0xe1b204, kResolvedVtable_15e2028, 0x38, kResolvedVtableTarget_e19f70Rva},  // table-level
    {0xe1b245, kResolvedVtable_15e2028, 0x18, kResolvedVtableTarget_e19cb0Rva},  // table-level
    {0xe1ee0d, kResolvedVtable_15e2d60, 0xd0, kResolvedVtableTarget_e24900Rva},  // table-level
    {0xe1ee31, kResolvedVtable_15e2d60, 0xd0, kResolvedVtableTarget_e24900Rva},  // table-level
    {0xe1ee7d, kResolvedVtable_15e2d60, 0xd0, kResolvedVtableTarget_e24900Rva},  // table-level
    {0xe1eea1, kResolvedVtable_15e2d60, 0xd0, kResolvedVtableTarget_e24900Rva},  // table-level
    {0xe52b97, kResolvedVtable_15ed2a0, 0xc0, kResolvedVtableTarget_e26f90Rva},  // table-level
    {0xe52bb3, kResolvedVtable_15ed2a0, 0xc0, kResolvedVtableTarget_e26f90Rva},  // table-level
    {0xe52bd2, kResolvedVtable_15ed2a0, 0xc0, kResolvedVtableTarget_e26f90Rva},  // table-level
    {0xe52cfb, kResolvedVtable_15ed2a0, 0xd0, kResolvedVtableTarget_e24900Rva},  // table-level
    {0xe52d30, kResolvedVtable_15ed2a0, 0x10, kResolvedVtableTarget_e52cb0Rva},  // table-level
    {0xe52d59, kResolvedVtable_15ed2a0, 0xc0, kResolvedVtableTarget_e26f90Rva},  // table-level
    {0xe52d76, kResolvedVtable_15ed2a0, 0xc0, kResolvedVtableTarget_e26f90Rva},  // table-level
    {0xe52d96, kResolvedVtable_15ed2a0, 0xc0, kResolvedVtableTarget_e26f90Rva},  // table-level
    {0xed171c, kResolvedVtable_15f2f08, 0x38, kResolvedVtableTarget_e16a00Rva},  // table-level
    {0xed1733, kResolvedVtable_15f2f08, 0x38, kResolvedVtableTarget_e16a00Rva},  // table-level
    {0xed2c55, kResolvedVtable_15f2f08, 0x88, kResolvedVtableTarget_ed4860Rva},  // table-level
};

inline constexpr std::size_t kResolvedVtableCallSiteCount =
    sizeof(kResolvedVtableCallSites) / sizeof(kResolvedVtableCallSites[0]);

// Returns the row for `site`, or nullptr when the site is not statically resolved.
[[nodiscard]] const ResolvedVtableCallSite* find_resolved_vtable_call_site(
    std::uint64_t site) noexcept;

// Number of distinct resolved targets (the table hits `0xc740` twice).
[[nodiscard]] std::size_t resolved_vtable_target_count() noexcept;

}  // namespace sotc
