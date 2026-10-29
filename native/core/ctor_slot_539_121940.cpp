#include "ctor_slot_539_121940.hpp"

namespace sotc {

namespace {

// 0x1219ca..0x1219ef and 0x121b1e..0x121b52: build the 0x30-byte holder from a stack view, f590(view), atexit.
bool build_holder(NameHolder30& holder, const char* text, std::size_t length, ConCmdRegistry& registry) noexcept {
    const SharedStr view = l2::measured_view(text, length);            // stack view {literal, length, 0, 0}
    holder.name = view;                       // vmovdqu [+8] = {data, size}; vmovups [+0x18] = 0 (aux, ctl)
    holder.tail = 0;                          // qword [+0x28] = 0
    holder.vptr = kSlot539HolderVptr;         // [holder] = 0x15976a0 + 0x10
    const bool inert = release_inert_view_f590(view);   // call 0xf590 on the stack view: control == nullptr
    const bool recorded = registry.atexit_log.record(kSlot539HolderDtorRva, &holder);
    return inert && recorded;
}

}  // namespace

Slot539Result init_slot539_121940(Slot539Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot539Result out{};
    // 0x121977..0x12197e: qword [0x2edf140] = 0 ; __cxa_atexit(0x112d80, cell)
    objects.cell = 0;
    out.cell_atexit = concmd.atexit_log.record(kSlot539CellDtorRva, &objects.cell);
    // 0x121983..0x1219ef: holder 1
    out.holder1_ok = build_holder(objects.holder1, kSlot539Holder1Name, kSlot539Holder1NameLength, concmd);
    // 0x1219f4..0x121adf: r_showObjectCount (empty description, value 0 / default 0)
    out.var = lane_m6::init_bool_var_literal(objects.var, kSlot539VarName, kSlot539VarNameLength, "", 0,
                                             kSlot539BoolFnTag, concmd, 0, 0);
    // 0x121ae4..0x121b52: holder 2
    out.holder2_ok = build_holder(objects.holder2, kSlot539Holder2Name, kSlot539Holder2NameLength, concmd);
    out.ok = out.cell_atexit && out.holder1_ok && out.var.ok && out.holder2_ok;
    return out;
}

}  // namespace sotc
