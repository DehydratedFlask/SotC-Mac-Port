#include "ctor_slot_453_48ac10.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot453ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}













// Named records (0xa81700 x N, each followed by f590 on the name view), then __cxa_atexit(0x1c8990, records). On first use the registrar initialises the
// registry and records the identity-tag atexit 0xa81b80. A missing registry or allocator is a visible refusal (the original would fault).
bool named_records(Slot453Records& recs, RecordRegistryCell* cell, const AllocDispatchContract& alloc, ConCmdRegistry& concmd) noexcept {
    if (cell == nullptr) return false;
    for (std::size_t n = 0; n < kSlot453RecordCount; ++n) {
        const SharedStr view = family_view(kSlot453RecName[n], kSlot453RecNameLength[n]);
        const NamedRecordRegistration reg = register_named_record_a81700(*cell, recs.rec[n], view, kSlot453RecKind[n], kSlot453RecFlag[n], alloc);
        if (!reg.ok) return false;
        if (reg.registry_initialised_now && !concmd.atexit_log.record(kRecordRegistryDtorRva, &cell->registry)) return false;
        if (!release_inert_view_f590(view)) return false;
    }
    return concmd.atexit_log.record(kSlot453RecsDtorRva, &recs);
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot453Result init_slot453_48ac10(Slot453Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot453Result out{};

    if (!named_records(objects.r00, objects.record_registry, objects.record_alloc, concmd)) return out;
    ++out.rows_done;   // row 00 recs
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

AllocDispatchContract default_slot453_record_alloc() noexcept {
    AllocDispatchContract c;
    c.allocate = &record_host_allocate;
    c.deallocate = &record_host_deallocate;
    return c;
}


}  // namespace sotc
