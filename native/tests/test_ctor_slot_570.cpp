// CPU-01-AOT-CFG-SCALE-01-BATCH21 (slot 570): fixture for startup constructor index 570 (0x16d80).
// Asserts the three bespoke preamble tables byte for byte (including the bytes the body never
// writes), the bool variable `mem_Verify`, the `Memory` / `Verify Memory Integrity` toggle, the
// five __cxa_atexit records in body order, the xchg semantics and the fail-closed tag refusal.
#include "l4_test_util.hpp"

#include <cstring>

#include "core/ctor_slot_570_16d80.hpp"

namespace {
using namespace l4;

// l4::poison fills every byte with 0xA5; these are the same bytes read back as a dword / qword.
constexpr std::uint32_t kPoisonDword = 0xA5A5A5A5u;
constexpr std::uint64_t kPoisonQword = 0xA5A5A5A5A5A5A5A5ull;

void section_constants() {
    CHECK(kSlot570Rva == 0x16d80 && kSlot570Size == 740);
    CHECK(kSlot570TableWord == 0x2dd70e0 - 8 * 570);
    CHECK(sizeof(Slot570ZeroCell) == 0x18 && sizeof(Slot570Sentinel) == 0x38);
    CHECK(sizeof(Slot570ZeroBlock2) == 0x18 && sizeof(Slot570Triple) == 0x18);
    CHECK(kSlot570SentinelSelf - kSlot570SentinelBlock == 0x38);
    CHECK(kSlot570TripleBackPtr == kSlot570BoolRva);
    CHECK(kSlot570ZeroCellStride == sizeof(Slot570ZeroCell));
    CHECK(kSlot570ZeroCell0 + (kSlot570ZeroCellCount - 1) * kSlot570ZeroCellStride == 0x2dd9fd8);
    CHECK(kSlot570BoolClassVptr == kBoolVarFinalVptr);
    CHECK(kSlot570AtexitCount == 5);
    CHECK(std::strlen(kSlot570BoolName) == kSlot570BoolNameLength);
    CHECK(std::strlen(kSlot570ToggleLabel) == kSlot570ToggleLabelLength);
    CHECK(std::strlen(kSlot570ToggleText) == kSlot570ToggleTextLength);
    CHECK(kSlot570BoolEmptyDescriptionLength == 0 && kSlot570BoolEmptyDescription[0] == 0);
}

void check_preamble(const Slot570Objects& o) {
    for (std::uint32_t f : o.flags) CHECK(f == kSlot570FlagValue);
    for (const Slot570ZeroCell& cell : o.cells) {
        CHECK(cell.zero == 0);
        CHECK(cell.not_written == kPoisonDword);     // +0x04 is never written by the body
        for (std::uint8_t b : cell.tail) CHECK(b == 0);
    }
    CHECK(o.xchg_cell == 0);

    const Slot570Sentinel& s = o.sentinel;
    CHECK(s.qword0 == 0);
    CHECK(s.gap08 == kPoisonQword);                  // +0x08 is never written by the body
    for (std::uint8_t b : s.zeros10) CHECK(b == 0);
    CHECK(s.self == kSlot570SentinelSelf);
    CHECK(s.ones == kSlot570SentinelOnes);
    CHECK(s.dword30 == 0);
    CHECK(s.capacity == kSlot570SentinelCapacity);

    CHECK(o.zero_block2.qword10 == 0);
    for (std::uint8_t b : o.zero_block2.zeros) CHECK(b == 0);

    CHECK(o.triple.a == kSlot570TripleSelf0 && o.triple.b == kSlot570TripleSelf1);
    CHECK(o.triple.back == static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&o.var)));
}

void check_variable_and_item(const Slot570Objects& o, const ConCmdRegistry& r, const DebugMenuRegistry& menu) {
    const ConCmdBoolVar& v = o.var;
    check_bool_with_description(v, kSlot570BoolFnTag, kSlot570BoolValue, "", 0);
    CHECK(same_view(v.base.name, kSlot570BoolName, kSlot570BoolNameLength));
    CHECK(v.value == kSlot570BoolValue && v.default_value == kSlot570BoolDefault);
    CHECK(v.base.vptr == kSlot570BoolClassVptr);

    const DebugMenuItem& it = o.toggle;
    CHECK(it.registered == 0);                        // the shared 0x268b0 shape leaves the flag clear
    CHECK(it.kind == 2);                              // 0x26ea0 toggle
    CHECK(it.flags == kSlot570ToggleFlags);
    CHECK(it.target != nullptr);
    CHECK(same_view(it.label, kSlot570ToggleLabel, kSlot570ToggleLabelLength));
    CHECK(same_view(it.text, kSlot570ToggleText, kSlot570ToggleTextLength));
    CHECK(menu.root.count >= 1);

    // Five records come from this body; the menu registry's own first-use root record (0x27280) is
    // recorded by the shared 0x268b0 shape between the bool and the item, not by this slot.
    CHECK(r.atexit_log.size() == kSlot570AtexitCount + 1);
    check_atexit(r, 0, kSlot570IdentityDtor0, &o.xchg_cell);
    check_atexit(r, 1, kSlot570IdentityDtor1, &o.zero_block2);
    check_atexit(r, 2, kSlot570IdentityDtor2, &o.triple);
    check_atexit(r, 3, kSlot570BoolDtor, &o.var);
    bool menu_record = false, root_record = false;
    for (std::size_t i = 0; i < r.atexit_log.size(); ++i) {
        if (r.atexit_log.at(i).dtor_rva == kSlot570MenuDtor && r.atexit_log.at(i).object == &o.toggle) menu_record = true;
        if (r.atexit_log.at(i).dtor_rva == 0x27280) root_record = true;   // the menu root destructor
    }
    CHECK(menu_record && root_record);
}

void run_full() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot570Objects o;
    poison(o);
    const Slot570Result res = init_slot570_16d80(o, creg, menu);
    CHECK(res.ok && res.preamble_ok && res.bool_var_ok && res.toggle_ok);
    CHECK(res.xchg_previous == kPoisonQword);
    CHECK(res.atexit_records == kSlot570AtexitCount);
    check_preamble(o);
    check_variable_and_item(o, creg, menu);
}

void run_second_time_reports_zero_previous() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot570Objects o;
    poison(o);
    CHECK(init_slot570_16d80(o, creg, menu).ok);
    // The body ran once; the cell is 0 now, so a second run's xchg loads 0.
    const Slot570Result second = init_slot570_16d80(o, creg, menu);
    CHECK(second.ok && second.xchg_previous == 0);
}

void run_refuses_foreign_tag() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot570Objects o;
    poison(o);
    const Slot570Result res = init_slot570_16d80(o, creg, menu, 0x15949f8);   // one slot past the family
    CHECK(!res.ok && !res.preamble_ok && !res.bool_var_ok && !res.toggle_ok);
    CHECK(creg.atexit_log.size() == 0);
    for (std::uint32_t f : o.flags) CHECK(f == kPoisonDword);   // nothing was written
}

}  // namespace

int main() {
    section_constants();
    run_full();
    run_second_time_reports_zero_previous();
    run_refuses_foreign_tag();
    std::printf("OK test_ctor_slot_570\n");
    return 0;
}
