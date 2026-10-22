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
        // Gap idx 95 (0xd71f60) — NEEDS_PRINCIPAL.
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
        {0xb743c0, 282, [](StartupState& s) noexcept { init_vec_b743c0(s.vec_b743c0); }},
        {0xb52110, 290, [](StartupState& s) noexcept { init_vec_b52110(s.vec_b52110); }},
        {0x196c90, 520, [](StartupState& s) noexcept { clear_flag32(s.flag32); }},
        {0x125990, 538, [](StartupState& s) noexcept { reset_scratch(s.scratch, s.scratch_slot); }},
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
