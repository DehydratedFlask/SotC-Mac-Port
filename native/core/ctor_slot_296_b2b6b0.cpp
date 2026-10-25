#include "ctor_slot_296_b2b6b0.hpp"

namespace sotc {
namespace {

// store_tsc_323d0's documented non-zero placeholder for an unusable 0xf8f860 result.
constexpr std::uint64_t kUnusableFrequencySentinel = 1;

}  // namespace

Slot296Result init_slot296_b2b6b0(Slot296Stamp& g, const StoreTsc323d0Hooks& hooks) noexcept {
    Slot296Result out;

    // OBS 0x323e1: cmp qword [0x2ea4700],0 selects the callee's init branch.
    out.guard_was_set = store_tsc_guard_cell_2ea4700() != 0;

    store_tsc_323d0(&g.tsc, hooks);   // the whole 12-byte thunk body (lea rdi,[obj]; jmp 0x323d0)

    out.frequency = store_tsc_guard_cell_2ea4700();
    out.base = store_tsc_global_cell_2ea4708();
    out.stamp = g.tsc;

    const bool hook_present = hooks.init_timebase_f8f860 != nullptr;
    if (!out.guard_was_set && hook_present && out.frequency != 0) {
        out.frequency_from_hook = true;   // the hook supplied this call's published frequency
    } else if (!out.guard_was_set && out.frequency == 0) {
        // A hook returning 0 is not a usable tick frequency (0 is the callee's "uninitialised"
        // state and would re-run 0xf8f860 on every later timer). Normalise to the documented
        // sentinel and report the frequency half as partial.
        store_tsc_guard_cell_2ea4700() = kUnusableFrequencySentinel;
        out.frequency = kUnusableFrequencySentinel;
    }
    return out;
}

}  // namespace sotc
