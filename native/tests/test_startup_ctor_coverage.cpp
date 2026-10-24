// CPU-01-CTOR-COVERAGE-BATCH-01: production-path coverage of the recovered constructor table.
//
// Proves that every lifted vec_*/leaf_* constructor module runs in recovered_constructors(), that
// the registered rows pair with the pinned original table, and that the remaining startup cliff is
// explicit (first unrecovered index 306 / RVA 0xa633b0 after slots 296-305).
#include <cstdio>
#include <cstdlib>

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
    const auto slots = recovered_constructors();
    const CtorRunResult result = run_constructors(slots, st);
    CHECK(result.ok());
    CHECK(!result.callback_failed);
    CHECK(result.blocked_on_rva == 0);
    CHECK(!st.init_failed);
    CHECK(result.completed == slots.size());
    CHECK(slots.size() == 317);

    // Every registered row is in original order and pairs with the pinned table.
    for (std::size_t i = 0; i < slots.size(); ++i) {
        CHECK(slots[i].run != nullptr);
        CHECK(slots[i].original_index < kOriginalCtorCount);
        CHECK(slots[i].original_rva == original_constructors()[slots[i].original_index].rva);
        if (i != 0) CHECK(slots[i - 1].original_index < slots[i].original_index);
    }

    // ---- explicit, machine-checked gap ---------------------------------------
    CHECK(first_unrecovered_constructor_index() == 306);
    CHECK(first_unrecovered_constructor_rva() == 0xa633b0);
    CHECK(slots.size() + 256 == kOriginalCtorCount);

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
    CHECK(st.concmd.pending.size() == 5);                        // 298, 299 A/B, 301, 303
    CHECK(st.concmd.pending.first() == &st.slot298_shader_type.base.pending);
    CHECK(st.debug_menu.root.count == 3);                        // idx 299 items C and D, idx 303 toggle
    CHECK(st.concmd.atexit_log.size() == 19);                    // 7 (298-301) + 10 (302) + 2 (303)
    for (std::size_t i = 0; i < kSlot302HandleCount; ++i) {
        CHECK(st.slot302.handles[i] == nullptr);
        CHECK(st.concmd.atexit_log.at(7 + i).dtor_rva == kSlot302DtorRva);
        CHECK(st.concmd.atexit_log.at(7 + i).object == &st.slot302.handles[i]);
    }
    CHECK(st.slot302.identity_vptr == kSlot302IdentityVptr);
    for (const Pair128& pair : st.slot305) CHECK(pair.lo == 0 && pair.hi == 0);
    CHECK(st.leaf_7ce8d0.word0 == 0x3f80000000000001ull);  // idx 346
    CHECK(st.leaf_7ce8d0.word1 == 0x3f80000000000001ull);
    CHECK(st.leaf_7ce8d0.flag0.load() == 0 && st.leaf_7ce8d0.flag1.load() == 0);

    std::puts("startup_ctor_coverage: all checks passed");
    return 0;
}
