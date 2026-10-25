#include "owner_prop_bag.hpp"

namespace sotc {

BagUserSelection select_bag_users(int initial_user, const int login_ids[4],
                                 bool login_list_ok) noexcept {
    BagUserSelection out{};
    out.user_ids[0] = initial_user;  // site A is unconditional, even for -1
    out.count = 1;
    out.login_list_ok = login_list_ok;
    if (!login_list_ok) {
        return out;
    }
    for (int i = 0; i < 4; ++i) {
        const int id = login_ids[i];
        if (id < 0 || id == initial_user) {
            continue;
        }
        // Sites D/E check `r13 <= 2`; r13 == count - 1, so count == 4 is full.
        if (i >= 2 && out.count - 1 > 2) {
            continue;
        }
        if (out.count >= 4) {
            continue;
        }
        out.user_ids[out.count++] = id;
    }
    return out;
}

OwnerBagConstructResult owner_bag_construct(OwnerPropBag& bag,
                                            const OwnerBagConstructHooks& hooks) noexcept {
    OwnerBagConstructResult out{};
    out.pad_init_called = hooks.sce_pad_init != nullptr && hooks.sce_pad_init();
    const int initial = hooks.initial_user != nullptr ? hooks.initial_user() : -1;
    out.gesture_init_called = hooks.gesture_init != nullptr && hooks.gesture_init();
    int login_ids[4] = {-1, -1, -1, -1};
    const bool login_ok =
        hooks.login_user_ids != nullptr && hooks.login_user_ids(login_ids);
    out.users = select_bag_users(initial, login_ids, login_ok);
    out.tables_created = out.users.count;
    bag.set_mode_10(1);  // last on every original path
    if (hooks.publish != nullptr) {
        hooks.publish(bag);
        out.published = true;
    }
    out.ok = out.pad_init_called && hooks.initial_user != nullptr && out.published;
    return out;
}

void OwnerPropBag::insert_a(std::uint32_t key, const OwnerValueA& value) {
    tree_a_.emplace(key, value);  // multimap: equal keys are kept in insertion order
}

void OwnerPropBag::insert_b(std::uint32_t key, const OwnerValueB& value) {
    tree_b_.emplace(key, value);
}

bool OwnerPropBagSink::insert_a(std::uint16_t key, std::int32_t i, float f,
                                std::uint32_t tag) {
    bag_.insert_a(key, OwnerValueA{i, f, tag});
    return true;
}

bool OwnerPropBagSink::insert_b(std::uint16_t key, float lo, float hi, float v,
                                std::uint32_t tag) {
    bag_.insert_b(key, OwnerValueB{lo, hi, v, tag});
    return true;
}

}  // namespace sotc
