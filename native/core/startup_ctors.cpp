#include <cmath>
#include "startup_ctors.hpp"

namespace sotc {

bool init_slot23_leaf_e71990(StartupState& state) noexcept {
    if (!load_slot23_names_fixture_default(state.slot23_names)) {
        state.init_failed = true;
        return false;
    }
    init_leaf_e71990(state.leaf_e71990, state.slot23_names.inputs);
    return true;
}

CtorRunResult run_constructors(std::span<const CtorSlot> in_order, StartupState& state) noexcept {
    CtorRunResult result;
    for (const CtorSlot& slot : in_order) {
        if (slot.run == nullptr) {
            result.blocked_on_rva = slot.original_rva;   // no stub-success: caller must treat as startup failure
            return result;
        }
        slot.run(state);
        if (state.init_failed) {
            // Propagate a callback's visible initialization failure instead of continuing.
            result.blocked_on_rva = slot.original_rva;
            result.callback_failed = true;
            return result;
        }
        ++result.completed;
    }
    return result;
}

std::span<const CtorSlot> recovered_constructors() noexcept {
    static constexpr CtorSlot kSlots[] = {
        {0xf6cad0, 0, [](StartupState& s) noexcept { init_math_constants(s.math); }},
        {0xf66640, 1, [](StartupState& s) noexcept { init_math_constants(s.math_b); }},
        {0xf53de0, 2, [](StartupState& s) noexcept { init_math_constants(s.math_c); }},
        {0xf50d70, 3, [](StartupState& s) noexcept { init_xyz_mask_copy(s.xyz_mask_b); }},
        {0xf4fd40, 4, [](StartupState& s) noexcept { init_f4fd40(s.f4fd40); }},
        {0xf4eba0, 5, [](StartupState& s) noexcept { init_f4eba0(s.f4eba0); }},
        {0xf4b540, 6, [](StartupState& s) noexcept { init_f4b540(s.f4b540); }},
        {0xf4b510, 7, [](StartupState& s) noexcept { init_vec_neg036(s.neg036); }},
        {0xf3d9d0, 8, [](StartupState& s) noexcept { init_math_constants(s.math_d); }},
        {0xf30460, 9, [](StartupState& s) noexcept { init_xyz_mask_copy(s.xyz_mask_copy); }},
        // Slot 10 / RVA 0x00f09e60: publishes the ClothingPreviewParam descriptor and creates the
        // once-guarded recursive + priority-inherit mutex. Verified from the constructor body and
        // its consumers; see docs/checkpoints/2026-10-20-root-cpu01-ctor10-descriptor-consumer-*.md.
        {0xf09e60, 10, [](StartupState& s) noexcept { (void)init_clothing_preview_descriptor(s); }},
        // Slot 11 / RVA 0x00f09160: publishes the ClothingPhysicalMeshParameters descriptor and
        // creates its recursive mutex (DescriptorRegistration). The 74-record reflection schema is NOT
        // built (lazy; see docs/checkpoints/2026-10-21-agentG-ctor11-reflection-scope.md).
        {0xf09160, 11, [](StartupState& s) noexcept { (void)init_clothing_physical_mesh_descriptor(s); }},
        // Slots 12-19: registration thunks of unidentified classes (descriptor + heap mutex + atexit).
        // Only the registration is modelled; each body was replayed from the ELF bytes by
        // tools/lift/check_ctor_thunks.py. See core/registration_thunks.hpp for what the original also does.
        {0xf06340, 12, [](StartupState& s) noexcept { (void)init_registration_thunk(s, 12); }},
        {0xf058f0, 13, [](StartupState& s) noexcept { (void)init_registration_thunk(s, 13); }},
        {0xf02ff0, 14, [](StartupState& s) noexcept { (void)init_registration_thunk(s, 14); }},
        {0xf01100, 15, [](StartupState& s) noexcept { (void)init_registration_thunk(s, 15); }},
        {0xf00090, 16, [](StartupState& s) noexcept { (void)init_registration_thunk(s, 16); }},
        {0xefe320, 17, [](StartupState& s) noexcept { (void)init_registration_thunk(s, 17); }},
        {0xefd020, 18, [](StartupState& s) noexcept { (void)init_registration_thunk(s, 18); }},
        {0xefacd0, 19, [](StartupState& s) noexcept { (void)init_registration_thunk(s, 19); }},
        {0xef85e0, 20, [](StartupState& s) noexcept { init_vec_ef85e0(s.vec_ef85e0); }},
        {0xee1130, 21, [](StartupState& s) noexcept { init_const_table_ee1130(s.const_table_ee1130); }},
        {0xea52f0, 22, [](StartupState& s) noexcept { init_ea52f0(s.ea52f0); }},
        // Slot 23 / RVA 0x00e71990: private asset/fixture import of 14 name strings + kLeafE71990Hash.
        // Missing fixture → init_failed (fail-visible).
        {0xe71990, 23, [](StartupState& s) noexcept { (void)init_slot23_leaf_e71990(s); }},
        // Slots 24-47: descriptor + mutex + atexit family (same contract as 12-19). Promoted from the
        // former registered-ahead table once slot 23 cleared (Phase B).
        {0xe56b70, 24, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 24); }},
        {0xe55f10, 25, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 25); }},
        {0xe55240, 26, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 26); }},
        {0xe54a30, 27, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 27); }},
        {0xe53a10, 28, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 28); }},
        {0xe52a20, 29, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 29); }},
        {0xe52150, 30, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 30); }},
        {0xe518e0, 31, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 31); }},
        {0xe510e0, 32, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 32); }},
        {0xe50980, 33, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 33); }},
        {0xe500b0, 34, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 34); }},
        {0xe4f840, 35, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 35); }},
        {0xe4f040, 36, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 36); }},
        {0xe4e8e0, 37, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 37); }},
        {0xe4e010, 38, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 38); }},
        {0xe4d7a0, 39, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 39); }},
        {0xe4cfa0, 40, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 40); }},
        {0xe4c840, 41, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 41); }},
        {0xe4bf10, 42, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 42); }},
        {0xe4b7b0, 43, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 43); }},
        {0xe4afb0, 44, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 44); }},
        {0xe47670, 45, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 45); }},
        {0xe467e0, 46, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 46); }},
        {0xe459e0, 47, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 47); }},
        {0xe10490, 48, [](StartupState& s) noexcept { init_trig_constants(s.trig); }},
        {0xe0b180, 49, [](StartupState& s) noexcept { init_vec_e0b180(s.vec_e0b180); }},
        {0xe094e0, 50, [](StartupState& s) noexcept { init_vec_e094e0(s.vec_e094e0); }},
        {0xe085e0, 51, [](StartupState& s) noexcept { init_vec_e085e0(s.vec_e085e0); }},
        {0xe05320, 52, [](StartupState& s) noexcept { init_vec_e05320(s.vec_e05320); }},
        {0xe026b0, 53, [](StartupState& s) noexcept { init_vec_e026b0(s.vec_e026b0); }},
        {0xdfd5f0, 54, [](StartupState& s) noexcept { init_vec_dfd5f0(s.vec_dfd5f0); }},
        {0xdf94c0, 55, [](StartupState& s) noexcept { init_leaf_df94c0(s.leaf_df94c0); }},
        {0xde7fb0, 56, [](StartupState& s) noexcept { init_vec_de7fb0(s.vec_de7fb0); }},
        {0xde6d20, 57, [](StartupState& s) noexcept { init_vec_de6d20(s.vec_de6d20); }},
        {0xde1ad0, 58, [](StartupState& s) noexcept { init_vec_de1ad0(s.vec_de1ad0); }},
        // idx 59 / 0xde0160: memset(buf,1,0x400) via PLT 0xf8f4f0 + zero 16 @ +0x400;
        // __cxa_atexit via PLT 0xf8f390 with ret-stub dtor 0xddf980 is omitted (no-op).
        {0xde0160, 59, [](StartupState& s) noexcept { init_leaf_de0160(s.leaf_de0160); }},
        {0xddef00, 60, [](StartupState& s) noexcept { init_vec_ddef00(s.vec_ddef00); }},
        {0xdddc90, 61, [](StartupState& s) noexcept { init_vec_dddc90(s.vec_dddc90); }},
        {0xddab00, 62, [](StartupState& s) noexcept { init_vec_ddab00(s.vec_ddab00); }},
        {0xdda990, 63, [](StartupState& s) noexcept { init_vec_dda990(s.vec_dda990); }},
        // idx 64–66: already-lifted guarded-constant clones (batch 30) — registered as bookkeeping.
        {0xdd91b0, 64, [](StartupState& s) noexcept { init_guarded_dd91b0(s.guarded_dd91b0); }},
        {0xdd7e30, 65, [](StartupState& s) noexcept { init_guarded_dd7e30(s.guarded_dd7e30); }},
        {0xdd4210, 66, [](StartupState& s) noexcept { init_guarded_dd4210(s.guarded_dd4210); }},
        {0xdd41e0, 67, [](StartupState& s) noexcept { init_vec_dd41e0(s.vec_dd41e0); }},
        {0xdc4180, 68, [](StartupState& s) noexcept { init_vec_dc4180(s.vec_dc4180); }},
        {0xdc2d50, 69, [](StartupState& s) noexcept { init_vec_dc2d50(s.vec_dc2d50); }},
        {0xdbfa60, 70, [](StartupState& s) noexcept { init_vec_dbfa60(s.vec_dbfa60); }},
        {0xdbb280, 71, [](StartupState& s) noexcept { init_vec_dbb280(s.vec_dbb280); }},
        {0xdb93a0, 72, [](StartupState& s) noexcept { init_vec_db93a0(s.vec_db93a0); }},
        {0xdb7e60, 73, [](StartupState& s) noexcept { init_vec_db7e60(s.vec_db7e60); }},
        {0xdb5dd0, 74, [](StartupState& s) noexcept { init_vec_db5dd0(s.vec_db5dd0); }},
        {0xdb3a70, 75, [](StartupState& s) noexcept { init_vec_db3a70(s.vec_db3a70); }},
        {0xdb3960, 76, [](StartupState& s) noexcept { init_vec_db3960(s.vec_db3960); }},
        {0xdac410, 77, [](StartupState& s) noexcept { init_vec_dac410(s.vec_dac410); }},
        {0xd94cb0, 78, [](StartupState& s) noexcept { init_vec_d94cb0(s.vec_d94cb0); }},
        {0xd90310, 79, [](StartupState& s) noexcept { init_vec_d90310(s.vec_d90310); }},
        // idx 80–81: already-lifted AVX constant chains (batch 32) — registered as bookkeeping.
        {0xd8fa40, 80, [](StartupState& s) noexcept { init_avx_d8fa40(s.avx_d8fa40); }},
        {0xd86ed0, 81, [](StartupState& s) noexcept { init_avx_d86ed0(s.avx_d86ed0); }},
        {0xd85490, 82, [](StartupState& s) noexcept { init_vec_d85490(s.vec_d85490); }},
        {0xd853a0, 83, [](StartupState& s) noexcept { init_vec_d853a0(s.vec_d853a0); }},
        {0xd84ee0, 84, [](StartupState& s) noexcept { init_vec_d84ee0(s.vec_d84ee0); }},
        {0xd80e60, 85, [](StartupState& s) noexcept { init_vec_d80e60(s.vec_d80e60); }},
        {0xd7fe90, 86, [](StartupState& s) noexcept { init_vec_d7fe90(s.vec_d7fe90); }},
        {0xd7fe40, 87, [](StartupState& s) noexcept { init_vec_d7fe40(s.vec_d7fe40); }},
        {0xd7e640, 88, [](StartupState& s) noexcept { init_vec_d7e640(s.vec_d7e640); }},
        {0xd7c9d0, 89, [](StartupState& s) noexcept { init_vec_d7c9d0(s.vec_d7c9d0); }},
        {0xd7c4d0, 90, [](StartupState& s) noexcept { init_vec_d7c4d0(s.vec_d7c4d0); }},
        {0xd7c380, 91, [](StartupState& s) noexcept { init_vec_d7c380(s.vec_d7c380); }},
        {0xd7be60, 92, [](StartupState& s) noexcept { init_vec_d7be60(s.vec_d7be60); }},
        {0xd7bc60, 93, [](StartupState& s) noexcept { init_vec_d7bc60(s.vec_d7bc60); }},
        {0xd7b930, 94, [](StartupState& s) noexcept { init_vec_d7b930(s.vec_d7b930); }},
        // idx 95 / 0xd71f60: the last byte-verified leaf_vec_copy module. The original is the standard
        // 17-byte form `vmovups xmm0,[0x1305650]; vmovups [0x33529c0],xmm0; ret`; the native module
        // copies the same {~0,~0,~0,0} pattern into its own cell (CPU-01-CTOR-COVERAGE-BATCH-01).
        {0xd71f60, 95, [](StartupState& s) noexcept { init_vec_d71f60(s.vec_d71f60); }},
        {0xd6e0f0, 96, [](StartupState& s) noexcept { init_vec_d6e0f0(s.vec_d6e0f0); }},
        {0xd6dbf0, 97, [](StartupState& s) noexcept { init_vec_d6dbf0(s.vec_d6dbf0); }},
        {0xd6d890, 98, [](StartupState& s) noexcept { init_vec_d6d890(s.vec_d6d890); }},
        {0xd6d280, 99, [](StartupState& s) noexcept { init_vec_d6d280(s.vec_d6d280); }},
        {0xd6a520, 100, [](StartupState& s) noexcept { init_vec_d6a520(s.vec_d6a520); }},
        {0xd62b00, 101, [](StartupState& s) noexcept { init_vec_d62b00(s.vec_d62b00); }},
        {0xd62690, 102, [](StartupState& s) noexcept { init_vec_d62690(s.vec_d62690); }},
        {0xd623c0, 103, [](StartupState& s) noexcept { init_vec_d623c0(s.vec_d623c0); }},
        {0xd5fbe0, 104, [](StartupState& s) noexcept { init_vec_d5fbe0(s.vec_d5fbe0); }},
        // idx 105: already-lifted AVX identity-table ctor (batch 32).
        {0xd5a280, 105, [](StartupState& s) noexcept { init_avx_d5a280(s.avx_d5a280); }},
        {0xd47880, 106, [](StartupState& s) noexcept { init_vec_d47880(s.vec_d47880); }},
        {0xd47420, 107, [](StartupState& s) noexcept { init_vec_d47420(s.vec_d47420); }},
        {0xd47270, 108, [](StartupState& s) noexcept { init_vec_d47270(s.vec_d47270); }},
        {0xd42b60, 109, [](StartupState& s) noexcept { init_vec_d42b60(s.vec_d42b60); }},
        {0xd40ea0, 110, [](StartupState& s) noexcept { init_vec_d40ea0(s.vec_d40ea0); }},
        {0xd405a0, 111, [](StartupState& s) noexcept { init_vec_d405a0(s.vec_d405a0); }},
        {0xd40010, 112, [](StartupState& s) noexcept { init_vec_d40010(s.vec_d40010); }},
        {0xd3b790, 113, [](StartupState& s) noexcept { init_vec_d3b790(s.vec_d3b790); }},
        {0xd3a6d0, 114, [](StartupState& s) noexcept { init_vec_d3a6d0(s.vec_d3a6d0); }},
        // idx 115–117: already-lifted guarded-constant clones (batch 30) — bookkeeping like 64–66.
        {0xd39b00, 115, [](StartupState& s) noexcept { init_guarded_d39b00(s.guarded_d39b00); }},
        {0xd366d0, 116, [](StartupState& s) noexcept { init_guarded_d366d0(s.guarded_d366d0); }},
        {0xd33b50, 117, [](StartupState& s) noexcept { init_guarded_d33b50(s.guarded_d33b50); }},
        {0xd32b70, 118, [](StartupState& s) noexcept { init_vec_d32b70(s.vec_d32b70); }},
        {0xd32720, 119, [](StartupState& s) noexcept { init_vec_d32720(s.vec_d32720); }},
        {0xd2ffe0, 120, [](StartupState& s) noexcept { init_vec_d2ffe0(s.vec_d2ffe0); }},
        {0xd2cb30, 121, [](StartupState& s) noexcept { init_vec_d2cb30(s.vec_d2cb30); }},
        {0xd2c7e0, 122, [](StartupState& s) noexcept { init_vec_d2c7e0(s.vec_d2c7e0); }},
        {0xd20590, 123, [](StartupState& s) noexcept { init_vec_d20590(s.vec_d20590); }},
        {0xd19bb0, 124, [](StartupState& s) noexcept { init_vec_d19bb0(s.vec_d19bb0); }},
        {0xd19af0, 125, [](StartupState& s) noexcept { init_vec_d19af0(s.vec_d19af0); }},
        {0xd19930, 126, [](StartupState& s) noexcept { init_vec_d19930(s.vec_d19930); }},
        {0xd17c10, 127, [](StartupState& s) noexcept { init_vec_d17c10(s.vec_d17c10); }},
        {0xd13300, 128, [](StartupState& s) noexcept { init_vec_d13300(s.vec_d13300); }},
        {0xd0bde0, 129, [](StartupState& s) noexcept { init_vec_d0bde0(s.vec_d0bde0); }},
        {0xd0bcd0, 130, [](StartupState& s) noexcept { init_vec_d0bcd0(s.vec_d0bcd0); }},
        {0xd0a250, 131, [](StartupState& s) noexcept { init_vec_d0a250(s.vec_d0a250); }},
        {0xd09160, 132, [](StartupState& s) noexcept { init_vec_d09160(s.vec_d09160); }},
        {0xd082e0, 133, [](StartupState& s) noexcept { init_vec_d082e0(s.vec_d082e0); }},
        {0xd08240, 134, [](StartupState& s) noexcept { init_vec_d08240(s.vec_d08240); }},
        {0xd06dd0, 135, [](StartupState& s) noexcept { init_vec_d06dd0(s.vec_d06dd0); }},
        {0xd04ce0, 136, [](StartupState& s) noexcept { init_vec_d04ce0(s.vec_d04ce0); }},
        {0xd00600, 137, [](StartupState& s) noexcept { init_vec_d00600(s.vec_d00600); }},
        {0xcfcc60, 138, [](StartupState& s) noexcept { init_vec_cfcc60(s.vec_cfcc60); }},
        {0xcfcbc0, 139, [](StartupState& s) noexcept { init_vec_cfcbc0(s.vec_cfcbc0); }},
        {0xcf44a0, 140, [](StartupState& s) noexcept { init_vec_cf44a0(s.vec_cf44a0); }},
        {0xcee8a0, 141, [](StartupState& s) noexcept { init_vec_cee8a0(s.vec_cee8a0); }},
        {0xceb890, 142, [](StartupState& s) noexcept { init_vec_ceb890(s.vec_ceb890); }},
        {0xceb2c0, 143, [](StartupState& s) noexcept { init_vec_ceb2c0(s.vec_ceb2c0); }},
        {0xceb1c0, 144, [](StartupState& s) noexcept { init_vec_ceb1c0(s.vec_ceb1c0); }},
        {0xceb070, 145, [](StartupState& s) noexcept { init_vec_ceb070(s.vec_ceb070); }},
        {0xce99c0, 146, [](StartupState& s) noexcept { init_vec_ce99c0(s.vec_ce99c0); }},
        {0xce8440, 147, [](StartupState& s) noexcept { init_vec_ce8440(s.vec_ce8440); }},
        {0xce74a0, 148, [](StartupState& s) noexcept { init_vec_ce74a0(s.vec_ce74a0); }},
        {0xce7300, 149, [](StartupState& s) noexcept { init_vec_ce7300(s.vec_ce7300); }},
        {0xce6fb0, 150, [](StartupState& s) noexcept { init_vec_ce6fb0(s.vec_ce6fb0); }},
        {0xce67b0, 151, [](StartupState& s) noexcept { init_vec_ce67b0(s.vec_ce67b0); }},
        // ---- CPU-01-CTOR-COVERAGE-BATCH-01: every already-lifted vec_*/leaf_* constructor module is
        // now registered at its pinned original index (rows are generated from the module headers and
        // verified against the backward table by tools/lift/check_ctor_slots.py). Modules with runtime
        // inputs (417, 548) take documented zero-initialised native stand-ins. The remaining 265
        // `other`-class slots are unregistered; ctor_original_table.hpp reports the first one (296).
        {0xce6130, 152, [](StartupState& s) noexcept { init_vec_ce6130(s.vec_ce6130); }},
        {0xce57b0, 153, [](StartupState& s) noexcept { init_vec_ce57b0(s.vec_ce57b0); }},
        {0xce38d0, 154, [](StartupState& s) noexcept { init_vec_ce38d0(s.vec_ce38d0); }},
        {0xce0890, 155, [](StartupState& s) noexcept { init_vec_ce0890(s.vec_ce0890); }},
        {0xcde740, 156, [](StartupState& s) noexcept { init_vec_cde740(s.vec_cde740); }},
        {0xcdc760, 157, [](StartupState& s) noexcept { init_vec_cdc760(s.vec_cdc760); }},
        {0xcd4d60, 158, [](StartupState& s) noexcept { init_vec_cd4d60(s.vec_cd4d60); }},
        {0xcd0420, 159, [](StartupState& s) noexcept { init_vec_cd0420(s.vec_cd0420); }},
        {0xcceaa0, 160, [](StartupState& s) noexcept { init_vec_cceaa0(s.vec_cceaa0); }},
        {0xccd0f0, 161, [](StartupState& s) noexcept { init_vec_ccd0f0(s.vec_ccd0f0); }},
        {0xcc8ac0, 162, [](StartupState& s) noexcept { init_vec_cc8ac0(s.vec_cc8ac0); }},
        {0xcc8330, 163, [](StartupState& s) noexcept { init_vec_cc8330(s.vec_cc8330); }},
        {0xcc5a10, 164, [](StartupState& s) noexcept { init_vec_cc5a10(s.vec_cc5a10); }},
        {0xcc07d0, 165, [](StartupState& s) noexcept { init_vec_cc07d0(s.vec_cc07d0); }},
        {0xcbb2c0, 166, [](StartupState& s) noexcept { init_vec_cbb2c0(s.vec_cbb2c0); }},
        {0xcb5cb0, 167, [](StartupState& s) noexcept { init_vec_cb5cb0(s.vec_cb5cb0); }},
        {0xcaeed0, 168, [](StartupState& s) noexcept { init_vec_caeed0(s.vec_caeed0); }},
        {0xcae740, 169, [](StartupState& s) noexcept { init_vec_cae740(s.vec_cae740); }},
        {0xcae240, 170, [](StartupState& s) noexcept { init_vec_cae240(s.vec_cae240); }},
        {0xcad450, 171, [](StartupState& s) noexcept { init_vec_cad450(s.vec_cad450); }},
        {0xcab080, 172, [](StartupState& s) noexcept { init_leaf_cab080(s.leaf_cab080); }},
        {0xca4dc0, 173, [](StartupState& s) noexcept { init_vec_ca4dc0(s.vec_ca4dc0); }},
        {0xca18d0, 174, [](StartupState& s) noexcept { init_leaf_ca18d0(s.leaf_ca18d0); }},
        {0xc9e8e0, 175, [](StartupState& s) noexcept { init_vec_c9e8e0(s.vec_c9e8e0); }},
        {0xc94ae0, 176, [](StartupState& s) noexcept { init_leaf_c94ae0(s.leaf_c94ae0); }},
        {0xc91b10, 177, [](StartupState& s) noexcept { init_vec_c91b10(s.vec_c91b10); }},
        {0xc891e0, 178, [](StartupState& s) noexcept { init_vec_c891e0(s.vec_c891e0); }},
        {0xc811b0, 179, [](StartupState& s) noexcept { init_leaf_c811b0(s.leaf_c811b0); }},
        {0xc7de60, 180, [](StartupState& s) noexcept { init_vec_c7de60(s.vec_c7de60); }},
        {0xc7c310, 181, [](StartupState& s) noexcept { init_vec_c7c310(s.vec_c7c310); }},
        {0xc786c0, 182, [](StartupState& s) noexcept { init_vec_c786c0(s.vec_c786c0); }},
        {0xc77070, 183, [](StartupState& s) noexcept { init_vec_c77070(s.vec_c77070); }},
        {0xc741c0, 184, [](StartupState& s) noexcept { init_vec_c741c0(s.vec_c741c0); }},
        {0xc70e20, 185, [](StartupState& s) noexcept { init_vec_c70e20(s.vec_c70e20); }},
        {0xc6fdf0, 186, [](StartupState& s) noexcept { init_vec_c6fdf0(s.vec_c6fdf0); }},
        {0xc6f520, 187, [](StartupState& s) noexcept { init_leaf_c6f520(s.leaf_c6f520); }},
        {0xc6e9d0, 188, [](StartupState& s) noexcept { init_leaf_c6e9d0(s.leaf_c6e9d0); }},
        {0xc6d910, 189, [](StartupState& s) noexcept { init_vec_c6d910(s.vec_c6d910); }},
        {0xc6bad0, 190, [](StartupState& s) noexcept { init_vec_c6bad0(s.vec_c6bad0); }},
        {0xc69f70, 191, [](StartupState& s) noexcept { init_vec_c69f70(s.vec_c69f70); }},
        {0xc686e0, 192, [](StartupState& s) noexcept { init_vec_c686e0(s.vec_c686e0); }},
        {0xc675f0, 193, [](StartupState& s) noexcept { init_leaf_c675f0(s.leaf_c675f0); }},
        {0xc66580, 194, [](StartupState& s) noexcept { init_vec_c66580(s.vec_c66580); }},
        {0xc65f80, 195, [](StartupState& s) noexcept { init_vec_c65f80(s.vec_c65f80); }},
        {0xc65da0, 196, [](StartupState& s) noexcept { init_vec_c65da0(s.vec_c65da0); }},
        {0xc64060, 197, [](StartupState& s) noexcept { init_vec_c64060(s.vec_c64060); }},
        {0xc61670, 198, [](StartupState& s) noexcept { init_vec_c61670(s.vec_c61670); }},
        {0xc5f6f0, 199, [](StartupState& s) noexcept { init_vec_c5f6f0(s.vec_c5f6f0); }},
        {0xc5c320, 200, [](StartupState& s) noexcept { init_vec_c5c320(s.vec_c5c320); }},
        {0xc5c1a0, 201, [](StartupState& s) noexcept { init_vec_c5c1a0(s.vec_c5c1a0); }},
        {0xc5c040, 202, [](StartupState& s) noexcept { init_vec_c5c040(s.vec_c5c040); }},
        {0xc5a400, 203, [](StartupState& s) noexcept { init_vec_c5a400(s.vec_c5a400); }},
        {0xc5a100, 204, [](StartupState& s) noexcept { init_vec_c5a100(s.vec_c5a100); }},
        {0xc555c0, 205, [](StartupState& s) noexcept { init_vec_c555c0(s.vec_c555c0); }},
        {0xc54d20, 206, [](StartupState& s) noexcept { init_vec_c54d20(s.vec_c54d20); }},
        {0xc54830, 207, [](StartupState& s) noexcept { init_vec_c54830(s.vec_c54830); }},
        {0xc53db0, 208, [](StartupState& s) noexcept { init_vec_c53db0(s.vec_c53db0); }},
        {0xc4e5e0, 209, [](StartupState& s) noexcept { init_vec_c4e5e0(s.vec_c4e5e0); }},
        {0xc4aa80, 210, [](StartupState& s) noexcept { init_vec_c4aa80(s.vec_c4aa80); }},
        {0xc47e00, 211, [](StartupState& s) noexcept { init_vec_c47e00(s.vec_c47e00); }},
        {0xc451d0, 212, [](StartupState& s) noexcept { init_vec_c451d0(s.vec_c451d0); }},
        {0xc44830, 213, [](StartupState& s) noexcept { init_vec_c44830(s.vec_c44830); }},
        {0xc42b00, 214, [](StartupState& s) noexcept { init_vec_c42b00(s.vec_c42b00); }},
        {0xc3eaa0, 215, [](StartupState& s) noexcept { init_vec_c3eaa0(s.vec_c3eaa0); }},
        {0xc3cf00, 216, [](StartupState& s) noexcept { init_vec_c3cf00(s.vec_c3cf00); }},
        {0xc37020, 217, [](StartupState& s) noexcept { init_vec_c37020(s.vec_c37020); }},
        {0xc36a80, 218, [](StartupState& s) noexcept { init_vec_c36a80(s.vec_c36a80); }},
        {0xc23650, 219, [](StartupState& s) noexcept { init_vec_c23650(s.vec_c23650); }},
        {0xc22300, 220, [](StartupState& s) noexcept { init_vec_c22300(s.vec_c22300); }},
        {0xc20710, 221, [](StartupState& s) noexcept { init_vec_c20710(s.vec_c20710); }},
        {0xc1a9a0, 222, [](StartupState& s) noexcept { init_vec_c1a9a0(s.vec_c1a9a0); }},
        {0xc191a0, 223, [](StartupState& s) noexcept { init_vec_c191a0(s.vec_c191a0); }},
        {0xc17a70, 224, [](StartupState& s) noexcept { init_vec_c17a70(s.vec_c17a70); }},
        {0xc16b30, 225, [](StartupState& s) noexcept { init_leaf_c16b30(s.leaf_c16b30); }},
        {0xc152c0, 226, [](StartupState& s) noexcept { init_vec_c152c0(s.vec_c152c0); }},
        {0xc14f60, 227, [](StartupState& s) noexcept { init_vec_c14f60(s.vec_c14f60); }},
        {0xc0d720, 228, [](StartupState& s) noexcept { init_vec_c0d720(s.vec_c0d720); }},
        {0xc0d380, 229, [](StartupState& s) noexcept { init_leaf_c0d380(s.leaf_c0d380); }},
        {0xc0bea0, 230, [](StartupState& s) noexcept { init_vec_c0bea0(s.vec_c0bea0); }},
        {0xc0bbd0, 231, [](StartupState& s) noexcept { init_vec_c0bbd0(s.vec_c0bbd0); }},
        {0xc0bb00, 232, [](StartupState& s) noexcept { init_vec_c0bb00(s.vec_c0bb00); }},
        {0xc0baa0, 233, [](StartupState& s) noexcept { init_vec_c0baa0(s.vec_c0baa0); }},
        {0xc0aae0, 234, [](StartupState& s) noexcept { init_vec_c0aae0(s.vec_c0aae0); }},
        {0xc09070, 235, [](StartupState& s) noexcept { init_vec_c09070(s.vec_c09070); }},
        {0xc060a0, 236, [](StartupState& s) noexcept { init_vec_c060a0(s.vec_c060a0); }},
        {0xc05980, 237, [](StartupState& s) noexcept { init_leaf_c05980(s.leaf_c05980); }},
        {0xbe5410, 238, [](StartupState& s) noexcept { init_vec_be5410(s.vec_be5410); }},
        {0xbe5270, 239, [](StartupState& s) noexcept { init_vec_be5270(s.vec_be5270); }},
        {0xbe50a0, 240, [](StartupState& s) noexcept { init_vec_be50a0(s.vec_be50a0); }},
        {0xbe4ff0, 241, [](StartupState& s) noexcept { init_vec_be4ff0(s.vec_be4ff0); }},
        {0xbd7a80, 242, [](StartupState& s) noexcept { init_vec_bd7a80(s.vec_bd7a80); }},
        {0xbd7950, 243, [](StartupState& s) noexcept { init_vec_bd7950(s.vec_bd7950); }},
        {0xbd60a0, 244, [](StartupState& s) noexcept { init_vec_bd60a0(s.vec_bd60a0); }},
        {0xbd4dd0, 245, [](StartupState& s) noexcept { init_vec_bd4dd0(s.vec_bd4dd0); }},
        {0xbd32f0, 246, [](StartupState& s) noexcept { init_vec_bd32f0(s.vec_bd32f0); }},
        {0xbd3190, 247, [](StartupState& s) noexcept { init_vec_bd3190(s.vec_bd3190); }},
        {0xbd06c0, 248, [](StartupState& s) noexcept { init_vec_bd06c0(s.vec_bd06c0); }},
        {0xbceda0, 249, [](StartupState& s) noexcept { init_vec_bceda0(s.vec_bceda0); }},
        {0xbccc00, 250, [](StartupState& s) noexcept { init_vec_bccc00(s.vec_bccc00); }},
        {0xbcc960, 251, [](StartupState& s) noexcept { init_vec_bcc960(s.vec_bcc960); }},
        {0xbcc4e0, 252, [](StartupState& s) noexcept { init_vec_bcc4e0(s.vec_bcc4e0); }},
        {0xbcc3f0, 253, [](StartupState& s) noexcept { init_vec_bcc3f0(s.vec_bcc3f0); }},
        {0xbc8210, 254, [](StartupState& s) noexcept { init_vec_bc8210(s.vec_bc8210); }},
        {0xbc7280, 255, [](StartupState& s) noexcept { init_vec_bc7280(s.vec_bc7280); }},
        {0xbc5b00, 256, [](StartupState& s) noexcept { init_vec_bc5b00(s.vec_bc5b00); }},
        {0xbc2800, 257, [](StartupState& s) noexcept { init_vec_bc2800(s.vec_bc2800); }},
        {0xbc07f0, 258, [](StartupState& s) noexcept { init_vec_bc07f0(s.vec_bc07f0); }},
        {0xbc0670, 259, [](StartupState& s) noexcept { init_vec_bc0670(s.vec_bc0670); }},
        {0xbbf580, 260, [](StartupState& s) noexcept { init_vec_bbf580(s.vec_bbf580); }},
        {0xbbd5e0, 261, [](StartupState& s) noexcept { init_vec_bbd5e0(s.vec_bbd5e0); }},
        {0xbbba10, 262, [](StartupState& s) noexcept { init_vec_bbba10(s.vec_bbba10); }},
        {0xbb9740, 263, [](StartupState& s) noexcept { init_vec_bb9740(s.vec_bb9740); }},
        {0xbb82d0, 264, [](StartupState& s) noexcept { init_vec_bb82d0(s.vec_bb82d0); }},
        {0xbb7990, 265, [](StartupState& s) noexcept { init_vec_bb7990(s.vec_bb7990); }},
        {0xbacfb0, 266, [](StartupState& s) noexcept { init_vec_bacfb0(s.vec_bacfb0); }},
        {0xbace80, 267, [](StartupState& s) noexcept { init_vec_bace80(s.vec_bace80); }},
        {0xbace50, 268, [](StartupState& s) noexcept { init_vec_bace50(s.vec_bace50); }},
        {0xbacb30, 269, [](StartupState& s) noexcept { init_vec_bacb30(s.vec_bacb30); }},
        {0xbab950, 270, [](StartupState& s) noexcept { init_vec_bab950(s.vec_bab950); }},
        {0xba3ef0, 271, [](StartupState& s) noexcept { init_vec_ba3ef0(s.vec_ba3ef0); }},
        {0xb950e0, 272, [](StartupState& s) noexcept { init_vec_b950e0(s.vec_b950e0); }},
        {0xb8d170, 273, [](StartupState& s) noexcept { init_vec_b8d170(s.vec_b8d170); }},
        {0xb89eb0, 274, [](StartupState& s) noexcept { init_vec_b89eb0(s.vec_b89eb0); }},
        {0xb84580, 275, [](StartupState& s) noexcept { init_vec_b84580(s.vec_b84580); }},
        {0xb7b270, 276, [](StartupState& s) noexcept { init_vec_b7b270(s.vec_b7b270); }},
        {0xb77e90, 277, [](StartupState& s) noexcept { init_vec_b77e90(s.vec_b77e90); }},
        {0xb77910, 278, [](StartupState& s) noexcept { init_vec_b77910(s.vec_b77910); }},
        {0xb77740, 279, [](StartupState& s) noexcept { init_vec_b77740(s.vec_b77740); }},
        {0xb76950, 280, [](StartupState& s) noexcept { init_vec_b76950(s.vec_b76950); }},
        {0xb747f0, 281, [](StartupState& s) noexcept { init_vec_b747f0(s.vec_b747f0); }},
        {0xb743c0, 282, [](StartupState& s) noexcept { init_vec_b743c0(s.vec_b743c0); }},
        {0xb71f80, 283, [](StartupState& s) noexcept { init_vec_b71f80(s.vec_b71f80); }},
        {0xb6e6d0, 284, [](StartupState& s) noexcept { init_vec_b6e6d0(s.vec_b6e6d0); }},
        {0xb692a0, 285, [](StartupState& s) noexcept { init_leaf_b692a0(s.leaf_b692a0); }},
        {0xb67a70, 286, [](StartupState& s) noexcept { init_vec_b67a70(s.vec_b67a70); }},
        {0xb5ecf0, 287, [](StartupState& s) noexcept { init_vec_b5ecf0(s.vec_b5ecf0); }},
        {0xb58690, 288, [](StartupState& s) noexcept { init_vec_b58690(s.vec_b58690); }},
        {0xb57360, 289, [](StartupState& s) noexcept { init_vec_b57360(s.vec_b57360); }},
        {0xb52110, 290, [](StartupState& s) noexcept { init_vec_b52110(s.vec_b52110); }},
        {0xb4ef70, 291, [](StartupState& s) noexcept { init_vec_b4ef70(s.vec_b4ef70); }},
        {0xb494d0, 292, [](StartupState& s) noexcept { init_vec_b494d0(s.vec_b494d0); }},
        {0xb47100, 293, [](StartupState& s) noexcept { init_vec_b47100(s.vec_b47100); }},
        {0xb44580, 294, [](StartupState& s) noexcept { init_vec_b44580(s.vec_b44580); }},
        {0xb33bd0, 295, [](StartupState& s) noexcept { init_vec_b33bd0(s.vec_b33bd0); }},
        // idx 296 / 0xb2b6b0: 12-byte tail thunk `lea rdi,[0x334ac18]; jmp 0x323d0`. Reuses the
        // lifted TSC store. A null/zero frequency hook leaves the documented sentinel 1 and reports
        // the frequency half as partial; the stamp is still written, so this is not a hard failure.
        {0xb2b6b0, 296, [](StartupState& s) noexcept { (void)init_slot296_b2b6b0(s.slot296_stamp); }},
        // idx 297 / 0xb25470: 400 x 0x118 record initializer. Missing/failed native stages are a
        // visible startup failure (the original's calls are unconditional).
        {0xb25470, 297, [](StartupState& s) noexcept {
             const Slot297Result r = init_slot297_b25470(s.slot297_table, default_slot297_hooks());
             if (!r.ok) s.init_failed = true;
         }},
        // idx 298 / 0xb0e250: `shaderTypeToLoad` ranged/enum console variable. Queues into the
        // pending ConCmd list (the manager is installed by the engine boot, after static init). The
        // formerly unresolved indirect call is the bare-ret functor destroy slot 0xb0e240. Any
        // failed stage is a visible failure.
        {0xb0e250, 298, [](StartupState& s) noexcept {
             if (!init_slot298_b0e250(s.slot298_shader_type, s.concmd).ok) s.init_failed = true;
         }},
        // idx 299 / 0xb0c7a0: debug-text scale settings (float var, bool var, two debug-menu items).
        // The menu parent cell is null at startup; any assert branch or parent contract is a
        // visible failure.
        {0xb0c7a0, 299, [](StartupState& s) noexcept {
             if (!init_slot299_b0c7a0(s.slot299, s.concmd, s.debug_menu).ok) s.init_failed = true;
         }},
        // idx 300 / 0xacc880: __cxa_atexit of the bare-ret destructor 0xacc5b0, null argument.
        {0xacc880, 300, [](StartupState& s) noexcept { (void)init_slot300_acc880(s.slot300_cleanup); }},
        // idx 301 / 0xac9550: `rumbleEnabled` bool console variable (shared bool-var shape).
        {0xac9550, 301, [](StartupState& s) noexcept {
             if (!init_slot301_ac9550(s.slot301_rumble, s.concmd).ok) s.init_failed = true;
         }},
        {0xaaeee0, 302, [](StartupState& s) noexcept {
             if (!init_slot302_aaeee0(s.slot302, s.concmd.atexit_log)) s.init_failed = true;
         }},
        // idx 303 / 0xa82170: `ProfileCPU` bool console variable (value/default 0) plus its
        // "Profile CPU" debug-menu toggle. Queues into the same pending list; the menu parent is null.
        {0xa82170, 303, [](StartupState& s) noexcept {
             if (!init_slot303_a82170(s.slot303, s.concmd, s.debug_menu).ok) s.init_failed = true;
         }},
        {0xa816d0, 304, [](StartupState& s) noexcept { init_leaf_a816d0(s.leaf_a816d0); }},
        {0xa77b60, 305, [](StartupState& s) noexcept { init_slot305_a77b60(s.slot305); }},
        // idx 306 / 0xa633b0: the `sndx_` console variables, commands and "Sound" debug menu (46 objects, in
        // table order). Any failed stage stops the fragment and is a visible startup failure.
        {0xa633b0, 306, [](StartupState& s) noexcept {
             s.slot306_result = init_slot306_a633b0(s.slot306, s.concmd, s.debug_menu, s.slot306_hooks);
             if (!s.slot306_result.ok) s.init_failed = true;
         }},
        {0xa5b090, 307, [](StartupState& s) noexcept { if (!init_slot307_a5b090(s.slot307, s.concmd, s.debug_menu).ok) s.init_failed = true; }},
        {0xa58f10, 308, [](StartupState& s) noexcept { if (!init_slot308_a58f10(s.slot308, s.concmd, s.debug_menu).ok) s.init_failed = true; }},
        // idx 309 / 0xa505e0: the "Save System" debug block. The three console variables go through
        // the shared 0xa4f4d0 family model (default_slot309_hooks), the bool variable through
        // init_bool_var_static, and the four toggles through 0x26ea0 + 0x268b0. Any failed stage is a
        // visible startup failure.
        {0xa505e0, 309, [](StartupState& s) noexcept {
             const Slot309Hooks hooks = default_slot309_hooks();
             s.slot309_result = init_slot309_a505e0(s.slot309, s.concmd, s.debug_menu, hooks);
             if (!s.slot309_result.ok) s.init_failed = true;
         }},
        {0xa479d0, 310, [](StartupState& s) noexcept { if (!init_slot310_a479d0(s.slot310, s.concmd, s.debug_menu).ok) s.init_failed = true; }},
        {0xa46b70, 311, [](StartupState& s) noexcept { if (!init_tiny_atexit_slot(311, s.tiny_atexit, &s.concmd.atexit_log).ok) s.init_failed = true; }},
        // idx 312 / 0xa46130: the "Platform Services" / "TitleInfo" entitlement block -- the `titleskuid`
        // string variable, `titleAgeRestriction` and `titleRegion` int variables, a menu group, three
        // toggles and one 0x80 B record. The two int variables go through the native wrapper+leaf model
        // (default_slot312_hooks); any failed stage is a visible startup failure.
        {0xa46130, 312, [](StartupState& s) noexcept {
             const Slot312Hooks hooks = default_slot312_hooks();
             s.slot312_result = init_slot312_a46130(s.slot312, s.concmd, s.debug_menu, hooks);
             if (!s.slot312_result.ok) s.init_failed = true;
         }},
        {0xa447a0, 313, [](StartupState& s) noexcept { if (!init_slot313_a447a0(s.slot313, s.concmd, s.debug_menu).ok) s.init_failed = true; }},
        // idx 314 / 0xa41810: the physics debug block -- one "PhysXToRenderThread" page, ten console
        // variables (two inline bools, three `0xc300` floats, five `physX_*` callback bools) and ten
        // "Physics" toggles, 22 __cxa_atexit records. The page ctor (0x475c0) and the callback bool ctor
        // (0xa41570) run through the native models (default_slot314_hooks); any failed stage is a
        // visible startup failure.
        {0xa41810, 314, [](StartupState& s) noexcept {
             const Slot314Hooks hooks = default_slot314_hooks();
             s.slot314_result = init_slot314_a41810(s.slot314, s.concmd, s.debug_menu, hooks);
             if (!s.slot314_result.ok) s.init_failed = true;
         }},
        {0xa3ade0, 315, [](StartupState& s) noexcept { store_zero32(s.slot315_cell); }},
        // idx 316 / 0xa33280: the Localization block -- four language console variables (`loc_text` /
        // `loc_subtitles` / `loc_speech` / `loc_system`) through the native wrapper+leaf model
        // (default_slot316_hooks), the `sLanguageUnlocalizedAsEnglish` bool, the `loc_reload` command,
        // six "Localization" toggles and the "!!!Unlocalized!!!" string, 14 __cxa_atexit records. The
        // 0xf1a0 allocator is the documented host stand-in; any failed stage is a visible startup failure.
        {0xa33280, 316, [](StartupState& s) noexcept {
             const Slot316Hooks hooks = default_slot316_hooks();
             s.slot316_result = init_slot316_a33280(s.slot316, s.concmd, s.debug_menu, hooks);
             if (!s.slot316_result.ok) s.init_failed = true;
         }},
        {0xa2c390, 317, [](StartupState& s) noexcept { if (!init_slot317_a2c390(s.slot317, s.concmd, s.debug_menu).ok) s.init_failed = true; }},
        {0xa20e80, 318, [](StartupState& s) noexcept { if (!init_slot318_a20e80(s.slot318, s.concmd).ok) s.init_failed = true; }},
        {0x9b75a0, 319, [](StartupState& s) noexcept { init_leaf_9b75a0(s.leaf_9b75a0); }},
        {0x8f3d90, 320, [](StartupState& s) noexcept { init_leaf_8f3d90(s.leaf_8f3d90); }},
        {0x8a0440, 321, [](StartupState& s) noexcept { init_leaf_8a0440(s.leaf_8a0440); }},
        {0x849e30, 322, [](StartupState& s) noexcept { if (!init_slot322_849e30(s.slot322, s.concmd, s.debug_menu).ok) s.init_failed = true; }},
        {0x842450, 323, [](StartupState& s) noexcept {
             constexpr std::array<const char*, 3> names{"BowShotsHit", "BowShotAccuracy", "FurthestArrowDistance"};
             init_ctor_842450(s.slot323_cells, names);
         }},
        {0x83d800, 324, [](StartupState& s) noexcept { init_leaf_83d800(s.leaf_83d800); }},
        {0x839f00, 325, [](StartupState& s) noexcept { if (!init_slot325_839f00(s.slot325, s.concmd, s.debug_menu).ok) s.init_failed = true; }},
        // idx 326 / 0x837f30: the Colossus / Shadow eye-colour debug page -- two console bools, one
        // threshold float, twelve eye-colour floats ("Colossus" group with three toggles, "Shadow Eye
        // Color" group with a kind-0 "MenuSeparator" row and three sub-groups of four sliders), 36
        // __cxa_atexit records. Every callee runs through a native model; the only stand-in is the
        // 0xf1a0 allocator of the label row, and any failed stage is a visible startup failure.
        {0x837f30, 326, [](StartupState& s) noexcept {
             const Slot326Hooks hooks = default_slot326_hooks();
             s.slot326_result = init_slot326_837f30(s.slot326, s.slot326_eye_values, s.concmd,
                                                    s.debug_menu, hooks);
             if (!s.slot326_result.ok) s.init_failed = true;
         }},
        {0x836330, 327, [](StartupState& s) noexcept { if (!init_slot327_836330(s.slot327, s.concmd, s.debug_menu).ok) s.init_failed = true; }},
        {0x82e040, 328, [](StartupState& s) noexcept { if (!init_slot328_82e040(s.slot328, s.concmd).ok) s.init_failed = true; }},
        // idx 329 / 0x829e90: the Shadow Path / path-finding debug page -- seven console bools, two
        // integer range variables (built by the block's OWN leaf 0x8298d0 and wrapper 0x829b30 +
        // leaf 0x829cf0), one float range, a "Shadow"/"Shadow Path" group, ten toggles and one
        // separator row, 22 __cxa_atexit records. Every callee runs through a native model; the only
        // stand-in is the 0xf1a0 allocator of the separator row, and any failed stage is a visible
        // startup failure.
        {0x829e90, 329, [](StartupState& s) noexcept {
             const Slot329Hooks hooks = default_slot329_hooks();
             s.slot329_result = init_slot329_829e90(s.slot329, s.concmd, s.debug_menu, hooks);
             if (!s.slot329_result.ok) s.init_failed = true;
         }},
        {0x810a50, 330, [](StartupState& s) noexcept { if (!init_slot330_810a50(s.slot330, s.concmd).ok) s.init_failed = true; }},
        {0x809640, 331, [](StartupState& s) noexcept { if (!init_slot331_809640(s.slot331, s.concmd, s.debug_menu).ok) s.init_failed = true; }},
        {0x805440, 332, [](StartupState& s) noexcept { if (!init_slot332_805440(s.slot332, s.concmd, s.debug_menu).ok) s.init_failed = true; }},
        {0x7fd6d0, 333, [](StartupState& s) noexcept { if (!init_slot333_7fd6d0(s.slot333, s.concmd).ok) s.init_failed = true; }},
        {0x7fc530, 334, [](StartupState& s) noexcept { if (!init_slot334_7fc530(s.slot334, s.concmd, s.debug_menu).ok) s.init_failed = true; }},
        // idx 335 / 0x7edf10: the "Shadow Cheat" assist page -- four unbounded Easy-Mode float
        // multipliers, three cheat bools, two FNV-1a name records for the health/grip bars, and two
        // bounded float sliders under the "Shadow Cheat" group, 11 __cxa_atexit records. Every callee
        // runs through a native model (0x6aa00, 0x20af0, 0x20d60, 0x27050, 0x268b0 and this block's
        // own 0x7edd80 / 0xf2f0 / 0x7dbca0 / 0x52a990); no allocator stand-in is needed and any failed
        // stage is a visible startup failure.
        {0x7edf10, 335, [](StartupState& s) noexcept {
             s.slot335_result = init_slot335_7edf10(s.slot335, s.slot335_cells, s.concmd,
                                                    s.debug_menu, default_slot335_hooks());
             if (!s.slot335_result.ok) s.init_failed = true;
         }},
        // idx 336 / 0x7ec4c0: the attract-mode debug page -- an enum-style ranged int (local 0x7ec160), a bool, a ranged int, the
        // rand() cell, the "CP10 / AttractMode" group with three toggles, a name holder, an unbounded float and a retained
        // 0x14d880 handle cell; default_slot336_hooks() = std::rand + the native getter / add-ref (cleared, the cell fails visibly).
        {0x7ec4c0, 336, [](StartupState& s) noexcept {
             s.slot336_result = init_slot336_7ec4c0(s.slot336, s.concmd, s.debug_menu, default_slot336_hooks());
             if (!s.slot336_result.ok) s.init_failed = true;
         }},
        {0x7e81d0, 337, [](StartupState& s) noexcept { s.slot337_result = init_slot337_7e81d0(s.slot337, s.concmd); if (!s.slot337_result.ok) s.init_failed = true; }},
        {0x7e6c90, 338, [](StartupState& s) noexcept { s.slot338_result = init_slot338_7e6c90(s.slot338, s.concmd); if (!s.slot338_result.ok) s.init_failed = true; }},
        {0x7e2fc0, 339, [](StartupState& s) noexcept { s.slot339_result = init_slot339_7e2fc0(s.slot339, s.concmd); if (!s.slot339_result.ok) s.init_failed = true; }},
        {0x7de7e0, 340, [](StartupState& s) noexcept { s.slot340_result = init_slot340_7de7e0(s.slot340, s.concmd, s.debug_menu); if (!s.slot340_result.ok) s.init_failed = true; }},
        {0x7dace0, 341, [](StartupState& s) noexcept {
             constexpr std::array<const char*, 2> names{"InEndCutscene", "TimeResistingTheLight"};
             init_ctor_7dace0(s.slot341_cells, names);
         }},
        {0x7d8e10, 342, [](StartupState& s) noexcept { s.slot342_result = init_slot342_7d8e10(s.slot342, s.concmd); if (!s.slot342_result.ok) s.init_failed = true; }},
        {0x7d7970, 343, [](StartupState& s) noexcept { s.slot343_result = init_slot343_7d7970(s.slot343, s.concmd); if (!s.slot343_result.ok) s.init_failed = true; }},
        {0x7d3810, 344, [](StartupState& s) noexcept { s.slot344_result = init_slot344_7d3810(s.slot344, s.concmd, s.debug_menu); if (!s.slot344_result.ok) s.init_failed = true; }},
        {0x7cec60, 345, [](StartupState& s) noexcept { s.slot345_result = init_slot345_7cec60(s.slot345, s.concmd, s.debug_menu); if (!s.slot345_result.ok) s.init_failed = true; }},
        {0x7ce8d0, 346, [](StartupState& s) noexcept { init_leaf_7ce8d0(s.leaf_7ce8d0); }},
        {0x7c93a0, 347, [](StartupState& s) noexcept { s.slot347_result = init_slot347_7c93a0(s.slot347, s.concmd); if (!s.slot347_result.ok) s.init_failed = true; }},
        {0x7c9150, 348, [](StartupState& s) noexcept { s.slot348_result = init_slot348_7c9150(s.slot348, s.concmd); if (!s.slot348_result.ok) s.init_failed = true; }},
        {0x7bde50, 349, [](StartupState& s) noexcept { s.slot349_result = init_slot349_7bde50(s.slot349, s.concmd, s.debug_menu); if (!s.slot349_result.ok) s.init_failed = true; }},
        {0x7af0b0, 350, [](StartupState& s) noexcept { s.slot350_result = init_slot350_7af0b0(s.slot350, s.concmd); if (!s.slot350_result.ok) s.init_failed = true; }},
        {0x7ae220, 351, [](StartupState& s) noexcept { s.slot351_result = init_slot351_7ae220(s.slot351, s.concmd, s.debug_menu); if (!s.slot351_result.ok) s.init_failed = true; }},
        {0x7a2390, 352, [](StartupState& s) noexcept { s.slot352_result = init_slot352_7a2390(s.slot352, s.concmd); if (!s.slot352_result.ok) s.init_failed = true; }},
        {0x7a14d0, 353, [](StartupState& s) noexcept { s.slot353_result = init_slot353_7a14d0(s.slot353, s.concmd); if (!s.slot353_result.ok) s.init_failed = true; }},
        {0x79e180, 354, [](StartupState& s) noexcept { s.slot354_result = init_slot354_79e180(s.slot354, s.concmd, s.debug_menu); if (!s.slot354_result.ok) s.init_failed = true; }},
        {0x7946d0, 355, [](StartupState& s) noexcept { s.slot355_result = init_slot355_7946d0(s.slot355, s.concmd, s.debug_menu); if (!s.slot355_result.ok) s.init_failed = true; }},
        {0x78e2f0, 356, [](StartupState& s) noexcept { s.slot356_result = init_slot356_78e2f0(s.slot356, s.concmd); if (!s.slot356_result.ok) s.init_failed = true; }},
        {0x78c760, 357, [](StartupState& s) noexcept { init_ctor_78c760(s.slot357_cell); }},
        {0x786f50, 358, [](StartupState& s) noexcept { s.slot358_result = init_slot358_786f50(s.slot358, s.concmd, s.debug_menu); if (!s.slot358_result.ok) s.init_failed = true; }},
        {0x785f60, 359, [](StartupState& s) noexcept { s.slot359_result = init_slot359_785f60(s.slot359, s.concmd); if (!s.slot359_result.ok) s.init_failed = true; }},
        // CPU-01-STARTUP-BATCH-DEBUG-PAGE-01: data rows of the cluster model core/debug_page_var_item
        // (one or two bounded-float console variables through 0xc300, plus optionally one 0x27050
        // slider / 0x26ea0 toggle item). Every callee is a native model, so no hook has to be
        // supplied and no unimplemented native body is on the production path.
        {0x7806f0, 360, [](StartupState& s) noexcept { s.slot360_result = init_slot360_7806f0(s.slot360, s.concmd, s.debug_menu); if (!s.slot360_result.ok) s.init_failed = true; }},
        {0x777390, 361, [](StartupState& s) noexcept {
             s.slot361_result = init_slot361_777390(s.slot361, s.concmd, s.debug_menu);
             if (!s.slot361_result.ok) s.init_failed = true;
         }},
        {0x76b8d0, 362, [](StartupState& s) noexcept { s.slot362_result = init_slot362_76b8d0(s.slot362, s.concmd); if (!s.slot362_result.ok) s.init_failed = true; }},
        {0x751c70, 363, [](StartupState& s) noexcept { s.slot363_result = init_slot363_751c70(s.slot363, s.concmd); if (!s.slot363_result.ok) s.init_failed = true; }},
        {0x74a8e0, 364, [](StartupState& s) noexcept { s.slot364_result = init_slot364_74a8e0(s.slot364, s.concmd, s.debug_menu); if (!s.slot364_result.ok) s.init_failed = true; }},
        {0x747870, 365, [](StartupState& s) noexcept { s.slot365_result = init_slot365_747870(s.slot365, s.concmd); if (!s.slot365_result.ok) s.init_failed = true; }},
        {0x744ac0, 366, [](StartupState& s) noexcept { s.slot366_result = init_slot366_744ac0(s.slot366, s.concmd, s.debug_menu); if (!s.slot366_result.ok) s.init_failed = true; }},
        {0x740980, 367, [](StartupState& s) noexcept { s.slot367_result = init_slot367_740980(s.slot367, s.concmd); if (!s.slot367_result.ok) s.init_failed = true; }},
        {0x73df50, 368, [](StartupState& s) noexcept { s.slot368_result = init_slot368_73df50(s.slot368, s.concmd, s.debug_menu); if (!s.slot368_result.ok) s.init_failed = true; }},
        {0x734190, 369, [](StartupState& s) noexcept { s.slot369_result = init_slot369_734190(s.slot369, s.concmd, s.debug_menu); if (!s.slot369_result.ok) s.init_failed = true; }},
        {0x720b50, 370, [](StartupState& s) noexcept { s.slot370_result = init_slot370_720b50(s.slot370, s.concmd, s.debug_menu); if (!s.slot370_result.ok) s.init_failed = true; }},
        {0x70cdc0, 371, [](StartupState& s) noexcept { if (!init_release_family_slot(371, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x70a040, 372, [](StartupState& s) noexcept { if (!init_release_family_slot(372, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x7081e0, 373, [](StartupState& s) noexcept { s.slot373_result = init_slot373_7081e0(s.slot373, s.concmd, s.debug_menu); if (!s.slot373_result.ok) s.init_failed = true; }},
        {0x701230, 374, [](StartupState& s) noexcept {
             s.slot374_result = init_slot374_701230(s.slot374, s.concmd, s.debug_menu);
             if (!s.slot374_result.ok) s.init_failed = true;
         }},
        {0x6f1c20, 375, [](StartupState& s) noexcept { s.slot375_result = init_slot375_6f1c20(s.slot375, s.concmd, s.debug_menu); if (!s.slot375_result.ok) s.init_failed = true; }},
        {0x6e4930, 376, [](StartupState& s) noexcept { s.slot376_result = init_slot376_6e4930(s.slot376, s.concmd); if (!s.slot376_result.ok) s.init_failed = true; }},
        {0x6de050, 377, [](StartupState& s) noexcept { s.slot377_result = init_slot377_6de050(s.slot377, s.concmd); if (!s.slot377_result.ok) s.init_failed = true; }},
        {0x6dde20, 378, [](StartupState& s) noexcept {
             constexpr std::array<const char*, 3> names{"InSaveVolume", "LastInSaveVolume", "FacingSaveVolume"};
             init_ctor_6dde20(s.slot378_cells, names);
         }},
        {0x6d8d20, 379, [](StartupState& s) noexcept {
             s.slot379_result = init_slot379_6d8d20(s.slot379, s.concmd, s.debug_menu);
             if (!s.slot379_result.ok) s.init_failed = true;
         }},
        {0x6d71f0, 380, [](StartupState& s) noexcept { s.slot380_result = init_slot380_6d71f0(s.slot380, s.concmd, s.debug_menu); if (!s.slot380_result.ok) s.init_failed = true; }},
        {0x6cd5b0, 381, [](StartupState& s) noexcept { s.slot381_result = init_slot381_6cd5b0(s.slot381, s.concmd); if (!s.slot381_result.ok) s.init_failed = true; }},
        {0x6c8600, 382, [](StartupState& s) noexcept { s.slot382_result = init_slot382_6c8600(s.slot382, s.concmd, s.debug_menu); if (!s.slot382_result.ok) s.init_failed = true; }},
        {0x6c62b0, 383, [](StartupState& s) noexcept { s.slot383_result = init_slot383_6c62b0(s.slot383, s.concmd, s.debug_menu); if (!s.slot383_result.ok) s.init_failed = true; }},
        {0x6b9260, 384, [](StartupState& s) noexcept { s.slot384_result = init_slot384_6b9260(s.slot384, s.concmd); if (!s.slot384_result.ok) s.init_failed = true; }},
        {0x6b4cf0, 385, [](StartupState& s) noexcept { s.slot385_result = init_slot385_6b4cf0(s.slot385, s.concmd); if (!s.slot385_result.ok) s.init_failed = true; }},
        {0x6b12f0, 386, [](StartupState& s) noexcept { s.slot386_result = init_slot386_6b12f0(s.slot386, s.concmd); if (!s.slot386_result.ok) s.init_failed = true; }},
        {0x6acf00, 387, [](StartupState& s) noexcept { s.slot387_result = init_slot387_6acf00(s.slot387, s.concmd); if (!s.slot387_result.ok) s.init_failed = true; }},
        {0x6a8c60, 388, [](StartupState& s) noexcept { s.slot388_result = init_slot388_6a8c60(s.slot388, s.concmd, s.debug_menu); if (!s.slot388_result.ok) s.init_failed = true; }},
        {0x695a50, 389, [](StartupState& s) noexcept { s.slot389_result = init_slot389_695a50(s.slot389, s.concmd, s.debug_menu); if (!s.slot389_result.ok) s.init_failed = true; }},
        {0x691c50, 390, [](StartupState& s) noexcept { s.slot390.tree_alloc = default_empty_tree_alloc(); s.slot390_result = init_slot390_691c50(s.slot390, s.concmd); if (!s.slot390_result.ok) s.init_failed = true; }},
        {0x68ebc0, 391, [](StartupState& s) noexcept { s.slot391_result = init_slot391_68ebc0(s.slot391, s.concmd, s.debug_menu); if (!s.slot391_result.ok) s.init_failed = true; }},
        {0x681690, 392, [](StartupState& s) noexcept { s.slot392_result = init_slot392_681690(s.slot392, s.concmd, s.debug_menu); if (!s.slot392_result.ok) s.init_failed = true; }},
        {0x66cc00, 393, [](StartupState& s) noexcept { s.slot393_result = init_slot393_66cc00(s.slot393, s.concmd); if (!s.slot393_result.ok) s.init_failed = true; }},
        {0x669450, 394, [](StartupState& s) noexcept {
             constexpr std::array<const char*, 1> names{"IsCutsceneRunning"};
             init_ctor_669450(s.slot394_cells, names);
         }},
        {0x65af60, 395, [](StartupState& s) noexcept { s.slot395_result = init_slot395_65af60(s.slot395, s.concmd, s.debug_menu); if (!s.slot395_result.ok) s.init_failed = true; }},
        {0x650f20, 396, [](StartupState& s) noexcept { s.slot396_result = init_slot396_650f20(s.slot396, s.concmd, s.debug_menu); if (!s.slot396_result.ok) s.init_failed = true; }},
        {0x64d760, 397, [](StartupState& s) noexcept { s.slot397_result = init_slot397_64d760(s.slot397, s.concmd); if (!s.slot397_result.ok) s.init_failed = true; }},
        {0x63cf60, 398, [](StartupState& s) noexcept { s.slot398_result = init_slot398_63cf60(s.slot398, s.concmd, s.debug_menu); if (!s.slot398_result.ok) s.init_failed = true; }},
        {0x632ca0, 399, [](StartupState& s) noexcept { s.slot399_result = init_slot399_632ca0(s.slot399, s.concmd, s.debug_menu); if (!s.slot399_result.ok) s.init_failed = true; }},
        {0x62b100, 400, [](StartupState& s) noexcept { s.slot400_result = init_slot400_62b100(s.slot400, s.concmd, s.debug_menu); if (!s.slot400_result.ok) s.init_failed = true; }},
        {0x621530, 401, [](StartupState& s) noexcept { s.slot401_result = init_slot401_621530(s.slot401, s.concmd, s.debug_menu); if (!s.slot401_result.ok) s.init_failed = true; }},
        {0x610430, 402, [](StartupState& s) noexcept { s.slot402_result = init_slot402_610430(s.slot402, s.concmd, s.debug_menu); if (!s.slot402_result.ok) s.init_failed = true; }},
        {0x5f0160, 403, [](StartupState& s) noexcept { s.slot403_result = init_slot403_5f0160(s.slot403, s.concmd, s.debug_menu); if (!s.slot403_result.ok) s.init_failed = true; }},
        {0x5e7c10, 404, [](StartupState& s) noexcept { init_slot404_5e7c10(s.slot404); }},
        {0x5e7790, 405, [](StartupState& s) noexcept { if (!init_release_family_slot(405, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x5dbe10, 406, [](StartupState& s) noexcept { if (!init_slot406_5dbe10(s.slot406, s.concmd.atexit_log).ok) s.init_failed = true; }},
        {0x5d41c0, 407, [](StartupState& s) noexcept { s.slot407_result = init_slot407_5d41c0(s.slot407, s.concmd); if (!s.slot407_result.ok) s.init_failed = true; }},
        {0x5c7f50, 408, [](StartupState& s) noexcept { s.slot408.perspective_tanf = [](float x) -> float { return std::tan(x); }; s.slot408_result = init_slot408_5c7f50(s.slot408, s.concmd); if (!s.slot408_result.ok) s.init_failed = true; }},
        {0x5b50f0, 409, [](StartupState& s) noexcept { if (!init_slot409_5b50f0(s.slot409, default_slot409_hooks(), s.concmd.atexit_log).ok) s.init_failed = true; }},
        {0x5ac580, 410, [](StartupState& s) noexcept { s.slot410_result = init_slot410_5ac580(s.slot410, s.concmd); if (!s.slot410_result.ok) s.init_failed = true; }},
        {0x591710, 411, [](StartupState& s) noexcept { s.slot411_result = init_slot411_591710(s.slot411, s.concmd, s.debug_menu); if (!s.slot411_result.ok) s.init_failed = true; }},
        {0x58bd10, 412, [](StartupState& s) noexcept { zero_xmm_qword(s.slot412_xmm, s.slot412_q); }},
        {0x57c810, 413, [](StartupState& s) noexcept { s.slot413_result = init_slot413_57c810(s.slot413, s.concmd); if (!s.slot413_result.ok) s.init_failed = true; }},
        {0x57b9b0, 414, [](StartupState& s) noexcept { if (!init_slot414_57b9b0(s.slot414, default_critsec_slot414_hooks(), s.concmd.atexit_log).ok) s.init_failed = true; }},
        {0x578b60, 415, [](StartupState& s) noexcept { s.slot415_result = init_slot415_578b60(s.slot415, s.concmd); if (!s.slot415_result.ok) s.init_failed = true; }},
        {0x5777a0, 416, [](StartupState& s) noexcept { zero_xmm16(s.slot416_xmm); }},
        {0x575d40, 417, [](StartupState& s) noexcept { init_leaf_575d40(s.leaf_575d40_source, s.leaf_575d40); }},
        {0x572a10, 418, [](StartupState& s) noexcept { s.slot418_result = init_slot418_572a10(s.slot418, s.concmd); if (!s.slot418_result.ok) s.init_failed = true; }},
        {0x56fae0, 419, [](StartupState& s) noexcept { s.slot419_result = init_slot419_56fae0(s.slot419); if (!s.slot419_result.ok) s.init_failed = true; }},
        // idx 420 / 0x56f6e0: the sound-occlusion debug rows -- bool + ranged int + "Sound" separator / toggle / slider. The
        // separator needs the 0xf1a0 allocator stand-in (default_slot420_hooks, malloc/free); cleared, the fragment fails visibly.
        {0x56f6e0, 420, [](StartupState& s) noexcept {
             s.slot420_result = init_slot420_56f6e0(s.slot420, s.concmd, s.debug_menu, default_slot420_hooks());
             if (!s.slot420_result.ok) s.init_failed = true;
         }},
        {0x56da20, 421, [](StartupState& s) noexcept { s.slot421_result = init_slot421_56da20(s.slot421, s.concmd); if (!s.slot421_result.ok) s.init_failed = true; }},
        {0x56b980, 422, [](StartupState& s) noexcept { s.slot422_result = init_slot422_56b980(s.slot422, s.concmd); if (!s.slot422_result.ok) s.init_failed = true; }},
        {0x55f080, 423, [](StartupState& s) noexcept { s.slot423_result = init_slot423_55f080(s.slot423, s.concmd); if (!s.slot423_result.ok) s.init_failed = true; }},
        {0x551140, 424, [](StartupState& s) noexcept { s.slot424_result = init_slot424_551140(s.slot424, s.concmd); if (!s.slot424_result.ok) s.init_failed = true; }},
        {0x54b9c0, 425, [](StartupState& s) noexcept { if (!init_tiny_atexit_slot(425, s.tiny_atexit, &s.concmd.atexit_log).ok) s.init_failed = true; }},
        {0x53fb50, 426, [](StartupState& s) noexcept { s.slot426_result = init_slot426_53fb50(s.slot426, s.concmd); if (!s.slot426_result.ok) s.init_failed = true; }},
        {0x5388f0, 427, [](StartupState& s) noexcept { s.slot427.record_registry = &s.record_registry; s.slot427.record_alloc = default_slot427_record_alloc(); s.slot427_result = init_slot427_5388f0(s.slot427, s.concmd, s.debug_menu); if (!s.slot427_result.ok) s.init_failed = true; }},
        {0x52ec10, 428, [](StartupState& s) noexcept { s.slot428.alloc = default_slot428_alloc(); s.slot428.name_dealloc = default_slot428_name_dealloc(); s.slot428_result = init_slot428_52ec10(s.slot428, s.concmd); if (!s.slot428_result.ok) s.init_failed = true; }},
        {0x52e2b0, 429, [](StartupState& s) noexcept { s.slot429.external_target_0 = &s.global_2dfcbd0_standin; s.slot429_result = init_slot429_52e2b0(s.slot429, s.concmd, s.debug_menu); if (!s.slot429_result.ok) s.init_failed = true; }},
        {0x529df0, 430, [](StartupState& s) noexcept { s.slot430_result = init_slot430_529df0(s.slot430, s.concmd, s.debug_menu); if (!s.slot430_result.ok) s.init_failed = true; }},
        {0x523a00, 431, [](StartupState& s) noexcept { if (!init_release_family_slot(431, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x51bc20, 432, [](StartupState& s) noexcept { if (!init_tiny_atexit_slot(432, s.tiny_atexit, &s.concmd.atexit_log).ok) s.init_failed = true; }},
        {0x517850, 433, [](StartupState& s) noexcept { if (!init_release_family_slot(433, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x511c00, 434, [](StartupState& s) noexcept { if (!init_release_family_slot(434, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x510210, 435, [](StartupState& s) noexcept { if (!init_release_family_slot(435, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x50dbe0, 436, [](StartupState& s) noexcept { s.slot436_result = init_slot436_50dbe0(s.slot436, s.concmd, s.debug_menu); if (!s.slot436_result.ok) s.init_failed = true; }},
        {0x5070c0, 437, [](StartupState& s) noexcept { s.slot437_result = init_slot437_5070c0(s.slot437, s.concmd); if (!s.slot437_result.ok) s.init_failed = true; }},
        {0x4fbd80, 438, [](StartupState& s) noexcept { if (!init_release_family_slot(438, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x4f7b70, 439, [](StartupState& s) noexcept { s.slot439_result = init_slot439_4f7b70(s.slot439, s.concmd, s.debug_menu); if (!s.slot439_result.ok) s.init_failed = true; }},
        {0x4f0c60, 440, [](StartupState& s) noexcept { s.slot440_result = init_slot440_4f0c60(s.slot440, s.concmd); if (!s.slot440_result.ok) s.init_failed = true; }},
        {0x4e68c0, 441, [](StartupState& s) noexcept { s.slot441_result = init_slot441_4e68c0(s.slot441, s.concmd, s.debug_menu); if (!s.slot441_result.ok) s.init_failed = true; }},
        {0x4e3760, 442, [](StartupState& s) noexcept { s.slot442_result = init_slot442_4e3760(s.slot442, s.concmd); if (!s.slot442_result.ok) s.init_failed = true; }},
        {0x4de370, 443, [](StartupState& s) noexcept { s.slot443_result = init_slot443_4de370(s.slot443, s.concmd, s.debug_menu); if (!s.slot443_result.ok) s.init_failed = true; }},
        {0x4d3f10, 444, [](StartupState& s) noexcept { s.slot444_result = init_slot444_4d3f10(s.slot444, s.concmd); if (!s.slot444_result.ok) s.init_failed = true; }},
        {0x4cfe80, 445, [](StartupState& s) noexcept { s.slot445_result = init_slot445_4cfe80(s.slot445, s.concmd); if (!s.slot445_result.ok) s.init_failed = true; }},
        {0x4c92a0, 446, [](StartupState& s) noexcept { s.slot446_result = init_slot446_4c92a0(s.slot446, s.concmd, s.debug_menu); if (!s.slot446_result.ok) s.init_failed = true; }},
        {0x4c3410, 447, [](StartupState& s) noexcept { if (!init_release_family_slot(447, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x4b8680, 448, [](StartupState& s) noexcept { s.slot448_result = init_slot448_4b8680(s.slot448, s.concmd); if (!s.slot448_result.ok) s.init_failed = true; }},
        {0x4b0200, 449, [](StartupState& s) noexcept { s.slot449_result = init_slot449_4b0200(s.slot449, s.concmd, s.debug_menu); if (!s.slot449_result.ok) s.init_failed = true; }},
        {0x4af4b0, 450, [](StartupState& s) noexcept { s.slot450_result = init_slot450_4af4b0(s.slot450, s.concmd); if (!s.slot450_result.ok) s.init_failed = true; }},
        {0x49d230, 451, [](StartupState& s) noexcept { s.slot451_result = init_slot451_49d230(s.slot451, s.concmd); if (!s.slot451_result.ok) s.init_failed = true; }},
        {0x498640, 452, [](StartupState& s) noexcept { s.slot452_result = init_slot452_498640(s.slot452, s.concmd, s.debug_menu); if (!s.slot452_result.ok) s.init_failed = true; }},
        {0x48ac10, 453, [](StartupState& s) noexcept { s.slot453.record_registry = &s.record_registry; s.slot453.record_alloc = default_slot453_record_alloc(); s.slot453_result = init_slot453_48ac10(s.slot453, s.concmd); if (!s.slot453_result.ok) s.init_failed = true; }},
        {0x483e40, 454, [](StartupState& s) noexcept { s.slot454_result = init_slot454_483e40(s.slot454, s.concmd); if (!s.slot454_result.ok) s.init_failed = true; }},
        {0x47bfa0, 455, [](StartupState& s) noexcept { s.slot455_result = init_slot455_47bfa0(s.slot455, s.concmd, s.debug_menu); if (!s.slot455_result.ok) s.init_failed = true; }},
        {0x471950, 456, [](StartupState& s) noexcept { s.slot456_result = init_slot456_471950(s.slot456, s.concmd); if (!s.slot456_result.ok) s.init_failed = true; }},
        {0x45c440, 457, [](StartupState& s) noexcept { s.slot457_result = init_slot457_45c440(s.slot457, s.concmd); if (!s.slot457_result.ok) s.init_failed = true; }},
        {0x45ace0, 458, [](StartupState& s) noexcept { s.slot458_result = init_slot458_45ace0(s.slot458, s.concmd); if (!s.slot458_result.ok) s.init_failed = true; }},
        {0x43f460, 459, [](StartupState& s) noexcept { s.slot459_result = init_slot459_43f460(s.slot459, s.concmd); if (!s.slot459_result.ok) s.init_failed = true; }},
        {0x41c340, 460, [](StartupState& s) noexcept { s.slot460_result = init_slot460_41c340(s.slot460, s.concmd); if (!s.slot460_result.ok) s.init_failed = true; }},
        {0x419480, 461, [](StartupState& s) noexcept { s.slot461_result = init_slot461_419480(s.slot461, s.concmd); if (!s.slot461_result.ok) s.init_failed = true; }},
        {0x3e2730, 462, [](StartupState& s) noexcept { s.slot462_result = init_slot462_3e2730(s.slot462, s.concmd, s.debug_menu); if (!s.slot462_result.ok) s.init_failed = true; }},
        {0x3e0040, 463, [](StartupState& s) noexcept { s.slot463_result = init_slot463_3e0040(s.slot463, s.concmd, s.debug_menu); if (!s.slot463_result.ok) s.init_failed = true; }},
        {0x3db070, 464, [](StartupState& s) noexcept { s.slot464.record_registry = &s.record_registry; s.slot464.record_alloc = default_slot464_record_alloc(); s.slot464_result = init_slot464_3db070(s.slot464, s.concmd); if (!s.slot464_result.ok) s.init_failed = true; }},
        {0x3cc9e0, 465, [](StartupState& s) noexcept { s.slot465_result = init_slot465_3cc9e0(s.slot465, s.concmd); if (!s.slot465_result.ok) s.init_failed = true; }},
        {0x3ca7c0, 466, [](StartupState& s) noexcept { init_leaf_3ca7c0(s.leaf_3ca7c0); }},
        {0x3ca110, 467, [](StartupState& s) noexcept {
             s.slot467_result = init_slot467_3ca110(s.slot467, s.concmd, s.debug_menu);
             if (!s.slot467_result.ok) s.init_failed = true;
         }},
        {0x3c3a90, 468, [](StartupState& s) noexcept { s.slot468_result = init_slot468_3c3a90(s.slot468, s.concmd, s.app_info_queue); if (!s.slot468_result.ok) s.init_failed = true; }},
        {0x394a20, 469, [](StartupState& s) noexcept { s.slot469.external_target_0 = &s.slot518.var; s.slot469_result = init_slot469_394a20(s.slot469, s.concmd, s.debug_menu); if (!s.slot469_result.ok) s.init_failed = true; }},
        {0x389a00, 470, [](StartupState& s) noexcept { if (!init_release_family_slot(470, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x3600f0, 471, [](StartupState& s) noexcept { if (!init_tiny_atexit_slot(471, s.tiny_atexit, &s.concmd.atexit_log).ok) s.init_failed = true; }},
        {0x3583c0, 472, [](StartupState& s) noexcept { if (!init_release_family_slot(472, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x355a00, 473, [](StartupState& s) noexcept { s.slot473.separator_alloc = default_family_separator_alloc(); s.slot473_result = init_slot473_355a00(s.slot473, s.concmd, s.debug_menu); if (!s.slot473_result.ok) s.init_failed = true; }},
        {0x344850, 474, [](StartupState& s) noexcept { s.slot474_result = init_slot474_344850(s.slot474, s.concmd); if (!s.slot474_result.ok) s.init_failed = true; }},
        {0x33cc60, 475, [](StartupState& s) noexcept { s.slot475_result = init_slot475_33cc60(s.slot475, s.concmd); if (!s.slot475_result.ok) s.init_failed = true; }},
        {0x329650, 476, [](StartupState& s) noexcept { s.slot476_result = init_slot476_329650(s.slot476, s.concmd); if (!s.slot476_result.ok) s.init_failed = true; }},
        {0x3270d0, 477, [](StartupState& s) noexcept { s.slot477_result = init_slot477_3270d0(s.slot477, s.concmd); if (!s.slot477_result.ok) s.init_failed = true; }},
        {0x31eb00, 478, [](StartupState& s) noexcept { s.slot478_result = init_slot478_31eb00(s.slot478, s.concmd); if (!s.slot478_result.ok) s.init_failed = true; }},
        {0x31b660, 479, [](StartupState& s) noexcept { s.slot479_result = init_slot479_31b660(s.slot479, s.concmd); if (!s.slot479_result.ok) s.init_failed = true; }},
        {0x30daf0, 480, [](StartupState& s) noexcept { s.slot480_result = init_slot480_30daf0(s.slot480, s.concmd); if (!s.slot480_result.ok) s.init_failed = true; }},
        {0x2fd8f0, 481, [](StartupState& s) noexcept { if (!init_release_family_slot(481, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x2fb2a0, 482, [](StartupState& s) noexcept { s.slot482_result = init_slot482_2fb2a0(s.slot482, s.concmd); if (!s.slot482_result.ok) s.init_failed = true; }},
        {0x2ef810, 483, [](StartupState& s) noexcept { if (!init_release_family_slot(483, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x2eee10, 484, [](StartupState& s) noexcept { if (!init_release_family_slot(484, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x2ee5e0, 485, [](StartupState& s) noexcept { if (!init_release_family_slot(485, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x2d5e00, 486, [](StartupState& s) noexcept { s.slot486_result = init_slot486_2d5e00(s.slot486, s.concmd); if (!s.slot486_result.ok) s.init_failed = true; }},
        {0x2cd060, 487, [](StartupState& s) noexcept { if (!init_release_family_slot(487, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x2ba5d0, 488, [](StartupState& s) noexcept { s.slot488_result = init_slot488_2ba5d0(s.slot488, s.concmd); if (!s.slot488_result.ok) s.init_failed = true; }},
        {0x2acc10, 489, [](StartupState& s) noexcept { s.slot489_result = init_slot489_2acc10(s.slot489, s.concmd); if (!s.slot489_result.ok) s.init_failed = true; }},
        {0x296d90, 490, [](StartupState& s) noexcept { s.slot490_result = init_slot490_296d90(s.slot490, s.concmd); if (!s.slot490_result.ok) s.init_failed = true; }},
        {0x295f90, 491, [](StartupState& s) noexcept { s.slot491_ok = init_slot491_295f90(s.slot491, s.concmd.atexit_log); if (!s.slot491_ok) s.init_failed = true; }},
        {0x278640, 492, [](StartupState& s) noexcept { s.slot492_result = init_slot492_278640(s.slot492, s.app_info_queue, s.concmd); if (!s.slot492_result.ok) s.init_failed = true; }},
        {0x277560, 493, [](StartupState& s) noexcept { s.slot493_result = init_slot493_277560(s.slot493, s.concmd, s.debug_menu); if (!s.slot493_result.ok) s.init_failed = true; }},
        {0x26f7c0, 494, [](StartupState& s) noexcept { s.slot494.hooks = slot494_default_pool_hooks(); s.slot494_result = init_slot494_pools_26f7c0(s.slot494, s.concmd); if (!s.slot494_result.ok) s.init_failed = true; }},
        {0x268ee0, 495, [](StartupState& s) noexcept { s.slot495_result = init_slot495_268ee0(s.slot495, s.concmd); if (!s.slot495_result.ok) s.init_failed = true; }},
        {0x268ce0, 496, [](StartupState& s) noexcept { init_ctor_268ce0(s.slot496_cell); }},
        // CPU-01-STARTUP-SLOT-497: 24 variables, 19 menu items, 7 records, 3 entries.
        {0x262a80, 497, [](StartupState& s) noexcept {
             s.slot497_result = init_slot497_262a80(s.slot497, s.concmd, s.debug_menu, s.record_registry, default_slot497_hooks());
             if (!s.slot497_result.ok) s.init_failed = true;
         }},
        {0x252ae0, 498, [](StartupState& s) noexcept { s.slot498.separator_alloc = default_family_separator_alloc(); s.slot498_result = init_slot498_252ae0(s.slot498, s.concmd, s.debug_menu); if (!s.slot498_result.ok) s.init_failed = true; }},
        {0x2500e0, 499, [](StartupState& s) noexcept { s.slot499_result = init_slot499_2500e0(s.slot499, s.concmd); if (!s.slot499_result.ok) s.init_failed = true; }},
        {0x24ebf0, 500, [](StartupState& s) noexcept { zero_struct12(s.slot500_cell); }},
        {0x24db80, 501, [](StartupState& s) noexcept { s.slot501.separator_alloc = default_family_separator_alloc(); s.slot501_result = init_slot501_24db80(s.slot501, s.concmd, s.debug_menu); if (!s.slot501_result.ok) s.init_failed = true; }},
        {0x249270, 502, [](StartupState& s) noexcept { s.slot502_result = init_slot502_249270(s.slot502, s.concmd); if (!s.slot502_result.ok) s.init_failed = true; }},
        {0x247f00, 503, [](StartupState& s) noexcept { s.slot503_result = init_slot503_247f00(s.slot503, s.concmd, s.debug_menu); if (!s.slot503_result.ok) s.init_failed = true; }},
        {0x23f4d0, 504, [](StartupState& s) noexcept { s.slot504_result = init_slot504_23f4d0(s.slot504, s.concmd); if (!s.slot504_result.ok) s.init_failed = true; }},
        // CPU-01-STARTUP-SLOT-505: 2,704-byte host table -> 226 vec3s -> zero-seeded bounds.
        {0x236030, 505, [](StartupState& s) noexcept { s.slot505_result = init_slot505_236030(s.slot505, s.slot505_source.data()); if (!s.slot505_result.ok) s.init_failed = true; }},
        {0x2340b0, 506, [](StartupState& s) noexcept { s.slot506_result = init_slot506_2340b0(s.slot506, s.concmd, s.debug_menu); if (!s.slot506_result.ok) s.init_failed = true; }},
        {0x22f3c0, 507, [](StartupState& s) noexcept { s.slot507_result = init_slot507_22f3c0(s.slot507, s.concmd); if (!s.slot507_result.ok) s.init_failed = true; }},
        {0x2241e0, 508, [](StartupState& s) noexcept { s.slot508_result = init_slot508_2241e0(s.slot508, s.concmd); if (!s.slot508_result.ok) s.init_failed = true; }},
        {0x2196f0, 509, [](StartupState& s) noexcept { s.slot509_result = init_slot509_2196f0(s.slot509, s.concmd, s.debug_menu); if (!s.slot509_result.ok) s.init_failed = true; }},
        // CPU-01-STARTUP-SLOT-510: property cell, bool var + toggle, pose constants, seven-token command.
        {0x211340, 510, [](StartupState& s) noexcept {
             s.slot510_result = init_slot510_211340(s.slot510, s.slot510_tokens, s.concmd, s.debug_menu, default_slot510_hooks());
             if (!s.slot510_result.ok) s.init_failed = true;
         }},
        {0x1d90c0, 511, [](StartupState& s) noexcept { s.slot511.tree_alloc = default_empty_tree_alloc(); s.slot511_result = init_slot511_1d90c0(s.slot511, s.concmd, s.debug_menu); if (!s.slot511_result.ok) s.init_failed = true; }},
        {0x1d5e80, 512, [](StartupState& s) noexcept { s.slot512_result = init_slot512_1d5e80(s.slot512, s.concmd, s.debug_menu); if (!s.slot512_result.ok) s.init_failed = true; }},
        {0x1d0770, 513, [](StartupState& s) noexcept { s.slot513.record_registry = &s.record_registry; s.slot513.record_alloc = default_slot513_record_alloc(); s.slot513_result = init_slot513_1d0770(s.slot513, s.concmd, s.debug_menu); if (!s.slot513_result.ok) s.init_failed = true; }},
        {0x1c4c50, 514, [](StartupState& s) noexcept { s.slot514_result = init_slot514_1c4c50(s.slot514, s.concmd, s.debug_menu); if (!s.slot514_result.ok) s.init_failed = true; }},
        {0x1c00e0, 515, [](StartupState& s) noexcept { if (!init_tiny_atexit_slot(515, s.tiny_atexit, &s.concmd.atexit_log).ok) s.init_failed = true; }},
        {0x1b97b0, 516, [](StartupState& s) noexcept { s.slot516_result = init_slot516_1b97b0(s.slot516, s.concmd); if (!s.slot516_result.ok) s.init_failed = true; }},
        {0x1aa510, 517, [](StartupState& s) noexcept { if (!init_slot517_1aa510(s.slot517, s.concmd.atexit_log).ok) s.init_failed = true; }},
        {0x1a57d0, 518, [](StartupState& s) noexcept {
             s.slot518_result = init_slot518_1a57d0(s.slot518, s.concmd, s.debug_menu);
             if (!s.slot518_result.ok) s.init_failed = true;
         }},
        {0x19d2d0, 519, [](StartupState& s) noexcept { if (!init_slot519_19d2d0(s.slot519, s.concmd.atexit_log).ok) s.init_failed = true; }},
        {0x196c90, 520, [](StartupState& s) noexcept { clear_flag32(s.flag32); }},
        {0x17bf50, 521, [](StartupState& s) noexcept { s.slot521_result = init_slot521_17bf50(s.slot521, s.concmd); if (!s.slot521_result.ok) s.init_failed = true; }},
        {0x16e960, 522, [](StartupState& s) noexcept { if (!init_tiny_atexit_slot(522, s.tiny_atexit, &s.concmd.atexit_log).ok) s.init_failed = true; }},
        {0x16cd50, 523, [](StartupState& s) noexcept { if (!init_tiny_atexit_slot(523, s.tiny_atexit, &s.concmd.atexit_log).ok) s.init_failed = true; }},
        {0x16a810, 524, [](StartupState& s) noexcept { s.slot524_result = init_slot524_16a810(s.slot524, s.concmd); if (!s.slot524_result.ok) s.init_failed = true; }},
        {0x166ef0, 525, [](StartupState& s) noexcept { if (!init_tiny_atexit_slot(525, s.tiny_atexit, &s.concmd.atexit_log).ok) s.init_failed = true; }},
        {0x163a40, 526, [](StartupState& s) noexcept { init_leaf_163a40(s.leaf_163a40); }},
        {0x157e60, 527, [](StartupState& s) noexcept { if (!init_tiny_atexit_slot(527, s.tiny_atexit, &s.concmd.atexit_log).ok) s.init_failed = true; }},
        {0x156fb0, 528, [](StartupState& s) noexcept { if (!init_tiny_atexit_slot(528, s.tiny_atexit, &s.concmd.atexit_log).ok) s.init_failed = true; }},
        {0x14c030, 529, [](StartupState& s) noexcept { if (!init_release_family_slot(529, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x149f00, 530, [](StartupState& s) noexcept { s.slot530_result = init_slot530_149f00(s.slot530, s.concmd, s.debug_menu); if (!s.slot530_result.ok) s.init_failed = true; }},
        {0x1442c0, 531, [](StartupState& s) noexcept { if (!init_tiny_atexit_slot(531, s.tiny_atexit, &s.concmd.atexit_log).ok) s.init_failed = true; }},
        {0x143f50, 532, [](StartupState& s) noexcept { s.slot532_result = init_slot532_143f50(s.slot532, s.concmd.atexit_log, kSlot532Texts); if (!s.slot532_result.ok) s.init_failed = true; }},
        {0x143300, 533, [](StartupState& s) noexcept { if (!init_tiny_atexit_slot(533, s.tiny_atexit, &s.concmd.atexit_log).ok) s.init_failed = true; }},
        {0x1418d0, 534, [](StartupState& s) noexcept { s.slot534_result = init_slot534_1418d0(s.slot534, s.concmd); if (!s.slot534_result.ok) s.init_failed = true; }},
        {0x138e20, 535, [](StartupState& s) noexcept { s.slot535_result = init_slot535_138e20(s.slot535, s.concmd.atexit_log); if (!s.slot535_result.ok) s.init_failed = true; }},
        {0x1342d0, 536, [](StartupState& s) noexcept {
             s.slot536_result = init_slot536_1342d0(s.slot536, s.concmd, s.debug_menu);
             if (!s.slot536_result.ok) s.init_failed = true;
         }},
        {0x1324c0, 537, [](StartupState& s) noexcept { s.slot537_result = init_slot537_1324c0(s.slot537, s.concmd.atexit_log, kSlot537InputBits); if (!s.slot537_result.ok) s.init_failed = true; }},
        {0x125990, 538, [](StartupState& s) noexcept { reset_scratch(s.scratch, s.scratch_slot); }},
        {0x121940, 539, [](StartupState& s) noexcept { s.slot539_result = init_slot539_121940(s.slot539, s.concmd); if (!s.slot539_result.ok) s.init_failed = true; }},
        {0x10e390, 540, [](StartupState& s) noexcept { s.slot540_result = init_slot540_10e390(s.slot540, s.concmd.atexit_log); if (!s.slot540_result.ok) s.init_failed = true; }},
        {0xfd210, 541, [](StartupState& s) noexcept { init_slot541_fd210(s.slot541); }},
        {0xfb3d0, 542, [](StartupState& s) noexcept { if (!init_tiny_atexit_slot(542, s.tiny_atexit, &s.concmd.atexit_log).ok) s.init_failed = true; }},
        {0xfae50, 543, [](StartupState& s) noexcept { s.slot543_result = init_slot543_fae50(s.slot543, s.app_info_queue, s.concmd); if (!s.slot543_result.ok) s.init_failed = true; }},
        {0xd1b20, 544, [](StartupState& s) noexcept { s.slot544_result = init_slot544_d1b20(s.slot544); if (!s.slot544_result.ok) s.init_failed = true; }},
        // The stack-cookie compare of the original has no native counterpart (a native function has no
        // cookie), so both guard words are the same local: the compare is satisfied by construction and the
        // fixture test, not this row, is what exercises the mismatch branch.
        {0xb2ec0, 545, [](StartupState& s) noexcept {
             constexpr std::uint64_t kNoCookie = 0;
             s.slot545_result = init_slot545_b2ec0(s.slot545, &s.concmd.atexit_log, kNoCookie, kNoCookie);
             if (!s.slot545_result.ok) s.init_failed = true;
         }},
        {0xa25f0, 546, [](StartupState& s) noexcept { if (!init_tiny_atexit_slot(546, s.tiny_atexit, &s.concmd.atexit_log).ok) s.init_failed = true; }},
        {0x9a970, 547, [](StartupState& s) noexcept { if (!init_tiny_atexit_slot(547, s.tiny_atexit, &s.concmd.atexit_log).ok) s.init_failed = true; }},
        {0x99350, 548, [](StartupState& s) noexcept { init_leaf_99350(s.leaf_99350, s.leaf_99350_inputs); }},
        {0x96650, 549, [](StartupState& s) noexcept { if (!init_tiny_atexit_slot(549, s.tiny_atexit, &s.concmd.atexit_log).ok) s.init_failed = true; }},
        {0x91290, 550, [](StartupState& s) noexcept { s.slot550_result = init_slot550_91290(s.slot550, s.concmd, s.debug_menu); if (!s.slot550_result.ok) s.init_failed = true; }},
        {0x8d740, 551, [](StartupState& s) noexcept { if (!init_tiny_atexit_slot(551, s.tiny_atexit, &s.concmd.atexit_log).ok) s.init_failed = true; }},
        // CPU-01-STARTUP-SLOT-552: 7 flag dwords, 17 zeroed records, 8 page objects (6 x 0x2e0 +
        // 0xf0 + 0x98) and 8 __cxa_atexit records.
        {0x8c880, 552, [](StartupState& s) noexcept {
             s.slot552_result = init_slot552_8c880(s.slot552, s.concmd, default_slot552_hooks());
             if (!s.slot552_result.ok) s.init_failed = true;
         }},
        {0x8bd40, 553, [](StartupState& s) noexcept { if (!init_slot553_8bd40(s.slot553, s.concmd.atexit_log).ok) s.init_failed = true; }},
        // CPU-01-STARTUP-SLOT-554-BATCH-01: page header + eight variables (0x2ea8ee8..0x2eaa010).
        {0x6efe0, 554, [](StartupState& s) noexcept {
             s.slot554_result = init_slot554_batch1(s.slot554, s.concmd, default_slot554_hooks());
             if (!s.slot554_result.ok) { s.init_failed = true; return; }
             s.slot554_batch2a_result = init_slot554_batch2a(
                 s.slot554_batch2a, s.slot554.page.data(), s.concmd, default_slot554_batch2a_hooks());
             if (!s.slot554_batch2a_result.ok) s.init_failed = true;
             s.slot554_batch2b_result = init_slot554_batch2b(
                 s.slot554_batch2b, s.slot554.page.data(), s.concmd, default_slot554_batch2b_hooks());
             if (!s.slot554_batch2b_result.ok) s.init_failed = true;
             s.slot554_batch2b_cont_result = init_slot554_b2b_cont(
                 s.slot554_batch2b_cont, s.slot554.page.data(), s.concmd,
                 default_slot554_b2b_cont_hooks());
             if (!s.slot554_batch2b_cont_result.ok) s.init_failed = true;
             s.slot554_batch3_result = init_slot554_batch3(
                 s.slot554_batch3, s.slot554.page.data(), s.concmd,
                 default_slot554_batch3_hooks());
             if (!s.slot554_batch3_result.ok) s.init_failed = true;
             s.slot554_batch4_result = init_slot554_batch4(
                 s.slot554_batch4, s.slot554.page.data(), s.concmd,
                 default_slot554_batch4_hooks());
             if (!s.slot554_batch4_result.ok) s.init_failed = true;
             s.slot554_batch5_result = init_slot554_batch5(
                 s.slot554_batch5, s.slot554.page.data(), s.concmd,
                 default_slot554_batch5_hooks());
             if (!s.slot554_batch5_result.ok) s.init_failed = true;
             s.slot554_batch6_result = init_slot554_batch6(
                 s.slot554_batch6, s.slot554.page.data(), s.concmd,
                 default_slot554_batch6_hooks());
             if (!s.slot554_batch6_result.ok) s.init_failed = true;
             s.slot554_batch7_result = init_slot554_batch7(
                 s.slot554_batch7, s.slot554.page.data(), s.concmd,
                 default_slot554_batch7_hooks());
             if (!s.slot554_batch7_result.ok) s.init_failed = true;
             s.slot554_batch8_result = init_slot554_batch8(
                 s.slot554_batch8, s.slot554.page.data(), s.concmd,
                 default_slot554_batch8_hooks());
             if (!s.slot554_batch8_result.ok) s.init_failed = true;
             s.slot554_batch9_result = init_slot554_batch9(
                 s.slot554_batch9, s.slot554.page.data(), s.concmd,
                 default_slot554_batch9_hooks());
             if (!s.slot554_batch9_result.ok) s.init_failed = true;
             s.slot554_batch10_result = init_slot554_batch10(
                 s.slot554_batch10, s.slot554.page.data(), s.concmd,
                 default_slot554_batch10_hooks());
             if (!s.slot554_batch10_result.ok) s.init_failed = true;
             s.slot554_batch11_result = init_slot554_batch11(
                 s.slot554_batch11, s.slot554.page.data(), s.concmd,
                 default_slot554_batch11_hooks());
             if (!s.slot554_batch11_result.ok) s.init_failed = true;
             s.slot554_batch12_result = init_slot554_batch12(
                 s.slot554_batch12, s.slot554.page.data(), s.concmd,
                 default_slot554_batch12_hooks());
             if (!s.slot554_batch12_result.ok) s.init_failed = true;
             s.slot554_batch13_result = init_slot554_batch13(
                 s.slot554_batch13, s.slot554.page.data(), s.concmd,
                 default_slot554_batch13_hooks());
             if (!s.slot554_batch13_result.ok) s.init_failed = true;
         }},
        {0x527f0, 555, [](StartupState& s) noexcept { s.slot555_result = init_slot555_527f0(s.slot555, s.concmd); if (!s.slot555_result.ok) s.init_failed = true; }},
        {0x4ccb0, 556, [](StartupState& s) noexcept { s.slot556_result = init_slot556_4ccb0(s.slot556, s.app_info_queue, s.concmd); if (!s.slot556_result.ok) s.init_failed = true; }},
        {0x47490, 557, [](StartupState& s) noexcept { s.slot557_result = init_slot557_47490(s.slot557, s.concmd); if (!s.slot557_result.ok) s.init_failed = true; }},
        {0x428a0, 558, [](StartupState& s) noexcept { s.slot558_result = init_slot558_428a0(s.slot558, s.concmd); if (!s.slot558_result.ok) s.init_failed = true; }},
        {0x3f2d0, 559, [](StartupState& s) noexcept { s.slot559_result = init_slot559_3f2d0(s.slot559, s.concmd); if (!s.slot559_result.ok) s.init_failed = true; }},
        {0x3bd70, 560, [](StartupState& s) noexcept { clear_flag64(s.slot560_cell); }},
        {0x3ac00, 561, [](StartupState& s) noexcept { s.slot561_result = init_slot561_3ac00(s.slot561, s.concmd, s.debug_menu); if (!s.slot561_result.ok) s.init_failed = true; }},
        {0x36100, 562, [](StartupState& s) noexcept { if (!init_slot562_36100(s.slot562, s.concmd.atexit_log).ok) s.init_failed = true; }},
        {0x32c70, 563, [](StartupState& s) noexcept { init_slot563_32c70(s.slot563); }},
        {0x31220, 564, [](StartupState& s) noexcept { if (!init_tiny_atexit_slot(564, s.tiny_atexit, &s.concmd.atexit_log).ok) s.init_failed = true; }},
        {0x2fcf0, 565, [](StartupState& s) noexcept { if (!init_tiny_atexit_slot(565, s.tiny_atexit, &s.concmd.atexit_log).ok) s.init_failed = true; }},
        {0x25de0, 566, [](StartupState& s) noexcept { s.slot566_result = init_slot566_25de0(s.slot566, s.concmd, s.debug_menu); if (!s.slot566_result.ok) s.init_failed = true; }},
        // CPU-01-STARTUP-SLOT-567: zero the ConCmdManager cell 0x2dfcbc8, build four 0xa0 console
        // commands and record five __cxa_atexit entries.
        {0x235d0, 567, [](StartupState& s) noexcept {
             s.slot567_result = init_slot567_235d0(s.slot567, s.concmd, default_slot567_hooks());
             if (!s.slot567_result.ok) s.init_failed = true;
         }},
        {0x207e0, 568, [](StartupState& s) noexcept { s.slot568_result = init_slot568_207e0(s.slot568, s.concmd); if (!s.slot568_result.ok) s.init_failed = true; }},
        {0x1cc80, 569, [](StartupState& s) noexcept { s.slot569_result = init_slot569_1cc80(s.slot569, s.concmd, s.debug_menu); if (!s.slot569_result.ok) s.init_failed = true; }},
        {0x16d80, 570, [](StartupState& s) noexcept { s.slot570_result = init_slot570_16d80(s.slot570, s.concmd, s.debug_menu); if (!s.slot570_result.ok) s.init_failed = true; }},
        {0xc780, 571, [](StartupState& s) noexcept { s.slot571.external_target_0 = &s.global_2dfcbd0_standin; s.slot571_result = init_slot571_c780(s.slot571, s.concmd, s.debug_menu); if (!s.slot571_result.ok) s.init_failed = true; }},
        {0x1a20, 572, [](StartupState& s) noexcept { s.slot572.external_target_0 = &s.global_2dfcbd0_standin; s.slot572_result = init_slot572_1a20(s.slot572, s.concmd, s.debug_menu); if (!s.slot572_result.ok) s.init_failed = true; }},

    };
    return kSlots;
}

// Convenience view of indices 24-47 (also present in recovered_constructors after Phase B).
// Focused ahead tests may run this span alone without executing 0-23.
std::span<const CtorSlot> registered_ahead_constructors() noexcept {
    static constexpr CtorSlot kAheadSlots[] = {
        {0xe56b70, 24, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 24); }},
        {0xe55f10, 25, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 25); }},
        {0xe55240, 26, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 26); }},
        {0xe54a30, 27, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 27); }},
        {0xe53a10, 28, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 28); }},
        {0xe52a20, 29, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 29); }},
        {0xe52150, 30, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 30); }},
        {0xe518e0, 31, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 31); }},
        {0xe510e0, 32, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 32); }},
        {0xe50980, 33, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 33); }},
        {0xe500b0, 34, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 34); }},
        {0xe4f840, 35, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 35); }},
        {0xe4f040, 36, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 36); }},
        {0xe4e8e0, 37, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 37); }},
        {0xe4e010, 38, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 38); }},
        {0xe4d7a0, 39, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 39); }},
        {0xe4cfa0, 40, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 40); }},
        {0xe4c840, 41, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 41); }},
        {0xe4bf10, 42, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 42); }},
        {0xe4b7b0, 43, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 43); }},
        {0xe4afb0, 44, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 44); }},
        {0xe47670, 45, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 45); }},
        {0xe467e0, 46, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 46); }},
        {0xe459e0, 47, [](StartupState& s) noexcept { (void)init_registration_thunk_ahead(s, 47); }},
    };
    return kAheadSlots;
}

}  // namespace sotc
