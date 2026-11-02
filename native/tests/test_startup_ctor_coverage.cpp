// CPU-01-CTOR-COVERAGE-BATCH-01: production-path coverage of the recovered constructor table.
//
// Proves that every lifted vec_*/leaf_* constructor module runs in recovered_constructors(), that
// the registered rows pair with the pinned original table, and that the remaining startup cliff is
// explicit (first unrecovered index 335 / RVA 0x7edf10, after slot 329 landed).
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
    CHECK(slots.size() == 455);

    // Every registered row is in original order and pairs with the pinned table.
    for (std::size_t i = 0; i < slots.size(); ++i) {
        CHECK(slots[i].run != nullptr);
        CHECK(slots[i].original_index < kOriginalCtorCount);
        CHECK(slots[i].original_rva == original_constructors()[slots[i].original_index].rva);
        if (i != 0) CHECK(slots[i - 1].original_index < slots[i].original_index);
    }

    // ---- explicit, machine-checked gap ---------------------------------------
    CHECK(first_unrecovered_constructor_index() == 335);
    CHECK(first_unrecovered_constructor_rva() == 0x7edf10);
    CHECK(slots.size() + 118 == kOriginalCtorCount);

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
    CHECK(st.concmd.pending.size() == 155);                      // observed: 114 through 312 + 10 (314) + 6 (316) + 15 (326) + 10 (329)
    CHECK(st.concmd.pending.first() == &st.slot298_shader_type.base.pending);
    CHECK(st.debug_menu.root.count == 117);                     // observed: 68 through 312 + 10 (314) + 6 (316) + 21 (326) + 12 (329)
    CHECK(st.concmd.atexit_log.size() == 382);                  // measured: 360 through 326 + 22 (329)
                                                                // (the menu root was already initialised, so
                                                                //  0x268b0 records nothing new here)
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
    CHECK(st.slot329.items[4].target == &st.slot329.ranged_chara_type);
    CHECK(st.slot329.items[7].target == &st.slot329.float_translation);

    std::puts("startup_ctor_coverage: all checks passed");
    return 0;
}
