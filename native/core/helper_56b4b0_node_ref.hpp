// CPU-01-HELPER-CALLEE-MODELS-03: native model of 0x56b4b0 (138 B, FDE 138; ELF sha256 d0723afb...5861d3), called once by
// startup constructor slot 422 (0x56b980, at 0x56c39d, rdi = the global record 0x2f08928; one other direct call site exists at 0x691cc9, outside this task).
//
// Every callee of the body is already native, so the whole body is modelled (no hook, no allocation):
//   - guarded function-local static HashedStr at 0x2ed3640 (guard byte 0x2ed3668): first use runs `0xf730` (the empty
//     string) then `0x14c4a0(static, 0, empty)` and registers `__cxa_atexit(0x95610, static, DSO)`. This is the SAME object
//     and guard that 0x14c9a0 builds (two compiler inlined copies there, a third here), already native as
//     `static_empty_hashed_2ed3640()`; its exit-time destructor 0x95610 is omitted exactly as in registry_object_14c9a0.hpp
//     (the static's `ctl` is null so the destructor would release nothing).
//   - `0x14c3a0(out, static)`            copy_hashed_14c3a0: hash, 16 B view, aux, ctl (+ `lock inc [ctl]` when non-null)
//   - `0x14d880()`                       getter_14d880: the singleton Object14c9a0 (its own add-ref on the first null cell)
//   - `[out + 0x28] = node ; byte [out + 0x30] = 0`
//   - tail `jmp 0x14ccb0` with rdi = node: add_ref_14ccb0(node) (`lock inc dword [node + 0x30]`)
// so the result is a 0x31 byte prefix of a node reference record {HashedStr id (0x28 B); Object14c9a0* node; u8 flag},
// the same {node, flag} pair 0xb25470 stores at +0x108 / +0x110 of its 0x118 byte elements.
//
// Bytes 0x04..0x07 (HashedStr padding) and 0x29..0x2f, and everything past 0x30, are never written. The consumers of the
// record and the meaning of the count in the node are NOT recovered. Byte-pinned by tools/lift/check_helper_56b4b0.py.
// Nothing here executes or calls an original RVA. FIXTURE_VERIFIED + STATIC_ANALYSIS_VERIFIED only.
#pragma once

#include <cstddef>
#include <cstdint>

#include "registry_object_14c9a0.hpp"

namespace sotc {

inline constexpr std::uint64_t kNodeRef56b4b0Rva = 0x56b4b0;
inline constexpr std::size_t kNodeRef56b4b0Size = 138;
inline constexpr std::uint64_t kNodeRefStaticHashedRva = 0x2ed3640;   // the guarded static (shared with 0x14c9a0)
inline constexpr std::uint64_t kNodeRefStaticGuardRva = 0x2ed3668;
inline constexpr std::uint64_t kNodeRefStaticDtorRva = 0x95610;       // exit-time destructor: identity only, omitted
inline constexpr std::size_t kNodeRefNodeOffset = 0x28;
inline constexpr std::size_t kNodeRefFlagOffset = 0x30;

struct NodeRef56b4b0 {
    HashedStr id;                 // +0x00 .. +0x27
    Object14c9a0* node;           // +0x28
    std::uint8_t flag;            // +0x30
    std::uint8_t gap31[7];        // never written
};
static_assert(sizeof(NodeRef56b4b0) == 0x38 && offsetof(NodeRef56b4b0, node) == 0x28 &&
              offsetof(NodeRef56b4b0, flag) == 0x30);

// 0x56b4b0(out). Writes id (copy of the static empty hashed string, ctl untouched because it is null), node and flag; adds
// exactly one reference more than the getter itself (getter's own add on a null cell + the tail add_ref).
void build_node_ref_56b4b0(NodeRef56b4b0& out) noexcept;

}  // namespace sotc
