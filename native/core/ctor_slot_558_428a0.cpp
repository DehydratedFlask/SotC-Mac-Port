#include "ctor_slot_558_428a0.hpp"

namespace sotc {

Slot558Result init_slot558_428a0(Slot558Objects& objects, ConCmdRegistry& registry) noexcept {
    Slot558Result out{};
    // 0x428c2..0x4292d: five times { qword [cell] = 0 ; __cxa_atexit(0x418c0, &cell, &dso) }
    for (std::uint64_t& cell : objects.cells) {
        cell = 0;
        if (registry.atexit_log.record(kSlot558CellDtorRva, &cell)) ++out.atexit_recorded;
    }
    // 0x42943..0x42960: F.begin = F.end = F + 0x20 ; F + 0x10 = F + 0x160 ; __cxa_atexit(0x41900, &F, &dso)
    Slot558InlineVector& v = objects.vector;
    v.begin = v.storage;
    v.end = v.storage;
    v.storage_end = v.storage + kSlot558VectorStorageBytes;
    if (registry.atexit_log.record(kSlot558VectorDtorRva, &v)) ++out.atexit_recorded;
    // 0x42976..0x42981: qword [G] = 0 ; tail jmp __cxa_atexit(0x41910, &G, &dso)
    objects.owner = 0;
    if (registry.atexit_log.record(kSlot558OwnerDtorRva, &objects.owner)) ++out.atexit_recorded;

    out.ok = out.atexit_recorded == kSlot558CellCount + 2;
    return out;
}

}  // namespace sotc
