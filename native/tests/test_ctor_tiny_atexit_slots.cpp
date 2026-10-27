// CPU-01-TINY-ATEXIT-SLOTS-01: fixture for the 19 "zero static object + __cxa_atexit" constructor slots.
// The expected table below is an INDEPENDENT copy of the decoded ELF facts (tools/lift/
// check_ctor_tiny_atexit_slots.py pins the same values against the pinned ELF); it is deliberately not
// derived from kTinyAtexitRows, so a missing, swapped or re-parameterised row fails here.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>

#include "core/concmd_base.hpp"
#include "core/ctor_tiny_atexit_slots.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

struct Exp {
    std::uint32_t slot;
    std::uint64_t rva;
    std::uint32_t size;
    std::uint32_t count;
    std::uint64_t obj[2], dtor[2];
    std::uint32_t off[2], len[2];
};

constexpr Exp kExp[19] = {
    {311, 0xa46b70, 37, 1, {0x32f7630, 0}, {0xa46620, 0}, {0, 0}, {8, 0}},
    {425, 0x54b9c0, 77, 2, {0x2eff448, 0x2eff450}, {0x53fcb0, 0x53fce0}, {0, 0}, {8, 8}},
    {432, 0x51bc20, 42, 1, {0x2efc930, 0}, {0x51b3a0, 0}, {0, 0}, {24, 0}},
    {471, 0x3600f0, 43, 1, {0x2ef54d0, 0}, {0x35efb0, 0}, {8, 0}, {24, 0}},
    {515, 0x1c00e0, 33, 1, {0x2ee68c8, 0}, {0x1bdae0, 0}, {0, 0}, {8, 0}},
    {522, 0x16e960, 33, 1, {0x2ee3e10, 0}, {0x16cda0, 0}, {0, 0}, {8, 0}},
    {523, 0x16cd50, 33, 1, {0x2ee3e08, 0}, {0x16b940, 0}, {0, 0}, {8, 0}},
    {525, 0x166ef0, 33, 1, {0x2ee3cd8, 0}, {0x163ad0, 0}, {0, 0}, {8, 0}},
    {527, 0x157e60, 33, 1, {0x2ee3c20, 0}, {0x156ff0, 0}, {0, 0}, {8, 0}},
    {528, 0x156fb0, 33, 1, {0x2ee3c18, 0}, {0x1561c0, 0}, {0, 0}, {8, 0}},
    {531, 0x1442c0, 33, 1, {0x2ee2cf8, 0}, {0x144290, 0}, {0, 0}, {8, 0}},
    {533, 0x143300, 33, 1, {0x2ee2a50, 0}, {0x143110, 0}, {0, 0}, {8, 0}},
    {542, 0xfb3d0, 33, 1, {0x2edef60, 0}, {0xfaf80, 0}, {0, 0}, {8, 0}},
    {546, 0xa25f0, 33, 1, {0x2ed37c0, 0}, {0xa1da0, 0}, {0, 0}, {8, 0}},
    {547, 0x9a970, 33, 1, {0x2ed37b0, 0}, {0x994c0, 0}, {0, 0}, {8, 0}},
    {549, 0x96650, 33, 1, {0x2ed3670, 0}, {0x95620, 0}, {0, 0}, {8, 0}},
    {551, 0x8d740, 33, 1, {0x2ed33f0, 0}, {0x8d620, 0}, {0, 0}, {8, 0}},
    {564, 0x31220, 33, 1, {0x2dfd678, 0}, {0x2fd20, 0}, {0, 0}, {8, 0}},
    {565, 0x2fcf0, 33, 1, {0x2dfd4b8, 0}, {0x26290, 0}, {0, 0}, {8, 0}},
};

constexpr std::uint8_t kFill = 0xA5;

void fill(TinyAtexitState& s) {
    for (auto& row : s.objects)
        for (auto& o : row) o.bytes.fill(kFill);
}

// Every byte of the state is either untouched (kFill) or inside a documented zero range.
void check_bytes(const TinyAtexitState& s, const std::array<bool, 19>& applied) {
    for (std::size_t r = 0; r < 19; ++r) {
        for (std::size_t k = 0; k < 2; ++k) {
            const auto& bytes = s.objects[r][k].bytes;
            const bool touched = applied[r] && k < kExp[r].count;
            for (std::size_t b = 0; b < bytes.size(); ++b) {
                const bool in_zero = touched && b >= kExp[r].off[k] && b < kExp[r].off[k] + kExp[r].len[k];
                if (bytes[b] != (in_zero ? std::uint8_t{0} : kFill)) {
                    std::fprintf(stderr, "FAIL slot %u record %zu byte %zu = %02x\n", kExp[r].slot, k, b, bytes[b]);
                    std::exit(1);
                }
            }
        }
    }
}

}  // namespace

int main() {
    // 1. The native table equals the independent copy, row by row, in original index order.
    CHECK(kTinyAtexitRows.size() == 19 && kTinyAtexitRowCount == 19);
    CHECK(kTinyAtexitDsoRva == 0x1604218 && kTinyAtexitPltRva == 0xf8f390);
    for (std::size_t i = 0; i < 19; ++i) {
        const TinyAtexitRow& row = kTinyAtexitRows[i];
        const Exp& e = kExp[i];
        CHECK(row.slot == e.slot && row.rva == e.rva && row.body_size == e.size && row.record_count == e.count);
        for (std::uint32_t k = 0; k < 2; ++k) {
            if (k < e.count) {
                CHECK(row.rec[k].object_rva == e.obj[k] && row.rec[k].dtor_rva == e.dtor[k]);
                CHECK(row.rec[k].zero_offset == e.off[k] && row.rec[k].zero_length == e.len[k]);
                CHECK(row.rec[k].zero_offset + row.rec[k].zero_length <= kTinyObjectBytes);
            } else {
                CHECK(row.rec[k].object_rva == 0 && row.rec[k].dtor_rva == 0 && row.rec[k].zero_length == 0);
            }
        }
        if (i > 0) CHECK(kTinyAtexitRows[i - 1].slot < row.slot);   // 311 < 425 < ... < 565 (index order)
        CHECK(find_tiny_atexit_row(e.slot) == &row);
        CHECK(tiny_atexit_row_index(e.slot) == i);
    }
    CHECK(find_tiny_atexit_row(306) == nullptr && find_tiny_atexit_row(0) == nullptr);
    CHECK(tiny_atexit_row_index(566) == kTinyAtexitRowCount);

    // 2. Slots run one at a time into a fresh 0xA5 state: only that row's zero range changes, exactly
    //    one (two for 425) atexit record(s) is appended, and its object pointer is the state buffer.
    for (std::size_t i = 0; i < 19; ++i) {
        const Exp& e = kExp[i];
        auto state = std::make_unique<TinyAtexitState>();
        fill(*state);
        StartupAtexitLog log;
        const TinyAtexitResult r = init_tiny_atexit_slot(e.slot, *state, &log);
        CHECK(r.ok && r.slot == e.slot && r.records_done == e.count && r.blocked == nullptr);
        CHECK(log.size() == e.count);
        for (std::uint32_t k = 0; k < e.count; ++k) {
            CHECK(log.at(k).dtor_rva == e.dtor[k]);
            CHECK(log.at(k).object == state->objects[i][k].bytes.data());
        }
        std::array<bool, 19> applied{};
        applied[i] = true;
        check_bytes(*state, applied);
    }

    // 3. Full ordered run: 20 atexit records (19 slots, 425 registers two), no overlap between slots.
    {
        auto state = std::make_unique<TinyAtexitState>();
        fill(*state);
        StartupAtexitLog log;
        std::size_t expect_log = 0;
        for (std::size_t i = 0; i < 19; ++i) {
            CHECK(init_tiny_atexit_slot(kExp[i].slot, *state, &log).ok);
            expect_log += kExp[i].count;
            CHECK(log.size() == expect_log);
        }
        CHECK(log.size() == 20);
        std::size_t at = 0;
        for (std::size_t i = 0; i < 19; ++i)
            for (std::uint32_t k = 0; k < kExp[i].count; ++k, ++at) {
                CHECK(log.at(at).dtor_rva == kExp[i].dtor[k]);
                CHECK(log.at(at).object == state->objects[i][k].bytes.data());
            }
        std::array<bool, 19> all;
        all.fill(true);
        check_bytes(*state, all);
    }

    // 4. Repeating a slot re-zeroes (dirty bytes cleared) and appends another record: the original is
    //    unguarded, so a second call is not a no-op.
    {
        auto state = std::make_unique<TinyAtexitState>();
        fill(*state);
        StartupAtexitLog log;
        CHECK(init_tiny_atexit_slot(471, *state, &log).ok);
        const std::size_t i = tiny_atexit_row_index(471);
        state->objects[i][0].bytes[8] = 0x77;
        state->objects[i][0].bytes[0x1f] = 0x66;
        state->objects[i][0].bytes[0] = 0x55;   // outside the zero range: must survive
        CHECK(init_tiny_atexit_slot(471, *state, &log).ok);
        CHECK(log.size() == 2);
        CHECK(state->objects[i][0].bytes[8] == 0 && state->objects[i][0].bytes[0x1f] == 0);
        CHECK(state->objects[i][0].bytes[0] == 0x55 && state->objects[i][0].bytes[7] == kFill);
    }

    // 5. Fail closed: null log and unknown slot write nothing and name the stage.
    {
        auto state = std::make_unique<TinyAtexitState>();
        fill(*state);
        const TinyAtexitResult nolog = init_tiny_atexit_slot(311, *state, nullptr);
        CHECK(!nolog.ok && nolog.records_done == 0 && std::strcmp(nolog.blocked, "atexit_log") == 0);
        StartupAtexitLog log;
        for (std::uint32_t bad : {0u, 306u, 310u, 566u, 100000u}) {
            const TinyAtexitResult r = init_tiny_atexit_slot(bad, *state, &log);
            CHECK(!r.ok && r.records_done == 0 && std::strcmp(r.blocked, "unknown_slot") == 0);
        }
        CHECK(log.size() == 0);
        std::array<bool, 19> none{};
        check_bytes(*state, none);
    }

    // 6. The record helper writes only inside its range: guarded 0x28-byte buffer, +8/24 shape (471).
    {
        std::array<std::uint8_t, 0x28> buf;
        buf.fill(kFill);
        StartupAtexitLog log;
        const TinyAtexitRecord rec{0x2ef54d0, 0x35efb0, 8, 24};
        CHECK(init_tiny_atexit_record(rec, buf.data(), log));
        for (std::size_t b = 0; b < buf.size(); ++b)
            CHECK(buf[b] == ((b >= 8 && b < 32) ? std::uint8_t{0} : kFill));
        CHECK(log.size() == 1 && log.at(0).dtor_rva == 0x35efb0 && log.at(0).object == buf.data());
    }

    std::puts("OK test_ctor_tiny_atexit_slots: 19 slots, 20 atexit records, zero ranges and fail-closed paths");
    return 0;
}
