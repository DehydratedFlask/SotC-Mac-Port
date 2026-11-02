#include "../core/vtable_1594dc0_slot10_4b0.hpp"
#include "../core/vtable_15953f8_slot_a0_a8_fios_reads.hpp"

#include <cstdint>
#include <cstdio>
#include <string_view>
#include <vector>

namespace {

using sotc::AllocDispatchContract;
using sotc::FiosReadCells;
using sotc::FiosReadHooks;
using sotc::FiosReadResult;
using sotc::Vtable1594dc0Slot10_4b0Cells;
using sotc::Vtable1594dc0Slot10_4b0Hooks;
using sotc::vtable_1594dc0_slot10_4b0;
using sotc::vtable_15953f8_slot_a0_31870;
using sotc::vtable_15953f8_slot_a8_318b0;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

struct FiosTrace {
    int readv = 0;
    int preadv = 0;
    std::uint32_t count = 0;
    std::uint64_t length = 0;
    std::uint64_t offset = 0;
};

FiosTrace g_fios;

std::int32_t readv(void*, std::uint32_t count, const void*, std::uint64_t length) {
    ++g_fios.readv; g_fios.count = count; g_fios.length = length; return 7;
}
std::int32_t preadv(void*, std::uint32_t count, const void*, std::uint64_t length,
                    std::uint64_t offset) {
    ++g_fios.preadv; g_fios.count = count; g_fios.length = length; g_fios.offset = offset; return 9;
}

void test_constants() {
    check("readv rva", sotc::kVtable15953f8SlotA0_31870Rva == 0x31870);
    check("preadv rva", sotc::kVtable15953f8SlotA8_318b0Rva == 0x318b0);
    check("slots", sotc::kVtable15953f8SlotA0 == 0xa0 && sotc::kVtable15953f8SlotA8 == 0xa8);
    check("plt names", sotc::kFiosReadvPlt == 0xf8f6f0 && sotc::kFiosPreadvPlt == 0xf8f6c0);
    check("marker", sotc::kVtable15953f8Marker == 0xFFFFFFFF00000004ULL);
    check("teardown", sotc::kVtable1594dc0Slot10_4b0Rva == 0x4b0 &&
                          sotc::kVtable1594dc0Slot10ElementStride == 0x50);
}

void test_read_and_pread() {
    g_fios = FiosTrace{};
    std::uint32_t count = 3;
    std::int32_t status = -1;
    std::uint64_t marker = 0;
    FiosReadCells cells{};
    cells.count = &count;
    cells.status = &status;
    cells.marker = &marker;
    cells.length = 512;
    cells.offset = 4096;
    FiosReadHooks hooks{};
    hooks.readv = readv;
    hooks.preadv = preadv;

    const FiosReadResult read = vtable_15953f8_slot_a0_31870(cells, hooks);
    check("readv ok", read.ok && read.service_called && read.status == 7);
    check("readv args", g_fios.readv == 1 && g_fios.count == 3 && g_fios.length == 512);
    check("readv marker", marker == 0xFFFFFFFF00000004ULL && read.marker_written);
    check("readv status stored", status == 7);

    const FiosReadResult pread = vtable_15953f8_slot_a8_318b0(cells, hooks);
    check("preadv ok", pread.ok && pread.service_called && pread.status == 9);
    check("preadv offset", g_fios.preadv == 1 && g_fios.offset == 4096);
    check("preadv status stored", status == 9);

    FiosReadCells missing{};
    check("missing fields fail", !vtable_15953f8_slot_a0_31870(missing, hooks).ok);
}

struct TeardownTrace {
    int releases = 0;
    std::uint32_t frees = 0;
    int posts = 0;
    std::uint32_t tag = 0;
};

TeardownTrace g_teardown;

void release_field(void*) { ++g_teardown.releases; }
void post(void*) { ++g_teardown.posts; }
bool deallocate(void*, std::uint32_t tag) {
    ++g_teardown.frees;
    g_teardown.tag = tag;
    return true;
}

void test_array_teardown() {
    g_teardown = TeardownTrace{};
    // Two elements of 0x50 bytes; the record only needs the begin/end values to line up.
    std::vector<std::uint8_t> elements(2 * 0x50, 0);
    const void* begin = elements.data();
    const void* end = elements.data() + elements.size();
    void* vtable = nullptr;
    AllocDispatchContract contract{};
    contract.deallocate = deallocate;
    Vtable1594dc0Slot10_4b0Cells cells{};
    cells.self = elements.data();
    cells.vtable_field = &vtable;
    cells.array_begin = &begin;
    cells.array_end = &end;
    Vtable1594dc0Slot10_4b0Hooks hooks{};
    hooks.release_element = release_field;
    hooks.post_call = post;
    hooks.alloc = &contract;
    const auto result = vtable_1594dc0_slot10_4b0(cells, hooks);
    check("teardown ok", result.ok);
    check("teardown vtable", vtable == reinterpret_cast<void*>(0x1594dd0));
    check("teardown elements", result.elements == 2);
    check("teardown releases", result.releases == 4 && g_teardown.releases == 4);
    check("teardown free", result.array_freed && g_teardown.frees == 1 && g_teardown.tag == 2);
    check("teardown post", result.post_called && g_teardown.posts == 1);
    check("teardown end address", sotc::kVtable1594dc0Slot10_4b0EndAddress == 0x521);

    // A missing allocation contract makes a non-null array fail visibly.
    Vtable1594dc0Slot10_4b0Hooks no_alloc{};
    no_alloc.release_element = release_field;
    const auto failed = vtable_1594dc0_slot10_4b0(cells, no_alloc);
    check("teardown without contract fails", !failed.ok && !failed.array_freed);
}

}  // namespace

int main() {
    test_constants();
    test_read_and_pread();
    test_array_teardown();

    if (failures == 0) {
        std::printf("OK test_vtable_15953f8_fios_and_4b0\n");
        return 0;
    }
    std::fprintf(stderr, "test_vtable_15953f8_fios_and_4b0: %d failure(s)\n", failures);
    return 1;
}
