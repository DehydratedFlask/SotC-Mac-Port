#include "vslot_40_533100_post.hpp"

namespace sotc {

Vslot40_533100PostResult vslot_40_533100_post(void* self, void* arg,
                                              const Vslot40_533100PostHooks& hooks) noexcept {
    Vslot40_533100PostResult out{};
    if (self == nullptr) {
        return out;
    }

    // OBS: lea rax,[rip→0x2efc9e0]; mov rdi,[rax]; call 0x525080
    void* f08 = singleton_f08_cell_2efc9e0();
    if (hooks.call_525080 != nullptr) {
        hooks.call_525080(f08);
    }
    ++out.call_525080_count;

    // OBS: mov rdi,self; mov rsi,arg; call 0x2569d0
    if (hooks.call_2569d0 != nullptr) {
        hooks.call_2569d0(self, arg);
    }
    ++out.call_2569d0_count;

    // OBS: lea rdi,[self+8]; lea rsi,"show_areaInfo"; call 0x1fc80
    void* table = static_cast<std::uint8_t*>(self) + 8;
    void* node = nullptr;
    ++out.lookup_count;
    if (hooks.lookup_1fc80_override != nullptr) {
        node = hooks.lookup_1fc80_override(table, kVslot40PostNameShowAreaInfo);
    } else if (hooks.use_lifted_lookup) {
        node = lookup_1fc80(table, kVslot40PostNameShowAreaInfo, hooks.lookup_detail);
    }
    out.lookup_node = node;
    out.lookup_hit = (node != nullptr);

    // OBS: test rax; jne hit — miss/hit bodies NOT modelled
    out.ok = (out.call_525080_count == 1) && (out.call_2569d0_count == 1) &&
             (out.lookup_count == 1);
    return out;
}

}  // namespace sotc
