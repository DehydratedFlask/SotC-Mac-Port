#include "ctor_slot_486_2d5e00.hpp"

namespace sotc {

Slot486Result init_slot486_2d5e00(Slot486Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot486Result out{};
    const SharedStr int_name = lane_m4::borrowed_view_of(kSlot486IntName, sizeof kSlot486IntName - 1);
    const SharedStr empty = lane_m4::borrowed_view_of("", 0);   // rodata NUL byte 0xfce360
    out.int_var = lane_m4::init_int_var_site(objects.int_var, int_name, empty, kSlot486ArgFnTag, concmd,
                                             kSlot486Value, kSlot486Default);
    if (!out.int_var.ok) return out;

    const SharedStr string_name = lane_m4::borrowed_view_of(kSlot486StringName, sizeof kSlot486StringName - 1);
    out.string_var = init_string_var_257a0(objects.string_var, string_name, empty, concmd);
    if (!out.string_var.ok) return out;
    out.string_views_inert = release_inert_view_f590(empty) && release_inert_view_f590(string_name);
    out.string_atexit_recorded = concmd.atexit_log.record(kStringVarDtorRva, &objects.string_var);
    out.ok = out.int_var.ok && out.string_var.ok && out.string_views_inert && out.string_atexit_recorded;
    return out;
}

}  // namespace sotc
