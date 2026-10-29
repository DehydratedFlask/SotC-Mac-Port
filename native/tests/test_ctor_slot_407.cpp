// CPU-01-STARTUP-SLOT-407: fixture for startup constructor index 407 (0x5d41c0): the bool variable
// `cp10_creditsUseNewList` (value/default 1, tag 0x15cb258), a 12-byte zero vec3 and a 4x4 identity matrix. Lane M2.
#include "core/lane_m2_testkit.hpp"

#include "core/ctor_slot_407_5d41c0.hpp"

namespace {
using namespace sotc::lane_m2;

void section_constants() {
    CHECK(kSlot407Rva == 0x5d41c0 && kSlot407Size == 374 && kSlot407FnTag == 0x15cb258 && kSlot407One == 0x3f800000u);
    CHECK(std::strlen(kSlot407VarName) == 0x16);
    CHECK(sizeof(Slot407Objects::vec3) == 12 && sizeof(Slot407Objects::matrix) == 64);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot407Objects o;
    poison(o);
    const Slot407Result r = init_slot407_5d41c0(o, creg);
    CHECK(r.ok && r.var.ok && r.var.views_inert && r.var.atexit_recorded);
    check_bool_var(o.var, "cp10_creditsUseNewList", 0x15cb258, 1, 1);
    for (std::uint32_t w : o.vec3) CHECK(w == 0);
    for (std::size_t i = 0; i < 16; ++i) {
        const bool diagonal = (i % 5) == 0;   // 0, 5, 10, 15 -> offsets 0x00, 0x14, 0x28, 0x3c
        CHECK(o.matrix[i] == (diagonal ? 0x3f800000u : 0u));
    }
    check_var_atexit_and_pending(creg, o.var);
    CHECK(creg.atexit_log.size() == 1);   // the vector and matrix register nothing
}

void section_failures() {
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;
    Slot407Objects o;
    std::memset(static_cast<void*>(&o.vec3), kPoison, sizeof o.vec3);
    std::memset(static_cast<void*>(&o.matrix), kPoison, sizeof o.matrix);
    const Slot407Result r = init_slot407_5d41c0(o, creg);
    CHECK(!r.ok && !r.var.ok);
    for (std::uint32_t w : o.vec3) CHECK(w == 0xA5A5A5A5u);   // nothing after the failing stage ran
    for (std::uint32_t w : o.matrix) CHECK(w == 0xA5A5A5A5u);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_407");
    return 0;
}
