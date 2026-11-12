#include "ctor_slot_474_344850.hpp"

#include "lane_m4_shapes.hpp"

namespace sotc {

namespace {

// 0x34486f..0x3448e7 (and the three twins): the 0x30 byte holder is built from a stack view (view copy at +8, zero tail at +0x28, class pointer
// stored last), the temporary view is released by an exact no-op f590 and __cxa_atexit(dtor, holder) is recorded.
bool build_holder(NameHolder30& holder, const char* name, std::size_t length, std::uint64_t vptr, std::uint64_t dtor, ConCmdRegistry& registry) noexcept {
    const SharedStr view = l2::measured_view(name, length);
    holder.name = view;
    holder.tail = 0;
    holder.vptr = vptr;
    const bool inert = release_inert_view_f590(view);
    const bool recorded = registry.atexit_log.record(dtor, &holder);
    return inert && recorded;
}

}  // namespace

Slot474Result init_slot474_344850(Slot474Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot474Result out{};
    NameHolder30* const holders[kSlot474HolderCount] = {&objects.holder0, &objects.holder1, &objects.holder2, &objects.holder3};
    ConCmdBoolVar* const bvars[kSlot474BoolCount] = {&objects.bvar0, &objects.bvar1, &objects.bvar2};
    for (std::size_t n = 0; n < kSlot474HolderCount; ++n) {
        const bool class_a = n < 2;
        out.holder_ok[n] = build_holder(*holders[n], kSlot474HolderNames[n], kSlot474HolderNameLengths[n],
                                        class_a ? kSlot474HolderVptrA : kSlot474HolderVptrB,
                                        class_a ? kSlot474HolderDtorA : kSlot474HolderDtorB, concmd);
        if (!out.holder_ok[n]) return out;
    }
    const auto bool_var = [&](std::size_t n) noexcept {
        out.bvar[n] = lane_m6::init_bool_var_literal(*bvars[n], kSlot474BoolNames[n], kSlot474BoolNameLengths[n], "", 0, kSlot474BoolFnTag, concmd,
                                                     kSlot474BoolValues[n], kSlot474BoolValues[n]);
        return out.bvar[n].ok;
    };
    if (!bool_var(0)) return out;
    if (!bool_var(1)) return out;
    // 0x344c1c..0x344c63: 0x257a0(string var, name, init) ; f590(init) ; f590(name) ; __cxa_atexit(0x23c90, var)
    const SharedStr name = lane_m4::borrowed_view_of(kSlot474StringName, kSlot474StringNameLength);
    const SharedStr init = lane_m4::borrowed_view_of(kSlot474StringInit, kSlot474StringInitLength);
    out.svar = init_string_var_257a0(objects.svar, name, init, concmd);
    if (!out.svar.ok) return out;
    out.svar_views_inert = release_inert_view_f590(init) && release_inert_view_f590(name);
    out.svar_atexit_recorded = concmd.atexit_log.record(kStringVarDtorRva, &objects.svar);
    if (!bool_var(2)) return out;
    out.ok = out.svar_views_inert && out.svar_atexit_recorded;
    for (std::size_t n = 0; n < kSlot474HolderCount; ++n) out.ok = out.ok && out.holder_ok[n];
    for (std::size_t n = 0; n < kSlot474BoolCount; ++n) out.ok = out.ok && out.bvar[n].ok;
    return out;
}

}  // namespace sotc
