#include "vtable_2dd09b0_slot08_9804c0.hpp"

#include <cstring>

namespace sotc {

Vtable2dd09b0Slot08_9804c0Result vtable_2dd09b0_slot08_9804c0(
    const Vtable2dd09b0Slot08_9804c0Cells& cells,
    const Vtable2dd09b0Slot08_9804c0Hooks& hooks) noexcept {
    Vtable2dd09b0Slot08_9804c0Result out{};

    // `movq %rsi,%rdi; leaq -0x34(%rbp),%rcx; callq 0x959eb0` is unconditional.
    if (cells.text == nullptr || hooks.parse_max_waves == nullptr || hooks.allocate == nullptr) {
        return out;
    }
    std::int32_t parsed = 0;
    hooks.parse_max_waves(cells.text, &parsed);
    out.parsed = parsed;

    // `cmpl $0x400,%eax; cmovbel %eax,%ecx` then `testl %eax,%eax; cmovel %edx,%ecx` (edx == 1).
    std::uint32_t count = static_cast<std::uint32_t>(parsed);
    if (count > kVtable2dd09b0Slot08_9804c0MaxRecords) {
        count = kVtable2dd09b0Slot08_9804c0MaxRecords;
        out.clamped_to_max = true;
    }
    if (parsed == 0) {
        count = kVtable2dd09b0Slot08_9804c0MinRecords;
        out.clamped_to_one = true;
    }
    out.count = count;

    // Both allocations are issued before the null checks.
    const std::uint64_t array_size =
        static_cast<std::uint64_t>(count) * kVtable2dd09b0Slot08_9804c0RecordSize;
    const std::uint64_t side_size =
        static_cast<std::uint64_t>(count) * kVtable2dd09b0Slot08_9804c0SideStride;
    out.array = hooks.allocate(cells.context, array_size, 1);
    out.side = hooks.allocate(cells.context, side_size, 1);

    // `testq %rbx,%rbx; je` then `testl %esi,%esi; je`.
    if (out.array != nullptr && count != 0) {
        if (hooks.construct_record == nullptr) {
            return out;  // `callq 0x993d00` runs once per record
        }
        auto* base = static_cast<std::uint8_t*>(out.array);
        for (std::uint32_t index = 0; index < count; ++index) {
            const std::uint64_t record_offset =
                static_cast<std::uint64_t>(index) * kVtable2dd09b0Slot08_9804c0RecordSize;
            void* record = base + record_offset;
            auto* destination =
                base + record_offset + kVtable2dd09b0Slot08_9804c0CtorDestinationOffset;
            hooks.construct_record(destination, 0,
                                   kVtable2dd09b0Slot08_9804c0CtorCallbackAddress, record);
            // `movq %r15,0x520(%rbx,%r13)` and `movq %rcx,0x618(%rbx,%r13)`.
            std::memcpy(base + record_offset + kVtable2dd09b0Slot08_9804c0RecordSelfOffset,
                        &record, sizeof(record));
            void* side_entry = reinterpret_cast<void*>(
                reinterpret_cast<std::uintptr_t>(out.side) +
                static_cast<std::uintptr_t>(index) * kVtable2dd09b0Slot08_9804c0SideStride);
            std::memcpy(base + record_offset + kVtable2dd09b0Slot08_9804c0SideSlotOffset,
                        &side_entry, sizeof(side_entry));
            ++out.constructed;
        }
    }

    // The three out-parameters are each null-checked independently.
    if (cells.out_array != nullptr) {
        *cells.out_array = out.array;
        out.array_written = true;
    }
    if (cells.out_stride != nullptr) {
        *cells.out_stride = kVtable2dd09b0Slot08_9804c0RecordSize;
        out.stride_written = true;
    }
    if (cells.out_count != nullptr) {
        *cells.out_count = count;
        out.count_written = true;
    }

    out.ok = true;  // `xorl %eax,%eax` after the canary check
    return out;
}

}  // namespace sotc
