// CPU-01-AOT-CFG-SCALE-01-BATCH25: fixture for the 0xa38960 blob reader and its two helpers.
// Boundaries: the slot's static blob; a leaf-only stream; a nested group (invalid by the validator's
// invert rule); an end marker; the recursion cap; the unterminated-stream cap; the allocator hooks.
#include "l4_test_util.hpp"

#include <array>
#include <cstring>
#include <vector>

#include "core/blob_parser_a38960.hpp"

namespace {
using namespace l4;
using namespace sotc;

struct Pool {
    std::array<std::uint64_t, 8> words{};
    std::size_t used = 0;
    std::size_t frees = 0;
};
Pool g_pool;

void* pool_alloc(std::size_t bytes) noexcept {
    const std::size_t words = (bytes + 7) / 8;
    if (g_pool.used + words > g_pool.words.size()) return nullptr;
    void* p = &g_pool.words[g_pool.used];
    g_pool.used += words;
    return p;
}

void pool_release(void* block) noexcept {
    if (block != nullptr) ++g_pool.frees;
}

void constants() {
    CHECK(kBlobParseRva == 0xa38960 && kBlobParseSize == 0x1b4);
    CHECK(kBlobValidateRva == 0xa39eb0 && kBlobValidateSize == 0x120);
    CHECK(kBlobAccumulateRva == 0xa39fd0 && kBlobAccumulateSize == 0xf0);
    CHECK(kBlobSentinel == 0xbeefbeef && kBlobKindMask == 0xf0000000);
    CHECK(kBlobKindLeaf == 0x10000000 && kBlobKindGroup == 0x20000000 && kBlobKindEnd == 0x30000000);
    CHECK(kBlobMaxDepth == 64 && kBlobMaxDwords == 4096 && sizeof(BlobHeader) == 0x28);
}

void slot_blob() {
    BlobArena arena;
    BlobHeader h;
    const std::uint32_t blob[2] = {0x10000020, 0xbeefbeef};
    const BlobParseResult r = parse_blob_a38960(h, blob, 2, arena);
    CHECK(r.ok && h.valid == 1);
    CHECK(h.a == 0x20 && h.size == 0x20 && h.bits == 2 && h.bytes == 1 && h.total == 0x21);
    CHECK(r.payload_dwords == 2 && r.allocations == 3 && r.first_refcount_released);
    CHECK(h.data != nullptr && h.data[0] == 0x10000020 && h.data[1] == 0xbeefbeef);
    CHECK(arena.control[0] == 0 && arena.control[1] == 1);   // first refcount decremented to zero
    CHECK(h.control == &arena.control[1]);

    // The three passes each start from b = 1, so a single leaf reports bits = 2.
    BlobCursor c{blob, 2};
    BlobAccum acc{};
    acc.b = 1;
    CHECK(blob_accumulate(c, acc) && acc.a == 0x20 && acc.b == 2 && acc.c == 0);
}

void leaf_blob() {
    BlobArena arena;
    BlobHeader h;
    const std::uint32_t blob[2] = {0x10000003, 0xbeefbeef};
    const BlobParseResult r = parse_blob_a38960(h, blob, 2, arena);
    CHECK(r.ok && h.valid == 1);
    CHECK(h.a == 3 && h.size == 3 && h.bits == 2 && h.bytes == 1 && h.total == 4);
    CHECK(r.payload_dwords == 2);
}

void group_blob() {
    BlobArena arena;
    BlobHeader h;
    // group(count 2) { leaf(count 1), kind-3 end } sentinel: a sub-run must end on kind 3, because
    // the outer loop continues after the recursion returns. valid stays 1 and the payload is all four
    // dwords. (A sub-run ended by the terminator makes the outer loop read past the stream; the port
    // refuses that instead, see stream_end.)
    const std::uint32_t blob[4] = {0x20000002, 0x10000001, 0x30000000, 0xbeefbeef};
    const BlobParseResult r = parse_blob_a38960(h, blob, 4, arena);
    CHECK(r.ok && h.valid == 1);
    CHECK(h.a == 2 && h.size == 2 && h.bits == 4 && h.bytes == 1 && h.total == 3);
    CHECK(r.payload_dwords == 4 && r.first_refcount_released);
    CHECK(h.data != nullptr && h.data[3] == 0xbeefbeef && h.control == &arena.control[1]);
}

void end_marker() {
    BlobArena arena;
    BlobHeader h;
    const std::uint32_t blob[2] = {0x30000000, 0xbeefbeef};
    const BlobParseResult r = parse_blob_a38960(h, blob, 2, arena);
    CHECK(r.ok && h.valid == 0);       // flag &= (invert ^ 1) with invert = 1
    CHECK(h.a == 0 && h.size == 0 && h.bits == 1 && h.bytes == 1 && h.total == 1);
}

void depth_cap() {
    std::vector<std::uint32_t> blob(71, 0x20000001);
    blob.back() = 0xbeefbeef;
    BlobArena arena;
    BlobHeader h;
    const BlobParseResult r = parse_blob_a38960(h, blob.data(), blob.size(), arena);
    CHECK(!r.ok && r.allocations == 0 && h.valid == 0 && h.control == nullptr);
}

void stream_end() {
    // Eight leaves and no terminator: the validator runs off the declared stream and refuses.
    std::vector<std::uint32_t> blob(8, 0x10000001);
    BlobArena arena;
    BlobHeader h;
    const BlobParseResult r = parse_blob_a38960(h, blob.data(), blob.size(), arena);
    CHECK(!r.ok && r.allocations == 0 && h.control == nullptr);

    // A group whose sub-run ends on the terminator leaves the outer loop with nowhere to read.
    const std::uint32_t bad[3] = {0x20000002, 0x10000001, 0xbeefbeef};
    BlobHeader h2;
    const BlobParseResult r2 = parse_blob_a38960(h2, bad, 3, arena);
    CHECK(!r2.ok && r2.allocations == 0);
}

void allocator_hooks() {
    g_pool = Pool{};
    BlobHooks hooks{};
    hooks.allocate = &pool_alloc;
    hooks.release = &pool_release;
    BlobArena arena;
    BlobHeader h;
    const std::uint32_t blob[2] = {0x10000020, 0xbeefbeef};
    const BlobParseResult r = parse_blob_a38960(h, blob, 2, arena, hooks);
    CHECK(r.ok && r.allocations == 3 && r.first_refcount_released);
    CHECK(g_pool.used == 3 && g_pool.frees == 1);
    CHECK(h.control != nullptr && *h.control == 1);

    BlobHooks half{};
    half.allocate = &pool_alloc;   // no release: the mandatory free cannot be modelled
    BlobHeader h2;
    const BlobParseResult refused = parse_blob_a38960(h2, blob, 2, arena, half);
    CHECK(!refused.ok && refused.allocations == 0 && h2.control == nullptr);
}

}  // namespace

int main() {
    constants();
    slot_blob();
    leaf_blob();
    group_blob();
    end_marker();
    depth_cap();
    stream_end();
    allocator_hooks();
    std::printf("OK test_blob_parser_a38960\n");
    return 0;
}
