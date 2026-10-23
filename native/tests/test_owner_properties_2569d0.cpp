// Fixture for the 0x2569d0 mode classification and the reachable property rows.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "core/owner_properties_2569d0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {
struct Row {
    char map;
    std::uint16_t key;
    std::int32_t i;
    float f;
    std::uint32_t tag;
    std::uint64_t pair;
    float v;
};

struct Sink : OwnerPropertySink {
    std::vector<Row> rows;
    int fail_at = -1;
    int calls = 0;

    bool push(Row row) {
        if (fail_at >= 0 && calls == fail_at) {
            return false;
        }
        rows.push_back(row);
        ++calls;
        return true;
    }
    bool insert_a(std::uint16_t key, std::int32_t i, float f, std::uint32_t tag) override {
        return push({'A', key, i, f, tag, 0, 0.0f});
    }
    bool insert_b(std::uint16_t key, std::uint64_t pair, float v, std::uint32_t tag) override {
        return push({'B', key, 0, 0.0f, tag, pair, v});
    }
};
}  // namespace

int main() {
    CHECK(classify_owner_mode(true, 1) == OwnerMode2569d0::retail_gate1);
    CHECK(classify_owner_mode(false, 1) == OwnerMode2569d0::retail_gate0);
    CHECK(classify_owner_mode(true, 2) == OwnerMode2569d0::mode2_unreachable);
    CHECK(classify_owner_mode(false, 2) == OwnerMode2569d0::mode2_unreachable);
    CHECK(classify_owner_mode(true, 0) == OwnerMode2569d0::none);
    CHECK(classify_owner_mode(false, 3) == OwnerMode2569d0::none);

    // ---- 1. Gate1: 2 head + pending 0x23 + 130 tail + final = 134. ----
    {
        Sink sink;
        const auto result = apply_owner_properties(OwnerMode2569d0::retail_gate1, sink);
        CHECK(result.ok);
        CHECK(result.inserted == 134);
        CHECK(sink.rows.size() == 134);
        CHECK(sink.rows[0].map == 'A' && sink.rows[0].key == 0x21 && sink.rows[0].i == 8 &&
              sink.rows[0].tag == 2);
        CHECK(sink.rows[1].key == 0x22 && sink.rows[1].tag == 8);
        CHECK(sink.rows[2].key == 0x23 && sink.rows[2].i == 8 && sink.rows[2].tag == 6);
        CHECK(sink.rows[3].key == 0x00 && sink.rows[3].i == 15 && sink.rows[3].tag == 2);
        CHECK(sink.rows.back().map == 'B' && sink.rows.back().key == 0x9f);
        CHECK(sink.rows.back().pair == 0x3f80000000000000ULL);
        CHECK(sink.rows.back().v == 1.0f);
        CHECK(sink.rows.back().tag == 5);
        for (const Row& row : sink.rows) {
            if (row.map == 'A') {
                CHECK(row.f == 1.0f);
            }
        }
    }

    // ---- 2. Gate0: 5 head + pending 0x1c + 130 tail + final = 137. ----
    {
        Sink sink;
        const auto result = apply_owner_properties(OwnerMode2569d0::retail_gate0, sink);
        CHECK(result.ok);
        CHECK(result.inserted == 137);
        CHECK(sink.rows[0].key == 0x21 && sink.rows[0].i == 30 && sink.rows[0].tag == 2);
        CHECK(sink.rows[1].key == 0x22 && sink.rows[1].i == 30 && sink.rows[1].tag == 8);
        CHECK(sink.rows[2].key == 0x23 && sink.rows[2].i == 30 && sink.rows[2].tag == 6);
        CHECK(sink.rows[3].key == 0x08 && sink.rows[3].i == 33 && sink.rows[3].tag == 2);
        CHECK(sink.rows[4].key == 0x1b && sink.rows[4].i == 34 && sink.rows[4].tag == 2);
        CHECK(sink.rows[5].key == 0x1c && sink.rows[5].i == 13 && sink.rows[5].tag == 2);
        CHECK(sink.rows[6].key == 0x00);
        CHECK(sink.rows.back().key == 0x9f);
    }

    // ---- 3. None inserts nothing; mode2 is an explicit failure. ----
    {
        Sink none;
        const auto r1 = apply_owner_properties(OwnerMode2569d0::none, none);
        CHECK(r1.ok);
        CHECK(r1.inserted == 0);
        Sink mode2;
        const auto r2 = apply_owner_properties(OwnerMode2569d0::mode2_unreachable, mode2);
        CHECK(!r2.ok);
        CHECK(r2.inserted == 0);
        CHECK(mode2.rows.empty());
    }

    // ---- 4. A sink failure stops at the exact row. ----
    {
        Sink sink;
        sink.fail_at = 2;  // first two rows succeed, the pending 0x23 fails
        const auto result = apply_owner_properties(OwnerMode2569d0::retail_gate1, sink);
        CHECK(!result.ok);
        CHECK(result.inserted == 2);
        CHECK(result.failed_key == 0x23);
        CHECK(sink.rows.size() == 2);
    }

    return 0;
}
