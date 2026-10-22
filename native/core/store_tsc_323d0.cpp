#include "store_tsc_323d0.hpp"

#include <cstring>

#if defined(__APPLE__)
#include <mach/mach_time.h>
#endif

namespace sotc {
namespace {

std::uint64_t g_guard_2ea4700 = 0;
std::uint64_t g_global_2ea4708 = 0;

}  // namespace

std::uint64_t& store_tsc_guard_cell_2ea4700() noexcept { return g_guard_2ea4700; }
std::uint64_t& store_tsc_global_cell_2ea4708() noexcept { return g_global_2ea4708; }

std::uint64_t host_rdtsc64() noexcept {
#if defined(__APPLE__)
    return mach_absolute_time();
#else
    // Fallback for non-Apple hosts in CI; not the measured Apple Silicon path.
    return 1;
#endif
}

void store_tsc_323d0(void* dest, const StoreTsc323d0Hooks& hooks) noexcept {
    if (dest == nullptr) {
        return;
    }
    // OBS: cmp [0x2ea4700],0 ; jne skip_init
    if (g_guard_2ea4700 == 0) {
        std::uint64_t init = 1;  // default non-zero sentinel (PLT body NOT modelled)
        if (hooks.init_timebase_f8f860 != nullptr) {
            init = hooks.init_timebase_f8f860();
        }
        g_guard_2ea4700 = init;
        g_global_2ea4708 = host_rdtsc64();
    }
    const std::uint64_t t = host_rdtsc64();
    std::memcpy(dest, &t, sizeof(t));
}

}  // namespace sotc
