// CPU-01-STARTUP-SLOT-559: fixture for startup constructor index 559 (0x3f2d0): three inline small vectors,
// __cxa_atexit(0x3d920, null) and the bool variable `loggingIncludeTimestamp` (tag 0x1595770, 0/0). Lane M6.
#include "l4_test_util.hpp"

#include "core/ctor_slot_559_3f2d0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot559Rva == 0x3f2d0 && kSlot559Size == 456 && kSlot559ObjectDtorRva == 0x3d920 && kSlot559BoolFnTag == 0x1595770);
    CHECK(std::strlen(kSlot559VarName) == 0x17 && kSlot559VarNameLength == 0x17);
    CHECK(sizeof(Slot559Vector) == 0x1158 && kSlot559VectorStorageBytes == 0x1130);
}

void section_alone() {
    ConCmdRegistry creg;
    auto* o = new Slot559Objects;
    poison(*o);
    const Slot559Result r = init_slot559_3f2d0(*o, creg);
    CHECK(r.ok && r.object_atexit && r.var.ok && r.var.views_inert && r.var.atexit_recorded);
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);
    for (const Slot559Vector& v : o->vectors) {
        CHECK(v.begin == v.storage && v.end == v.storage && v.inline_begin == v.storage);
        CHECK(v.storage_end == reinterpret_cast<const std::uint8_t*>(&v) + 0x1158);
        check_untouched(v, {{0x00, 0x18}, {0x20, 8}});   // gap18 and the inline storage are never written
    }
    CHECK(o->vectors[1].storage_end == reinterpret_cast<const std::uint8_t*>(&o->vectors[2]));
    check_bool_common(o->var, 0x1595770, 0);
    CHECK(same_view(o->var.base.name, "loggingIncludeTimestamp", 0x17));
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o->var.base.pending);
    CHECK(creg.atexit_log.size() == 2);
    check_atexit(creg, 0, 0x3d920, nullptr);   // the object argument of the first registration is the null pointer
    check_atexit(creg, 1, 0x20b0, &o->var);
    delete o;
}

void section_failures() {
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;
    auto* o = new Slot559Objects;
    const Slot559Result r = init_slot559_3f2d0(*o, creg);
    CHECK(!r.ok && r.object_atexit && !r.var.ok && !r.var.registration.ok && creg.pending.size() == 0);
    delete o;
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_559");
    return 0;
}
