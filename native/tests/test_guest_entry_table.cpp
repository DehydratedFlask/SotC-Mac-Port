// Fixture for the guest-address entry-kind table (CPU-01-ENTRY-MAP-CLOSURE-01).
// Asserts the measured classification from docs/entry_closure.json and spot-checks one RVA per
// kind, including the negative lookup.
#include <cstdio>
#include <cstdlib>

#include "core/guest_entry_table.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {

GuestEntryKind kind_of(std::uint64_t rva) {
    const GuestEntry* entry = guest_entry_find(rva);
    CHECK(entry != nullptr);
    return entry->kind;
}

}  // namespace

int main() {
    const GuestEntryCounts& counts = guest_entry_counts();
    CHECK(counts.entries == 1957);
    CHECK(counts.function_start == 830);
    CHECK(counts.function_interior == 381);
    CHECK(counts.plt == 12);
    CHECK(counts.executable_data == 203);
    CHECK(counts.image_data == 531);
    CHECK(counts.outside_image == 0);

    // One measured sample per kind, from the pinned ELF's FDE/PT_LOAD map.
    CHECK(kind_of(0x531d60) == GuestEntryKind::FunctionStart);   // Init entry, FDE start
    CHECK(kind_of(0x5b9f90) == GuestEntryKind::FunctionStart);   // menu page builder, FDE start
    CHECK(kind_of(0x2170) == GuestEntryKind::FunctionStart);     // S1 page builder, FDE start
    CHECK(kind_of(0x1353) == GuestEntryKind::FunctionInterior);  // kFn1290End, inside 0x1290
    CHECK(kind_of(0xf8f390) == GuestEntryKind::Plt);             // __cxa_atexit stub
    CHECK(kind_of(0x1014a80) == GuestEntryKind::ExecutableData); // literal in the R+X segment
    CHECK(kind_of(0x1594d90) == GuestEntryKind::ImageData);      // vtable in 0x1594000
    CHECK(guest_entry_find(0x12345678) == nullptr);

    CHECK(guest_entry_kind_name(GuestEntryKind::FunctionStart)[0] == 'f');
    CHECK(guest_entry_kind_name(GuestEntryKind::Plt)[0] == 'p');

    std::printf("OK test_guest_entry_table: entries=%u start=%u interior=%u plt=%u xdata=%u data=%u\n",
                counts.entries, counts.function_start, counts.function_interior, counts.plt,
                counts.executable_data, counts.image_data);
    return 0;
}
