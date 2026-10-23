// Native model of the 0x2569d0 mode/gate dispatch and the reachable 0xac5260/0xac5320 property rows.
//
// E570 (owner-rows report):
//   - gate = the "RetailBuild" boolean at `*(u8*)(*(void**)(0x2dfce70+0x60))`.
//   - mode = `dword [a2b960()+0x10]`, provably 1 at the only call site (`0x256d27`); mode 2 is
//     unreachable from 0x2569d0 and is kept in the table as data only.
//   - map A (`0xac5260`) record = {i32 i, f32 f, u32 tag}; f is 1.0f in every row.
//   - map B (`0xac5320`) record = {u64 pair, f32 v, u32 tag}.
//   - 259 rows are constants; the reachable set is gate1 head 2 + pending 1 + tail 130 + final 1
//     (134 calls) or gate0 head 5 + pending 1 + tail 130 + final 1 (137 calls).
//
// The table lives in `owner_properties_2569d0_table.inc.hpp`, generated from the private symbolic
// evidence. The sink is the only unresolved piece: the real map object and its red-black inserts
// (`0xac5260`/`0xac5320`/`0xac8a90`/`0xac8d40`) are not lifted.
//
// The shared-tail quirk is explicit: the predecessor emits a pending key (0x23 for gate1, 0x1c for
// gate0) before the 130 shared rows; the table marks those rows separately and the driver emits
// them in that order. G1 PARTIAL.
#pragma once

#include <cstdint>

#include "owner_properties_2569d0_table.inc.hpp"

namespace sotc {

enum class OwnerMode2569d0 : std::uint8_t {
    none = 0,               // mode not 1/2: epilogue, nothing registered
    retail_gate1 = 1,       // mode 1, RetailBuild true
    retail_gate0 = 2,       // mode 1, RetailBuild false
    mode2_unreachable = 3,  // mode 2: kept as data, not executed from 0x2569d0
};

// Pure classification from the two recovered scalars. The caller supplies both; this function never
// fabricates a gate or mode value.
[[nodiscard]] OwnerMode2569d0 classify_owner_mode(bool retail_build, std::int32_t mode_field) noexcept;

class OwnerPropertySink {
public:
    virtual ~OwnerPropertySink() = default;
    // 0xac5260: map A row {i, f, tag}.
    virtual bool insert_a(std::uint16_t key, std::int32_t i, float f, std::uint32_t tag) = 0;
    // 0xac5320: map B row {pair, v, tag}.
    virtual bool insert_b(std::uint16_t key, std::uint64_t pair, float v,
                          std::uint32_t tag) = 0;
};

struct OwnerPropertiesResult {
    bool ok = false;
    int inserted = 0;
    std::uint16_t failed_key = 0;
};

// Emits the reachable rows for `mode` in the recovered order. `none` inserts nothing and succeeds;
// `mode2_unreachable` returns ok == false without inserting anything.
[[nodiscard]] OwnerPropertiesResult apply_owner_properties(OwnerMode2569d0 mode,
                                                          OwnerPropertySink& sink) noexcept;

}  // namespace sotc
