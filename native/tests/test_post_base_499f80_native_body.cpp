#include <cstdio>
#include <cstdlib>

#include "core/init_531d60_post_base_499f80.hpp"
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
    static void* allocate(std::size_t size, std::uint32_t, std::uint32_t, std::uint32_t) {
        return std::malloc(size);
    }
    static bool deallocate(void* ptr, std::uint32_t) {
        if (ptr == nullptr) return true;
        std::free(ptr);
        return true;
    }
};

}  // namespace

int main() {
    std::uint8_t app[0x4000]{};
    void* store = nullptr;
    void* old_obj = nullptr;
    void* default_arg = app + 0x100;

    sotc::Init499f80Globals globals{};
    globals.default_arg = default_arg;
    globals.store_cell = &store;
    globals.old_object_cell = &old_obj;

    sotc::Init531d60PostBase499f80Hooks hooks{};
    hooks.body_globals = &globals;
    hooks.body_hooks = sotc::default_init_499f80_body_hooks(
        sotc::AllocDispatchContract{&TestAlloc::allocate, &TestAlloc::deallocate});

    const auto r = sotc::init_531d60_post_base_499f80(app, hooks);
    CHECK(r.ok && r.body_native && r.body.ok);
    CHECK(r.arg == app + sotc::kInit531d60PostBase499f80ArgOff);
    CHECK(store == r.arg);
    CHECK(old_obj == r.body.new_object);

    std::puts("OK test_post_base_499f80_native_body");
    return 0;
}
