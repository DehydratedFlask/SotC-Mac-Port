#include "ctor_slot_408_5c7f50.hpp"

#include <cstring>

namespace sotc {

namespace {

// The 0x27120 slider: ctor(item, label, variable, 0x30, lo, hi) (text = the variable's own name view), 0x268b0, 0xf590(label), __cxa_atexit(0x272d0, item).
[[maybe_unused]] bool slider_item(DebugMenuItem& item, const char* label, std::size_t label_length, const ConCmdRangedVar& var, std::uint32_t lo_bits,
                 std::uint32_t hi_bits, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    float lo = 0.0f;
    float hi = 0.0f;
    std::memcpy(&lo, &lo_bits, sizeof lo);
    std::memcpy(&hi, &hi_bits, sizeof hi);
    const SharedStr view = family_view(label, label_length);
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot408ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}





// Console command (0xf2f0 name view, 0x20af0 flags 2, register-once [0x1594dc0 + 0x20], two f590, __cxa_atexit(0x4b0)); the handler is an identity tag, never called.
[[maybe_unused]] bool cmd_var(ConCmdCommand& command, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot408CmdDesc[n];
    const FConCmdCommandSpec spec{kSlot408CmdNames[n], kSlot408MenuStrings[d], kSlot408MenuStringLengths[d], kSlot408CmdHandler[n]};
    const FConCmdBuildResult built = init_fconcmd_object(command, spec, concmd);
    return built.ok && built.vtable_register_slot_ok && built.views_inert && built.atexit_recorded && !built.handler_invoked;
}








// The 0x30 byte holder: view copy at +8, zero tail at +0x28, vptr identity tag; the stack view is released by an exact no-op f590 and
// __cxa_atexit(dtor, holder) is recorded (dtor 0x8ef00 or 0x2fced0 per holder class).
bool name_holder(NameHolder30& holder, const char* name, std::size_t length, std::uint64_t vptr, std::uint64_t dtor, ConCmdRegistry& concmd) noexcept {
    const SharedStr view = family_view(name, length);
    holder.name = view;
    holder.tail = 0;
    holder.vptr = vptr;
    const bool inert = release_inert_view_f590(view);
    const bool recorded = concmd.atexit_log.record(dtor, &holder);
    return inert && recorded;
}

// Sentinel container: the self-referential header stores of the original (q20, then q00 = q08, then q10); the +0x18 field and the 0x80 B area are never written. __cxa_atexit(dtor, &container) is recorded.
bool sentinel_container(Slot408Sentinel& c, std::uint64_t dtor, ConCmdRegistry& concmd) noexcept {
    const std::uintptr_t base = reinterpret_cast<std::uintptr_t>(&c);
    c.q20 = base + 0x28;
    c.q00 = base + 0x28;
    c.q08 = base + 0x28;
    c.q10 = base + 0xa8;
    return concmd.atexit_log.record(dtor, &c);
}

// Zero-filled container: [+0x08, +0x18) and [+0x18, +0x20) are zeroed, nothing else is written; __cxa_atexit(0x1c5090, container) is recorded (identity tag).
bool zero_container(Slot408Zero& c, std::uint64_t dtor, ConCmdRegistry& concmd) noexcept {
    c.q08 = 0;
    c.q10 = 0;
    c.q18 = 0;
    return concmd.atexit_log.record(dtor, &c);
}

// Perspective row: 0x1580a0(out, &fov, aspect, near, far, flag). Every argument is a constant of the slot body (the fov immediate, three .rodata dwords, edx = 1); tanf is the host hook.
bool perspective_row(Slot408Perspective& p, TanfFn tanf_hook) noexcept {
    float fov = 0.0f;
    float aspect = 0.0f;
    float near_plane = 0.0f;
    float far_plane = 0.0f;
    std::memcpy(&fov, &kSlot408PerspectiveFovBits, sizeof fov);
    std::memcpy(&aspect, &kSlot408PerspectiveAspectBits, sizeof aspect);
    std::memcpy(&near_plane, &kSlot408PerspectiveNearBits, sizeof near_plane);
    std::memcpy(&far_plane, &kSlot408PerspectiveFarBits, sizeof far_plane);
    return helper_1580a0_perspective(p.m, &fov, aspect, near_plane, far_plane, kSlot408PerspectiveFlag, tanf_hook).ok;
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot408Result init_slot408_5c7f50(Slot408Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot408Result out{};

    if (!perspective_row(objects.r00, objects.perspective_tanf)) return out;
    ++out.rows_done;   // row 00 pre
    if (!name_holder(objects.r01, kSlot408MenuStrings[0], kSlot408MenuStringLengths[0], kSlot408HolderVptr, kSlot408HolderDtorRva, concmd)) return out;
    ++out.rows_done;   // row 01 holder
    if (!name_holder(objects.r02, kSlot408MenuStrings[1], kSlot408MenuStringLengths[1], kSlot408HolderVptr, kSlot408HolderDtorRva, concmd)) return out;
    ++out.rows_done;   // row 02 holder
    if (!name_holder(objects.r03, kSlot408MenuStrings[2], kSlot408MenuStringLengths[2], kSlot408HolderVptr, kSlot408HolderDtorRva, concmd)) return out;
    ++out.rows_done;   // row 03 holder
    if (!name_holder(objects.r04, kSlot408MenuStrings[3], kSlot408MenuStringLengths[3], kSlot408HolderVptr, kSlot408HolderDtorRva, concmd)) return out;
    ++out.rows_done;   // row 04 holder
    if (!name_holder(objects.r05, kSlot408MenuStrings[4], kSlot408MenuStringLengths[4], kSlot408HolderVptrB, kSlot408HolderDtorRvaB, concmd)) return out;
    ++out.rows_done;   // row 05 holder
    if (!name_holder(objects.r06, kSlot408MenuStrings[5], kSlot408MenuStringLengths[5], kSlot408HolderVptrC, kSlot408HolderDtorRvaC, concmd)) return out;
    ++out.rows_done;   // row 06 holder
    if (!name_holder(objects.r07, kSlot408MenuStrings[6], kSlot408MenuStringLengths[6], kSlot408HolderVptrD, kSlot408HolderDtorRvaD, concmd)) return out;
    ++out.rows_done;   // row 07 holder
    if (!zero_container(objects.r08, kSlot408ZVecDtorRva[0], concmd)) return out;
    ++out.rows_done;   // row 08 zvec
    if (!sentinel_container(objects.r09, kSlot408SentinelDtorRva, concmd)) return out;
    ++out.rows_done;   // row 09 sent
    if (!cmd_var(objects.r10, 0, concmd)) return out;
    ++out.rows_done;   // row 10 cmd
    if (!cmd_var(objects.r11, 1, concmd)) return out;
    ++out.rows_done;   // row 11 cmd
    if (!cmd_var(objects.r12, 2, concmd)) return out;
    ++out.rows_done;   // row 12 cmd
    if (!cmd_var(objects.r13, 3, concmd)) return out;
    ++out.rows_done;   // row 13 cmd
    if (!cmd_var(objects.r14, 4, concmd)) return out;
    ++out.rows_done;   // row 14 cmd
    if (!cmd_var(objects.r15, 5, concmd)) return out;
    ++out.rows_done;   // row 15 cmd
    if (!cmd_var(objects.r16, 6, concmd)) return out;
    ++out.rows_done;   // row 16 cmd
    if (!cmd_var(objects.r17, 7, concmd)) return out;
    ++out.rows_done;   // row 17 cmd
    if (!cmd_var(objects.r18, 8, concmd)) return out;
    ++out.rows_done;   // row 18 cmd
    if (!cmd_var(objects.r19, 9, concmd)) return out;
    ++out.rows_done;   // row 19 cmd
    if (!cmd_var(objects.r20, 10, concmd)) return out;
    ++out.rows_done;   // row 20 cmd
    if (!cmd_var(objects.r21, 11, concmd)) return out;
    ++out.rows_done;   // row 21 cmd
    if (!cmd_var(objects.r22, 12, concmd)) return out;
    ++out.rows_done;   // row 22 cmd
    if (!cmd_var(objects.r23, 13, concmd)) return out;
    ++out.rows_done;   // row 23 cmd
    if (!zero_container(objects.r24, kSlot408ZVecDtorRva[1], concmd)) return out;
    ++out.rows_done;   // row 24 zvec
    out.ok = true;
    return out;
}


}  // namespace sotc
