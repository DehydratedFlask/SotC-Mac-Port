#include "ctor_slot_550_91290.hpp"

namespace sotc {

namespace {

// Build the 0x30-byte holder from a stack view: view copy at +8, zero tail at +0x28, vptr, f590(view) (exact
// no-op), __cxa_atexit(0x8ef00, holder).
bool build_holder(NameHolder30& holder, const char* text, std::size_t length, ConCmdRegistry& registry) noexcept {
    const SharedStr view = l2::measured_view(text, length);
    holder.name = view;
    holder.tail = 0;
    holder.vptr = kSlot550HolderVptr;
    const bool inert = release_inert_view_f590(view);
    const bool recorded = registry.atexit_log.record(kSlot550HolderDtorRva, &holder);
    return inert && recorded;
}

}  // namespace

Slot550Result init_slot550_91290(Slot550Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot550Result out{};
    out.var0 = lane_m6::init_bool_var_literal(objects.var0, kSlot550Var0Name, kSlot550Var0NameLength,
                                                "", 0, kSlot550BoolFnTag, concmd, 0, 0);
    out.item0 = lane_m6::init_toggle_site_target(objects.item0, debug_menu_target_for(objects.var0),
                                                  kSlot550Item0Label, kSlot550Item0LabelLength, kSlot550Item0Text,
                                                  kSlot550Item0TextLength, concmd, menu);
    out.item1 = lane_m6::init_toggle_site_target(objects.item1, debug_menu_target_for(objects.var0),
                                                  kSlot550Item1Label, kSlot550Item1LabelLength, kSlot550Item1Text,
                                                  kSlot550Item1TextLength, concmd, menu);
    out.holder0_ok = build_holder(objects.holder0, kSlot550Holder0Name, kSlot550Holder0NameLength, concmd);
    out.ok = out.var0.ok && out.item0.ok && out.item1.ok && out.holder0_ok;
    return out;
}

}  // namespace sotc
