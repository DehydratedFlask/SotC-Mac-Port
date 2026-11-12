// Fixture for the guest-address entry-kind/callable table (CPU-01-AOT-CFG-CALLABLE-01).
// Asserts the measured classification from docs/entry_closure.json and spot-checks one RVA per kind
// and per callable shape, including the negative lookup.
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

GuestEntryCallable callable_of(std::uint64_t rva) {
    const GuestEntry* entry = guest_entry_find(rva);
    CHECK(entry != nullptr);
    return entry->callable;
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
    CHECK(counts.retail_entry == 786);
    CHECK(counts.jmp_thunk == 10);
    CHECK(counts.ret_stub == 34);
    CHECK(counts.not_callable == 1127);

    // One measured sample per kind, from the pinned ELF's FDE/PT_LOAD map.
    CHECK(kind_of(0x531d60) == GuestEntryKind::FunctionStart);
    CHECK(kind_of(0x5b9f90) == GuestEntryKind::FunctionStart);
    CHECK(kind_of(0x1353) == GuestEntryKind::FunctionInterior);
    CHECK(kind_of(0xf8f390) == GuestEntryKind::Plt);
    CHECK(kind_of(0x1014a80) == GuestEntryKind::ExecutableData);
    CHECK(kind_of(0x1594d90) == GuestEntryKind::ImageData);

    // Callable shape: a normal frame entry, a direct jmp thunk, a one-byte ret, a non-entry.
    CHECK(callable_of(0x531d60) == GuestEntryCallable::RetailEntry);
    CHECK(callable_of(0x5b9f90) == GuestEntryCallable::RetailEntry);
    CHECK(callable_of(0x30e50) == GuestEntryCallable::JmpThunk);
    CHECK(callable_of(0x3b380) == GuestEntryCallable::JmpThunk);
    CHECK(callable_of(0x56dca0) == GuestEntryCallable::RetStub);
    CHECK(callable_of(0x1353) == GuestEntryCallable::NotCallable);
    CHECK(guest_entry_find(0x12345678) == nullptr);

    // Indirect frontier measured from the pinned disassembly (105 functions / 499 sites).
    const GuestIndirectSummary& indirect = guest_entry_indirect();
    CHECK(indirect.functions == 105);
    CHECK(indirect.sites == 499);
    CHECK(indirect.resolved == 222);
    CHECK(indirect.blocked == 277);

    CHECK(guest_entry_kind_name(GuestEntryKind::FunctionStart)[0] == 'f');
    CHECK(guest_entry_callable_name(GuestEntryCallable::RetailEntry)[0] == 'r');

    std::printf("OK test_guest_entry_table: entries=%u start=%u retail=%u thunk=%u stub=%u "
                "indirect=%u/%u\n",
                counts.entries, counts.function_start, counts.retail_entry, counts.jmp_thunk,
                counts.ret_stub, indirect.resolved, indirect.sites);
    return 0;
}
