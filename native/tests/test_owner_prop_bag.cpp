// Fixture for the native owner property bag: duplicate-preserving insert_equal semantics.
#include <cstdio>
#include <cstdlib>
#include <set>

#include "core/owner_prop_bag.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

int main() {
    CHECK(kOwnerPropBagSize == 0x178);
    CHECK(kOwnerPropBagTreeAOffset == 0x48);
    CHECK(kOwnerPropBagTreeBOffset == 0x60);

    // ---- 1. Gate 1: 122 A + 12 B, 114 distinct A keys, duplicates preserved. ----
    {
        OwnerPropBag bag;
        OwnerPropBagSink sink(bag);
        const auto result = apply_owner_properties(OwnerMode2569d0::retail_gate1, sink);
        CHECK(result.ok);
        CHECK(result.inserted == 134);
        CHECK(bag.size_a() == 122);
        CHECK(bag.size_b() == 12);
        CHECK(bag.total() == 134);

        std::set<std::uint32_t> distinct;
        for (const auto& [key, value] : bag.tree_a()) {
            distinct.insert(key);
            CHECK(value.f == 1.0f);
        }
        CHECK(distinct.size() == 114);

        // The eight duplicated keys are 0x7b..0x82, each inserted twice in source order.
        for (std::uint32_t key = 0x7b; key <= 0x82; ++key) {
            CHECK(bag.tree_a().count(key) == 2);
        }
        const auto range = bag.tree_a().equal_range(0x7b);
        auto it = range.first;
        CHECK(it != range.second && it->second.i == 0x0c && it->second.tag == 2);
        ++it;
        CHECK(it != range.second && it->second.i == 0x11 && it->second.tag == 2);

        const auto last = bag.tree_b().find(0x9f);
        CHECK(last != bag.tree_b().end());
        CHECK(last->second.lo == 0.0f);
        CHECK(last->second.hi == 1.0f);
        CHECK(last->second.v == 1.0f);
        CHECK(last->second.tag == 5);
    }

    // ---- 2. Gate 0: 125 A + 12 B, 117 distinct A keys. ----
    {
        OwnerPropBag bag;
        OwnerPropBagSink sink(bag);
        const auto result = apply_owner_properties(OwnerMode2569d0::retail_gate0, sink);
        CHECK(result.ok);
        CHECK(result.inserted == 137);
        CHECK(bag.size_a() == 125);
        CHECK(bag.size_b() == 12);
        std::set<std::uint32_t> distinct;
        for (const auto& [key, value] : bag.tree_a()) {
            (void)value;
            distinct.insert(key);
        }
        CHECK(distinct.size() == 117);
    }

    // ---- 3. None inserts nothing; mode 2 is an explicit failure. ----
    {
        OwnerPropBag bag;
        OwnerPropBagSink sink(bag);
        const auto none = apply_owner_properties(OwnerMode2569d0::none, sink);
        CHECK(none.ok);
        CHECK(none.inserted == 0);
        CHECK(bag.total() == 0);

        const auto mode2 = apply_owner_properties(OwnerMode2569d0::mode2_unreachable, sink);
        CHECK(!mode2.ok);
        CHECK(mode2.inserted == 0);
        CHECK(bag.total() == 0);
    }

    // ---- 4. Direct insert_equal: duplicates and ordered iteration. ----
    {
        OwnerPropBag bag;
        bag.insert_a(0x10, OwnerValueA{1, 1.0f, 2});
        bag.insert_a(0x10, OwnerValueA{2, 1.0f, 6});
        bag.insert_b(0x10, OwnerValueB{0.0f, 1.0f, -1.0f, 3});
        CHECK(bag.size_a() == 2);
        CHECK(bag.tree_a().count(0x10) == 2);
        CHECK(bag.tree_b().count(0x10) == 1);
        auto it = bag.tree_a().begin();
        CHECK(it->second.i == 1);
        ++it;
        CHECK(it->second.i == 2);
    }

    // ---- 5. 0xa2afb0 user selection: initial device + login ids, cap and dedupe. ----
    {
        const int none[4] = {-1, -1, -1, -1};
        auto s = select_bag_users(-1, none, false);
        CHECK(s.count == 1 && s.user_ids[0] == -1 && !s.login_list_ok);

        const int four[4] = {10, 11, 12, 13};
        s = select_bag_users(-1, four, true);
        CHECK(s.count == 4);  // initial -1 + 10,11,12; 13 is dropped by the cap
        CHECK(s.user_ids[0] == -1 && s.user_ids[1] == 10 && s.user_ids[2] == 11 &&
              s.user_ids[3] == 12);

        s = select_bag_users(10, four, true);
        CHECK(s.count == 4);  // 10 is the initial; 11,12,13 are added
        CHECK(s.user_ids[0] == 10 && s.user_ids[1] == 11 && s.user_ids[2] == 12 &&
              s.user_ids[3] == 13);

        const int negatives[4] = {-1, 20, -3, 21};
        s = select_bag_users(-1, negatives, true);
        CHECK(s.count == 3);  // initial -1 + 20 + 21
        CHECK(s.user_ids[1] == 20 && s.user_ids[2] == 21);

        const int duplicates[4] = {20, 20, 21, 22};
        s = select_bag_users(-1, duplicates, true);
        CHECK(s.count == 4);  // no dedupe among login ids
        CHECK(s.user_ids[1] == 20 && s.user_ids[2] == 20 && s.user_ids[3] == 21);

        s = select_bag_users(-1, four, false);
        CHECK(s.count == 1 && s.user_ids[0] == -1);

        // The report's exact cap case: initial 5 + login {0,1,2,3} -> {5,0,1,2}, user 3 dropped.
        const int cap[4] = {0, 1, 2, 3};
        s = select_bag_users(5, cap, true);
        CHECK(s.count == 4);
        CHECK(s.user_ids[0] == 5 && s.user_ids[1] == 0 && s.user_ids[2] == 1 &&
              s.user_ids[3] == 2);
    }

    return 0;
}
