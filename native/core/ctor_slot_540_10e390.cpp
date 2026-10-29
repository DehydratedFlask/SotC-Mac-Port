#include "ctor_slot_540_10e390.hpp"

#include <cstdlib>

#include "l2_slot_common.hpp"
#include "store_value_clear_flag_158340.hpp"

namespace sotc {
namespace {

int host_rand() noexcept { return std::rand(); }

void set_entry(Slot540Entry& entry, std::uint32_t id, const char* name) noexcept {
    entry.id = id;       // 0xc1c70: mov [rdi],esi
    entry.name = name;   //          mov [rdi+8],rdx   (+4 is never written)
}

}  // namespace

Slot540Result init_slot540_10e390(Slot540Objects& objects, StartupAtexitLog& log,
                                  Slot540RandSource rand_source) noexcept {
    Slot540Result out{};
    const char* const names[kSlot540EntryCount] = {kSlot540Name0, kSlot540Name1, kSlot540Name2, kSlot540Name3,
                                                   kSlot540Name4, kSlot540Name5, kSlot540Name6, kSlot540Name7,
                                                   kSlot540Name8, kSlot540Name9};
    for (std::size_t k = 0; k < kSlot540EntryCount; ++k) set_entry(objects.entries[k], kSlot540EntryId, names[k]);

    out.rand_value = static_cast<std::uint32_t>((rand_source != nullptr ? rand_source : &host_rand)());   // 0x10e49b
    store_value_clear_flag_158340(objects.rand_cell, out.rand_value);                                       // 0x10e4a9

    // 0x10e4cb..0x10e50e: name {data, size, aux, ctl} + tail, then the vptr last
    const SharedStr view = l2::literal_view(kSlot540ErrorsName);
    objects.name_object.name = view;
    objects.name_object.tail = 0;
    objects.name_object.vptr = kSlot540NameVptr;
    if (!release_inert_view_f590(view)) { out.blocked = Slot540Stage::view_release; return out; }         // 0x10e515
    if (!log.record(kSlot540NameDtorRva, &objects.name_object)) {                                          // 0x10e52e
        out.blocked = Slot540Stage::name_atexit;
        return out;
    }
    objects.sentinel.next = &objects.sentinel;                                                             // 0x10e549..0x10e556
    objects.sentinel.prev = &objects.sentinel;
    objects.sentinel.count = 0;
    if (!log.record(kSlot540SentinelDtorRva, &objects.sentinel)) {                                         // 0x10e55a
        out.blocked = Slot540Stage::sentinel_atexit;
        return out;
    }
    out.ok = true;
    return out;
}

}  // namespace sotc
