// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 5): fixture for startup constructor index 437 (0x5070c0): 3 BOOL console
// variable(s) and the 16-element borrowed-view array built by 0xf2f0 (four __cxa_atexit chunk records, NULL argument).
#include "l4_test_util.hpp"

#include "core/ctor_slot_437_5070c0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot437Rva == 0x5070c0 && kSlot437Size == 1142);
    CHECK(kSlot437BoolFnTag == 0x15c59a0);
    CHECK(std::strlen(kSlot437Var0Name) == kSlot437Var0NameLength && kSlot437Var0NameLength == 0x1a);
    CHECK(std::strlen(kSlot437Var1Name) == kSlot437Var1NameLength && kSlot437Var1NameLength == 0x18);
    CHECK(std::strlen(kSlot437Var2Name) == kSlot437Var2NameLength && kSlot437Var2NameLength == 0x18);
    CHECK(kSlot437ViewCount == 16 && kSlot437ChunkCount == 4 && kSlot437ChunkSize == 4);
    CHECK(kSlot437ViewArrayAddress == 0x2efc550);
    CHECK(kSlot437ChunkDtorRva[0] == 0x502920 && kSlot437ChunkDtorRva[1] == 0x502960 &&
          kSlot437ChunkDtorRva[2] == 0x5029a0 && kSlot437ChunkDtorRva[3] == 0x5029e0);
    CHECK(sizeof(ConCmdBoolVar) == 0xc8 && sizeof(SharedStr) == 0x20);
    CHECK(offsetof(Slot437Objects, views) == 3 * sizeof(ConCmdBoolVar));   // 0x2efc550 - 0x2efc2f8 == 0x258 == 3 * 0xc8
    CHECK(offsetof(Slot437Objects, views) == 0x258);
    CHECK(sizeof(Slot437Objects) == 0x258 + 16 * 0x20);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot437Objects o;
    poison(o);
    const Slot437Result r = init_slot437_5070c0(o, creg);
    CHECK(r.ok && r.views_ok && r.chunk_records == 4);
    CHECK(r.var0.ok && r.var0.views_inert && r.var1.ok && r.var1.views_inert && r.var2.ok && r.var2.views_inert);
    CHECK(r.var0.registration.ok && r.var0.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var1.registration.ok && r.var1.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var2.registration.ok && r.var2.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_with_description(o.var0, kSlot437BoolFnTag, 0, "", 0);
    CHECK(same_view(o.var0.base.name, kSlot437Var0Name, kSlot437Var0NameLength));
    check_bool_with_description(o.var1, kSlot437BoolFnTag, 0, "", 0);
    CHECK(same_view(o.var1.base.name, kSlot437Var1Name, kSlot437Var1NameLength));
    check_bool_with_description(o.var2, kSlot437BoolFnTag, 0, "", 0);
    CHECK(same_view(o.var2.base.name, kSlot437Var2Name, kSlot437Var2NameLength));
    CHECK(creg.pending.size() == 3);

    // Sixteen borrowed views {ptr, strlen, 0, 0} in element order; the text pointer is the literal itself.
    for (std::size_t i = 0; i < kSlot437ViewCount; ++i) {
        CHECK(o.views[i].data == reinterpret_cast<const std::uint8_t*>(kSlot437ViewText[i]));
        CHECK(o.views[i].size == static_cast<std::int64_t>(std::strlen(kSlot437ViewText[i])));
        CHECK(o.views[i].aux == 0 && o.views[i].ctl == nullptr);
    }
    CHECK(o.views[0].size == 0x17 && o.views[1].size == 0x18 && o.views[2].size == 0x16 && o.views[3].size == 0x17);
    CHECK(o.views[8].size == 0x14 && o.views[12].size == 0x12 && o.views[15].size == 0x12);

    // Records in body order: var0, var1, var2 (0x20b0, the variable), then four chunk records (dtor, NULL).
    CHECK(creg.atexit_log.size() == 7);
    check_atexit(creg, 0, 0x20b0, &o.var0);
    check_atexit(creg, 1, 0x20b0, &o.var1);
    check_atexit(creg, 2, 0x20b0, &o.var2);
    check_atexit(creg, 3, 0x502920, nullptr);
    check_atexit(creg, 4, 0x502960, nullptr);
    check_atexit(creg, 5, 0x5029a0, nullptr);
    check_atexit(creg, 6, 0x5029e0, nullptr);
}

void section_chunk_destructors() {
    ConCmdRegistry creg;
    Slot437Objects o;
    poison(o);
    CHECK(init_slot437_5070c0(o, creg).ok);
    // Borrowed views have no control block, so every chunk destructor's four 0xf590 releases are exact no-ops.
    for (std::size_t chunk = 0; chunk < kSlot437ChunkCount; ++chunk) CHECK(slot437_chunk_destructor(o, chunk));
    CHECK(!slot437_chunk_destructor(o, kSlot437ChunkCount));
    for (std::size_t i = 0; i < kSlot437ViewCount; ++i) {
        CHECK(o.views[i].data == reinterpret_cast<const std::uint8_t*>(kSlot437ViewText[i]) && o.views[i].ctl == nullptr);
    }
    // A cell that does carry a counter is NOT an inert release (the model does not hide it).
    std::int64_t counter = 1;
    o.views[5].ctl = &counter;
    CHECK(!slot437_chunk_destructor(o, 1));
}

void section_failures() {
    // A registration blocked by a non-null manager with no insert hook fails visibly for every variable, and the
    // view array is still built (the original has no failure path; the model reports instead of hiding it).
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;
    Slot437Objects o;
    const Slot437Result r = init_slot437_5070c0(o, creg);
    CHECK(!r.ok && !r.var0.ok && !r.var0.registration.ok && !r.var1.ok && !r.var1.registration.ok &&
          !r.var2.ok && !r.var2.registration.ok && creg.pending.size() == 0);
    CHECK(r.views_ok && r.chunk_records == 4);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_chunk_destructors();
    section_failures();
    std::puts("OK test_ctor_slot_437");
    return 0;
}
