#include "ctor_slot_400_62b100.hpp"

namespace sotc {

Slot400Result init_slot400_62b100(Slot400Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot400Result out{};
    ConCmdBoolVar* const bvars[kSlot400BoolCount] = {&objects.bvar0, &objects.bvar1, &objects.bvar2, &objects.bvar3, &objects.bvar4};
    DebugMenuItem* const bitems[kSlot400BoolCount] = {&objects.bitem0, &objects.bitem1, &objects.bitem2, &objects.bitem3, &objects.bitem4};
    const auto bool_var = [&](std::size_t n) noexcept {
        out.bvar[n] = lane_m6::init_bool_var_literal(*bvars[n], kSlot400BoolNames[n], kSlot400BoolNameLengths[n], "", 0, kSlot400BoolFnTag, concmd, kSlot400BoolValues[n], kSlot400BoolValues[n]);
        return out.bvar[n].ok;
    };
    const auto toggle = [&](std::size_t n) noexcept {
        out.bitem[n] = init_toggle_site_26db0(*bitems[n], *bvars[n], kSlot400Parents[n], concmd, menu);   // 0x26db0 (label = the parent string)
        return out.bitem[n].ok;
    };
    if (!bool_var(0)) return out;
    if (!bool_var(1)) return out;
    if (!bool_var(2)) return out;
    if (!toggle(0)) return out;
    if (!toggle(1)) return out;
    if (!toggle(2)) return out;
    if (!bool_var(3)) return out;
    if (!bool_var(4)) return out;
    if (!toggle(3)) return out;
    if (!toggle(4)) return out;
    out.ok = true;
    return out;
}

}  // namespace sotc
