// CPU-01-CTOR-COVERAGE-BATCH-01: production-path coverage of the recovered constructor table.
//
// Proves that every lifted vec_*/leaf_* constructor module runs in recovered_constructors(), that
// the registered rows pair with the pinned original table, and that the remaining startup cliff is
// explicit (first unrecovered index 309 / RVA 0xa505e0).
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/ctor_original_table.hpp"
#include "core/startup_ctors.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main() {
    using namespace sotc;

    // ---- original table shape -------------------------------------------------
    CHECK(kOriginalCtorCount == 573);
    CHECK(original_constructors().size() == kOriginalCtorCount);
    for (std::size_t i = 0; i < original_constructors().size(); ++i)
        CHECK(original_constructors()[i].index == i);

    // ---- production run -------------------------------------------------------
    StartupState st;
    st.slot315_cell = 0xffffffffu;
    for (auto& cell : st.slot323_cells) cell = {nullptr, 0xffffffffu, 0xa5a5a5a5u};
    for (auto& cell : st.slot341_cells) cell = {nullptr, 0xffffffffu, 0xa5a5a5a5u};
    st.slot357_cell = {0xffffffffffffffffull, 0xffffffffffffffffull};
    for (auto& cell : st.slot378_cells) cell = {nullptr, 0xffffffffu, 0xa5a5a5a5u};
    for (auto& cell : st.slot394_cells) cell = {nullptr, 0xffffffffu, 0xa5a5a5a5u};
    st.slot412_xmm.fill(0xffffffffu);
    st.slot412_q = 0xffffffffffffffffull;
    st.slot416_xmm.fill(0xffffffffu);
    st.slot496_cell = 0;
    st.slot500_cell.fill(0xff);
    st.slot560_cell.store(0xffffffffffffffffull, std::memory_order_relaxed);
    const auto slots = recovered_constructors();
    const CtorRunResult result = run_constructors(slots, st);
    CHECK(result.ok());
    CHECK(!result.callback_failed);
    CHECK(result.blocked_on_rva == 0);
    CHECK(!st.init_failed);
    CHECK(result.completed == slots.size());
    CHECK(slots.size() == 397);

    // Every registered row is in original order and pairs with the pinned table.
    for (std::size_t i = 0; i < slots.size(); ++i) {
        CHECK(slots[i].run != nullptr);
        CHECK(slots[i].original_index < kOriginalCtorCount);
        CHECK(slots[i].original_rva == original_constructors()[slots[i].original_index].rva);
        if (i != 0) CHECK(slots[i - 1].original_index < slots[i].original_index);
    }

    // ---- explicit, machine-checked gap ---------------------------------------
    CHECK(first_unrecovered_constructor_index() == 309);
    CHECK(first_unrecovered_constructor_rva() == 0xa505e0);
    CHECK(slots.size() + 176 == kOriginalCtorCount);

    // ---- CPU-01-STARTUP-LEAF-ROWS-315-560-01 -------------------------------
    CHECK(st.slot315_cell == 0);
    const auto check_name_cells = [](const auto& cells, const char* const* names,
                                     const std::uint32_t* hashes, std::size_t count) {
        for (std::size_t i = 0; i < count; ++i) {
            CHECK(cells[i].name != nullptr);
            CHECK(std::strcmp(cells[i].name, names[i]) == 0);
            CHECK(cells[i].hash == hashes[i]);
            CHECK(cells[i].unwritten == 0xa5a5a5a5u);
        }
    };
    const char* const names323[] = {"BowShotsHit", "BowShotAccuracy", "FurthestArrowDistance"};
    const std::uint32_t hashes323[] = {0x9744358du, 0x019b017au, 0x49104024u};
    check_name_cells(st.slot323_cells, names323, hashes323, 3);
    const char* const names341[] = {"InEndCutscene", "TimeResistingTheLight"};
    const std::uint32_t hashes341[] = {0xc30b4677u, 0x472539c3u};
    check_name_cells(st.slot341_cells, names341, hashes341, 2);
    CHECK(st.slot357_cell.lo == 0 && st.slot357_cell.hi == 0);
    const char* const names378[] = {"InSaveVolume", "LastInSaveVolume", "FacingSaveVolume"};
    const std::uint32_t hashes378[] = {0xbeb62505u, 0x559f2211u, 0xff2b538cu};
    check_name_cells(st.slot378_cells, names378, hashes378, 3);
    const char* const names394[] = {"IsCutsceneRunning"};
    const std::uint32_t hashes394[] = {0xbedd0298u};
    check_name_cells(st.slot394_cells, names394, hashes394, 1);
    CHECK(st.slot412_xmm == Xmm16{} && st.slot412_q == 0);
    CHECK(st.slot416_xmm == Xmm16{});
    CHECK(st.slot496_cell == 0xff);
    for (std::uint8_t byte : st.slot500_cell) CHECK(byte == 0);
    CHECK(st.slot560_cell.load(std::memory_order_seq_cst) == 0);

    // ---- spot checks: destination cells hold their module constants ----------
    CHECK(st.vec_d71f60 == kVecD71f60Bits);     // idx 95, the batch's new lift
    CHECK(st.vec_ce6130 == kVecCe6130Bits);     // idx 152
    CHECK(st.vec_cdc760 == kVecCdc760Bits);     // idx 157
    CHECK(st.vec_c891e0 == kVecC891e0Bits);     // idx 178
    CHECK(st.vec_c5c320 == kVecC5c320Bits);     // idx 200
    CHECK(st.vec_c09070 == kVecC09070Bits);     // idx 235
    CHECK(st.vec_b76950 == kVecB76950Bits);     // idx 280
    CHECK(st.vec_b33bd0 == kVecB33BD0Bits);     // idx 295
    // ---- CPU-01-STARTUP-SLOTS-296-297-300-01 ---------------------------------
    CHECK(st.slot296_stamp.tsc != 0);            // idx 296: the 8-byte TSC stamp
    CHECK(st.slot297_table[0].node == &static_object_2ee3b10());    // idx 297
    CHECK(st.slot297_table[399].node == &static_object_2ee3b10());
    CHECK(st.slot297_table[0].flag00 == 0 && st.slot297_table[0].flag110 == 0);
    CHECK(st.slot300_cleanup.registered == 1);   // idx 300: one inert cleanup
    // ---- CPU-01-CONCMD-298-299-301-01 ----------------------------------------
    CHECK(st.slot298_shader_type.base.registered == 1);          // idx 298: shaderTypeToLoad
    CHECK(st.slot298_shader_type.fn_vptr == 0x15d5960);
    CHECK(st.slot299.scale.value == 0x3f800000u);                // idx 299: globalDebugTextScale 1.0f
    CHECK(st.slot299.adjust.value == 1);                         //          ...AdjustFor4K
    CHECK(st.slot299.scale_item.target == &st.slot299.scale);
    CHECK(st.slot301_rumble.value == 1 && st.slot301_rumble.base.registered == 1);   // idx 301
    // 298, 299 (A, B) and 301 queue into the pending ConCmd list in index order; the manager is
    // installed by the engine boot after static init, so nothing here ever sees one.
    CHECK(st.concmd.manager.manager == nullptr);
    CHECK(st.slot303.var.base.registered == 1 && st.slot303.var.value == 0);   // idx 303: ProfileCPU
    CHECK(st.slot303.var.default_value == 0 && st.slot303.var.fn_vptr == 0x15d2478);
    CHECK(st.slot303.item.target == &st.slot303.var && st.slot303.item.flags == 0x30);
    CHECK(st.concmd.manager.manager == nullptr);
    CHECK(st.concmd.pending.size() == 62);                       // observed after the 2026-10-26 lanes: 27 through slot 306 + 35 from 307-356
    CHECK(st.concmd.pending.first() == &st.slot298_shader_type.base.pending);
    CHECK(st.debug_menu.root.count == 50);                       // observed: 26 through slot 306 + 24 from the later lanes
    CHECK(st.concmd.atexit_log.size() == 188);                   // observed: 65 through slot 306 + 123 from the later lanes
    for (std::size_t i = 0; i < kSlot302HandleCount; ++i) {
        CHECK(st.slot302.handles[i] == nullptr);
        CHECK(st.concmd.atexit_log.at(7 + i).dtor_rva == kSlot302DtorRva);
        CHECK(st.concmd.atexit_log.at(7 + i).object == &st.slot302.handles[i]);
    }
    CHECK(st.slot302.identity_vptr == kSlot302IdentityVptr);
    for (const Pair128& pair : st.slot305) CHECK(pair.lo == 0 && pair.hi == 0);
    // ---- CPU-01-STARTUP-SLOT-306-ASSEMBLE-01: the 46 `sndx_` objects ----------
    CHECK(st.slot306_result.ok && st.slot306_result.built_count == 46 && st.slot306_result.gap_count == 0);
    CHECK(kSlot306Gaps.size() == 0);
    CHECK(st.slot306.int_vars[0].base.registered == 1 && st.slot306.int_vars[0].default_value == 0);
    CHECK(st.slot306.floats[0].base.vptr == 0x15942b0);
    CHECK(st.slot306.bools[0].base.vptr == kBoolVarFinalVptr && st.slot306.bools[0].value == 0);
    CHECK(st.slot306.commands[0].base.vptr == 0x1594dd0 && st.slot306.commands[0].base.flags == 2);
    CHECK(st.slot306.items[0].kind == 1 && st.slot306.items[0].flags == 0x30);
    CHECK(st.slot306.items[45 - 23].target == &st.slot306.command_target_cell);
    CHECK(st.concmd.atexit_log.at(19).object == slot306_object_address(st.slot306, 0));
    CHECK(st.concmd.atexit_log.at(19 + 45).object == slot306_object_address(st.slot306, 45));
    CHECK(st.leaf_7ce8d0.word0 == 0x3f80000000000001ull);  // idx 346
    CHECK(st.leaf_7ce8d0.word1 == 0x3f80000000000001ull);
    CHECK(st.leaf_7ce8d0.flag0.load() == 0 && st.leaf_7ce8d0.flag1.load() == 0);

    std::puts("startup_ctor_coverage: all checks passed");
    return 0;
}
