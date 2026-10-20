// Independent oracle: the specification "buffer becomes all zero, slot becomes zero, nothing else changes".
// Canary bytes around the buffer detect off-by-one memset sizes; no value is copied from the binary.
#include <array>
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <thread>

#include "core/scratch_reset.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {
struct Block {
    std::array<std::byte, 16> before;
    std::array<std::byte, sotc::kScratchBytes> buffer;
    std::array<std::byte, 16> after;
};
}  // namespace

int main(int argc, char**) {
    auto* block = new Block;
    block->before.fill(std::byte{0xC3});
    block->buffer.fill(std::byte{0xA5});
    block->after.fill(std::byte{0x3C});
    std::atomic<std::uint64_t> slot{0xDEADBEEFCAFEF00Dull};

    sotc::reset_scratch(block->buffer, slot);

    for (auto b : block->buffer) CHECK(b == std::byte{0});
    for (auto b : block->before) CHECK(b == std::byte{0xC3});
    for (auto b : block->after) CHECK(b == std::byte{0x3C});
    CHECK(slot.load() == 0);

    // Idempotent on already-zero state; also valid when slot is already zero.
    sotc::reset_scratch(block->buffer, slot);
    CHECK(slot.load() == 0 && block->buffer.front() == std::byte{0} && block->buffer.back() == std::byte{0});

    // Concurrent observer never sees a torn 64-bit value (only the old value or zero).
    slot.store(0x1122334455667788ull);
    std::atomic<bool> go{false}, torn{false};
    std::thread reader([&] {
        while (!go.load()) {}
        for (int i = 0; i < 100000; ++i) {
            auto v = slot.load();
            if (v != 0x1122334455667788ull && v != 0) torn = true;
        }
    });
    go = true;
    sotc::reset_scratch(block->buffer, slot);
    reader.join();
    CHECK(!torn.load());

    delete block;
    if (argc > 1) std::printf("{\"scratch_bytes\":%zu}\n", sotc::kScratchBytes);
    else std::puts("scratch_reset: all checks passed");
    return 0;
}
