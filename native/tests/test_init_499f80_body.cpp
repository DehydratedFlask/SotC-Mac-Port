#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "core/critsec_3b310.hpp"
#include "core/init_499f80_body.hpp"

#define CHECK(c)                                                                                   \
    do {                                                                                           \
        if (!(c)) {                                                                                \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);                       \
            std::exit(1);                                                                          \
        }                                                                                          \
    } while (0)

namespace {

struct TestAlloc {
    std::vector<void*> live;

    static void* allocate(std::size_t size, std::uint32_t, std::uint32_t, std::uint32_t flags) {
        auto* self = current();
        void* p = std::malloc(size);
        if (p != nullptr) self->live.push_back(p);
        (void)flags;
        return p;
    }

    static bool deallocate(void* ptr, std::uint32_t) {
        if (ptr == nullptr) return true;
        auto* self = current();
        for (auto it = self->live.begin(); it != self->live.end(); ++it) {
            if (*it == ptr) {
                self->live.erase(it);
                std::free(ptr);
                return true;
            }
        }
        return false;
    }

    static TestAlloc* current() {
        static TestAlloc inst;
        return &inst;
    }
};

sotc::AllocDispatchContract test_contract() {
    sotc::AllocDispatchContract c{};
    c.allocate = &TestAlloc::allocate;
    c.deallocate = &TestAlloc::deallocate;
    return c;
}

}  // namespace

int main() {
    using sotc::Init499f80Globals;
    using sotc::run_init_499f80_body;

    CHECK(sotc::kInit499f80BodyRva == 0x499f80);
    CHECK(sotc::kInit499f80BodySize == 193);

    void* store = nullptr;
    void* old_obj = nullptr;
    void* default_arg = reinterpret_cast<void*>(0x1234);

    Init499f80Globals g{};
    g.default_arg = default_arg;
    g.store_cell = &store;
    g.old_object_cell = &old_obj;

    const auto hooks = sotc::default_init_499f80_body_hooks(test_contract());
    const auto r1 = run_init_499f80_body(nullptr, g, hooks);
    CHECK(r1.ok && r1.arg_used == default_arg && store == default_arg && old_obj == r1.new_object);
    CHECK(r1.new_object != nullptr);
    CHECK(sotc::critsec_init_at(r1.new_object) == EALREADY);

    const auto r2 = run_init_499f80_body(reinterpret_cast<void*>(0xABCD), g, hooks);
    CHECK(r2.ok && r2.previous_object == r1.new_object);

    std::puts("OK test_init_499f80_body");
    return 0;
}
