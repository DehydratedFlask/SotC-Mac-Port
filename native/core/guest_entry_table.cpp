#include "core/guest_entry_table.hpp"

#include <algorithm>
#include <iterator>

namespace sotc {
namespace {

#include "core/guest_entry_table.inc"

constexpr std::size_t kCount = std::size(kGuestEntryTable);

constexpr bool sorted() {
    for (std::size_t i = 1; i < kCount; ++i) {
        if (kGuestEntryTable[i - 1].rva >= kGuestEntryTable[i].rva) return false;
    }
    return true;
}
static_assert(kCount == 1957, "guest entry table drifted from docs/entry_closure.json");
static_assert(sorted(), "guest entry table must be strictly sorted by RVA");

}  // namespace

const GuestEntry* guest_entry_find(std::uint64_t rva) noexcept {
    const GuestEntry* end = kGuestEntryTable + kCount;
    const GuestEntry* it = std::lower_bound(kGuestEntryTable, end, rva,
                                            [](const GuestEntry& entry, std::uint64_t value) {
                                                return entry.rva < value;
                                            });
    if (it == end || it->rva != rva) return nullptr;
    return it;
}

const GuestEntryCounts& guest_entry_counts() noexcept {
    static const GuestEntryCounts counts = [] {
        GuestEntryCounts c{};
        c.entries = static_cast<std::uint32_t>(kCount);
        for (const GuestEntry& entry : kGuestEntryTable) {
            switch (entry.kind) {
                case GuestEntryKind::FunctionStart: ++c.function_start; break;
                case GuestEntryKind::FunctionInterior: ++c.function_interior; break;
                case GuestEntryKind::Plt: ++c.plt; break;
                case GuestEntryKind::ExecutableData: ++c.executable_data; break;
                case GuestEntryKind::ImageData: ++c.image_data; break;
                case GuestEntryKind::OutsideImage: ++c.outside_image; break;
            }
        }
        return c;
    }();
    return counts;
}

const char* guest_entry_kind_name(GuestEntryKind kind) noexcept {
    switch (kind) {
        case GuestEntryKind::FunctionStart: return "function_start";
        case GuestEntryKind::FunctionInterior: return "function_interior";
        case GuestEntryKind::Plt: return "plt";
        case GuestEntryKind::ExecutableData: return "executable_data";
        case GuestEntryKind::ImageData: return "image_data";
        case GuestEntryKind::OutsideImage: return "outside_image";
    }
    return "unknown";
}

}  // namespace sotc
