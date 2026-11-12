#include "ctor_slot_513_1d0770.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot513ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}


[[maybe_unused]] bool uint_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const HelperIntSiteSpec spec{kSlot513UIntNames[n], kSlot513UIntNameLengths[n], "", 0, 0, 0, kSlot513VarFlags, kSlot513UIntFnTag, kSlot513UIntValue[n], kSlot513UIntValue[n]};
    return helper_init_unbounded_int_var_site(var, spec, concmd).ok;
}





[[maybe_unused]] bool int_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const HelperIntSiteSpec spec{kSlot513IntNames[n], kSlot513IntNameLengths[n], "", 0, kSlot513IntMin[n], kSlot513IntMax[n], kSlot513VarFlags, kSlot513IntFnTag, kSlot513IntValue[n], kSlot513IntValue[n]};
    return helper_init_ranged_int_var_site(var, spec, concmd).ok;
}

[[maybe_unused]] bool lint_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const SharedStr view = kSlot513LintDesc[n] == nullptr ? empty_string_f730() : family_view(kSlot513LintDesc[n], kSlot513LintDescLength[n]);
    return helper_init_local_int_var_site(var, kSlot513LocalIntSites[n], view, concmd).ok;
}

[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    return lane_m6::init_bool_var_literal(var, kSlot513BoolNames[n], kSlot513BoolNameLengths[n], "", 0, kSlot513BoolFnTag, concmd, kSlot513BoolValues[n],
                                          kSlot513BoolValues[n]).ok;
}

// Named records (0xa81700 x N, each followed by f590 on the name view), then __cxa_atexit(0x1c8990, records). On first use the registrar initialises the
// registry and records the identity-tag atexit 0xa81b80. A missing registry or allocator is a visible refusal (the original would fault).
bool named_records(Slot513Records& recs, RecordRegistryCell* cell, const AllocDispatchContract& alloc, ConCmdRegistry& concmd) noexcept {
    if (cell == nullptr) return false;
    for (std::size_t n = 0; n < kSlot513RecordCount; ++n) {
        const SharedStr view = family_view(kSlot513RecName[n], kSlot513RecNameLength[n]);
        const NamedRecordRegistration reg = register_named_record_a81700(*cell, recs.rec[n], view, kSlot513RecKind[n], kSlot513RecFlag[n], alloc);
        if (!reg.ok) return false;
        if (reg.registry_initialised_now && !concmd.atexit_log.record(kRecordRegistryDtorRva, &cell->registry)) return false;
        if (!release_inert_view_f590(view)) return false;
    }
    return concmd.atexit_log.record(kSlot513RecsDtorRva, &recs);
}

// Zero-filled container: [+0x08, +0x18) and [+0x18, +0x20) are zeroed, nothing else is written; __cxa_atexit(0x1c5090, container) is recorded (identity tag).
bool zero_container(Slot513Zero& c, std::uint64_t dtor, ConCmdRegistry& concmd) noexcept {
    c.q08 = 0;
    c.q10 = 0;
    c.q18 = 0;
    return concmd.atexit_log.record(dtor, &c);
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot513Result init_slot513_1d0770(Slot513Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot513Result out{};

    if (!named_records(objects.r00, objects.record_registry, objects.record_alloc, concmd)) return out;
    ++out.rows_done;   // row 00 recs
    if (!uint_var(objects.r01, 0, concmd)) return out;
    ++out.rows_done;   // row 01 uint
    if (!lint_var(objects.r02, 0, concmd)) return out;
    ++out.rows_done;   // row 02 lint
    if (!int_var(objects.r03, 0, concmd)) return out;
    ++out.rows_done;   // row 03 int
    if (!bool_var(objects.r04, 0, concmd)) return out;
    ++out.rows_done;   // row 04 bool
    if (!bool_var(objects.r05, 1, concmd)) return out;
    ++out.rows_done;   // row 05 bool
    if (!bool_var(objects.r06, 2, concmd)) return out;
    ++out.rows_done;   // row 06 bool
    if (!bool_var(objects.r07, 3, concmd)) return out;
    ++out.rows_done;   // row 07 bool
    if (!family_toggle_item_26ea0(objects.r08, kSlot513MenuStrings[0], kSlot513MenuStringLengths[0], kSlot513MenuStrings[1], kSlot513MenuStringLengths[1], local_int_var_menu_target(objects.r02), kSlot513ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 08 toge
    if (!family_toggle_item_26ea0(objects.r09, kSlot513MenuStrings[0], kSlot513MenuStringLengths[0], kSlot513MenuStrings[2], kSlot513MenuStringLengths[2], ranged_int_c580_menu_target(objects.r03), kSlot513ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 09 toge
    if (!family_toggle_item_26ea0(objects.r10, kSlot513MenuStrings[0], kSlot513MenuStringLengths[0], kSlot513MenuStrings[3], kSlot513MenuStringLengths[3], target_of(objects.r04), kSlot513ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 10 toge
    if (!family_toggle_item_26ea0(objects.r11, kSlot513MenuStrings[0], kSlot513MenuStringLengths[0], kSlot513MenuStrings[4], kSlot513MenuStringLengths[4], target_of(objects.r05), kSlot513ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 11 toge
    if (!family_toggle_item_26ea0(objects.r12, kSlot513MenuStrings[0], kSlot513MenuStringLengths[0], kSlot513MenuStrings[5], kSlot513MenuStringLengths[5], unbounded_int_menu_target(objects.r01), kSlot513ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 12 toge
    if (!family_toggle_item_26ea0(objects.r13, kSlot513MenuStrings[0], kSlot513MenuStringLengths[0], kSlot513MenuStrings[6], kSlot513MenuStringLengths[6], target_of(objects.r06), kSlot513ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 13 toge
    if (!zero_container(objects.r14, kSlot513ZeroDtorRva, concmd)) return out;
    ++out.rows_done;   // row 14 zcont
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

AllocDispatchContract default_slot513_record_alloc() noexcept {
    AllocDispatchContract c;
    c.allocate = &record_host_allocate;
    c.deallocate = &record_host_deallocate;
    return c;
}


}  // namespace sotc
