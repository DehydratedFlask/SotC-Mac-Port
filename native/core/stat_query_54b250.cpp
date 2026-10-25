#include "stat_query_54b250.hpp"

#include <array>

namespace sotc {

StatQuery54b250Result stat_query_54b250(void* this_obj, std::uint32_t arg1, std::uint32_t arg2,
                                         const StatsEngine54b250& engine) noexcept {
    StatQuery54b250Result out{};

    // 0x60 B stack descriptor: zeroed through +0x20, traits table at +0x40, zeros at +0x30/+0x48.
    std::array<std::uintptr_t, kStatQuery54b250DescriptorSize / sizeof(std::uintptr_t)> desc{};
    desc[0x40 / sizeof(std::uintptr_t)] = kStatQuery54b250TraitsRva;
    desc[0x30 / sizeof(std::uintptr_t)] = 0;
    desc[0x48 / sizeof(std::uintptr_t)] = 0;
    out.traits_40 = desc[0x40 / sizeof(std::uintptr_t)];
    out.zero_30 = desc[0x30 / sizeof(std::uintptr_t)];
    out.zero_48 = desc[0x48 / sizeof(std::uintptr_t)];

    // call 0xa56560(this, arg1, arg2, &descriptor, 0, 0)
    out.engine_known = (engine.query != nullptr);
    if (engine.query != nullptr) {
        out.engine_ok = engine.query(this_obj, arg1, arg2, desc.data(), 0, 0);
    }
    ++out.engine_calls;

    // call *0x18(descriptor[+0x40]) -> 0x15c9920 slot 3 = 0x538200, a bare ret. Pinned, elided.
    ++out.noop_dispatches;

    // Canary compare and fail path are not modelled: the guard is read-only and can only abort.
    out.ok = (out.engine_calls == 1) && (out.noop_dispatches == 1) && out.engine_known &&
             out.engine_ok;
    return out;
}

}  // namespace sotc
