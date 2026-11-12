#include "ctor_slot_339_7e2fc0.hpp"

namespace sotc {

namespace {

// 0x7e2fdf..0x7e3096: for each record `[cell] = name; cell->hash = 0x52a990(name)`; the four bytes at +0x0c are never written.
void build_cells(NameHashCell* cells) noexcept {
    for (std::size_t n = 0; n < kSlot339CellCount; ++n) {
        cells[n].name = kSlot339CellNames[n];
        cells[n].hash = fnv_cstr_52a990(kSlot339CellNames[n]);
    }
}

// 0x7e30b0..0x7e3193: the 0x30 byte holder is built from a stack view (view copy at +8, zero tail at +0x28, vptr = 0x15976a0 + 0x10), the
// temporary view is released by an exact no-op f590 and __cxa_atexit(0x8ef00, holder) is recorded.
bool build_holder(NameHolder30& holder, const char* name, std::size_t length, ConCmdRegistry& registry) noexcept {
    const SharedStr view = l2::measured_view(name, length);
    holder.name = view;
    holder.tail = 0;
    holder.vptr = kSlot339HolderVptr;
    const bool inert = release_inert_view_f590(view);
    const bool recorded = registry.atexit_log.record(kSlot339HolderDtorRva, &holder);
    return inert && recorded;
}

}  // namespace

Slot339Result init_slot339_7e2fc0(Slot339Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot339Result out{};
    build_cells(objects.cells);
    out.cells_ok = true;
    const bool h0 = build_holder(objects.holder0, kSlot339Holder0Name, kSlot339Holder0NameLength, concmd);
    const bool h1 = build_holder(objects.holder1, kSlot339Holder1Name, kSlot339Holder1NameLength, concmd);
    out.holders_ok = h0 && h1;
    const HelperFloatSiteSpec spec{kSlot339FloatName, kSlot339FloatNameLength, "", 0, 1, kSlot339FloatFnTag, kSlot339FloatValueBits, true};
    out.fvar = helper_init_unbounded_float_var_site(objects.fvar, spec, concmd);
    out.refusal = out.fvar.refusal;
    if (!out.fvar.ctor_ok) return out;
    const FConCmdCommandSpec cmd{kSlot339CommandName, kSlot339CommandDescription, kSlot339CommandDescriptionLength, kSlot339HandlerTag};
    out.command = init_fconcmd_object(objects.command, cmd, concmd);
    out.ok = out.cells_ok && out.holders_ok && out.fvar.ok && out.command.ok;
    return out;
}

}  // namespace sotc
