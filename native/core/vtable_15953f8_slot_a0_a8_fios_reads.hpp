// Two resolved slots on vtable `0x15953f8`: `+0xa0 -> 0x31870` and `+0xa8 -> 0x318b0`.
//
// CORRECTION (E1095): the table rows for the call sites once recorded for these slots (`0x312ce`,
// `0x3133e` for `+0xa8`; `0x312de`, `0x3134e` for `+0xa0`) were withdrawn. Those calls read the
// object's vptr (`0x15953f8 + 0x10`), so they reach `0x15954b0 -> 0x31900` and `0x15954a8 -> 0x318f0`.
// The slot facts below are still true of the image (the qwords at `0x15953f8 + 0xa8` / `+ 0xa0` are
// `0x318b0` / `0x31870`); what is withdrawn is the claim that those calls dispatch to them.
//
// Both are file-handle read entry points, and the call targets name the imported service:
//
//   0x31870 (55 B): *(u64*)(this+0x54) = 0xFFFFFFFF00000004;
//                   status = sceFiosFHReadv((FiosVec*)(this+0x20), *(u32*)(this+0x10), arg1, arg2);
//                   *(u32*)(this+0x50) = status;
//   0x318b0 (56 B): the same with an extra argument and **sceFiosFHPreadv** (positioned read):
//                   status = sceFiosFHPreadv((FiosVec*)(this+0x20), *(u32*)(this+0x10), arg1, arg2, arg3);
//
// `0xf8f6f0` and `0xf8f6c0` are PLT stubs for `sceFiosFHReadv` / `sceFiosFHPreadv` in `libSceFios2`
// (see `local/validation/registry-analysis-agent/plt_map.json`), so the service name here is not a
// guess. What the marker at `+0x54` and the counter at `+0x10` mean beyond "written by the caller"
// is not modelled; the read is supplied as a hook so a host can bind it to our own Fios layer.
//
// NOTE (E1100): `0x31900` (`frontier_31900_fios_poll`) shows what the fields really are. `+0x50` is the
// `SceFiosOp` handle that `sceFiosFHReadv` / `sceFiosFHPreadv` return (the poll feeds it to
// `sceFiosOpIsDone` / `GetError` / `GetActualCount` / `Delete`), so the cell and constant below named
// "status" hold an op handle, and `+0x54 = 0xFFFFFFFF00000004` is state dword 4 (a data transfer)
// plus an actual-count dword of -1. Names in this header are kept for compatibility.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable15953f8SlotA0_31870Rva = 0x31870;
inline constexpr std::size_t kVtable15953f8SlotA0_31870Size = 0x37;
inline constexpr std::uint64_t kVtable15953f8SlotA8_318b0Rva = 0x318b0;
inline constexpr std::size_t kVtable15953f8SlotA8_318b0Size = 0x38;
inline constexpr std::uint64_t kVtable15953f8Address = 0x15953f8;
inline constexpr std::uint32_t kVtable15953f8SlotA0 = 0xa0;
inline constexpr std::uint32_t kVtable15953f8SlotA8 = 0xa8;
inline constexpr std::uint64_t kVtable15953f8SlotA0Reloc = 0x1595498;
inline constexpr std::uint64_t kVtable15953f8SlotA8Reloc = 0x15954a0;
inline constexpr std::uint64_t kFiosReadvPlt = 0xf8f6f0;    // sceFiosFHReadv
inline constexpr std::uint64_t kFiosPreadvPlt = 0xf8f6c0;   // sceFiosFHPreadv
inline constexpr std::uint64_t kVtable15953f8FieldVector = 0x20;   // first read argument
inline constexpr std::uint64_t kVtable15953f8FieldCount = 0x10;    // second read argument
inline constexpr std::uint64_t kVtable15953f8FieldStatus = 0x50;   // result store
inline constexpr std::uint64_t kVtable15953f8FieldMarker = 0x54;   // 0xFFFFFFFF00000004
inline constexpr std::uint64_t kVtable15953f8Marker = 0xFFFFFFFF00000004ULL;

struct FiosReadCells {
    void* self = nullptr;
    // Host storage for the three fields the body reads or writes.
    std::uint32_t* count = nullptr;    // this+0x10, read
    std::int32_t* status = nullptr;    // this+0x50, written
    std::uint64_t* marker = nullptr;   // this+0x54, written before the call
    // The read arguments after the vector and the count.
    const void* buffer = nullptr;
    std::uint64_t length = 0;
    std::uint64_t offset = 0;          // only used by the positioned variant
};

struct FiosReadHooks {
    // Both return the host's status, which the body stores at this+0x50.
    std::int32_t (*readv)(void* vector, std::uint32_t count, const void* buffer,
                          std::uint64_t length) = nullptr;
    std::int32_t (*preadv)(void* vector, std::uint32_t count, const void* buffer,
                           std::uint64_t length, std::uint64_t offset) = nullptr;
};

struct FiosReadResult {
    bool ok = false;
    bool marker_written = false;
    bool service_called = false;
    std::int32_t status = 0;
};

[[nodiscard]] FiosReadResult vtable_15953f8_slot_a0_31870(
    const FiosReadCells& cells = {}, const FiosReadHooks& hooks = {}) noexcept;

[[nodiscard]] FiosReadResult vtable_15953f8_slot_a8_318b0(
    const FiosReadCells& cells = {}, const FiosReadHooks& hooks = {}) noexcept;

}  // namespace sotc
