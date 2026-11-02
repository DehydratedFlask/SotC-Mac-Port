#include "../core/vtable_1594dc0_slot10_4b0.hpp"
#include "../core/vtable_15953f8_slot18_31310.hpp"
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
using sotc::Vtable15953f8Slot18_31310Cells;
using sotc::Vtable15953f8Slot18_31310Hooks;
using sotc::vtable_15953f8_slot18_31310;
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

struct WaitTrace {
    int waits = 0;
    int reads = 0;
    int plain = 0;
    int mode_one = 0;
    int finish = 0;
    int pending_rounds = 0;
};
WaitTrace g_wait;

bool sub_init_done(void*) { return false; }
void wait_op(std::uint32_t) { ++g_wait.waits; }
bool positioned_read(void*) {
    ++g_wait.reads;
    return g_wait.reads > g_wait.pending_rounds;   // completes after the configured rounds
}
void plain_read(void*) { ++g_wait.plain; }
void mode_one(void*) { ++g_wait.mode_one; }
void finish(void*) { ++g_wait.finish; }

void test_fios_wait_loop_teardown() {
    g_wait = WaitTrace{};
    g_wait.pending_rounds = 2;
    void* vtable = nullptr;
    std::uint32_t op_handle = 0;
    std::uint32_t mode = 1;
    Vtable15953f8Slot18_31310Cells cells{};
    cells.vtable_field = &vtable;
    cells.op_handle = &op_handle;
    cells.mode = &mode;
    Vtable15953f8Slot18_31310Hooks hooks{};
    hooks.sub_init = sub_init_done;
    hooks.wait = wait_op;
    hooks.positioned_read = positioned_read;
    hooks.plain_read = plain_read;
    hooks.mode_one = mode_one;
    hooks.finish = finish;
    const auto result = vtable_15953f8_slot18_31310(cells, hooks);
    check("wait teardown ok", result.ok);
    check("wait teardown vtable", vtable == reinterpret_cast<void*>(0x1595408));
    check("wait teardown waits", result.waits == 3 && g_wait.waits == 3);
    check("wait teardown reads", result.plain_read_called && g_wait.plain == 1);
    check("wait teardown mode", result.mode_one_called && g_wait.mode_one == 1);
    check("wait teardown finish", result.finish_called && g_wait.finish == 1);
    check("wait teardown free", result.freed && result.free_tag == 1);

    // A wait that never completes is refused instead of spinning.
    g_wait = WaitTrace{};
    g_wait.pending_rounds = 1 << 30;
    const auto overflow = vtable_15953f8_slot18_31310(cells, hooks);
    check("wait overflow refused", !overflow.ok && overflow.wait_overflow);
    check("wait overflow bounded", overflow.waits == sotc::kVtable15953f8Slot18MaxWaits);

    Vtable15953f8Slot18_31310Cells missing{};
    check("wait missing fields fail", !vtable_15953f8_slot18_31310(missing, hooks).ok);
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
    test_fios_wait_loop_teardown();
    test_array_teardown();

    if (failures == 0) {
        std::printf("OK test_vtable_15953f8_fios_and_4b0\n");
        return 0;
    }
    std::fprintf(stderr, "test_vtable_15953f8_fios_and_4b0: %d failure(s)\n", failures);
    return 1;
}
