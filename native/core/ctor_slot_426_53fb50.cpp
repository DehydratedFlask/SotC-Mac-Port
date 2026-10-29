#include "ctor_slot_426_53fb50.hpp"

namespace sotc {
namespace {

// vmovups xmm0(0) -> [cell]; __cxa_atexit(0x539a00, cell, &dso): recorded, never run.
bool init_slot426_pair16(Slot426Pair16& cell, ConCmdRegistry& registry) noexcept {
    cell.ptr = 0;
    cell.ctl = 0;
    return registry.atexit_log.record(kSlot426PairDtorRva, &cell);
}

}  // namespace

Slot426Result init_slot426_53fb50(Slot426Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot426Result out{};
    out.var = init_bool_var_site(objects.var, kSlot426Name, kSlot426FnTag, concmd, 0, 0);
    if (!out.var.ok) return out;
    out.cell_atexit = init_slot426_pair16(objects.cell, concmd);
    out.ok = out.var.ok && out.cell_atexit;
    return out;
}

}  // namespace sotc
