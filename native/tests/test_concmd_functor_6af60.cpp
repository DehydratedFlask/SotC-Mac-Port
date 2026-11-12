// CPU-01-AOT-CFG-SCALE-01-BATCH27: fixture for the functor-command helper pair 0x6b0f0/0x6af60.
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/concmd_functor_6af60.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {

std::size_t g_stages = 0;
bool count_stage(void*, ConCmdFunctor& object, unsigned stage) noexcept {
    ++g_stages;
    object.region[0] = stage + 1;
    return true;
}

bool view_is(const SharedStr& v, const char* text) {
    return v.data != nullptr && v.size == static_cast<std::int64_t>(std::strlen(text)) && v.aux == 0 &&
           v.ctl == nullptr && std::memcmp(v.data, text, std::strlen(text)) == 0 && v.data[v.size] == 0;
}

}  // namespace

int main() {
    const SharedStr name = SharedStr{reinterpret_cast<const std::uint8_t*>("functorCommand"), 14, 0, nullptr};
    const SharedStr description = SharedStr{reinterpret_cast<const std::uint8_t*>(""), 0, 0, nullptr};
    const ConCmdFunctorSpec spec{reinterpret_cast<const void*>(0x2ea8ee8), 0x44};

    ConCmdFunctorView view{};
    view.q0 = 0x1111111111111111ull;
    view.q8 = 0x2222222222222222ull;
    view.q10 = 0x3333333333333333ull;
    view.q18 = 0x4444444444444444ull;
    for (std::size_t i = 0; i < sizeof view.payload; ++i) view.payload[i] = static_cast<std::uint8_t>(i);
    view.vtable = 0x15b18e8;
    view.tag48 = 0x5555555555555555ull;

    ConCmdFunctorHooks hooks{};
    hooks.transfer = count_stage;

    ConCmdFunctor inner{};
    ConCmdRegistry registry{};
    const ConCmdFunctorResult r1 =
        init_concmd_functor_6b0f0(inner, name, description, 3, 7, 9, spec, view, hooks);
    CHECK(r1.ok && !r1.registered && r1.transfers == 5 && g_stages == 5);
    CHECK(inner.base.vptr == kConCmdFunctorInnerVptr);
    CHECK(inner.base.flags == 3 && inner.base.registered == 0);
    CHECK(inner.spec == reinterpret_cast<std::uint64_t>(spec.address));
    CHECK(inner.low == 7 && inner.one70 == 1 && inner.high == 9 && inner.one78 == 1);
    CHECK(inner.view0 == reinterpret_cast<std::uint64_t>(&inner.view0));   // q18 != 0
    CHECK(inner.view8 == view.q8 && inner.view10 == view.q10 && inner.view18 == view.q18);
    CHECK(inner.tag_c0 == kConCmdFunctorTagVptr && inner.tag_c8 == view.tag48);
    CHECK(inner.region[0] == 5);
    CHECK(registry.pending.size() == 0);

    ConCmdFunctor outer{};
    ConCmdRegistry registry2{};
    g_stages = 0;
    const ConCmdFunctorResult r2 =
        init_concmd_functor_6af60(outer, name, description, 1, 0, 0x24, spec, view, registry2, hooks);
    CHECK(r2.ok && r2.registered && r2.transfers == 5 && g_stages == 5);
    CHECK(outer.base.vptr == kConCmdFunctorFinalVptr);
    CHECK(outer.base.registered == 1 && outer.base.flags == 1);
    CHECK(outer.spec_word == 0x44);
    CHECK(outer.low == 0 && outer.high == 0x24);
    CHECK(outer.tag_c0 == kConCmdFunctorTagVptr && outer.tag_c8 == view.tag48);
    CHECK(view_is(outer.base.name, "functorCommand"));
    CHECK(view_is(outer.base.description, ""));
    CHECK(registry2.pending.size() == 1);

    std::printf("concmd_functor_6af60: inner 352 B / outer 397 B, 5 transfers, registration queued\n");
    return 0;
}
