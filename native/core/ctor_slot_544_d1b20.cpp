#include "ctor_slot_544_d1b20.hpp"

#include <cstring>

namespace sotc {

namespace {

// Dword id store + qword name store (the inline form of the same two stores that 0xc1c70 performs).
void set_entry(Slot544Entry& entry, std::uint32_t id, const char* name) noexcept {
    entry.id = id;
    entry.name = name;
}

}  // namespace

Slot544Result init_slot544_d1b20(Slot544Objects& objects) noexcept {
    Slot544Result out{};
    // 0xd1b24..0xd1b32: memset(0x2ed3f10, 0, 0xae40)
    std::memset(objects.block.data(), 0, objects.block.size());

    Slot544Entry* e = objects.entries;
    // 0xd1b37..0xd1b99: SpotShadow0..3 are entries k0..k3 with ids 7..10
    set_entry(e[0], 7, kSlot544Name_SpotShadow0);
    set_entry(e[1], 8, kSlot544Name_SpotShadow1);
    set_entry(e[2], 9, kSlot544Name_SpotShadow2);
    set_entry(e[3], 10, kSlot544Name_SpotShadow3);
    // entries k4..k6 (0x2ededd0..0x2ededf0) are not written by this constructor
    // 0xd1b56..0xd1c18: ShadowCascade0..3 are k7..k10 (ids 0xc..0xf), ShadowCascade4(SuperClose) is k11 (id 0x10)
    set_entry(e[7], 0xc, kSlot544Name_ShadowCascade0);
    set_entry(e[8], 0xd, kSlot544Name_ShadowCascade1);
    set_entry(e[9], 0xe, kSlot544Name_ShadowCascade2);
    set_entry(e[10], 0xf, kSlot544Name_ShadowCascade3);
    set_entry(e[11], 0x10, kSlot544Name_ShadowCascade4);
    // 0xd1c1f: call 0xc1c70(&k12, 0, "EntireFrame")
    set_entry(e[12], 0, kSlot544Name_EntireFrame);
    // 0xd1c24: qword [0x2edee60] = 0
    objects.terminator = 0;

    out.ok = true;
    return out;
}

}  // namespace sotc
