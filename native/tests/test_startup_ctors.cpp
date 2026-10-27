#include <cstdio>
#include <cstdlib>

#include "core/startup_ctors.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main() {
    using namespace sotc;
    {
        StartupState st;
        st.scratch.fill(std::byte{0x77});
        st.scratch_slot = 5;
        st.flag32 = 0xA5A5A5A5u;
        auto r = run_constructors(recovered_constructors(), st);
        // Slot 23 is recovered via private name fixture; runner completes every registered row.
        CHECK(r.ok() && r.completed == recovered_constructors().size() && r.blocked_on_rva == 0);
        CHECK(!r.callback_failed);
        CHECK(st.descriptor == &kClothingPreviewParamDescriptor);
        CHECK(st.descriptor_mutex_initialized && !st.init_failed);
        CHECK(st.slot23_names.loaded);
        CHECK(st.leaf_e71990.cells[8] == static_cast<std::uint8_t>(kLeafE71990Hash[0] & 0xff));
        for (std::size_t i = 0; i < kAheadRegistrationCount; ++i) {
            CHECK(st.ahead_registrations[i].published());
            CHECK(st.ahead_registrations[i].descriptor() == &kAheadRegistrationDescriptors[i]);
        }
        CHECK(st.descriptor->object_size == ClothingPreviewParam::kObjectSize);
        // Slot 11 published its descriptor and mutex; the lazy schema stays unavailable.
        CHECK(st.physmesh_registration.published());
        CHECK(st.physmesh_registration.descriptor() == &kClothingPhysicalMeshParametersDescriptor);
        CHECK(!st.physmesh_tree.built());
        CHECK(st.physmesh_tree.get().error == ParamTreeError::schema_not_available);
        // Slots 12-19 each published their own registration (see test_registration_thunks).
        for (std::size_t i = 0; i < kRegistrationThunkCount; ++i) {
            CHECK(st.thunk_registrations[i].published());
            CHECK(st.thunk_registrations[i].descriptor() == &kRegistrationThunkDescriptors[i]);
        }
        // Slot 21 ran too: the first constant-table cell is zero and the second is 0xffffffff00000000 (bytes 8..15).
        CHECK(st.const_table_ee1130[0] == 0 && st.const_table_ee1130[12] == 0xff && st.const_table_ee1130[8] == 0);

        // Preserve leaf-fixture coverage independently of the fail-fast runner boundary.
        for (const CtorSlot& slot : recovered_constructors())
            if (slot.original_index > 10 && slot.run != nullptr)
                slot.run(st);
        CHECK(st.trig.cos_3_degrees > 0.9986f);
        CHECK(st.leaf_df94c0.cos_3_degrees > 0.9986f);
        CHECK(st.leaf_de0160.fill[0] == std::byte{1} &&
              st.leaf_de0160.fill[sotc::kLeafDe0160FillBytes - 1] == std::byte{1} &&
              st.leaf_de0160.trailer[0] == std::byte{0} &&
              st.leaf_de0160.trailer[15] == std::byte{0});
        CHECK(st.neg036.guard == 1 && st.neg036.lanes[0] == kNeg036Bits);
        CHECK(st.xyz_mask_copy == kXyzMaskBits && st.xyz_mask_b == kXyzMaskBits);
        CHECK(st.f4fd40.ones0 == ~0ull && st.f4fd40.epsilon_x4[0] == kEpsilonBits);
        CHECK(st.f4eba0.ones == ~0ull && st.f4eba0.q1 == 0xffffffff00000000ull);
        CHECK(st.f4b540[0] == 0x00 && st.f4b540[3] == 0x80 && st.f4b540.size() == 240);
        CHECK(st.vec_ef85e0 == kVecEf85e0Bits);
        CHECK(st.ea52f0[0] == 0xff && st.ea52f0[16] == 0x0c && st.ea52f0.size() == 184);
        CHECK(st.vec_e0b180 == kVecE0b180Bits);
        CHECK(st.vec_e085e0 == kVecE085e0Bits);
        CHECK(st.vec_e05320 == kVecE05320Bits);
        CHECK(st.vec_e026b0 == kVecE026b0Bits);
        CHECK(st.vec_dfd5f0 == kVecDfd5f0Bits);
        CHECK(st.vec_de7fb0 == kVecDe7fb0Bits);
        CHECK(st.vec_de6d20 == kVecDe6d20Bits);
        CHECK(st.vec_de1ad0 == kVecDe1ad0Bits);
        CHECK(st.vec_ddef00 == kVecDdef00Bits);
        CHECK(st.vec_dddc90 == kVecDddc90Bits);
        CHECK(st.vec_ddab00 == kVecDdab00Bits);
        CHECK(st.vec_dda990 == kVecDda990Bits);
        CHECK(st.guarded_dd91b0.pi_guard == 1 && st.guarded_dd7e30.pi_guard == 1 &&
              st.guarded_dd4210.pi_guard == 1);
        CHECK(st.vec_dd41e0.guard == 1 && st.vec_dd41e0.lanes[0] == kNeg036BitsDd41);
        CHECK(st.vec_dc4180 == kVecDc4180Bits);
        CHECK(st.vec_dc2d50 == kVecDc2d50Bits);
        CHECK(st.vec_dbb280 == kVecDbb280Bits);
        CHECK(st.vec_db93a0 == kVecDb93a0Bits);
        CHECK(st.vec_db7e60 == kVecDb7e60Bits);
        CHECK(st.vec_db5dd0 == kVecDb5dd0Bits);
        CHECK(st.vec_db3a70 == kVecDb3a70Bits);
        CHECK(st.vec_db3960 == kVecDb3960Bits);
        CHECK(st.vec_dac410 == kVecDac410Bits);
        CHECK(st.vec_d94cb0 == kVecD94cb0Bits);
        CHECK(st.vec_d90310 == kVecD90310Bits);
        // idx 80/81: first published cell is the {~0,~0,~0,0} lane mask.
        CHECK(st.avx_d8fa40.cells[0] == 0xff && st.avx_d8fa40.cells[8] == 0xff &&
              st.avx_d8fa40.cells[12] == 0x00);
        CHECK(st.avx_d86ed0.cells[0] == 0xff && st.avx_d86ed0.cells[8] == 0xff &&
              st.avx_d86ed0.cells[12] == 0x00);
        CHECK(st.vec_d853a0 == kVecD853a0Bits);
        CHECK(st.vec_d84ee0 == kVecD84ee0Bits);
        CHECK(st.vec_d80e60 == kVecD80e60Bits);
        CHECK(st.vec_d7fe90 == kVecD7fe90Bits);
        CHECK(st.vec_d7fe40 == kVecD7fe40Bits);
        CHECK(st.vec_d7e640 == kVecD7e640Bits);
        CHECK(st.vec_d7c9d0 == kVecD7c9d0Bits);
        CHECK(st.vec_d7c4d0 == kVecD7c4d0Bits);
        CHECK(st.vec_d7c380 == kVecD7c380Bits);
        CHECK(st.vec_d7be60 == kVecD7be60Bits);
        CHECK(st.vec_d7bc60 == kVecD7bc60Bits);
        CHECK(st.vec_d7b930 == kVecD7b930Bits);
        CHECK(st.vec_d6e0f0 == kVecD6e0f0Bits);
        CHECK(st.vec_d6dbf0 == kVecD6dbf0Bits);
        CHECK(st.vec_d6d890 == kVecD6d890Bits);
        CHECK(st.vec_d6d280 == kVecD6d280Bits);
        CHECK(st.vec_d6a520 == kVecD6a520Bits);
        CHECK(st.vec_d62b00 == kVecD62b00Bits);
        CHECK(st.vec_d62690 == kVecD62690Bits);
        CHECK(st.vec_d623c0 == kVecD623c0Bits);
        CHECK(st.vec_d5fbe0 == kVecD5fbe0Bits);
        CHECK(st.avx_d5a280.prefix[0] == 0xff && st.avx_d5a280.table[0] == 0 &&
              st.avx_d5a280.table[4] == 1 && st.avx_d5a280.table[8] == 2);
        CHECK(st.vec_d47880 == kVecD47880Bits);
        CHECK(st.vec_d47420 == kVecD47420Bits);
        CHECK(st.vec_d47270 == kVecD47270Bits);
        CHECK(st.vec_d42b60 == kVecD42b60Bits);
        CHECK(st.vec_d40ea0 == kVecD40ea0Bits);
        CHECK(st.vec_d405a0 == kVecD405a0Bits);
        CHECK(st.vec_d40010 == kVecD40010Bits);
        CHECK(st.vec_d3b790 == kVecD3b790Bits);
        CHECK(st.vec_d3a6d0 == kVecD3a6d0Bits);
        CHECK(st.guarded_d39b00.pi_guard == 1 && st.guarded_d366d0.pi_guard == 1 &&
              st.guarded_d33b50.pi_guard == 1);
        CHECK(st.vec_d32720 == kVecD32720Bits);
        CHECK(st.vec_d2ffe0 == kVecD2ffe0Bits);
        CHECK(st.vec_d32b70 == kVecD32b70Bits);
        CHECK(st.vec_d2cb30 == kVecD2cb30Bits);
        CHECK(st.vec_d2c7e0 == kVecD2c7e0Bits);
        CHECK(st.vec_d20590 == kVecD20590Bits);
        CHECK(st.vec_d19bb0 == kVecD19bb0Bits);
        CHECK(st.vec_d19af0 == kVecD19af0Bits);
        CHECK(st.vec_d19930 == kVecD19930Bits);
        CHECK(st.vec_d17c10 == kVecD17c10Bits);
        CHECK(st.vec_d13300 == kVecD13300Bits);
        CHECK(st.vec_d0bde0 == kVecD0bde0Bits);
        CHECK(st.vec_d0bcd0 == kVecD0bcd0Bits);
        CHECK(st.vec_d0a250 == kVecD0a250Bits);
        CHECK(st.vec_d09160 == kVecD09160Bits);
        CHECK(st.vec_d082e0 == kVecD082e0Bits);
        CHECK(st.vec_d08240 == kVecD08240Bits);
        CHECK(st.vec_d06dd0 == kVecD06dd0Bits);
        CHECK(st.vec_d04ce0 == kVecD04ce0Bits);
        CHECK(st.vec_d00600 == kVecD00600Bits);
        CHECK(st.vec_cfcc60 == kVecCfcc60Bits);
        CHECK(st.vec_cfcbc0 == kVecCfcbc0Bits);
        CHECK(st.vec_cf44a0 == kVecCf44a0Bits);
        CHECK(st.vec_cee8a0 == kVecCee8a0Bits);
        CHECK(st.vec_ceb890 == kVecCeb890Bits);
        CHECK(st.vec_ceb2c0 == kVecCeb2c0Bits);
        CHECK(st.vec_ceb1c0 == kVecCeb1c0Bits);
        CHECK(st.vec_ceb070 == kVecCeb070Bits);
        CHECK(st.vec_ce99c0 == kVecCe99c0Bits);
        CHECK(st.vec_ce74a0 == kVecCe74a0Bits);
        CHECK(st.vec_ce7300 == kVecCe7300Bits);
        CHECK(st.vec_ce6fb0 == kVecCe6fb0Bits);
        CHECK(st.vec_ce67b0 == kVecCe67b0Bits);
        CHECK(st.vec_b743c0 == kVecB743c0Bits);
        CHECK(st.vec_b52110 == kVecB52110Bits);
        CHECK(st.math.pi_guard == 1 && st.math_b.pi_guard == 1 && st.math_c.pi_guard == 1 && st.math_d.pi_guard == 1);
        CHECK(st.flag32.load() == 0);
        CHECK(st.scratch.back() == std::byte{0} && st.scratch_slot.load() == 0);
    }
    {
        StartupState st;
        st.scratch.fill(std::byte{0x77});
        const auto slots = recovered_constructors();
        const CtorSlot order[] = {slots[0], {0xABCDEF, 1, nullptr}, slots[slots.size() - 1]};
        auto r = run_constructors(order, st);
        CHECK(!r.ok() && r.completed == 1 && r.blocked_on_rva == 0xABCDEF);
        CHECK(st.math.pi_guard == 1);
        CHECK(st.scratch.front() == std::byte{0x77});
    }
    {
        StartupState st;
        auto r = run_constructors({}, st);
        CHECK(r.ok() && r.completed == 0);
    }
    {
        // A recovered callback that reports a visible failure stops the runner at that slot.
        StartupState st;
        const CtorSlot order[] = {
            {0x111111, 0, [](StartupState& s) noexcept { s.init_failed = true; }},
            {0x222222, 1, [](StartupState&) noexcept {}},
        };
        auto r = run_constructors(order, st);
        CHECK(!r.ok() && r.completed == 0 && r.callback_failed && r.blocked_on_rva == 0x111111);
    }
    // Contiguous 0-47 recovered (23 via private fixture; 24-47 via ahead-family registrations).
    {
        const auto slots = recovered_constructors();
        bool saw11 = false, saw23 = false;
        std::uint32_t seen_thunks = 0, seen_ahead = 0;
        for (const CtorSlot& slot : slots) {
            if (slot.original_index == 11) {
                saw11 = true;
                CHECK(slot.original_rva == 0xf09160 && slot.run != nullptr);
            }
            if (slot.original_index >= 12 && slot.original_index <= 19) {
                CHECK(slot.run != nullptr);
                CHECK(slot.original_rva ==
                      kRegistrationThunkDescriptors[slot.original_index - 12].original_thunk_rva);
                ++seen_thunks;
            }
            if (slot.original_index == 21) CHECK(slot.original_rva == 0xee1130 && slot.run != nullptr);
            if (slot.original_index == 23) {
                saw23 = true;
                CHECK(slot.original_rva == 0xe71990 && slot.run != nullptr);
            }
            if (slot.original_index >= 24 && slot.original_index <= 47) {
                CHECK(slot.run != nullptr);
                CHECK(slot.original_rva ==
                      kAheadRegistrationDescriptors[slot.original_index - 24].original_thunk_rva);
                ++seen_ahead;
            }
            CHECK(slot.run != nullptr);  // production table has no explicit null rows
        }
        CHECK(saw11 && saw23 && seen_thunks == 8 && seen_ahead == 24);
    }
    // Fail-visible when the private fixture cannot be resolved.
    {
        CHECK(setenv(kSlot23NamesFixtureEnv, "/no/such/slot23_fixture.bin", 1) == 0);
        StartupState st;
        auto r = run_constructors(recovered_constructors(), st);
        CHECK(!r.ok() && r.callback_failed && r.blocked_on_rva == 0xe71990 && r.completed == 23);
        CHECK(st.init_failed);
        unsetenv(kSlot23NamesFixtureEnv);
    }
    const std::uint32_t expect[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
        16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
        32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
        48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
        64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
        80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
        96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
        112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
        128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
        144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
        160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
        176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
        192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
        208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
        224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
        240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
        256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271,
        272, 273, 274, 275, 276, 277, 278, 279, 280, 281, 282, 283, 284, 285, 286, 287,
        288, 289, 290, 291, 292, 293, 294, 295, 296, 297, 298, 299, 300, 301, 302, 303, 304, 305, 306,
        307, 308, 310, 311, 313, 317, 318, 319, 320, 321, 322, 324, 325, 327, 328, 330,
        331, 332, 333, 334, 343, 344, 345, 346, 347, 348, 350, 353, 355, 356, 371, 372,
        404, 405, 406, 417, 425, 431, 432, 433, 434, 435, 438, 447, 466, 470, 471, 472,
        481, 483, 484, 485, 487, 515, 517, 519, 520, 522, 523, 525, 526, 527, 528, 529,
        531, 533, 538, 541, 542, 546, 547, 548, 549, 551, 553, 562, 563, 564, 565,
    };
    CHECK(recovered_constructors().size() == sizeof(expect) / sizeof(expect[0]));
    for (std::size_t i = 0; i < sizeof(expect) / sizeof(expect[0]); ++i)
        CHECK(recovered_constructors()[i].original_index == expect[i]);
    for (std::size_t i = 1; i < recovered_constructors().size(); ++i)
        CHECK(recovered_constructors()[i - 1].original_index < recovered_constructors()[i].original_index);
    std::puts("startup_ctors: all checks passed");
    return 0;
}
