#include "vslot_40_533100_tail_140d90.hpp"

#include <cstring>

namespace sotc {
namespace {

alignas(8) std::uint8_t g_path_dtor_standin[8]{};

}  // namespace

Vslot40_533100Tail140d90Result vslot_40_533100_tail_140d90(
    void* self, const Vslot40_533100Tail140d90Hooks& hooks) noexcept {
    Vslot40_533100Tail140d90Result out{};

    if (self == nullptr) {
        return out;
    }

    // OBS: mov esi, 9; mov rdi, r15 (= self+0x3cc8); call 0x140d90
    out.slot_obj = static_cast<std::uint8_t*>(self) + kVslot40Tail140d90SlotOff;
    out.arg = kVslot40Tail140d90Arg;
    if (hooks.call_140d90 != nullptr) {
        hooks.call_140d90(out.slot_obj, out.arg);
    }
    ++out.call_140d90_count;

    // OBS: lea rdi, [rbp-0x70]; call 0xf590
    if (hooks.dtor_f590 != nullptr) {
        hooks.dtor_f590(g_path_dtor_standin);
    }
    ++out.dtor_f590_count;

    // stack-check / epilogue @ 0x534052 — see vslot_40_533100_stackchk.*
    out.ok = (out.call_140d90_count == 1) && (out.dtor_f590_count == 1) &&
             (out.arg == kVslot40Tail140d90Arg);
    return out;
}

}  // namespace sotc
