#include "ctor_slot_428_52ec10.hpp"

#include <cstdlib>
#include <cstring>

namespace sotc {

namespace {

void* host_allocate(std::size_t size, std::uint32_t, std::uint32_t, std::uint32_t) noexcept { return std::malloc(size); }
bool host_deallocate(void* ptr, std::uint32_t) noexcept {
    std::free(ptr);
    return true;
}

class HostNameDeallocator final : public SharedStrDeallocatorF590 {
public:
    void deallocate(const void* allocation, SharedStrDeallocationKindF590) noexcept override { std::free(const_cast<void*>(allocation)); }
};

// A borrowed view {data, size, 0, null}: what the original builds on the stack for the callee and releases through the (inert) 0xf590.
SharedStr borrowed_view(const char* text, std::uint64_t length) noexcept {
    SharedStr view{};
    view.data = reinterpret_cast<const std::uint8_t*>(text);
    view.size = static_cast<std::int64_t>(length);
    return view;
}

bool contract_present(const Slot428Objects& o) noexcept {
    return o.alloc.allocate != nullptr && o.alloc.deallocate != nullptr && o.name_dealloc != nullptr;
}

}  // namespace

AllocDispatchContract default_slot428_alloc() noexcept {
    AllocDispatchContract c;
    c.allocate = &host_allocate;
    c.deallocate = &host_deallocate;
    return c;
}

SharedStrDeallocatorF590* default_slot428_name_dealloc() noexcept {
    static HostNameDeallocator instance;
    return &instance;
}

Slot428Result init_slot428_52ec10(Slot428Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot428Result out{};
    // The whole slot needs the allocator contract (phases C and D); without it nothing is written.
    if (!contract_present(objects)) return out;

    // A. the static object: the ELF-tied store run, then __cxa_atexit(0x52e980, 0).
    const StoreTableSpec spec{kSlot428Stores, kSlot428StoreCount, kSlot428StoreStringBlob, sizeof kSlot428StoreStringBlob, kSlot428StoreStringOffset, kSlot428StoreStringCount};
    if (!replay_store_table(objects.statics.bytes, sizeof objects.statics.bytes, spec)) return out;
    out.statics_applied = true;
    if (!concmd.atexit_log.record(kSlot428StoreDtorTag, nullptr)) return out;
    ++out.atexit_recorded;

    // B. five save descriptors: id, name view, four path-folding hashes, flag, zero tail; atexit(0x52eaa0, record).
    for (std::size_t i = 0; i < kSlot428SaveRecordCount; ++i) {
        const Slot428SaveSpec& s = kSlot428SaveSpecs[i];
        Slot428SaveRecord& r = objects.saves[i];
        r.id = s.id;
        r.name = borrowed_view(s.name, s.name_length);
        for (std::size_t k = 0; k < 4; ++k) r.hash[k] = fnv_cstr_a2ddc0(s.hash_text[k]);
        r.flag = s.flag;
        std::memset(r.zero40, 0, sizeof r.zero40);
        r.zero50 = 0;
        if (!concmd.atexit_log.record(kSlot428SaveDtorTag, &r)) return out;
        ++out.saves_done;
        ++out.atexit_recorded;
    }

    // C. 37 id entries: 0xa59620 (V1 doubling push), 0xf590 on the stack view, atexit(0x52eb00, entry).
    for (std::size_t i = 0; i < kSlot428IdEntryCount; ++i) {
        const Slot428IdSpec& s = kSlot428IdSpecs[i];
        const SharedStr view = borrowed_view(s.text, s.text_length);
        const IdEntryRegistration reg = register_id_entry_a59620(objects.id_vec, objects.ids[i], s.id, view, objects.alloc);
        if (!reg.ok) return out;
        if (!release_inert_view_f590(view)) return out;
        if (!concmd.atexit_log.record(kSlot428IdDtorTag, &objects.ids[i])) return out;
        ++out.ids_done;
        ++out.atexit_recorded;
    }

    // D. six availability entries: 0xa42960 (formatted-name bool variable, V2 doubling push), 0xf590 on b then a, atexit(0x52eb10, entry).
    AvailEntryHooks hooks;
    hooks.alloc = objects.alloc;
    hooks.name_alloc.alloc = objects.alloc;
    hooks.name_dealloc = objects.name_dealloc;
    for (std::size_t i = 0; i < kSlot428AvailEntryCount; ++i) {
        const Slot428AvailSpec& s = kSlot428AvailSpecs[i];
        const SharedStr a = borrowed_view(s.a, s.a_length);
        const SharedStr b = borrowed_view(s.b, s.b_length);
        const AvailEntryResult reg = register_avail_entry_a42960(objects.avail_vec, objects.avails[i], a, b, concmd, hooks);
        if (!reg.ok) return out;
        if (!release_inert_view_f590(b) || !release_inert_view_f590(a)) return out;
        if (!concmd.atexit_log.record(kSlot428AvailDtorTag, &objects.avails[i])) return out;
        ++out.avails_done;
        ++out.atexit_recorded;
    }

    out.ok = out.atexit_recorded == kSlot428AtexitCount;
    return out;
}

}  // namespace sotc
