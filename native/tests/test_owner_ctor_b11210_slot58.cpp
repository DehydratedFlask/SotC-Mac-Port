// Fixture for owner vtable slot +0x58 (`0xb11210`): the RetailBuild-gated dev property insert.
//
// Pins the gate direction, the exact key/record constants, the sink argument order and every hook
// failure surface. Byte-exact size and the vtable slot wiring are pinned by
// `tools/lift/check_fowner_ctor_b111e0_b11210.py`.
#include <bit>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "core/owner_ctor_b11210_slot58.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {

bool g_retail = false;
void* g_bag = nullptr;
int g_bag_calls = 0;
int g_insert_calls = 0;
void* g_insert_bag = nullptr;
std::int32_t g_insert_key = -1;
std::int32_t g_insert_value_int = -1;
float g_insert_value_float = 0.0F;
std::uint32_t g_insert_tag = 0xFFFFFFFFU;
bool g_insert_ok = true;

void reset() {
    g_retail = false;
    g_bag = reinterpret_cast<void*>(0x1234);
    g_bag_calls = 0;
    g_insert_calls = 0;
    g_insert_bag = nullptr;
    g_insert_key = -1;
    g_insert_value_int = -1;
    g_insert_value_float = 0.0F;
    g_insert_tag = 0xFFFFFFFFU;
    g_insert_ok = true;
}

bool retail_build() {
    return g_retail;
}

void* bag_singleton() {
    ++g_bag_calls;
    return g_bag;
}

bool insert_tree0(void* bag, std::int32_t key, std::int32_t value_int, float value_float,
                  std::uint32_t tag) {
    ++g_insert_calls;
    g_insert_bag = bag;
    g_insert_key = key;
    g_insert_value_int = value_int;
    g_insert_value_float = value_float;
    g_insert_tag = tag;
    return g_insert_ok;
}

OwnerSlot58Hooks full_hooks() {
    OwnerSlot58Hooks hooks{};
    hooks.retail_build = retail_build;
    hooks.bag_singleton = bag_singleton;
    hooks.insert_tree0 = insert_tree0;
    return hooks;
}

}  // namespace

int main() {
    CHECK(kOwnerSlot58Rva == 0xb11210);
    CHECK(kOwnerSlot58Size == 0x69);
    CHECK(kOwnerRetailGateRva == 0x2dfce70);
    CHECK(kOwnerRetailGateFieldOffset == 0x60);
    CHECK(kOwnerBagGlobalRva == 0x3305568);
    CHECK(kOwnerTree0InsertRva == 0xac53e0);
    CHECK(kOwnerTree0InsertSize == 0xb2);
    CHECK(kOwnerBagTree0HandleOffset == 0x18);
    CHECK(kOwnerBagTree0SentinelOffset == 0x20);
    CHECK(kOwnerSlot58Key == 4);
    CHECK(kOwnerSlot58ValueInt == 0x42);
    CHECK(kOwnerSlot58Tag == 2);

    // The guest builds one qword `0x3f80000000000042`: i32 0x42 in the low half, f32 1.0f above.
    CHECK(std::bit_cast<std::uint32_t>(kOwnerSlot58ValueFloat) == kOwnerSlot58ValueFloatBits);
    CHECK(kOwnerSlot58ValueFloatBits == 0x3f800000);
    const std::uint64_t record =
        (static_cast<std::uint64_t>(kOwnerSlot58ValueFloatBits) << 32) |
        static_cast<std::uint32_t>(kOwnerSlot58ValueInt);
    CHECK(record == 0x3f80000000000042ULL);

    // ---- 1. Retail gate non-zero: skip the insert, report it. ----
    reset();
    {
        g_retail = true;
        const OwnerSlot58Result out = owner_slot58_run(full_hooks());
        CHECK(out.ok);
        CHECK(out.gated);
        CHECK(!out.inserted);
        CHECK(g_bag_calls == 0);
        CHECK(g_insert_calls == 0);
    }

    // ---- 2. Gate zero: insert key 4 with the exact record. ----
    reset();
    {
        const OwnerSlot58Result out = owner_slot58_run(full_hooks());
        CHECK(out.ok);
        CHECK(!out.gated);
        CHECK(out.inserted);
        CHECK(g_bag_calls == 1);
        CHECK(g_insert_calls == 1);
        CHECK(g_insert_bag == g_bag);
        CHECK(g_insert_key == 4);
        CHECK(g_insert_value_int == 0x42);
        CHECK(g_insert_value_float == 1.0F);
        CHECK(g_insert_tag == 2);
    }

    // ---- 3. Missing gate hook: visible failure before anything else. ----
    reset();
    {
        OwnerSlot58Hooks hooks = full_hooks();
        hooks.retail_build = nullptr;
        const OwnerSlot58Result out = owner_slot58_run(hooks);
        CHECK(!out.ok);
        CHECK(!out.gated);
        CHECK(g_bag_calls == 0);
        CHECK(g_insert_calls == 0);
    }

    // ---- 4. Missing sink hooks on the dev path: visible failure, no partial insert. ----
    reset();
    {
        OwnerSlot58Hooks no_bag = full_hooks();
        no_bag.bag_singleton = nullptr;
        const OwnerSlot58Result a = owner_slot58_run(no_bag);
        CHECK(!a.ok);
        CHECK(!a.inserted);

        OwnerSlot58Hooks no_insert = full_hooks();
        no_insert.insert_tree0 = nullptr;
        const OwnerSlot58Result b = owner_slot58_run(no_insert);
        CHECK(!b.ok);
        CHECK(!b.inserted);
        CHECK(g_insert_calls == 0);
    }

    // ---- 5. Null bag: reported, not dereferenced. ----
    reset();
    {
        g_bag = nullptr;
        const OwnerSlot58Result out = owner_slot58_run(full_hooks());
        CHECK(!out.ok);
        CHECK(!out.gated);
        CHECK(!out.inserted);
        CHECK(g_bag_calls == 1);
        CHECK(g_insert_calls == 0);
    }

    // ---- 6. Sink rejection surfaces as ok == false. ----
    reset();
    {
        g_insert_ok = false;
        const OwnerSlot58Result out = owner_slot58_run(full_hooks());
        CHECK(!out.ok);
        CHECK(!out.gated);
        CHECK(!out.inserted);
        CHECK(g_insert_calls == 1);
    }

    return 0;
}
