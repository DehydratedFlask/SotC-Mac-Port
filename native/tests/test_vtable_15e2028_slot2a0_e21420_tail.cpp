#include "../core/vtable_15e2028_slot2a0_e21420_tail.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string_view>
#include <vector>

namespace {

using sotc::E21420FoundCells;
using sotc::E21420FoundHooks;
using sotc::E21420FoundResult;
using sotc::E21420MissingCells;
using sotc::E21420MissingHooks;
using sotc::E21420MissingResult;
using sotc::E21420TailStatus;
using sotc::e21420_found;
using sotc::e21420_missing;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

struct Trace {
    int lookups = 0;
    int tails = 0;
    int allocs = 0;
    int releases = 0;
    int subtree = 0;
    void* tail_object = nullptr;
    std::uint8_t tail_byte = 0xFF;
    std::int32_t tail_value = 7;
};

Trace g;

void* lookup_found(void*, const char*) { ++g.lookups; return reinterpret_cast<void*>(0x1234); }
void* lookup_null(void*, const char*) { ++g.lookups; return nullptr; }
std::int32_t tail(void*, void*, void* object, std::uint8_t byte) {
    ++g.tails;
    g.tail_object = object;
    g.tail_byte = byte;
    return g.tail_value;
}
void* allocate(void*, std::uint32_t size) {
    ++g.allocs;
    return size == sotc::kE21420AllocationSize ? reinterpret_cast<void*>(0x9000) : nullptr;
}
std::int32_t count_two(void*) { return 2; }
const char* names[2] = {"alpha", "beta"};
const char* name_at(void*, std::int32_t index) {
    return index >= 0 && index < 2 ? names[index] : nullptr;
}
void release(void*) { ++g.releases; }
bool subtree(void*) { ++g.subtree; return true; }

void test_constants() {
    check("found rva", sotc::kE21420Found18000Rva == 0xe2167e);
    check("found size", sotc::kE21420Found18000Size == 0x4b);
    check("missing rva", sotc::kE21420Missing18000Rva == 0xe214f7);
    check("missing size", sotc::kE21420Missing18000Size == 0xbf);
    check("record vtable", sotc::kE21420MissingVtable == 0x15e2d70);
    check("tail slot", sotc::kE21420TailSlot == 0x148);
    check("statuses", sotc::kE21420StatusMissing == 0x1b &&
                          sotc::kE21420StatusScanExhausted == 0x14);
}

void test_found_paths() {
    g = Trace{};
    E21420FoundCells cells{};
    cells.provider = reinterpret_cast<void*>(0x1);
    cells.name = "alpha";
    cells.arg4_byte = 0x5A;
    E21420FoundHooks hooks{};
    hooks.lookup = lookup_found;
    hooks.tail = tail;
    const E21420FoundResult ok = e21420_found(cells, hooks);
    check("found ok", ok.ok && ok.code == 7);
    check("found hooks", g.lookups == 1 && g.tails == 1 && g.tail_object ==
                             reinterpret_cast<void*>(0x1234) && g.tail_byte == 0x5A);

    g = Trace{};
    hooks.lookup = lookup_null;
    const E21420FoundResult missing = e21420_found(cells, hooks);
    check("lookup missing ok", missing.ok && missing.code == 0x1b);
    check("lookup missing status", missing.status == E21420TailStatus::lookup_missing);
    check("lookup missing no tail", g.tails == 0);

    E21420FoundHooks no_tail{};
    no_tail.lookup = lookup_found;
    const E21420FoundResult without = e21420_found(cells, no_tail);
    check("tail missing fails", !without.ok && without.status == E21420TailStatus::tail_missing);

    E21420FoundCells no_provider{};
    const E21420FoundResult none = e21420_found(no_provider, hooks);
    check("no provider fails", !none.ok && none.status == E21420TailStatus::no_provider);
}

void test_missing_record_layout() {
    g = Trace{};
    std::vector<std::uint8_t> record(sotc::kE21420AllocationSize, 0xCC);
    E21420MissingCells cells{};
    cells.provider = reinterpret_cast<void*>(0xABCD);
    cells.name = "beta";
    cells.record = record.data();
    E21420MissingHooks hooks{};
    hooks.allocate = allocate;
    hooks.count = count_two;
    hooks.name_at = name_at;
    hooks.release = release;
    const E21420MissingResult result = e21420_missing(cells, hooks);
    check("missing ok", result.ok && result.record_constructed);
    check("missing status", result.code == 0x1b || result.code == 0x14);
    check("missing rescan matched", result.rescan_matched && result.code == 0x1b);
    check("missing scanned", result.scanned == 2);
    check("missing no release", !result.release_called && g.releases == 0);

    std::uint64_t vtable = 0;
    std::uint64_t owner = 0;
    std::memcpy(&vtable, record.data() + sotc::kE21420RecordVtable, 8);
    std::memcpy(&owner, record.data() + sotc::kE21420RecordOwner, 8);
    check("record vtable", vtable == sotc::kE21420MissingVtable);
    check("record owner", owner == 0xABCD);
    bool zeros = true;
    for (std::uint32_t offset = sotc::kE21420RecordZeroA; offset < 0x40; ++offset) {
        if (record[offset] != 0) zeros = false;
    }
    check("record zeroed body", zeros);
    check("record flags", record[0x40] == 1 && record[0x41] == 1 && record[0x42] == 1);
}

void test_missing_without_match_releases() {
    g = Trace{};
    std::vector<std::uint8_t> record(sotc::kE21420AllocationSize, 0);
    E21420MissingCells cells{};
    cells.provider = reinterpret_cast<void*>(0x1);
    cells.name = "gamma";
    cells.record = record.data();
    E21420MissingHooks hooks{};
    hooks.allocate = allocate;
    hooks.count = count_two;
    hooks.name_at = name_at;
    hooks.release = release;
    const E21420MissingResult result = e21420_missing(cells, hooks);
    check("exhausted status", result.ok && result.code == 0x14);
    check("exhausted released", result.release_called && g.releases == 1);
}

void test_missing_null_name_subtree() {
    g = Trace{};
    std::vector<std::uint8_t> record(sotc::kE21420AllocationSize, 0);
    E21420MissingCells cells{};
    cells.provider = reinterpret_cast<void*>(0x1);
    cells.name = nullptr;
    cells.record = record.data();
    E21420MissingHooks hooks{};
    hooks.allocate = allocate;
    hooks.null_name_subtree = subtree;
    const E21420MissingResult result = e21420_missing(cells, hooks);
    check("null name reported", result.ok && result.null_name_subtree);
    check("null name subtree hook", g.subtree == 1);
    check("null name no rescan", result.scanned == 0);
}

void test_missing_requires_storage() {
    E21420MissingCells cells{};
    cells.provider = reinterpret_cast<void*>(0x1);
    cells.name = "x";
    E21420MissingHooks hooks{};
    hooks.allocate = allocate;
    const E21420MissingResult result = e21420_missing(cells, hooks);
    check("no storage fails", !result.ok && result.status == E21420TailStatus::no_provider);
}

}  // namespace

int main() {
    test_constants();
    test_found_paths();
    test_missing_record_layout();
    test_missing_without_match_releases();
    test_missing_null_name_subtree();
    test_missing_requires_storage();

    if (failures == 0) {
        std::printf("OK test_vtable_15e2028_slot2a0_e21420_tail\n");
        return 0;
    }
    std::fprintf(stderr, "test_vtable_15e2028_slot2a0_e21420_tail: %d failure(s)\n", failures);
    return 1;
}
