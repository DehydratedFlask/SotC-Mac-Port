// Continue-C slot-0 sites call run_lazy_bind_91210 and read *(bound+0x60).
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/init_531d60_post_base_continue_c.hpp"
#include "core/lazy_bind_91210.hpp"

#define CHECK(c)                                                                                   \
    do {                                                                                           \
        if (!(c)) {                                                                                \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);                       \
            std::exit(1);                                                                          \
        }                                                                                          \
    } while (0)

namespace {

using namespace sotc;

struct Host {
    void* sub = nullptr;
    int v18 = 1;
    int v30 = 0;
};

void* lookup(void* host, void*) { return static_cast<Host*>(host)->sub; }
int v18(void* host, void*) { return static_cast<Host*>(host)->v18; }
int v30(void* host, void*) { return static_cast<Host*>(host)->v30; }

struct Demo {
    std::uint8_t bytes[0x40]{};
};

struct Sub {
    std::uint8_t bytes[0x68]{};
};

void point_flag(Sub& sub, std::uint8_t* flag) {
    void* p = flag;
    std::memcpy(sub.bytes + 0x60, &p, sizeof(p));
}

Init531d60PostBaseContinueCHooks native_hooks(Demo& demo, Host& host) {
    Init531d60PostBaseContinueCHooks hooks{};
    hooks.use_native_lazy_bind_91210 = true;
    hooks.demo_object = &demo;
    hooks.lazy_bind.lookup_21620 = &lookup;
    hooks.lazy_bind.vcall_18 = &v18;
    hooks.lazy_bind.vcall_30 = &v30;
    hooks.lazy_bind.host = &host;
    return hooks;
}

}  // namespace

int main() {
    Sub sub{};
    std::uint8_t flag = 0;
    point_flag(sub, &flag);
    Host host{};
    host.sub = &sub;

    {
        Demo demo{};
        const auto taken = init_531d60_post_base_continue_c(native_hooks(demo, host));
        CHECK(taken.ok);
        CHECK(taken.lazy_bind_calls == 3);
        CHECK(taken.lazy_bind_ok);
        CHECK(taken.lazy_bind_last_returned == 1);
        CHECK(taken.vcall_count == 1);
        CHECK(taken.flag1_known && taken.flag2_known);
        CHECK(taken.sil_iter1 == 1 && taken.sil_iter2 == 1);
        CHECK(taken.merge_branch_outcome_modelled);
        CHECK(taken.merge_branch_taken);
        CHECK(!taken.branch_outcome_modelled);
    }
    {
        Demo demo{};
        flag = 1;
        const auto fall = init_531d60_post_base_continue_c(native_hooks(demo, host));
        CHECK(fall.ok);
        CHECK(fall.sil_iter1 == 0 && fall.sil_iter2 == 0);
        CHECK(fall.merge_branch_outcome_modelled);
        CHECK(!fall.merge_branch_taken);
    }
    {
        Demo demo{};
        flag = 0;
        auto hooks = native_hooks(demo, host);
        hooks.enter_via_5328e0 = true;
        const auto via = init_531d60_post_base_continue_c(hooks);
        CHECK(via.ok);
        CHECK(via.preamble_ran);
        CHECK(via.preamble_r14b == 1);
        CHECK(via.first_esi == 1);
        CHECK(via.lazy_bind_calls == 4);
        CHECK(via.merge_branch_taken);
    }
    {
        Demo demo{};
        Init531d60PostBaseContinueCHooks hooks{};
        hooks.use_native_lazy_bind_91210 = true;
        hooks.demo_object = &demo;
        const auto closed = init_531d60_post_base_continue_c(hooks);
        CHECK(!closed.ok);
        CHECK(closed.lazy_bind_calls == 1);
    }
    {
        const auto plain = init_531d60_post_base_continue_c({});
        CHECK(plain.ok);
        CHECK(plain.lazy_bind_calls == 0);
        CHECK(!plain.merge_branch_outcome_modelled);
    }

    std::printf("OK init_continue_c_lazy_bind\n");
    return 0;
}
