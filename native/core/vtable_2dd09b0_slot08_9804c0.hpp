// Table slot resolved by the provenance-audited vtable-closure sweep:
// `0x2dd09b0 + 0x08 -> 0x9804c0`.
//
// Evidence:
//   * `0x96c533` is `call *0x8(%r12)` where `%r12` is loaded by
//     `leaq 0x2dd09b0(%rip), %r12` at `0x96c4ae` and only read afterwards; the intervening call
//     `0x959620` must preserve `%r12` (callee-saved);
//   * the qword at `0x2dd09b0 + 0x08` in the pinned image is `0x9804c0`;
//   * the dynamic relocation table has one type-8 row at `0x2dd09b8` with addend `0x9804c0`;
//   * `0x9804c0` is an FDE function start with length `0x155`.
//
// The container is a mixed data table: `+0x00` is read as an `i32` counter (`movl (%r12),%eax`) and
// `+0x08` holds this handler, so it is a function-pointer slot inside a data block, not a vtable.
//
// Body (341 B) builds a variable-length record array:
//   parse(0x959eb0)(text, "maxWaves", 0x100, &count)
//   count = clamp(count, 1, 0x400)          ; unsigned <= 0x400 keeps the parsed value, 0 becomes 1
//   array = alloc(0x959620)(context, count * 0x670, 1)
//   side  = alloc(0x959620)(context, count * 0x80, 1)      ; always issued, even if array is null
//   if (array != null && count != 0)
//       for (i = 0; i < count; ++i) {
//           record = array + i*0x670
//           0x993d00(record + 0x538, 0, 0x9838a0, record)
//           *(void**)(record + 0x520) = record
//           *(void**)(record + 0x618) = side + i*0x80
//       }
//   if (out_array)  *out_array  = array
//   if (out_stride) *out_stride = 0x670
//   if (out_count)  *out_count  = count
//   canary check; return 0
//
// Modelled: the clamp, both allocations, the stride arithmetic, the per-record constructor call and
// the two per-record stores, plus the three optional out-parameters. Not modelled: the bodies of
// `0x959eb0`, `0x959620`, `0x993d00` and whatever the records own. A missing hook is a visible
// failure; the retail body would call through a fixed address.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable2dd09b0Slot08_9804c0Rva = 0x9804c0;
inline constexpr std::size_t kVtable2dd09b0Slot08_9804c0Size = 0x155;
inline constexpr std::uint64_t kVtable2dd09b0Slot08_9804c0EndAddress = 0x980615;
inline constexpr std::uint64_t kVtable2dd09b0Slot08_9804c0TableAddress = 0x2dd09b0;
inline constexpr std::uint32_t kVtable2dd09b0Slot08_9804c0SlotOffset = 0x08;
inline constexpr std::uint64_t kVtable2dd09b0Slot08_9804c0RelocAddress = 0x2dd09b8;
// Hook targets: guest addresses with no native entry, so they carry no `Rva` suffix.
inline constexpr std::uint64_t kVtable2dd09b0Slot08_9804c0ParseCallAddress = 0x959eb0;
inline constexpr std::uint64_t kVtable2dd09b0Slot08_9804c0AllocCallAddress = 0x959620;
inline constexpr std::uint64_t kVtable2dd09b0Slot08_9804c0CtorCallAddress = 0x993d00;
inline constexpr std::uint64_t kVtable2dd09b0Slot08_9804c0CtorCallbackAddress = 0x9838a0;
inline constexpr std::uint64_t kVtable2dd09b0Slot08_9804c0FormatAddress = 0x12b5527;  // "maxWaves"
inline constexpr std::uint32_t kVtable2dd09b0Slot08_9804c0ParseBufferSize = 0x100;
inline constexpr std::uint32_t kVtable2dd09b0Slot08_9804c0MaxRecords = 0x400;
inline constexpr std::uint32_t kVtable2dd09b0Slot08_9804c0MinRecords = 0x1;
inline constexpr std::uint64_t kVtable2dd09b0Slot08_9804c0RecordSize = 0x670;
inline constexpr std::uint64_t kVtable2dd09b0Slot08_9804c0SideStride = 0x80;
inline constexpr std::uint64_t kVtable2dd09b0Slot08_9804c0CtorDestinationOffset = 0x538;
inline constexpr std::uint64_t kVtable2dd09b0Slot08_9804c0RecordSelfOffset = 0x520;
inline constexpr std::uint64_t kVtable2dd09b0Slot08_9804c0SideSlotOffset = 0x618;

struct Vtable2dd09b0Slot08_9804c0Cells {
    void* context = nullptr;           // rdi at entry: the allocator context
    const char* text = nullptr;        // rsi at entry: the string the count is parsed from
    void** out_array = nullptr;        // rdx (arg 3): optional
    std::uint64_t* out_stride = nullptr;  // rcx (arg 4): optional
    std::uint32_t* out_count = nullptr;   // r8  (arg 5): optional
};

struct Vtable2dd09b0Slot08_9804c0Hooks {
    // Stands in for `call 0x959eb0(text, "maxWaves", 0x100, &count)`. The body ignores any return
    // value and uses the int the callee stores.
    void (*parse_max_waves)(const char* text, std::int32_t* out) = nullptr;
    // Stands in for `call 0x959620(context, size, 1)`.
    void* (*allocate)(void* context, std::uint64_t size, std::uint32_t align) = nullptr;
    // Stands in for `call 0x993d00(destination, 0, 0x9838a0, record)`.
    void (*construct_record)(void* destination, std::uint32_t mode,
                             std::uint64_t callback_address, void* record) = nullptr;
};

struct Vtable2dd09b0Slot08_9804c0Result {
    bool ok = false;
    std::int32_t parsed = 0;          // the int the parse call stored
    std::uint32_t count = 0;          // after the [1, 0x400] clamp
    bool clamped_to_max = false;
    bool clamped_to_one = false;
    void* array = nullptr;
    void* side = nullptr;
    std::uint32_t constructed = 0;
    bool array_written = false;
    bool stride_written = false;
    bool count_written = false;
};

[[nodiscard]] Vtable2dd09b0Slot08_9804c0Result vtable_2dd09b0_slot08_9804c0(
    const Vtable2dd09b0Slot08_9804c0Cells& cells = {},
    const Vtable2dd09b0Slot08_9804c0Hooks& hooks = {}) noexcept;

}  // namespace sotc
