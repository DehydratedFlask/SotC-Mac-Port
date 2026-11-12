#include "ctor_slot_427_5388f0.hpp"

#include <cstdlib>
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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot427ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}


[[maybe_unused]] bool uint_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const HelperIntSiteSpec spec{kSlot427UIntNames[n], kSlot427UIntNameLengths[n], "", 0, 0, 0, kSlot427VarFlags, kSlot427UIntFnTag, kSlot427UIntValue[n], kSlot427UIntValue[n], kSlot427UIntEarly[n] != 0};
    return helper_init_unbounded_int_var_site(var, spec, concmd).ok;
}


// String variable (0x257a0 -> 0x258f0): borrowed name and initial views, then f590(init), f590(name), __cxa_atexit(0x23c90, var).
[[maybe_unused]] bool str_var(ConCmdStringVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const SharedStr name = family_view(kSlot427StrNames[n], kSlot427StrNameLengths[n]);
    const std::int32_t d = kSlot427StrInit[n];
    const SharedStr init = family_view(kSlot427MenuStrings[d], kSlot427MenuStringLengths[d]);
    if (!init_string_var_257a0(var, name, init, concmd).ok) return false;
    if (!release_inert_view_f590(init) || !release_inert_view_f590(name)) return false;
    return concmd.atexit_log.record(kStringVarDtorRva, &var);
}


[[maybe_unused]] bool float_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const FloatVarSpec spec{kSlot427FloatNames[n], "", true, kSlot427FloatLoBits[n], kSlot427FloatHiBits[n], kSlot427FloatValueBits[n], kSlot427FloatFnTag, kSlot427VarFlags};
    return init_float_var_static(var, spec, concmd).ok;
}





[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot427BoolDesc[n];   // d < 0: the empty description; otherwise the description is a borrowed view of the menu string literal
    return lane_m6::init_bool_var_literal(var, kSlot427BoolNames[n], kSlot427BoolNameLengths[n], d < 0 ? "" : kSlot427MenuStrings[d], d < 0 ? 0 : kSlot427MenuStringLengths[d],
                                          kSlot427BoolFnTag, concmd, kSlot427BoolValues[n], kSlot427BoolValues[n], kSlot427BoolEarly[n] != 0).ok;
}

[[maybe_unused]] bool boolx_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const BoolVar274730SiteResult r = helper_init_bool_var_274730_site(var, kSlot427BoolXSites[n], concmd);   // 0x274730 ; call [argfn + 0x18] ; f590(name) ; __cxa_atexit(0x20b0)
    return r.ok && r.refusal == HelperRefusal::none;
}

// `[cell] = name; cell->hash = 0x52a990(name)`; the four bytes at +0x0c are never written.
bool name_cell(NameHashCell& cell, const char* name) noexcept {
    cell.name = name;
    cell.hash = fnv_cstr_52a990(name);
    return true;
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

// Single named record (0xa81700 + f590 on the name view + __cxa_atexit(0xa81950, record)). On first use the registrar initialises the registry and records the identity-tag atexit 0xa81b80
// (BEFORE the record's own). A missing registry or allocator is a visible refusal (the original would fault).
bool named_record1(NamedRecord& rec, std::size_t n, RecordRegistryCell* cell, const AllocDispatchContract& alloc, ConCmdRegistry& concmd) noexcept {
    if (cell == nullptr) return false;
    const SharedStr view = family_view(kSlot427Rec1Name[n], kSlot427Rec1NameLength[n]);
    const NamedRecordRegistration reg = register_named_record_a81700(*cell, rec, view, kSlot427Rec1Kind[n], kSlot427Rec1Flag[n], alloc);
    if (!reg.ok) return false;
    if (reg.registry_initialised_now && !concmd.atexit_log.record(kRecordRegistryDtorRva, &cell->registry)) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kSlot427Rec1DtorRva, &rec);
}

// Zero cell: the three qwords [+0x00, +0x18) are zeroed, nothing else is written; __cxa_atexit(0x534580, cell) is recorded (identity tag; the destructor frees [+0] and keeps the cell, never run).
bool zero_cell(Slot427ZeroCell& c, ConCmdRegistry& concmd) noexcept {
    c.q00 = 0;
    c.q08 = 0;
    c.q10 = 0;
    return concmd.atexit_log.record(kSlot427ZeroCellDtorRva, &c);
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot427Result init_slot427_5388f0(Slot427Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot427Result out{};

    if (!name_cell(objects.r00, kSlot427MenuStrings[0])) return out;
    ++out.rows_done;   // row 00 cell
    if (!name_cell(objects.r01, kSlot427MenuStrings[1])) return out;
    ++out.rows_done;   // row 01 cell
    if (!name_holder(objects.r02, kSlot427MenuStrings[2], kSlot427MenuStringLengths[2], kSlot427HolderVptr, kSlot427HolderDtorRva, concmd)) return out;
    ++out.rows_done;   // row 02 holder
    if (!bool_var(objects.r03, 0, concmd)) return out;
    ++out.rows_done;   // row 03 bool
    if (!family_group_item_26c10(objects.r04, kSlot427MenuStrings[3], kSlot427MenuStringLengths[3], kSlot427MenuStrings[4], kSlot427MenuStringLengths[4], kSlot427ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 04 group
    if (!family_toggle_item_26ea0(objects.r05, kSlot427MenuStrings[4], kSlot427MenuStringLengths[4], kSlot427MenuStrings[5], kSlot427MenuStringLengths[5], target_of(objects.r03), kSlot427ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 05 toge
    if (!bool_var(objects.r06, 1, concmd)) return out;
    ++out.rows_done;   // row 06 bool
    if (!family_toggle_item_26ea0(objects.r07, kSlot427MenuStrings[6], kSlot427MenuStringLengths[6], kSlot427MenuStrings[7], kSlot427MenuStringLengths[7], target_of(objects.r06), kSlot427ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 07 toge
    if (!named_record1(objects.r08, 0, objects.record_registry, objects.record_alloc, concmd)) return out;
    ++out.rows_done;   // row 08 rec1
    if (!named_record1(objects.r09, 1, objects.record_registry, objects.record_alloc, concmd)) return out;
    ++out.rows_done;   // row 09 rec1
    if (!str_var(objects.r10, 0, concmd)) return out;
    ++out.rows_done;   // row 10 str
    if (!bool_var(objects.r11, 2, concmd)) return out;
    ++out.rows_done;   // row 11 bool
    if (!zero_cell(objects.r12, concmd)) return out;
    ++out.rows_done;   // row 12 zcell
    if (!boolx_var(objects.r13, 0, concmd)) return out;
    ++out.rows_done;   // row 13 boolx
    if (!uint_var(objects.r14, 0, concmd)) return out;
    ++out.rows_done;   // row 14 uint
    if (!boolx_var(objects.r15, 1, concmd)) return out;
    ++out.rows_done;   // row 15 boolx
    if (!bool_var(objects.r16, 3, concmd)) return out;
    ++out.rows_done;   // row 16 bool
    if (!float_var(objects.r17, 0, concmd)) return out;
    ++out.rows_done;   // row 17 float
    if (!bool_var(objects.r18, 4, concmd)) return out;
    ++out.rows_done;   // row 18 bool
    if (!bool_var(objects.r19, 5, concmd)) return out;
    ++out.rows_done;   // row 19 bool
    if (!family_named_item_26db0(objects.r20, kSlot427MenuStrings[9], kSlot427MenuStringLengths[9], objects.r17.base, target_of(objects.r17), kSlot427ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 20 tog
    if (!family_named_item_26db0(objects.r21, kSlot427MenuStrings[9], kSlot427MenuStringLengths[9], objects.r18.base, target_of(objects.r18), kSlot427ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 21 tog
    if (!family_named_item_26db0(objects.r22, kSlot427MenuStrings[9], kSlot427MenuStringLengths[9], objects.r19.base, target_of(objects.r19), kSlot427ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 22 tog
    if (!bool_var(objects.r23, 6, concmd)) return out;
    ++out.rows_done;   // row 23 bool
    if (!family_toggle_item_26ea0(objects.r24, kSlot427MenuStrings[10], kSlot427MenuStringLengths[10], kSlot427MenuStrings[11], kSlot427MenuStringLengths[11], target_of(objects.r23), kSlot427ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 24 toge
    out.ok = true;
    return out;
}


namespace {
void* record_host_allocate(std::size_t size, std::uint32_t, std::uint32_t, std::uint32_t) noexcept { return std::malloc(size); }
bool record_host_deallocate(void* ptr, std::uint32_t) noexcept {
    std::free(ptr);
    return true;
}
}  // namespace

AllocDispatchContract default_slot427_record_alloc() noexcept {
    AllocDispatchContract c;
    c.allocate = &record_host_allocate;
    c.deallocate = &record_host_deallocate;
    return c;
}


}  // namespace sotc
