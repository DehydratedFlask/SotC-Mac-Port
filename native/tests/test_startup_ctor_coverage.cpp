// CPU-01-CTOR-COVERAGE-BATCH-01: production-path coverage of the recovered constructor table.
//
// Proves that every lifted vec_*/leaf_* constructor module runs in recovered_constructors(), that
// the registered rows pair with the pinned original table, and that the remaining startup cliff is
// explicit (first unrecovered index 336 / RVA 0x7ec4c0, after slot 335 landed).
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
    CHECK(slots.size() == 573);   // slot 554 batch 1 registers the final row; the full 554 body remains PARTIAL

    // Every registered row is in original order and pairs with the pinned table.
    for (std::size_t i = 0; i < slots.size(); ++i) {
        CHECK(slots[i].run != nullptr);
        CHECK(slots[i].original_index < kOriginalCtorCount);
        CHECK(slots[i].original_rva == original_constructors()[slots[i].original_index].rva);
        if (i != 0) CHECK(slots[i - 1].original_index < slots[i].original_index);
    }

    // ---- explicit, machine-checked gap ---------------------------------------
    CHECK(first_unrecovered_constructor_index() == -1);
    CHECK(first_unrecovered_constructor_rva() == 0);
    CHECK(slots.size() == kOriginalCtorCount);   // all 573 rows registered; slot 554 is PARTIAL (batch 1)

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
    // ---- CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 2): 374 / 448 / 507 / 539 / 545 --------------------
    CHECK(st.slot374_result.ok && st.slot374_result.hashed_names_built == 1);
    CHECK(st.slot374.record.hash == kSlot374RecordHash && st.slot374.var.value == kSlot374VarValueBits);
    CHECK(st.slot374.item.target == &st.slot374.var && st.slot374.item.flags == 0x30);
    CHECK(st.slot448_result.ok && st.slot448.disable_world_zoom_out.value == 1 &&
          st.slot448.map_show_boss_cloud_idx.value == 0);
    CHECK(st.slot507_result.ok && st.slot507.lookahead.value == 1 && st.slot507.smoothing.value == 1);
    CHECK(st.slot539_result.ok && st.slot539.cell == 0 && st.slot539.var.value == 0 &&
          st.slot539.var.fn_vptr == kSlot539BoolFnTag);
    CHECK(st.slot545_result.ok && st.slot545_result.atexit_recorded);
    // ---- CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 3): 518 ---------------------------------------------
    CHECK(st.slot518_result.ok && st.slot518_result.tail_written);
    CHECK(st.slot518.var.value == kSlot518VarValueBits && st.slot518.var.default_value == kSlot518VarValueBits);
    CHECK(st.slot518.basis.spacing == kSlot518TailE0Bits && st.slot518.basis.pitch == kSlot518TailE4Bits &&
          st.slot518.basis.inverse == kSlot518TailE8Bits);
    // ---- CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 3): bool debug pages 362 / 410 / 423 / 449 / 477 ----------
    // Measured by this test: +16 console variables (410 3, 362 3, 477 4, 423 4, 449 2 -> pending 176 -> 192), +18
    // atexit records (the 16 variables + 449's two items; the menu root was already initialised) and +2 menu items.
    CHECK(st.slot410_result.ok && st.slot410.var0.value == 1 && st.slot410.var1.value == 0 && st.slot410.var2.value == 0);
    CHECK(st.slot362_result.ok && st.slot362_result.records_written == 1 &&
          st.slot362.record0.hash == kSlot362Record0Hash && st.slot362.record0.name == kSlot362Record0Name);
    CHECK(st.slot477_result.ok && st.slot477.var0.value == 0 && st.slot477.var3.value == 0 &&
          st.slot477.var0.base.description.size == static_cast<std::int64_t>(kSlot477Var0DescriptionLength));
    CHECK(st.slot423_result.ok && st.slot423_result.records_written == 2 &&
          st.slot423.record0.hash == kSlot423Record0Hash && st.slot423.record1.hash == kSlot423Record1Hash);
    CHECK(st.slot449_result.ok && st.slot449.item0.target == &st.slot449.var0 &&
          st.slot449.item1.target == &st.slot449.var1 && st.slot449.item0.flags == 0x30);
    // ---- CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 4): 338 / 368 / 443 / 512 / 550 ----------------------------
    // Expected deltas from the pinned bodies: +17 console variables (338 9, 368 3, 443 2, 512 2, 550 1), +11 menu
    // objects (368 3 toggles, 443 group + 2, 512 group + 2, 550 2 toggles) and +29 atexit records (17 variables, 11
    // menu objects, 550's name holder); the menu root was already initialised.
    CHECK(st.slot338_result.ok && st.slot338.var0.value == 0 && st.slot338.var8.value == 0);
    CHECK(st.slot368_result.ok && st.slot368_result.statics_written == kSlot368StaticStoreCount &&
          st.slot368.item2.target == &st.slot368.var2);
    CHECK(st.slot443_result.ok && st.slot443.group0.kind == kMenuKindGroup && st.slot443.item1.target == &st.slot443.var1);
    CHECK(st.slot512_result.ok && st.slot512.group0.kind == kMenuKindGroup && st.slot512.item1.target == &st.slot512.var1);
    CHECK(st.slot550_result.ok && st.slot550.holder0.vptr == kSlot550HolderVptr &&
          st.slot550.item0.target == &st.slot550.var0 && st.slot550.item1.target == &st.slot550.var0);
    // ---- CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 5): 437 ----------------------------------------------------
    // Expected deltas from the pinned body: +3 console variables, +0 menu objects, +7 atexit records (3 variables and
    // the four NULL-argument chunk destructors 0x502920 / 0x502960 / 0x5029a0 / 0x5029e0).
    CHECK(st.slot437_result.ok && st.slot437_result.views_ok && st.slot437_result.chunk_records == 4 &&
          st.slot437.var0.value == 0 && st.slot437.var2.value == 0 &&
          st.slot437.views[0].data == reinterpret_cast<const std::uint8_t*>(kSlot437ViewText[0]) &&
          st.slot437.views[15].size == 0x12);
    // ---- CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 6): 366 / 411 / 430 / 504 / 555 --------------------------------
    // Expected deltas from the pinned bodies: +10 console variables (366 2, 411 2, 430 1, 504 2, 555 3), +5 menu
    // objects (366 2, 411 2, 430 1) and +16 atexit records (366 4, 411 4, 430 3 incl. the 0x523ba0 cell destructor,
    // 504 2, 555 3; the menu-root record 0x27280 already exists, so 0x268b0 records nothing new).
    CHECK(st.slot366_result.ok && st.slot366.var0.value == 1 && st.slot366.var1.value == 0 &&
          st.slot366.item1.target == &st.slot366.var1);
    CHECK(st.slot411_result.ok && st.slot411.var0.value == 1 && st.slot411.var1.value == 0 &&
          st.slot411.item1.target == &st.slot411.var1);
    CHECK(st.slot430_result.ok && st.slot430_result.var.refusal == HelperRefusal::none && st.slot430.cell == 0 &&
          st.slot430.var.value == 0xffffffffu && st.slot430.item.target == &st.slot430.var);
    CHECK(st.slot504_result.ok && st.slot504_result.refusal == HelperRefusal::none && st.slot504.float_var.value == 0 &&
          st.slot504.bool_var.value == 0);
    CHECK(st.slot555_result.ok && st.slot555_result.port.refusal == HelperRefusal::none && st.slot555.port.value == 25 &&
          st.slot555.server.base.name.size == 11 && st.slot555.from.base.name.size == 9);
    CHECK(st.slot397_result.ok && st.slot397_result.refusal == HelperRefusal::none && st.slot397.v0.value == 0x3f400000u &&
          st.slot397.v3.value == 0x3f11eb85u);
    CHECK(st.slot442_result.ok && st.slot442_result.refusal == HelperRefusal::none && st.slot442.v1.value == 0x3f800000u &&
          st.slot442.v0.value == 0 && st.slot442.v2.value == 0);
    CHECK(st.slot450_result.ok && st.slot450_result.refusal == HelperRefusal::none && st.slot450.v1.value == 0x3fc00000u);
    CHECK(st.slot460_result.ok && st.slot460_result.refusal == HelperRefusal::none && st.slot460.v0.value == 0 &&
          st.slot460.v1.value == 0 && st.slot460.v2.value == 0x3f800000u);
    // batch 27: slot 462 (the presentation / point-of-interest page) = 7 registrations, 4 menu items,
    // 15 atexit records; the functor command keeps the final vptr 0x1596980 and the bool var 0x1594218.
    CHECK(st.slot462_result.ok && st.slot462_result.registered_commands == 7 &&
          st.slot462_result.menu_items == 4 && st.slot462_result.atexit_records == 15);
    // startup slot 468: three app-info delegates, five scripted entries, ten commands, 18 atexit rows.
    CHECK(st.slot468_result.ok && st.slot468_result.apps == 3 && st.slot468_result.entries == 5 &&
          st.slot468_result.commands == 10 && st.slot468_result.app_registrations == 3 &&
          st.slot468_result.command_registrations == 10 && st.slot468_result.atexit_records == 18);
    CHECK(st.slot468_result.rewind_reserved && st.slot468_result.rewind_inline_push &&
          st.slot468_result.rewind_registered && st.slot468_result.rewind_secondary_called);
    // startup slot 568: one LoadCmdsFromFile command, two inline tokens, one atexit row.
    CHECK(st.slot568_result.ok && st.slot568_result.registered && st.slot568_result.tokens == 2 &&
          st.slot568_result.registrations == 1 && st.slot568_result.atexit_records == 1 &&
          st.slot568_result.reserved && st.slot568_result.inline_push_first &&
          st.slot568_result.inline_push_second && !st.slot568_result.grow_path &&
          st.slot568_result.empty_normalized && !st.slot568_result.first_flag_written &&
          st.slot568_result.second_flag_written);
    // startup slot 556: four app-info entries, one sendEmail command, six atexit rows.
    CHECK(st.slot556_result.ok && st.slot556_result.apps == 4 &&
          st.slot556_result.app_registrations == 4 && st.slot556_result.app_atexit_records == 4 &&
          st.slot556_result.command_registered && st.slot556_result.command_registrations == 1 &&
          st.slot556_result.command_atexit_records == 1 && st.slot556_result.atexit_records == 6 &&
          st.slot556_result.singleton_cleared && st.slot556_result.singleton_atexit_recorded &&
          st.slot556_result.command_vector_cleared && !st.slot556_result.command_gap98_written);
    // startup slot 530: the Resources page, 16 body atexit rows + 7 pending registrations + 8 menu items.
    CHECK(st.slot530_result.ok && st.slot530_result.atexit_records == 16 &&
          st.slot530_result.bools_built == 5 && st.slot530_result.ranged_built &&
          st.slot530_result.command_built && st.slot530_result.menu_items == 8 &&
          st.slot530_result.views_inert && st.slot530_result.args_functor_inert);
    CHECK(st.slot530.ranged.base.vptr == kRangedIntFinalVptr && st.slot530.ranged.value == 0x1e &&
          st.slot530.ranged.default_value == 0x1e && st.slot530.ranged.fn_vptr == kRangedIntFnVtbl);
    CHECK(st.slot530.items[2].target == &st.slot530.command &&
          st.slot530.items[3].target == &st.slot530.ranged);
    // startup slot 524: the screenshot command with two argument tokens and two atexit rows.
    CHECK(st.slot524_result.ok && st.slot524_result.atexit_records == 2 &&
          st.slot524_result.tokens == 2 && st.slot524_result.registered &&
          st.slot524_result.first_label_off && st.slot524_result.empty_normalized &&
          st.slot524_result.noop_vcall && st.slot524_result.views_inert);
    // startup slot 505: host table stand-in -> 226 points, zero BSS seed, no atexit/menu row.
    CHECK(st.slot505_result.ok && st.slot505_result.points == 226 &&
          st.slot505_result.seed_copied && st.slot505.bounds.max.z > 0.8f);

    // startup slot 552: seven flag dwords, seventeen zeroed records, eight page objects (six
    // class-A pages, class-B and class-C) and one atexit record per page; no command/menu effect.
    CHECK(st.slot552_result.ok && st.slot552_result.pages_built == 8 &&
          st.slot552_result.atexit_records == 8 && st.slot552_result.flags_written == 7 &&
          st.slot552_result.records_written == 17 && st.slot552.pages_a[0].base.vptr == 0x1594ca0 &&
          st.slot552.page_b.base.vptr == 0x1595b70 && st.slot552.page_c.base.vptr == 0x1597560);
    // startup slot 510: property cell + bool var + toggle + pose + seven-token command, four atexit.
    CHECK(st.slot510_result.ok && st.slot510_result.tokens == 7 &&
          st.slot510_result.atexit_records == 4 && st.slot510_result.registered &&
          st.slot510.var.base.vptr == 0x1594218 && st.slot510.var.value == 0 &&
          st.slot510.command.base.vptr == 0x1594dd0 &&
          st.slot510.command.delegate.handler == 0x20e900 &&
          st.slot510_tokens.count == 7 && st.slot510_tokens.items[0].hint == 1);
    // startup slot 497: 24 console vars, 19 menu items, 7 records, 3 entries, page + 54 atexit.
    CHECK(st.slot554_result.ok && st.slot554_result.built == 8 &&
          st.slot554_batch2a_result.ok && st.slot554_batch2a_result.built == 5 &&
          st.slot554_batch2a_result.registered == 5 &&
          st.slot554_batch2a_result.atexit_records == 5 &&
          st.slot554_batch2b_result.ok && st.slot554_batch2b_result.built == 8 &&
          st.slot554_batch2b_result.registered == 8 &&
          st.slot554_batch2b_result.atexit_records == 8 &&
          st.slot554_batch2b_result.views_inert == 16);
    CHECK(st.slot497_result.ok && st.slot497_result.vars_registered == 24 &&
          st.slot497_result.menus_registered == 19 && st.slot497_result.records_registered == 7 &&
          st.slot497_result.entries_registered == 3 && st.slot497_result.atexit_records == 54 &&
          st.slot497_result.page_atexit && st.slot497_result.canary_checked);
    // startup slot 567: manager cell zeroed, four console commands, five atexit records.
    CHECK(st.slot567_result.ok && st.slot567_result.commands_built == 4 &&
          st.slot567_result.registered == 4 && st.slot567_result.atexit_records == 5 &&
          st.slot567.manager == 0 && st.slot567.commands[0].base.vptr == 0x1594dd0 &&
          st.slot567.commands[0].base.name.size == 10 &&
          st.slot567.commands[3].base.flags == 0 &&
          st.slot567.commands[3].tail.handler == 0x22a40);
    CHECK(st.slot524.tokens[0].key == 0 && st.slot524.tokens[1].kind == 4 &&
          st.slot524.tokens[0].text.size == 3 && st.slot524.tokens[1].text.size == 0);
    CHECK(st.slot462.show_description.base.vptr == kSlot462BoolFinalVptr);
    CHECK(st.slot462.show_render_target.spec == kSlot462FunctorSpec &&
          st.slot462.show_render_target.spec_word == 0);
    CHECK(st.slot514_result.ok && st.slot514.var0.value == 0 && st.slot514.var1.value == 1 &&
          st.slot514.item1.target == &st.slot514.var1);
    CHECK(st.slot455_result.ok && st.slot455.var0.value == 1 && st.slot455.var1.value == 0 &&
          st.slot455.group.kind == kMenuKindGroup && st.slot455.item1.target == &st.slot455.var1);
    CHECK(st.slot488_result.ok && st.slot488.var0.max_engaged == 1 && st.slot488.holder.vptr == kSlot488HolderVtbl &&
          st.slot488.var2.value == 0);
    CHECK(st.slot369_result.ok && st.slot369_result.refusal == HelperRefusal::none && st.slot369.var0.value == 1 &&
          st.slot369.var1.value == 0x3e19999au && st.slot369.item1.target == &st.slot369.var1);
    CHECK(st.slot420_result.ok && st.slot420_result.refusal == HelperRefusal::none && st.slot420.var0.value == 0 &&
          st.slot420.var1.max_engaged == 1 && st.slot420.sep.kind == kMenuKindSeparator &&
          st.slot420.item2.target == &st.slot420.var1 && st.slot420.holder == 0);
    // ---- CPU-01-STARTUP-BATCH-7: 336 ---------------------------------------------------------------------------------
    // Expected deltas from the pinned body: +4 console variables (enum 0..3, bool, ranged int, unbounded float), +4 menu
    // objects (group and three toggles) and +10 atexit records (4 variables, group, 3 toggles, the 0x8ef00 holder and the
    // 0x7ea1a0 cell; the menu-root record 0x27280 already exists).
    CHECK(st.slot336_result.ok && st.slot336_result.refusal == HelperRefusal::none && st.slot336.var0.value == 0 &&
          st.slot336.var0.base.vptr == kEnumVarFinalVptr && st.slot336.var1.value == 1 && st.slot336.var2.value == 0 &&
          st.slot336.var3.value == 0x40a00000u && st.slot336.group.kind == kMenuKindGroup &&
          st.slot336.item0.target == &st.slot336.var0 && st.slot336.item2.target == &st.slot336.var2 &&
          st.slot336.holder.vptr == kSlot336HolderVptr && st.slot336.cell.handle != nullptr && st.slot336.cell.flag == 0);
    // 506 (the Touch Manager page): +2 variables, +2 menu objects (group and Draw toggle), +5 atexit records.
    CHECK(st.slot506_result.ok && st.slot506_result.refusal == HelperRefusal::none && st.slot506.var0.value == 0 &&
          st.slot506.var1.value == 0 && st.slot506.group.kind == kMenuKindGroup &&
          st.slot506.item.target == &st.slot506.var0);
    // 409 (sentinel-node cell, host malloc stand-in) and 414 (native CritSec handle; the 0x3b380 destroy is recorded, never run).
    CHECK(st.slot409.node != nullptr && st.slot409.obj.head == st.slot409.node);
    CHECK(st.slot414.obj[8] == 0 && st.slot414.obj[0x10] == 0);
    // 396 (the root-fitting page): +6 bool and +2 bounded float variables, +6 toggles (one 0x26ea0) and +2 0x27120 sliders, +16 atexit records.
    CHECK(st.slot396_result.ok && st.slot396.bvar0.value == 1 && st.slot396.bvar3.value == 0 && st.slot396.bvar4.value == 0 &&
          st.slot396.fvar0.value == 0x3f000000u && st.slot396.fvar1.value == 0x3f400000u &&
          st.slot396.bitem3.target == &st.slot396.bvar3 && st.slot396.fitem1.target == &st.slot396.fvar1);
    // 392 (the shadow hack-scale page): +4 bounded float variables, +4 menu sliders, +8 atexit records (the 0x27280 root record exists).
    CHECK(st.slot392_result.ok && st.slot392.var[0].value == 0x3f99999au && st.slot392.var[3].value == 0x3fc00000u &&
          st.slot392.item[0].target == &st.slot392.var[0] && st.slot392.item[3].target == &st.slot392.var[3]);
    CHECK(st.slot480_result.ok && st.slot480_result.rows_done == 7);   // batch 10: the on-screen message scales, bools and clear commands
    CHECK(st.slot388_result.ok && st.slot388_result.rows_done == 3);   // batch 10: the scripted-events bool, toggle and command
    CHECK(st.slot349_result.ok && st.slot349_result.rows_done == kSlot349RowCount);   // batch 10: the shadow camera / photo-mode page
    CHECK(st.slot446_result.ok && st.slot446_result.rows_done == kSlot446RowCount);   // batch 14
    CHECK(st.slot494_result.ok && st.slot494_result.built0 && st.slot494_result.atexit0 && st.slot494_result.built1 && st.slot494_result.atexit1);   // batch 20: two pool registries (0x26e340 / 0x26e770 lifted)
    CHECK(st.slot494.r0.begin != nullptr && st.slot494.r0.end == st.slot494.r0.begin + 1 && st.slot494.r1.end == st.slot494.r1.begin + 1);
    CHECK(st.slot501_result.ok && st.slot501_result.rows_done == kSlot501RowCount);   // batch 20
    CHECK(st.slot445_result.ok && st.slot445_result.rows_done == kSlot445RowCount);   // batch 20
    CHECK(st.slot429_result.ok && st.slot429_result.rows_done == kSlot429RowCount);   // batch 14
    CHECK(st.slot428_result.ok && st.slot428_result.atexit_recorded == kSlot428AtexitCount);   // batch 19: statics, 5 save descriptors, 37 id entries, 6 availability entries
    CHECK(st.slot424_result.ok && st.slot424_result.rows_done == kSlot424RowCount);   // batch 14
    CHECK(st.slot566_result.ok && st.slot566_result.rows_done == kSlot566RowCount);   // batch 14
    CHECK(st.slot534_result.ok && st.slot534_result.rows_done == kSlot534RowCount);   // batch 14
    CHECK(st.slot451_result.ok && st.slot451_result.rows_done == kSlot451RowCount);   // batch 14
    CHECK(st.slot427_result.ok && st.slot427_result.rows_done == kSlot427RowCount);   // batch 14
    CHECK(st.slot498_result.ok && st.slot498_result.rows_done == kSlot498RowCount);   // batch 14
    CHECK(st.slot473_result.ok && st.slot473_result.rows_done == kSlot473RowCount);   // batch 14
    CHECK(st.slot408_result.ok && st.slot408_result.rows_done == kSlot408RowCount);   // batch 14
    CHECK(st.slot511_result.ok && st.slot511_result.rows_done == kSlot511RowCount);   // batch 14
    CHECK(st.slot503_result.ok && st.slot503_result.rows_done == kSlot503RowCount);   // batch 14
    CHECK(st.slot493_result.ok && st.slot493_result.rows_done == kSlot493RowCount);   // batch 14
    CHECK(st.slot422_result.ok && st.slot422_result.rows_done == kSlot422RowCount);   // batch 14
    CHECK(st.slot464_result.ok && st.slot464_result.rows_done == kSlot464RowCount);   // batch 14
    CHECK(st.slot453_result.ok && st.slot453_result.rows_done == kSlot453RowCount);   // batch 14
    CHECK(st.slot572_result.ok && st.slot572_result.rows_done == kSlot572RowCount);   // batch 14
    CHECK(st.slot571_result.ok && st.slot571_result.rows_done == kSlot571RowCount);   // batch 14
    CHECK(st.slot402_result.ok && st.slot402_result.rows_done == kSlot402RowCount);   // batch 14
    CHECK(st.slot390_result.ok && st.slot390_result.rows_done == kSlot390RowCount);   // batch 12
    CHECK(st.slot508_result.ok && st.slot508_result.rows_done == kSlot508RowCount);   // batch 12
    CHECK(st.slot513_result.ok && st.slot513_result.rows_done == kSlot513RowCount);   // batch 12
    CHECK(st.slot469_result.ok && st.slot469_result.rows_done == kSlot469RowCount);   // batch 12
    // Pointer identities the startup rows hand to the slot models (host-supplied cross-slot objects), batch 16 follow-up.
    CHECK(st.slot469.external_target_0 == &st.slot518.var);
    CHECK(st.slot571.external_target_0 == &st.global_2dfcbd0_standin && st.slot572.external_target_0 == &st.global_2dfcbd0_standin);
    CHECK(st.slot453.record_registry == &st.record_registry && st.slot464.record_registry == &st.record_registry &&
          st.slot513.record_registry == &st.record_registry);
    CHECK(st.slot370_result.ok && st.slot370_result.rows_done == kSlot370RowCount);   // batch 12
    CHECK(st.slot561_result.ok && st.slot561_result.rows_done == kSlot561RowCount);   // batch 12
    CHECK(st.slot418_result.ok && st.slot418_result.rows_done == kSlot418RowCount);   // batch 12
    CHECK(st.slot399_result.ok && st.slot399_result.rows_done == kSlot399RowCount);   // batch 12
    CHECK(st.slot391_result.ok && st.slot391_result.rows_done == kSlot391RowCount);   // batch 12
    CHECK(st.slot381_result.ok && st.slot381_result.rows_done == kSlot381RowCount);   // batch 12
    CHECK(st.slot358_result.ok && st.slot358_result.rows_done == kSlot358RowCount);   // batch 12
    CHECK(st.slot398_result.ok && st.slot398_result.rows_done == kSlot398RowCount);   // batch 11
    CHECK(st.slot367_result.ok && st.slot367_result.rows_done == kSlot367RowCount);   // batch 11
    CHECK(st.slot363_result.ok && st.slot363_result.rows_done == kSlot363RowCount);   // batch 11
    CHECK(st.slot401_result.ok && st.slot401_result.rows_done == kSlot401RowCount);   // batch 11
    CHECK(st.slot352_result.ok && st.slot352_result.rows_done == kSlot352RowCount);   // batch 11
    std::fprintf(stderr, "DIAG pending=%zu menu=%zu atexit=%zu\n",
                 st.concmd.pending.size(),
                 static_cast<std::size_t>(st.debug_menu.root.count),
                 st.concmd.atexit_log.size());
    {
        std::size_t i = 0;
        for (const ConCmdListNode* node = st.concmd.pending.first();
             node != nullptr && node != &st.concmd.pending.head && i < 4000;
             node = node->next, ++i) {
            const auto* cmd = reinterpret_cast<const ConCmdBase*>(
                reinterpret_cast<const char*>(node) - offsetof(ConCmdBase, pending));
            const char* name = cmd->name.data != nullptr
                                   ? reinterpret_cast<const char*>(cmd->name.data)
                                   : "?";
            std::fprintf(stderr, "PEND %zu %s\n", i, name);
        }
    }
    CHECK(st.concmd.pending.size() == 1434);   // batch 49 (+32: slot 554 batch 13's 31 block starts and one command); batch 48 (+36: slot 554 batch 12's 17 bools, 17 bounded floats and 2 wrappers); batch 47 (+36: slot 554 batch 11's 6 bools, 26 bounded floats, 3 bounded dwords and one wrapper); batch 46 (+36: slot 554 batch 10's 7 bools, 22 bounded floats, 4 unbounded floats, 2 unbounded ints and one wrapper); batch 45 (+36: slot 554 batch 9's 6 bools, 28 bounded floats and 2 wrappers); batch 44 (+36: slot 554 batch 8's 7 bools, 26 bounded floats and 3 wrappers); batch 43 (+36: slot 554 batch 7's 9 bools, 19 bounded floats, 4 unbounded ints, 3 object-hosted dwords and one wrapper); batch 42 (+36: slot 554 batch 6's 11 bools, 22 bounded floats and three wrappers); batch 41 (+36: slot 554 batch 5's 13 bools, 22 bounded floats and one wrapper); batch 40 (+36: slot 554 batch 4's 21 bools, 12 bounded floats, 1 bounded dword and 2 wrappers); batch 39 (+36: slot 554 batch 3's 33 bools, 2 bounded floats and one 0x6ba10 wrapper); batch 38 (+23: slot 554 batch 2b continuation's 4 wrapper objects, 1 int, 1 bounded float and 17 bools); batch 37: slot 497's 24 variables; batch 35: slot 567 queues executeCmd/getValue/help/core_unittests // batch 33 (+0: slot 505 registers no command); batch 32 (+1: slot 524's screenshot command); batch 31 (+7: slot 530's 5 bools, ranged int and command); batch 30 (+1: slot 556's sendEmail command); batch 29 (+1: slot 568's LoadCmdsFromFile command); startup slot 468 (+10: its ten console commands); after batch 27 it was 948 (+7: slot 462's six commands and bool variable); after batch 24 (+4: slot 452's four console commands); after batch 21 (+1: slot 570 the mem_Verify bool; +5: slot 446 four bool variables and one local-int variable); after batch 20 it was 927 (+8: slot 501 five bools, one uint, one ufloat, one string variable; +1: slot 445 the uiForceAllowExtendedVideoModes bool); after batch 19 it was 918 (+3: slot 429 one bool and two commands; +6: slot 428's six entitlement_available_* bool variables, queued by 0xa42960); after batch 18 it was 909 (+8: slot 566 five variables, slot 424 three console commands); after batch 17 it was 901 (+55: slots 451, 427, 534 variables; each slot test derives its own count from the ELF rows); after batch 16 it was 846 (+36: slots 408, 473, 498 variables; each slot test derives its own count from the ELF rows); after batch 15 it was 810 (+31: slots 493, 422, 511, 503); after batch 14 it was 779 (+1: slot 464 one bool; slot 453 none; +43: slot 571 26 bools, 13 bounded floats and 4 commands; +5: slot 572 five commands; +8: slot 402 three floats, three bools, the inline enum and the string variable; +2: slot 390 showLegacyParticle bools); after batch 13 (+12: slot 508 three ranged ints, two unbounded ints, two bools and five unbounded floats; +7: slot 513 one unbounded int, one local-int, one ranged int, four bools; +186: slot 469 five bools and three bounded floats; slot 370 +178: slot 370, 36 bool, 131 bounded float, 5 unbounded float, 2 local-int, 3 command variables and one string variable); after batch 12 it was 515 (+34: slot 358 four, 391 three, 381 eleven, 561 four, 399 twelve variables, 418 none); after batch 11 it was 481 (+26: slot 398 two bools and two local-int variables, slot 363 three local-int variables, slot 367 six bools, one bounded and twelve unbounded floats; +12 from slot 401: ten bool variables and two local-int wrapper variables; +8 from slot 352: six bounded floats, one ranged int, one unbounded float); batch 10 was 435 (+7 from slot 480: three bounded floats, two bools, two commands; +2 from slot 388: the bool variable and gCP10ExtractCP3EventFrameTables; +37 from slot 349: 9 bool and 28 bounded float variables; +4 from slot 342: the bool variable, two unbounded floats and cp10_skipCurCutsceneCut; +6: slot 340, four shadow_gameflag* commands, bp_giveAllOrbs and bp_dumpCp3GameStateInfo); batch 9 was 379 (+78: 339, 395, 380, 375, 382, 360, 373, 354); batch 8 was 301 (+37: 337 7, 351 9, 400 5, 444 5, 474 4, 521 7); batch 7 was 264 (+8 from 396; +4 from 336, +2 from 506); batch 6 (+2 from 420; batch 5 was 212; +10 variables, +13 from 397 4, 442 3, 450 3, 460 3, +2 from 514, +2 from 455, +3 from 488, +2 from 369)
                                                                // 175 after batch 2 (169 through the first batch + 6)
    CHECK(st.concmd.pending.first() == &st.slot298_shader_type.base.pending);
    CHECK(st.debug_menu.root.count == 440); // batch 37 (+19: slot 497's label/slider/toggles/groups/separators); batch 33 (+0: slot 505 builds no menu item); batch 32 (+0: slot 524 builds no menu item); batch 31 (+8: slot 530's Resources group and seven toggles); batch 27 (+4: slot 462's two toggles, command item and group); measured after batch 21 (+1: slot 570 the Memory toggle; +2: slot 446 one 0x26c10 group and one 0x26ea0 toggle); after batch 20 it was 404 (+10: slot 501 group, label and eight toggles); after batch 19 it was 394 (+5: slot 429 two groups, one toggle and two command items); after batch 18 it was 389; batch 18 (+1: slot 566's memoryToStringDisplayUnits row; slot 424 none); after batch 17 it was 388 (+7 menu items: slots 451 and 427; slot 534 none); after batch 16 it was 381 (+27 menu items: slot 473 22, slot 498 5, slot 408 none); after batch 15 it was 354 (+8 menu items of slots 493, 422, 511, 503); after batch 14 it was 346 (+1: slot 571 command item; +3: slot 572 two 0x26ea0 toggles and a command item; +5: slot 402 four 0x26db0 toggles and one 0x27120 slider); after batch 13 (+6: slot 513 six 0x26ea0 toggles; +53: slot 469 group, five 0x26ea0 toggles and four 0x27050 sliders; slot 370 +43: slot 370 23 toggles, 6 toggle-with-extra, 14 sliders); after batch 12 it was 278 (+21: slot 358 five, 391 one, 561 five, 399 ten items); after batch 11 it was 257 (+11 toggles of slot 401; +4 items of slot 398: two 0x26db0 toggles and two 0x26ea0 toggles); batch 10 was 242 (+1: slot 388 toggle; +5 from slot 349: two toggles and three sliders; +2: slot 340 group and Give All Orbs toggle); batch 9 was 234 (+58 items of the eight batch-9 slots; the menu root already exists); batch 8 was 176 (+7: 351 two sliders, 400 five toggles); batch 7 was 169 (+8 from 396: six toggles and two sliders; +2 from 506; +4 from 336: group and three toggles); batch 6 (+3 from 420: separator, toggle and slider; +2 from 369: toggle and slider; +5 menu objects: 366 2, 411 2, 430 1; +3 from 455: group and two toggles); batch 4 (+11 menu objects); batch 2 was 125; 122 through the first batch
                                                                // + 1 (374's toggle; no other batch-2 slot builds an item)
    CHECK(st.concmd.atexit_log.size() == 2194); // batch 49 (+32: slot 554 batch 13's 31 block starts and one command); batch 48 (+36: slot 554 batch 12's 36 variables); batch 47 (+36: slot 554 batch 11's 36 variables); batch 46 (+36: slot 554 batch 10's 36 variables); batch 45 (+36: slot 554 batch 9's 36 variables); batch 44 (+36: slot 554 batch 8's 36 variables); batch 43 (+36: slot 554 batch 7's 36 variables); batch 42 (+36: slot 554 batch 6's 36 variables); batch 41 (+36: slot 554 batch 5's 36 variables); batch 40 (+36: slot 554 batch 4's 36 variables); batch 39 (+36: slot 554 batch 3's 36 variables); batch 38 (+23: slot 554 batch 2b continuation's 23 variables); batch 37 (+54: slot 497's 24 vars, page, 19 menu items, 7 records, 3 entries); batch 35 (+5: slot 567's manager cell and four commands); batch 34 (+8: slot 552's eight page records); batch 33 (+0: slot 505 records no atexit); batch 32 (+2: slot 524's cell and command); batch 31 (+16: slot 530's cell, 5 bools, ranged int, command, group and 7 toggles); batch 30 (+6: slot 556's 0x4a660 singleton, four 0x4b660 entries and 0x4b0 command); batch 29 (+1: slot 568's 0x4b0); startup slot 468 (+18: three 0x4b660, five 0x3c15e0, ten 0x4b0); after batch 27 it was 1644 (+15: slot 462); after batch 25 (+14: slot 495 the command 0x20b0, the blob record 0x209d80 and 12 x 0x268e20); after batch 24 (+7: slot 452 the node 0x497610, the cell 0x493190, four commands 0x4b0 and the toggle 0x272d0); after batch 23 (+1: slot 537 registers its single 0x131f00 record for the 8-byte word 0x2ee12e0; the 40 half conversions register nothing); after batch 22 it was 1599 (+21: slot 532 twenty-one borrowed views, identity only); after batch 21 it was 1578 (+5: slot 570 the three identity tables and the bool and toggle records; +10: slot 446 four bools, one local int, a NameHolder30, a group, a toggle and two 0x90-byte registry entries); after batch 20 it was 1563 (+2: slot 494 the two pool-registry destructor records 0x269590 / 0x269610, identity only; +18: slot 501 eight variables and ten menu items; +5: slot 445 four name holders (destructors 0x4cb9e0 / 0x4cba00) and the bool; the hash-to-dword table registers nothing); after batch 19 it was 1538 (+12: slot 429 one bool, two commands, two command items, two groups, one toggle, the zero pointer slot 0x52a010 and three zero vectors 0x52a0d0 / 0x52a140 / 0x52a1b0; +49: slot 428's 49 __cxa_atexit records: 0x52e980, five 0x52eaa0, 37 x 0x52eb00, six 0x52eb10; identity only); after batch 18 it was 1477 (+10: slot 566 six records, slot 424 four = three commands and the store-table destructor 0x54dbb0 with a null object); after batch 17 it was 1467 (+70: slots 451, 427, 534; each slot test derives its own count); after batch 16 it was 1397 (+75: slots 408, 473, 498; each slot test derives its own count); after batch 15 it was 1322 (+43: one record per non-cell row of slots 493, 422, 511, 503; the 503 strncpy cell records none); after batch 14 it was 1279 (+3: slot 453 its group destructor 0x48a250; slot 464 the bool and its group destructor 0x3d9b40 (the registry's first-use record 0xa81b80 only moves to whichever records slot runs first); +44: slot 571 43 variables and the command item; +8: slot 572 five commands, two toggles, one command item; +14: slot 402 eight variables, the name holder and five menu items; +7: slot 390 tree header, node reference, three name holders, two bools); after batch 13 (+12: slot 508 one record per variable, no menu rows; +16: slot 513 the registry's first-use 0xa81b80, the records' 0x1c8990, seven variables, six items and the zero container 0x1c5090; +241: slot 469 eight variables, ten items and the sentinel container 0x3941b0; slot 370 +222: one record per non-cell row of slot 370); after batch 12 it was 934 (+55: one record per non-cell row of slots 358 9, 391 4, 381 11, 561 9, 399 22; 418 is cells only); after batch 11 it was 879 (+30: slot 398 four variables and four items, slot 363 three local-int dtors, slot 367 nineteen variables; +23 from slot 401: ten bool 0x20b0, two local-int dtors 0x6112e0 / 0x611320, eleven toggle items 0x272d0; +8 from slot 352: floats 0x20f0 x7, int 0x18a60 x1; no menu rows); batch 10 was 818 (+7 from slot 480: floats 0x20f0 x3, bools 0x20b0 x2, commands 0x4b0 x2; +3 from slot 388: bool 0x20b0, toggle 0x272d0, command 0x4b0; +49 from slot 349: 37 variables, 5 items, 7 holders (0x8ef00 x1, 0x2fced0 x6); +4 from slot 342: bool 0x20b0, floats 0x20f0 x2, command 0x4b0; the two 0x23fd00 name cells record nothing; +8: slot 340, six commands, group, toggle; the 0x27280 root record exists); batch 9 was 747 (+139: the batch-9 variables, items and cell/holder records; the 0x27280 root record already exists); batch 8 was 608 (+49: 337 8, 351 11, 400 10, 444 5, 474 8, 521 7; the 0x27280 root record already exists); batch 7 was 559 (+1 each for 409 and 414 (the atexit records 0x5b3d10 / 0x3b380, identity only); +16 for 396: eight variables and eight items, the root record exists; +10 for 336, +4 for 506: the 0x27280 root record already exists); batch 6 (+6 for 420: two variables, separator, toggle, slider and the 0x56ef80 cell; +4 for 369: two variables, toggle and slider; +4 for 488: three variables and the 0x2b5050 holder; +5 for 455: two variables, group and two toggles, the root record already exists; +16, +13 for the 397 / 442 / 450 / 460 variables, +4 for 514: two variables and two items); batch 5 was 467 (+7: 437's 3 variables and 4 chunk records); batch 4 was 460 (+29); batch 3 was 431 (518 adds one); batch 2 was 412, 401 through the first batch
                                                                // + 11 (374 2, 448 2, 507 2, 539 4, 545 1). The menu root
                                                                // was already initialised, so 0x268b0 records nothing new here.
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
    // ---- CPU-01-RANGED-BOUND-VAR-A4F4D0-01: idx 309, the "Save System" block -------------
    CHECK(st.slot309_result.ok && st.slot309_result.vars_built == 3 &&
          st.slot309_result.menu_items == 4);
    CHECK(st.slot309_result.atexit_records == 12 && st.slot309_result.functors_destroyed == 3);
    const struct { std::uint64_t final_vptr, fn_vtbl; std::uint32_t max; } want309[3] = {
        {0x15d1ba8, 0x15d1b78, 5}, {0x15d1c60, 0x15d1c30, 0x12}, {0x15d1cf8, 0x15d1cc8, 0x14}};
    const ConCmdRangedVar* const vars309[3] = {&st.slot309.var1.var, &st.slot309.var2.var,
                                               &st.slot309.var3.var};
    for (int i = 0; i < 3; ++i) {
        CHECK(vars309[i]->base.vptr == want309[i].final_vptr);
        CHECK(vars309[i]->fn_vptr == want309[i].fn_vtbl);
        CHECK(vars309[i]->base.registered == 1 && vars309[i]->base.flags == 1);
        CHECK(vars309[i]->min_engaged == 1 && vars309[i]->max_engaged == 1);
        std::uint32_t max_cell = 0;
        std::memcpy(&max_cell, vars309[i]->max_value, 4);
        CHECK(max_cell == want309[i].max);
        CHECK(vars309[i]->value == 0 && vars309[i]->default_value == 0);
    }
    CHECK(st.slot309.toggle1.target == &st.slot309.var1);
    CHECK(st.slot309.toggle2.target == &st.slot309.bool_var.var);
    CHECK(st.slot309.toggle1.kind == 2 && st.slot309.toggle1.flags == 0x30);

    // ---- CPU-01-STARTUP-SLOT-312-CHARACTERIZATION-01: idx 312, the entitlement/title block --------
    CHECK(st.slot312_result.ok && st.slot312_result.atexit_records == 8 &&
          st.slot312_result.functors_destroyed == 2 && st.slot312_result.var_functor_calls == 22);
    CHECK(st.slot312_result.vars_built == 3 && st.slot312_result.menu_items == 4);
    CHECK(st.slot312.title_sku.base.vptr == kStringVarFinalVptr &&
          st.slot312.title_sku.base.registered == 1);
    CHECK(st.slot312.title_sku.value.size == 7 &&
          std::memcmp(st.slot312.title_sku.value.data, "DEFAULT", 7) == 0);
    CHECK(st.slot312.age_restriction.base.vptr == 0x1594c08 && st.slot312.region.base.vptr == 0x15d18f0);
    CHECK(st.slot312.age_restriction.value == 0xffffffffu && st.slot312.region.value == 0);
    CHECK(st.slot312.age_restriction.fn_vptr == 0x15944a0 && st.slot312.region.fn_vptr == 0x15d18c0);
    CHECK(st.slot312.age_restriction.value_ref == &st.slot312.age_restriction.value);
    CHECK(st.slot312.age_restriction.args[3] == 0xa44a40 && st.slot312.region.args[3] == 0xa44a60);
    CHECK(st.slot312.toggle_sku.target == &st.slot312.title_sku);
    CHECK(st.slot312.toggle_age.target == &st.slot312.age_restriction);
    CHECK(st.slot312.toggle_region.target == &st.slot312.region);
    CHECK(st.slot312.group.kind == 1 && st.slot312.toggle_sku.kind == 2);
    CHECK(st.slot312.record.word38 == 0xffffffffu && st.slot312.record.word3c == 0x72);

    // ---- CPU-01-STARTUP-SLOT-316-CHARACTERIZATION-01: idx 316, the Localization block -------------
    CHECK(st.slot316_result.ok && st.slot316_result.atexit_records == 14);
    CHECK(st.slot316_result.functors_destroyed == 4 && st.slot316_result.functor_dispatches == 48);
    CHECK(st.slot316_result.vars_built == 6 && st.slot316_result.menu_items == 6);
    CHECK(st.slot316.var_text.base.vptr == 0x15d11b8 && st.slot316.var_text.base.registered == 1);
    CHECK(st.slot316.var_text.value == 0xffffffffu && st.slot316.var_text.default_value == 0xffffffffu);
    CHECK(st.slot316.var_text.fn_vptr == 0x15d1188 && st.slot316.var_text.args[1] == 0xa324a0);
    CHECK(st.slot316.var_system.args[3] == 0xa2ca10);
    CHECK(st.slot316.var_english.base.vptr == kBoolVarFinalVptr && st.slot316.var_english.value == 0);
    CHECK(st.slot316.command.base.vptr == 0x1594dd0 && st.slot316.command.base.flags == 2);
    CHECK(st.slot316.command.delegate.handler == 0xa2cad0);
    CHECK(st.slot316.item_text.target == &st.slot316.var_text &&
          st.slot316.item_reload.target == &st.slot316.command);
    CHECK(st.slot316.item_text.kind == 2 && st.slot316.item_text.flags == 0x30);
    CHECK(st.slot316.unlocalized.size == 17 &&
          std::memcmp(st.slot316.unlocalized.data, "!!!Unlocalized!!!", 17) == 0);

    // ---- CPU-01-STARTUP-SLOT-326-CHARACTERIZATION-01: idx 326, the Colossus / Shadow eye block -----
    CHECK(st.slot326_result.ok && st.slot326_result.atexit_records == 36);
    CHECK(st.slot326_result.functors_destroyed == 13 && st.slot326_result.vars_built == 15 &&
          st.slot326_result.menu_items == 21);
    CHECK(st.slot326_result.eye_values_mirrored && st.slot326_result.views_inert);
    CHECK(st.slot326.bool_eye_state.base.vptr == 0x1594218 && st.slot326.bool_eye_state.value == 0);
    CHECK(st.slot326.bool_force_angry.base.registered == 1 && st.slot326.bool_force_angry.value == 0);
    CHECK(st.slot326.threshold.base.vptr == 0x15942b0 && st.slot326.threshold.value == 0x40400000u);
    CHECK(st.slot326.threshold.default_value == 0x40400000u && st.slot326.threshold.min_engaged == 1);
    CHECK(st.slot326.eye[0].base.vptr == 0x15943c0 && st.slot326.eye[11].base.vptr == 0x15943c0);
    CHECK(st.slot326.eye[0].default_value == 0x3f800000u && st.slot326.eye[3].default_value == 0);
    CHECK(st.slot326.eye[4].default_value == 0x3e99999au && st.slot326.eye[9].default_value == 0x3f000000u);
    CHECK(st.slot326_eye_values.inactive[0] == 0x3f800000u && st.slot326_eye_values.red[1] == 0x3f000000u);
    CHECK(st.slot326.group_colossus.kind == 1 && st.slot326.group_colossus.flags == 0x30);
    CHECK(st.slot326.item_label.kind == 0 && st.slot326.item_label.flags == 0);
    CHECK(st.slot326.item_label.text.size == 13 &&
          std::memcmp(st.slot326.item_label.text.data, "MenuSeparator", 13) == 0);
    CHECK(st.slot326.toggle_threshold.target == &st.slot326.threshold);
    CHECK(st.slot326.toggle_force_angry.target == &st.slot326.bool_force_angry);
    CHECK(st.slot326.slider_red[3].target == &st.slot326.eye[11]);

    // ---- CPU-01-STARTUP-SLOT-329-CHARACTERIZATION-01: idx 329, the Shadow Path block ------------
    CHECK(st.slot329_result.ok && st.slot329_result.atexit_records == 22);
    CHECK(st.slot329_result.functors_destroyed == 3 && st.slot329_result.functor_dispatches == 24);
    CHECK(st.slot329_result.vars_built == 10 && st.slot329_result.menu_items == 12);
    CHECK(st.slot329_result.views_inert);
    CHECK(st.slot329.bool_path_finding.base.vptr == 0x1594218 && st.slot329.bool_path_finding.value == 1);
    CHECK(st.slot329.bool_path_finding.default_value == 1 && st.slot329.bool_draw_enable.value == 0);
    CHECK(st.slot329.ranged_path_data.base.vptr == 0x15cea60);
    CHECK(st.slot329.ranged_path_data.value == 2 && st.slot329.ranged_path_data.default_value == 2);
    CHECK(st.slot329.ranged_path_data.fn_vptr == 0x15ceac8);
    CHECK(st.slot329.ranged_chara_type.base.vptr == 0x15ceb18);
    CHECK(st.slot329.ranged_chara_type.value == 1 && st.slot329.ranged_chara_type.fn_vptr == 0x15ceae8);
    CHECK(st.slot329.ranged_chara_type.base.description.size == 0x46);
    CHECK(st.slot329.float_translation.base.vptr == 0x15942b0);
    CHECK(st.slot329.float_translation.value == 0x3f800000u &&
          st.slot329.float_translation.fn_vptr == 0x1594318);
    CHECK(st.slot329.bool_raycasts_enable.value == 1 && st.slot329.bool_raycasts_debug.value == 0);
    CHECK(st.slot329.group.kind == 1 && st.slot329.group.flags == 0x30);
    CHECK(st.slot329.items[5].kind == 3 && st.slot329.items[5].flags == 0x30);
    CHECK(st.slot329.items[5].text.size == 13 &&
          std::memcmp(st.slot329.items[5].text.data, "MenuSeparator", 13) == 0);
    CHECK(st.slot329.items[0].target == &st.slot329.bool_draw_enable);
    CHECK(st.slot329.items[3].target == &st.slot329.ranged_chara_type);
    CHECK(st.slot329.items[4].target == &st.slot329.bool_path_finding);
    CHECK(st.slot329.items[7].target == &st.slot329.float_translation);

    std::puts("startup_ctor_coverage: all checks passed");
    return 0;
}
