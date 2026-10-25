// Measured Init remainder beyond the registry fragment: trivial leaves → registry (E405).
//
// Appmain triage (docs/checkpoints/2026-10-22-orchestrator-session-handoff.md): after
// input-table + registry builder, lift trivial Init leaves (0x4c4950, one-byte rets,
// 0xa431e0/0xa52b70/0xa599e0 setters) + splash hide contract, then continue the measured
// startup path into the Init-hash registry install.
//
// Order matches OBS Init mid→late: splash/ret-nops (early) modelled as callable contracts;
// flag pair + setters (0x5327a0..0x53297a) then registry fragment (0x532b0c..).
//
// Allocator: tip AllocatorFront + FakeHeap (same as E405). PR #5 heap E399–E403 cite-only.
// NOT modelled: pad 0x1fc80/0x1e4c0, pools/listeners, idx 95.
// (0xf08 singleton + 0x255130 are modelled on earlier Init slices.)
#pragma once

#include "allocator_front.hpp"
#include "appmain_init_registry_slice.hpp"
#include "init_531d60_trivial_leaves.hpp"
#include "input_table_551140.hpp"

#include <cstdint>

namespace sotc {

struct AppmainInitRemainderSliceResult {
    bool trivial_ok = false;
    std::uint8_t flag_2efa890 = 0;
    std::uint8_t flag_2efa891 = 0;
    std::uint8_t setter_at_01 = 0;
    std::uint8_t setter_at_29 = 0;
    std::uint8_t setter_at_51 = 0;
    AppmainInitRegistrySliceResult registry{};
};

// Run Init trivial remainder on `setter_obj` (must be ≥ 0x52 bytes), then registry fragment.
// `flag_in` models the dil argument to 0x4c4950 (Init passes 0 or 1 from pad outcome).
[[nodiscard]] AppmainInitRemainderSliceResult run_appmain_init_remainder_slice(
    InputTable551140& table, AllocatorFront& alloc, void* setter_obj, std::uint8_t flag_in,
    std::uint8_t set_a, std::uint8_t set_b, std::uint8_t set_c);

}  // namespace sotc
