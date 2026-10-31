#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

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

struct Object {
    std::uint8_t bytes[0x40]{};
};

void* bound_of(Object& object) {
    void* value = nullptr;
    std::memcpy(&value, object.bytes + kLazyBind91210BoundOffset, sizeof(value));
    return value;
}

struct Host {
    void* sub = nullptr;
    int v18 = 1;
    int v30 = 0;
    int lookups = 0;
};

void* lookup(void* host, void* name_slot) {
    auto* h = static_cast<Host*>(host);
    ++h->lookups;
    CHECK(name_slot != nullptr);
    return h->sub;
}

int v18(void* host, void*) { return static_cast<Host*>(host)->v18; }
int v30(void* host, void*) { return static_cast<Host*>(host)->v30; }

LazyBind91210Hooks hooks_for(Host& host) {
    LazyBind91210Hooks hooks{};
    hooks.lookup_21620 = &lookup;
    hooks.vcall_18 = &v18;
    hooks.vcall_30 = &v30;
    hooks.host = &host;
    return hooks;
}

}  // namespace

int main() {
    int sentinel = 0;
    Host host{};
    host.sub = &sentinel;

    {
        Object object{};
        void* already = &sentinel;
        std::memcpy(object.bytes + kLazyBind91210BoundOffset, &already, sizeof(already));
        const auto fast = run_lazy_bind_91210(&object, hooks_for(host));
        CHECK(fast.ok && fast.fast_path && fast.returned == 1);
        CHECK(host.lookups == 0);
    }
    {
        Object object{};
        host.sub = nullptr;
        const auto miss = run_lazy_bind_91210(&object, hooks_for(host));
        CHECK(miss.ok && miss.returned == 0 && bound_of(object) == nullptr);
    }
    {
        Object object{};
        host.sub = &sentinel;
        host.v18 = 0;
        const auto reject = run_lazy_bind_91210(&object, hooks_for(host));
        CHECK(reject.ok && !reject.stored && reject.returned == 0);
    }
    {
        Object object{};
        host.v18 = 1;
        host.v30 = 0;
        const auto kept = run_lazy_bind_91210(&object, hooks_for(host));
        CHECK(kept.ok && kept.stored && !kept.rolled_back && kept.returned == 1);
        CHECK(bound_of(object) == &sentinel);
    }
    {
        Object object{};
        host.v30 = 7;
        const auto rolled = run_lazy_bind_91210(&object, hooks_for(host));
        CHECK(rolled.ok && rolled.stored && rolled.rolled_back && rolled.returned == 0);
        CHECK(bound_of(object) == nullptr);
    }
    {
        Object object{};
        const auto closed = run_lazy_bind_91210(&object, {});
        CHECK(!closed.ok && closed.returned == 0);
    }

    std::printf("OK lazy_bind_91210\n");
    return 0;
}
