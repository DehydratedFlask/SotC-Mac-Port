// CPU-01-STARTUP-SLOT-558: fixture for startup constructor index 558 (0x428a0): five zeroed qword cells (atexit
// 0x418c0), one inline small vector (atexit 0x41900) and one owning pointer cell (atexit 0x41910). Lane M6.
#include "l4_test_util.hpp"

#include "core/ctor_slot_558_428a0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot558Rva == 0x428a0 && kSlot558Size == 230 && kSlot558CellCount == 5);
    CHECK(kSlot558CellDtorRva == 0x418c0 && kSlot558VectorDtorRva == 0x41900 && kSlot558OwnerDtorRva == 0x41910);
    CHECK(sizeof(Slot558InlineVector) == 0x160 && kSlot558VectorStorageBytes == 0x140);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot558Objects o;
    poison(o);
    const Slot558Result r = init_slot558_428a0(o, creg);
    CHECK(r.ok && r.atexit_recorded == 7);
    for (std::uint64_t cell : o.cells) CHECK(cell == 0);
    const Slot558InlineVector& v = o.vector;
    CHECK(v.begin == v.storage && v.end == v.storage && v.storage_end == v.storage + 0x140);
    CHECK(v.storage_end == reinterpret_cast<const std::uint8_t*>(&v) + 0x160);
    check_untouched(v, {{0x00, 0x18}});   // gap18 and the whole inline storage are never written
    CHECK(o.owner == 0);
    CHECK(creg.pending.size() == 0 && creg.atexit_log.size() == 7);
    for (std::size_t i = 0; i < 5; ++i) check_atexit(creg, i, 0x418c0, &o.cells[i]);
    check_atexit(creg, 5, 0x41900, &o.vector);
    check_atexit(creg, 6, 0x41910, &o.owner);
}

void section_overwrites_previous_contents() {
    ConCmdRegistry creg;
    Slot558Objects o;
    for (auto& c : o.cells) c = 0x1122334455667788ull;
    o.owner = 0x8877665544332211ull;
    CHECK(init_slot558_428a0(o, creg).ok);
    for (std::uint64_t cell : o.cells) CHECK(cell == 0);   // stored, not merged
    CHECK(o.owner == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_overwrites_previous_contents();
    std::puts("OK test_ctor_slot_558");
    return 0;
}
