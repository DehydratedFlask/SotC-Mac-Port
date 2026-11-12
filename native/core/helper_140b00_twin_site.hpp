// CPU-01-AOT-CFG-SCALE-01-BATCH17: native model of the CALL SITE of the unbounded-int constructor twins 0x140b00 (startup slot 534, 3 sites) and 0x25ab0 (startup slot 566, 1 site).
// The twin constructors themselves are helper_140b00_unbounded_int_twin; this adds what the slot-side bodies do around them, in the observed order:
//   [obj] = final vptr + 0x10 (pre-store) ; twin ctor (own seed class as the argument functor) ; f590(description) ; call [seed + 0x18] (bare ret) ; [obj] = final vptr ;
//   value / default dwords ; 0x20d60 ; f590(name) ; __cxa_atexit(dtor, obj).
// The final vptr and the atexit destructor are per-class IDENTITY TAGS (slot 534: 0x1598360 / 0x13a090); they are never read or run. The value cell is the caller's: the inline +0xd0 for the
// 0x140b00 sites; slot 566's 0x25ab0 site passes an EXTERNAL cell (obj - 8) that the site then writes through value_ref.
#pragma once

#include <cstddef>
#include <cstdint>

#include "helper_140b00_unbounded_int_twin.hpp"
#include "helper_int_var_sites.hpp"

namespace sotc {

struct HelperIntTwinSiteSpec {
    const char* name;
    std::size_t name_length;
    const char* description;
    std::size_t description_length;
    std::uint32_t flags;
    std::int32_t value;               // dword at the value cell
    std::int32_t default_value;       // dword [var+0x68]
    std::uint64_t final_vptr;         // identity tag: the site's literal final class pointer (table + 0x10)
    std::uint64_t dtor_rva;           // identity tag: the __cxa_atexit destructor
    bool early_description_release = true;
    // Twin B (0x25ab0, slot 566) differs from twin A (0x140b00, slot 534) in two observed ways: it does NOT pre-store the final vptr before the constructor,
    // and it does NOT write a value (the external value cell is an INPUT; [var+0x68] is loaded from it). Defaults keep the twin-A sequence.
    bool pre_store_vptr = true;
    bool write_value = true;
};

// value_cell == nullptr: the inline [var+0xd0]. Otherwise the cell the site passed in rdx (value_ref = value_cell) and writes the value into.
[[nodiscard]] HelperIntSiteResult helper_init_unbounded_int_twin_var_site(ConCmdRangedVar& var, UnboundedIntTwinVariant variant, const HelperIntTwinSiteSpec& spec,
                                                                          std::uint32_t* value_cell, ConCmdRegistry& registry) noexcept;

}  // namespace sotc
