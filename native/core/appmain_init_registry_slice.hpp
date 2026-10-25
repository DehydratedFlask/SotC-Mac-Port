// Measured app-main → Init → registry startup slice (Drive tip).
//
// Static path (OBS, ELF sha256 d0723afb…861d3; E392 / E238):
//   entry 0xf120 → main 0x536730 → ctor 0x536aa0 (vptr 0x15c8cd8)
//   → Run 0x25a8d0 (vtable +0x18 @ 0x15c8cf0) → Init 0x531d60 (vtable +0x20 @ 0x15c8cf8)
//   → registry fragment at 0x532b0c..0x532bf8 (alloc 0x98 via 0x17070 → fill 0x52a220 →
//     store cell 0x2efd160).
//
// This module runs ONLY that registry fragment through tip AllocatorFront (wrapper
// depth 0x17070 / FakeHeap in tests). It is NOT registered into recovered_constructors().
//
// Allocator merge gap (honest): PR #5 heap hierarchy on branch cursor/heap-depth-5d26
// tip 4d7e484 / pause 0510d35 — Composite/Bucket/HeapAllocator + bootstrap 0x13380 +
// lazy latch 0x2dfaaa0 (PR #5 evidence E399–E403) — is NOT an ancestor of this Drive tip
// and is NOT consumed here. Cite only; do not edit the heap worktree from Drive.
//
// NOT modelled: remainder of Init (3890 B), pool/listener half of 0x52a220, splash/pad/
// base Initialize 0x255130, full app Run body, idx 95, leaf-vec tourism.
#pragma once

#include "allocator_front.hpp"
#include "input_table_551140.hpp"
#include "registry_init_531d60.hpp"

#include <cstdint>

namespace sotc {

// ELF-pinned landmarks for the measured edge (checker re-reads these from eboot.elf).
inline constexpr std::uint64_t kAppmainVtableBase = 0x15c8cd8;
inline constexpr std::uint64_t kAppmainVtableRunSlot = 0x15c8cf0;     // +0x18 → 0x25a8d0
inline constexpr std::uint64_t kAppmainVtableInitSlot = 0x15c8cf8;    // +0x20 → 0x531d60
inline constexpr std::uint64_t kAppmainRunRva = 0x25a8d0;
inline constexpr std::uint64_t kAppmainInitRva = 0x531d60;
inline constexpr std::uint64_t kAppmainInitCallsite = 0x25aa06;       // call [rax+0x20]

struct AppmainInitRegistrySliceResult {
    RegistryInit531d60Status status = RegistryInit531d60Status::missing_names;
    std::uint32_t entry_count = 0;          // table A size when ok (expect 275)
    std::uint32_t object_bytes = 0;         // 0x98 when cell installed
    bool cell_installed = false;
    RegistryObject2efd160* cell = nullptr;
};

// Clean slate → init_registry_fragment_531d60(table, alloc) → measure installed cell.
// Fail-visible without private I275 names (same contract as the Init fragment).
[[nodiscard]] AppmainInitRegistrySliceResult run_appmain_init_registry_slice(
    InputTable551140& table, AllocatorFront& alloc);

}  // namespace sotc
