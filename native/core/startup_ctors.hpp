// Native replacement for the CRT constructor runner (original RVA 0x20): runs recovered constructors in the
// original list order and FAILS VISIBLY at the first constructor that has not been recovered.
#pragma once
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <span>

#include "math_constants.hpp"
#include "scratch_reset.hpp"
#include "trig_constants.hpp"
#include "vec_neg036.hpp"
#include "xyz_mask_copy.hpp"
#include "clear_flag32.hpp"
#include "store_zero32.hpp"
#include "hashed_name_ctors.hpp"
#include "guarded_static_helpers.hpp"
#include "zero_xmm_qword.hpp"
#include "zero_xmm16.hpp"
#include "zero_struct12.hpp"
#include "clear_flag64.hpp"
#include "init_f4fd40.hpp"
#include "init_f4eba0.hpp"
#include "init_f4b540.hpp"
#include "recursive_mutex.hpp"
#include "clothing_preview_param.hpp"
#include "descriptor_registration.hpp"
#include "lazy_param_def_tree.hpp"
#include "clothing_physical_mesh_parameters.hpp"
#include "registration_thunks.hpp"
#include "const_table_ee1130.hpp"
#include "vec_ef85e0.hpp"
#include "init_ea52f0.hpp"
#include "vec_e0b180.hpp"
#include "vec_e085e0.hpp"
#include "vec_e05320.hpp"
#include "vec_e094e0.hpp"
#include "vec_e026b0.hpp"
#include "vec_de1ad0.hpp"
#include "vec_ddef00.hpp"
#include "vec_de7fb0.hpp"
#include "vec_de6d20.hpp"
#include "vec_dfd5f0.hpp"
#include "leaf_df94c0.hpp"
#include "leaf_de0160.hpp"
#include "vec_ddab00.hpp"
#include "vec_dda990.hpp"
#include "guarded_dd91b0.hpp"
#include "guarded_dd7e30.hpp"
#include "guarded_dd4210.hpp"
#include "guarded_d39b00.hpp"
#include "guarded_d366d0.hpp"
#include "guarded_d33b50.hpp"
#include "avx_d5a280.hpp"
#include "vec_dd41e0.hpp"
#include "vec_b52110.hpp"
#include "vec_b743c0.hpp"
#include "vec_dc2d50.hpp"
#include "vec_dbfa60.hpp"
#include "vec_dbb280.hpp"
#include "vec_db93a0.hpp"
#include "vec_db5dd0.hpp"
#include "vec_db7e60.hpp"
#include "vec_db3a70.hpp"
#include "vec_db3960.hpp"
#include "vec_d7be60.hpp"
#include "vec_d7bc60.hpp"
#include "vec_d7b930.hpp"
#include "vec_d6e0f0.hpp"
#include "vec_d6dbf0.hpp"
#include "vec_d6d890.hpp"
#include "vec_d6d280.hpp"
#include "vec_d6a520.hpp"
#include "vec_d62b00.hpp"
#include "vec_d62690.hpp"
#include "vec_d623c0.hpp"
#include "vec_d5fbe0.hpp"
#include "vec_d47880.hpp"
#include "vec_d47420.hpp"
#include "vec_d47270.hpp"
#include "vec_d42b60.hpp"
#include "vec_d40ea0.hpp"
#include "vec_d405a0.hpp"
#include "vec_d40010.hpp"
#include "vec_d3b790.hpp"
#include "vec_d3a6d0.hpp"
#include "vec_d32b70.hpp"
#include "vec_d32720.hpp"
#include "vec_d2ffe0.hpp"
#include "vec_d2cb30.hpp"
#include "vec_d2c7e0.hpp"
#include "vec_d20590.hpp"
#include "vec_d19bb0.hpp"
#include "vec_d19af0.hpp"
#include "vec_d19930.hpp"
#include "vec_d17c10.hpp"
#include "vec_d13300.hpp"
#include "vec_d0bde0.hpp"
#include "vec_d0bcd0.hpp"
#include "vec_d0a250.hpp"
#include "vec_d09160.hpp"
#include "vec_d082e0.hpp"
#include "vec_d08240.hpp"
#include "vec_d06dd0.hpp"
#include "vec_d04ce0.hpp"
#include "vec_d00600.hpp"
#include "vec_cfcc60.hpp"
#include "vec_cfcbc0.hpp"
#include "vec_cf44a0.hpp"
#include "vec_cee8a0.hpp"
#include "vec_ceb890.hpp"
#include "vec_ceb2c0.hpp"
#include "vec_ceb1c0.hpp"
#include "vec_ceb070.hpp"
#include "vec_ce99c0.hpp"
#include "vec_ce8440.hpp"
#include "vec_ce74a0.hpp"
#include "vec_ce7300.hpp"
#include "vec_ce6fb0.hpp"
#include "vec_ce67b0.hpp"
#include "slot23_names_fixture.hpp"
#include "vec_d7c380.hpp"
#include "vec_d7c4d0.hpp"
#include "vec_d7c9d0.hpp"
#include "vec_d7e640.hpp"
#include "vec_d7fe40.hpp"
#include "vec_d7fe90.hpp"
#include "vec_d80e60.hpp"
#include "vec_d84ee0.hpp"
#include "vec_dac410.hpp"
#include "vec_d853a0.hpp"
#include "vec_d90310.hpp"
#include "avx_d8fa40.hpp"
#include "avx_d86ed0.hpp"
#include "vec_d85490.hpp"
#include "vec_d94cb0.hpp"
#include "vec_dc4180.hpp"
#include "vec_dddc90.hpp"
#include "vec_d71f60.hpp"
#include "vec_ce6130.hpp"
#include "vec_ce57b0.hpp"
#include "vec_ce38d0.hpp"
#include "vec_ce0890.hpp"
#include "vec_cde740.hpp"
#include "vec_cdc760.hpp"
#include "vec_cd4d60.hpp"
#include "vec_cd0420.hpp"
#include "vec_cceaa0.hpp"
#include "vec_ccd0f0.hpp"
#include "vec_cc8ac0.hpp"
#include "vec_cc8330.hpp"
#include "vec_cc5a10.hpp"
#include "vec_cc07d0.hpp"
#include "vec_cbb2c0.hpp"
#include "vec_cb5cb0.hpp"
#include "vec_caeed0.hpp"
#include "vec_cae740.hpp"
#include "vec_cae240.hpp"
#include "vec_cad450.hpp"
#include "leaf_cab080.hpp"
#include "vec_ca4dc0.hpp"
#include "leaf_ca18d0.hpp"
#include "vec_c9e8e0.hpp"
#include "leaf_c94ae0.hpp"
#include "vec_c91b10.hpp"
#include "vec_c891e0.hpp"
#include "leaf_c811b0.hpp"
#include "vec_c7de60.hpp"
#include "vec_c7c310.hpp"
#include "vec_c786c0.hpp"
#include "vec_c77070.hpp"
#include "vec_c741c0.hpp"
#include "vec_c70e20.hpp"
#include "vec_c6fdf0.hpp"
#include "leaf_c6f520.hpp"
#include "leaf_c6e9d0.hpp"
#include "vec_c6d910.hpp"
#include "vec_c6bad0.hpp"
#include "vec_c69f70.hpp"
#include "vec_c686e0.hpp"
#include "leaf_c675f0.hpp"
#include "vec_c66580.hpp"
#include "vec_c65f80.hpp"
#include "vec_c65da0.hpp"
#include "vec_c64060.hpp"
#include "vec_c61670.hpp"
#include "vec_c5f6f0.hpp"
#include "vec_c5c320.hpp"
#include "vec_c5c1a0.hpp"
#include "vec_c5c040.hpp"
#include "vec_c5a400.hpp"
#include "vec_c5a100.hpp"
#include "vec_c555c0.hpp"
#include "vec_c54d20.hpp"
#include "vec_c54830.hpp"
#include "vec_c53db0.hpp"
#include "vec_c4e5e0.hpp"
#include "vec_c4aa80.hpp"
#include "vec_c47e00.hpp"
#include "vec_c451d0.hpp"
#include "vec_c44830.hpp"
#include "vec_c42b00.hpp"
#include "vec_c3eaa0.hpp"
#include "vec_c3cf00.hpp"
#include "vec_c37020.hpp"
#include "vec_c36a80.hpp"
#include "vec_c23650.hpp"
#include "vec_c22300.hpp"
#include "vec_c20710.hpp"
#include "vec_c1a9a0.hpp"
#include "vec_c191a0.hpp"
#include "vec_c17a70.hpp"
#include "leaf_c16b30.hpp"
#include "vec_c152c0.hpp"
#include "vec_c14f60.hpp"
#include "vec_c0d720.hpp"
#include "leaf_c0d380.hpp"
#include "vec_c0bea0.hpp"
#include "vec_c0bbd0.hpp"
#include "vec_c0bb00.hpp"
#include "vec_c0baa0.hpp"
#include "vec_c0aae0.hpp"
#include "vec_c09070.hpp"
#include "vec_c060a0.hpp"
#include "leaf_c05980.hpp"
#include "vec_be5410.hpp"
#include "vec_be5270.hpp"
#include "vec_be50a0.hpp"
#include "vec_be4ff0.hpp"
#include "vec_bd7a80.hpp"
#include "vec_bd7950.hpp"
#include "vec_bd60a0.hpp"
#include "vec_bd4dd0.hpp"
#include "vec_bd32f0.hpp"
#include "vec_bd3190.hpp"
#include "vec_bd06c0.hpp"
#include "vec_bceda0.hpp"
#include "vec_bccc00.hpp"
#include "vec_bcc960.hpp"
#include "vec_bcc4e0.hpp"
#include "vec_bcc3f0.hpp"
#include "vec_bc8210.hpp"
#include "vec_bc7280.hpp"
#include "vec_bc5b00.hpp"
#include "vec_bc2800.hpp"
#include "vec_bc07f0.hpp"
#include "vec_bc0670.hpp"
#include "vec_bbf580.hpp"
#include "vec_bbd5e0.hpp"
#include "vec_bbba10.hpp"
#include "vec_bb9740.hpp"
#include "vec_bb82d0.hpp"
#include "vec_bb7990.hpp"
#include "vec_bacfb0.hpp"
#include "vec_bace80.hpp"
#include "vec_bace50.hpp"
#include "vec_bacb30.hpp"
#include "vec_bab950.hpp"
#include "vec_ba3ef0.hpp"
#include "vec_b950e0.hpp"
#include "vec_b8d170.hpp"
#include "vec_b89eb0.hpp"
#include "vec_b84580.hpp"
#include "vec_b7b270.hpp"
#include "vec_b77e90.hpp"
#include "vec_b77910.hpp"
#include "vec_b77740.hpp"
#include "vec_b76950.hpp"
#include "vec_b747f0.hpp"
#include "vec_b71f80.hpp"
#include "vec_b6e6d0.hpp"
#include "leaf_b692a0.hpp"
#include "vec_b67a70.hpp"
#include "vec_b5ecf0.hpp"
#include "vec_b58690.hpp"
#include "vec_b57360.hpp"
#include "vec_b4ef70.hpp"
#include "vec_b494d0.hpp"
#include "vec_b47100.hpp"
#include "vec_b44580.hpp"
#include "vec_b33bd0.hpp"
#include "leaf_a816d0.hpp"
#include "leaf_9b75a0.hpp"
#include "leaf_8f3d90.hpp"
#include "leaf_8a0440.hpp"
#include "leaf_83d800.hpp"
#include "leaf_7ce8d0.hpp"
#include "leaf_575d40.hpp"
#include "leaf_3ca7c0.hpp"
#include "leaf_163a40.hpp"
#include "leaf_99350.hpp"
#include "ctor_slot_296_b2b6b0.hpp"
#include "ctor_slot_297_b25470.hpp"
#include "ctor_slot_300_acc880.hpp"
#include "ctor_slot_298_b0e250.hpp"
#include "ctor_slot_299_b0c7a0.hpp"
#include "ctor_slot_301_ac9550.hpp"
#include "ctor_slot_302_aaeee0.hpp"
#include "ctor_slot_303_a82170.hpp"
#include "ctor_slot_305_a77b60.hpp"
#include "ctor_slot_306_a633b0.hpp"
#include "ctor_slots_322_334.hpp"
#include "ctor_tiny_atexit_slots.hpp"
#include "ctor_tiny_misc_slots.hpp"
#include "ctor_release_family_slots.hpp"
#include "ctor_slot_307_a5b090.hpp"
#include "ctor_slot_308_a58f10.hpp"
#include "ctor_slot_309_a505e0.hpp"
#include "ctor_slot_310_a479d0.hpp"
#include "ctor_slot_313_a447a0.hpp"
#include "ctor_slot_317_a2c390.hpp"
#include "ctor_slot_318_a20e80.hpp"
#include "ctor_slot_343_7d7970.hpp"
#include "ctor_slot_344_7d3810.hpp"
#include "ctor_slot_345_7cec60.hpp"
#include "ctor_slot_347_7c93a0.hpp"
#include "ctor_slot_348_7c9150.hpp"
#include "ctor_slot_350_7af0b0.hpp"
#include "ctor_slot_353_7a14d0.hpp"
#include "ctor_slot_355_7946d0.hpp"
#include "ctor_slot_356_78e2f0.hpp"

#include "ctor_slot_359_785f60.hpp"
#include "ctor_slot_364_74a8e0.hpp"
#include "ctor_slot_365_747870.hpp"
#include "ctor_slot_376_6e4930.hpp"
#include "ctor_slot_377_6de050.hpp"
#include "ctor_slot_383_6c62b0.hpp"
#include "ctor_slot_384_6b9260.hpp"
#include "ctor_slot_385_6b4cf0.hpp"
#include "ctor_slot_386_6b12f0.hpp"
#include "ctor_slot_387_6acf00.hpp"
#include "ctor_slot_389_695a50.hpp"
#include "ctor_slot_393_66cc00.hpp"
#include "ctor_slot_403_5f0160.hpp"
#include "ctor_slot_407_5d41c0.hpp"
#include "ctor_slot_413_57c810.hpp"
#include "ctor_slot_415_578b60.hpp"
#include "ctor_slot_419_56fae0.hpp"
#include "ctor_slot_421_56da20.hpp"
#include "ctor_slot_426_53fb50.hpp"
#include "ctor_slot_436_50dbe0.hpp"
#include "ctor_slot_439_4f7b70.hpp"
#include "ctor_slot_440_4f0c60.hpp"
#include "ctor_slot_441_4e68c0.hpp"
#include "ctor_slot_456_471950.hpp"
#include "ctor_slot_457_45c440.hpp"
#include "ctor_slot_458_45ace0.hpp"
#include "ctor_slot_459_43f460.hpp"
#include "ctor_slot_461_419480.hpp"
#include "ctor_slot_463_3e0040.hpp"
#include "ctor_slot_465_3cc9e0.hpp"
#include "ctor_slot_475_33cc60.hpp"
#include "ctor_slot_476_329650.hpp"
#include "ctor_slot_478_31eb00.hpp"
#include "ctor_slot_479_31b660.hpp"
#include "ctor_slot_482_2fb2a0.hpp"
#include "ctor_slot_486_2d5e00.hpp"
#include "ctor_slot_489_2acc10.hpp"
#include "ctor_slot_490_296d90.hpp"
#include "lane_m5_app_entry.hpp"
#include "ctor_slot_491_295f90.hpp"
#include "ctor_slot_492_278640.hpp"
#include "ctor_slot_499_2500e0.hpp"
#include "ctor_slot_502_249270.hpp"
#include "ctor_slot_509_2196f0.hpp"
#include "ctor_slot_516_1b97b0.hpp"
#include "ctor_slot_535_138e20.hpp"
#include "ctor_slot_540_10e390.hpp"
#include "ctor_slot_543_fae50.hpp"
#include "ctor_slot_544_d1b20.hpp"
#include "ctor_slot_557_47490.hpp"
#include "ctor_slot_558_428a0.hpp"
#include "ctor_slot_559_3f2d0.hpp"
#include "ctor_slot_569_1cc80.hpp"

namespace sotc {

// Native objects standing in for the original globals the recovered constructors write.
// math_b/c/d: clones of 0xf6cad0 at RVAs 0xf66640 / 0xf53de0 / 0xf3d9d0 (same bits, distinct BSS).
// xyz_mask_b: clone of the unguarded xyz-mask pattern at 0xf50d70 (distinct from 0xf30460's cell).
struct StartupState {
    MathConstants math{};
    MathConstants math_b{};
    MathConstants math_c{};
    MathConstants math_d{};
    TrigConstants trig{};
    VecNeg036 neg036{};
    XyzMaskCopy xyz_mask_copy{};
    XyzMaskCopy xyz_mask_b{};
    InitF4fd40 f4fd40{};
    InitF4eba0 f4eba0{};
    InitF4b540 f4b540{};
    VecEf85e0 vec_ef85e0{};   // idx 20 (peer lift; registered here)
    InitEa52f0 ea52f0{};      // idx 22
    VecE0b180 vec_e0b180{};   // idx 49
    VecE085e0 vec_e085e0{};   // idx 51
    VecE05320 vec_e05320{};   // idx 52
    VecE094E0 vec_e094e0{};   // idx 50
    VecE026b0 vec_e026b0{};   // idx 53
    VecDfd5f0 vec_dfd5f0{};   // idx 54
    LeafDf94c0 leaf_df94c0{}; // idx 55 / 0xdf94c0
    VecDe7fb0 vec_de7fb0{};   // idx 56
    VecDe6d20 vec_de6d20{};   // idx 57
    VecDe1ad0 vec_de1ad0{};   // idx 58
    LeafDe0160 leaf_de0160{}; // idx 59 / 0xde0160 (memset-1 + zero trailer; atexit omitted)
    VecDdef00 vec_ddef00{};   // idx 60
    VecDddc90 vec_dddc90{};   // idx 61
    VecDdab00 vec_ddab00{};   // idx 62
    VecDda990 vec_dda990{};   // idx 63
    GuardedDd91b0 guarded_dd91b0{};  // idx 64 / 0xdd91b0
    GuardedDd7e30 guarded_dd7e30{};  // idx 65 / 0xdd7e30
    GuardedDd4210 guarded_dd4210{};  // idx 66 / 0xdd4210
    VecDd41e0 vec_dd41e0{};   // idx 67
    VecDc4180 vec_dc4180{};   // idx 68
    VecDc2d50 vec_dc2d50{};   // idx 69
    VecDBFA60 vec_dbfa60{};   // idx 70
    VecDbb280 vec_dbb280{};   // idx 71
    VecDb93a0 vec_db93a0{};   // idx 72
    VecDb7e60 vec_db7e60{};   // idx 73
    VecDb5dd0 vec_db5dd0{};   // idx 74
    VecDb3a70 vec_db3a70{};   // idx 75
    VecDb3960 vec_db3960{};   // idx 76
    VecDac410 vec_dac410{};   // idx 77
    VecD94cb0 vec_d94cb0{};   // idx 78
    VecD90310 vec_d90310{};   // idx 79
    AvxD8fa40 avx_d8fa40{};   // idx 80 / 0xd8fa40
    AvxD86ed0 avx_d86ed0{};   // idx 81 / 0xd86ed0
    VecD85490 vec_d85490{};   // idx 82
    VecD853a0 vec_d853a0{};   // idx 83
    VecD84ee0 vec_d84ee0{};   // idx 84
    VecD80e60 vec_d80e60{};   // idx 85
    VecD7fe90 vec_d7fe90{};   // idx 86
    VecD7fe40 vec_d7fe40{};   // idx 87
    VecD7e640 vec_d7e640{};   // idx 88
    VecD7c9d0 vec_d7c9d0{};   // idx 89
    VecD7c4d0 vec_d7c4d0{};   // idx 90
    VecD7c380 vec_d7c380{};   // idx 91
    VecD7be60 vec_d7be60{};   // idx 92
    VecD7bc60 vec_d7bc60{};   // idx 93
    VecD7b930 vec_d7b930{};   // idx 94
    VecD6e0f0 vec_d6e0f0{};   // idx 96
    VecD6dbf0 vec_d6dbf0{};   // 0xd6dbf0 idx 97
    VecD6d890 vec_d6d890{};   // 0xd6d890 idx 98
    VecD6d280 vec_d6d280{};   // 0xd6d280 idx 99
    VecD6a520 vec_d6a520{};   // 0xd6a520 idx 100
    VecD62b00 vec_d62b00{};   // 0xd62b00 idx 101
    VecD62690 vec_d62690{};   // 0xd62690 idx 102
    VecD623c0 vec_d623c0{};   // 0xd623c0 idx 103
    VecD5fbe0 vec_d5fbe0{};   // 0xd5fbe0 idx 104
    AvxD5a280 avx_d5a280{};   // idx 105 / 0xd5a280
    VecD47880 vec_d47880{};   // 0xd47880 idx 106
    VecD47420 vec_d47420{};   // 0xd47420 idx 107
    VecD47270 vec_d47270{};   // 0xd47270 idx 108
    VecD42b60 vec_d42b60{};   // 0xd42b60 idx 109
    VecD40ea0 vec_d40ea0{};   // 0xd40ea0 idx 110
    VecD405a0 vec_d405a0{};   // 0xd405a0 idx 111
    VecD40010 vec_d40010{};   // 0xd40010 idx 112
    VecD3b790 vec_d3b790{};   // 0xd3b790 idx 113
    VecD3a6d0 vec_d3a6d0{};   // 0xd3a6d0 idx 114
    GuardedD39b00 guarded_d39b00{};  // idx 115 / 0xd39b00
    GuardedD366d0 guarded_d366d0{};  // idx 116 / 0xd366d0
    GuardedD33b50 guarded_d33b50{};  // idx 117 / 0xd33b50
    VecD32720 vec_d32720{};   // 0xd32720 idx 119
    VecD2ffe0 vec_d2ffe0{};   // 0xd2ffe0 idx 120
    VecD32b70 vec_d32b70{};   // 0xd32b70 idx 118
    VecD2cb30 vec_d2cb30{};   // 0xd2cb30 idx 121
    VecD2c7e0 vec_d2c7e0{};   // 0xd2c7e0 idx 122
    VecD20590 vec_d20590{};   // 0xd20590 idx 123
    VecD19bb0 vec_d19bb0{};   // 0xd19bb0 idx 124
    VecD19af0 vec_d19af0{};   // 0xd19af0 idx 125
    VecD19930 vec_d19930{};   // 0xd19930 idx 126
    VecD17c10 vec_d17c10{};   // 0xd17c10 idx 127
    VecD13300 vec_d13300{};   // 0xd13300 idx 128
    VecD0bde0 vec_d0bde0{};   // 0xd0bde0 idx 129
    VecD0bcd0 vec_d0bcd0{};   // 0xd0bcd0 idx 130
    VecD0a250 vec_d0a250{};   // 0xd0a250 idx 131
    VecD09160 vec_d09160{};   // 0xd09160 idx 132
    VecD082e0 vec_d082e0{};   // 0xd082e0 idx 133
    VecD08240 vec_d08240{};   // 0xd08240 idx 134
    VecD06dd0 vec_d06dd0{};   // 0xd06dd0 idx 135
    VecD04ce0 vec_d04ce0{};   // 0xd04ce0 idx 136
    VecD00600 vec_d00600{};   // 0xd00600 idx 137
    VecCfcc60 vec_cfcc60{};   // 0xcfcc60 idx 138
    VecCfcbc0 vec_cfcbc0{};   // 0xcfcbc0 idx 139
    VecCf44a0 vec_cf44a0{};   // 0xcf44a0 idx 140
    VecCee8a0 vec_cee8a0{};   // 0xcee8a0 idx 141
    VecCeb890 vec_ceb890{};   // 0xceb890 idx 142
    VecCeb2c0 vec_ceb2c0{};   // 0xceb2c0 idx 143
    VecCeb1c0 vec_ceb1c0{};   // 0xceb1c0 idx 144
    VecCeb070 vec_ceb070{};   // 0xceb070 idx 145
    VecCe99c0 vec_ce99c0{};   // 0xce99c0 idx 146
    VecCe8440 vec_ce8440{};   // 0xce8440 idx 147
    VecCe74a0 vec_ce74a0{};   // 0xce74a0 idx 148
    VecCe7300 vec_ce7300{};   // 0xce7300 idx 149
    VecCe6fb0 vec_ce6fb0{};   // 0xce6fb0 idx 150
    VecCe67b0 vec_ce67b0{};   // 0xce67b0 idx 151
    VecB743c0 vec_b743c0{};   // idx 282
    VecB52110 vec_b52110{};   // idx 290
    VecD71f60 vec_d71f60{};  // idx 95 / 0xd71f60 (batch 01: last leaf_vec_copy module)
    VecCe6130 vec_ce6130{};  // idx 152 / 0xce6130
    VecCe57B0 vec_ce57b0{};  // idx 153 / 0xce57b0
    VecCe38d0 vec_ce38d0{};  // idx 154 / 0xce38d0
    VecCe0890 vec_ce0890{};  // idx 155 / 0xce0890
    VecCde740 vec_cde740{};  // idx 156 / 0xcde740
    VecCdc760 vec_cdc760{};  // idx 157 / 0xcdc760
    VecCd4d60 vec_cd4d60{};  // idx 158 / 0xcd4d60
    VecCd0420 vec_cd0420{};  // idx 159 / 0xcd0420
    VecCceaa0 vec_cceaa0{};  // idx 160 / 0xcceaa0
    VecCcd0f0 vec_ccd0f0{};  // idx 161 / 0xccd0f0
    VecCc8ac0 vec_cc8ac0{};  // idx 162 / 0xcc8ac0
    VecCc8330 vec_cc8330{};  // idx 163 / 0xcc8330
    VecCc5a10 vec_cc5a10{};  // idx 164 / 0xcc5a10
    VecCc07d0 vec_cc07d0{};  // idx 165 / 0xcc07d0
    VecCbb2c0 vec_cbb2c0{};  // idx 166 / 0xcbb2c0
    VecCb5cb0 vec_cb5cb0{};  // idx 167 / 0xcb5cb0
    VecCaeed0 vec_caeed0{};  // idx 168 / 0xcaeed0
    VecCae740 vec_cae740{};  // idx 169 / 0xcae740
    VecCae240 vec_cae240{};  // idx 170 / 0xcae240
    VecCad450 vec_cad450{};  // idx 171 / 0xcad450
    LeafCab080 leaf_cab080{};  // idx 172 / 0xcab080
    VecCa4dc0 vec_ca4dc0{};  // idx 173 / 0xca4dc0
    LeafCa18d0 leaf_ca18d0{};  // idx 174 / 0xca18d0
    VecC9e8e0 vec_c9e8e0{};  // idx 175 / 0xc9e8e0
    LeafC94ae0 leaf_c94ae0{};  // idx 176 / 0xc94ae0
    VecC91b10 vec_c91b10{};  // idx 177 / 0xc91b10
    VecC891e0 vec_c891e0{};  // idx 178 / 0xc891e0
    LeafC811b0 leaf_c811b0{};  // idx 179 / 0xc811b0
    VecC7de60 vec_c7de60{};  // idx 180 / 0xc7de60
    VecC7c310 vec_c7c310{};  // idx 181 / 0xc7c310
    VecC786c0 vec_c786c0{};  // idx 182 / 0xc786c0
    VecC77070 vec_c77070{};  // idx 183 / 0xc77070
    VecC741c0 vec_c741c0{};  // idx 184 / 0xc741c0
    VecC70e20 vec_c70e20{};  // idx 185 / 0xc70e20
    VecC6fdf0 vec_c6fdf0{};  // idx 186 / 0xc6fdf0
    LeafC6f520 leaf_c6f520{};  // idx 187 / 0xc6f520
    LeafC6e9d0 leaf_c6e9d0{};  // idx 188 / 0xc6e9d0
    VecC6d910 vec_c6d910{};  // idx 189 / 0xc6d910
    VecC6bad0 vec_c6bad0{};  // idx 190 / 0xc6bad0
    VecC69f70 vec_c69f70{};  // idx 191 / 0xc69f70
    VecC686e0 vec_c686e0{};  // idx 192 / 0xc686e0
    LeafC675f0 leaf_c675f0{};  // idx 193 / 0xc675f0
    VecC66580 vec_c66580{};  // idx 194 / 0xc66580
    VecC65f80 vec_c65f80{};  // idx 195 / 0xc65f80
    VecC65da0 vec_c65da0{};  // idx 196 / 0xc65da0
    VecC64060 vec_c64060{};  // idx 197 / 0xc64060
    VecC61670 vec_c61670{};  // idx 198 / 0xc61670
    VecC5f6f0 vec_c5f6f0{};  // idx 199 / 0xc5f6f0
    VecC5c320 vec_c5c320{};  // idx 200 / 0xc5c320
    VecC5c1a0 vec_c5c1a0{};  // idx 201 / 0xc5c1a0
    VecC5C040 vec_c5c040{};  // idx 202 / 0xc5c040
    VecC5A400 vec_c5a400{};  // idx 203 / 0xc5a400
    VecC5A100 vec_c5a100{};  // idx 204 / 0xc5a100
    VecC555C0 vec_c555c0{};  // idx 205 / 0xc555c0
    VecC54D20 vec_c54d20{};  // idx 206 / 0xc54d20
    VecC54830 vec_c54830{};  // idx 207 / 0xc54830
    VecC53DB0 vec_c53db0{};  // idx 208 / 0xc53db0
    VecC4E5E0 vec_c4e5e0{};  // idx 209 / 0xc4e5e0
    VecC4AA80 vec_c4aa80{};  // idx 210 / 0xc4aa80
    VecC47E00 vec_c47e00{};  // idx 211 / 0xc47e00
    VecC451D0 vec_c451d0{};  // idx 212 / 0xc451d0
    VecC44830 vec_c44830{};  // idx 213 / 0xc44830
    VecC42B00 vec_c42b00{};  // idx 214 / 0xc42b00
    VecC3EAA0 vec_c3eaa0{};  // idx 215 / 0xc3eaa0
    VecC3CF00 vec_c3cf00{};  // idx 216 / 0xc3cf00
    VecC37020 vec_c37020{};  // idx 217 / 0xc37020
    VecC36A80 vec_c36a80{};  // idx 218 / 0xc36a80
    VecC23650 vec_c23650{};  // idx 219 / 0xc23650
    VecC22300 vec_c22300{};  // idx 220 / 0xc22300
    VecC20710 vec_c20710{};  // idx 221 / 0xc20710
    VecC1A9A0 vec_c1a9a0{};  // idx 222 / 0xc1a9a0
    VecC191A0 vec_c191a0{};  // idx 223 / 0xc191a0
    VecC17A70 vec_c17a70{};  // idx 224 / 0xc17a70
    LeafC16b30 leaf_c16b30{};  // idx 225 / 0xc16b30
    VecC152C0 vec_c152c0{};  // idx 226 / 0xc152c0
    VecC14F60 vec_c14f60{};  // idx 227 / 0xc14f60
    VecC0D720 vec_c0d720{};  // idx 228 / 0xc0d720
    LeafC0d380 leaf_c0d380{};  // idx 229 / 0xc0d380
    VecC0BEA0 vec_c0bea0{};  // idx 230 / 0xc0bea0
    VecC0BBD0 vec_c0bbd0{};  // idx 231 / 0xc0bbd0
    VecC0BB00 vec_c0bb00{};  // idx 232 / 0xc0bb00
    VecC0BAA0 vec_c0baa0{};  // idx 233 / 0xc0baa0
    VecC0AAE0 vec_c0aae0{};  // idx 234 / 0xc0aae0
    VecC09070 vec_c09070{};  // idx 235 / 0xc09070
    VecC060A0 vec_c060a0{};  // idx 236 / 0xc060a0
    LeafC05980 leaf_c05980{};  // idx 237 / 0xc05980
    VecBE5410 vec_be5410{};  // idx 238 / 0xbe5410
    VecBE5270 vec_be5270{};  // idx 239 / 0xbe5270
    VecBE50A0 vec_be50a0{};  // idx 240 / 0xbe50a0
    VecBE4FF0 vec_be4ff0{};  // idx 241 / 0xbe4ff0
    VecBD7A80 vec_bd7a80{};  // idx 242 / 0xbd7a80
    VecBD7950 vec_bd7950{};  // idx 243 / 0xbd7950
    VecBD60A0 vec_bd60a0{};  // idx 244 / 0xbd60a0
    VecBD4DD0 vec_bd4dd0{};  // idx 245 / 0xbd4dd0
    VecBD32F0 vec_bd32f0{};  // idx 246 / 0xbd32f0
    VecBD3190 vec_bd3190{};  // idx 247 / 0xbd3190
    VecBD06C0 vec_bd06c0{};  // idx 248 / 0xbd06c0
    VecBCEDA0 vec_bceda0{};  // idx 249 / 0xbceda0
    VecBCCC00 vec_bccc00{};  // idx 250 / 0xbccc00
    VecBCC960 vec_bcc960{};  // idx 251 / 0xbcc960
    VecBCC4E0 vec_bcc4e0{};  // idx 252 / 0xbcc4e0
    VecBCC3F0 vec_bcc3f0{};  // idx 253 / 0xbcc3f0
    VecBC8210 vec_bc8210{};  // idx 254 / 0xbc8210
    VecBC7280 vec_bc7280{};  // idx 255 / 0xbc7280
    VecBC5B00 vec_bc5b00{};  // idx 256 / 0xbc5b00
    VecBC2800 vec_bc2800{};  // idx 257 / 0xbc2800
    VecBC07F0 vec_bc07f0{};  // idx 258 / 0xbc07f0
    VecBC0670 vec_bc0670{};  // idx 259 / 0xbc0670
    VecBBF580 vec_bbf580{};  // idx 260 / 0xbbf580
    VecBBD5E0 vec_bbd5e0{};  // idx 261 / 0xbbd5e0
    VecBBBA10 vec_bbba10{};  // idx 262 / 0xbbba10
    VecBB9740 vec_bb9740{};  // idx 263 / 0xbb9740
    VecBB82D0 vec_bb82d0{};  // idx 264 / 0xbb82d0
    VecBB7990 vec_bb7990{};  // idx 265 / 0xbb7990
    VecBACFB0 vec_bacfb0{};  // idx 266 / 0xbacfb0
    VecBACE80 vec_bace80{};  // idx 267 / 0xbace80
    VecBACE50 vec_bace50{};  // idx 268 / 0xbace50
    VecBACB30 vec_bacb30{};  // idx 269 / 0xbacb30
    VecBAB950 vec_bab950{};  // idx 270 / 0xbab950
    VecBA3EF0 vec_ba3ef0{};  // idx 271 / 0xba3ef0
    VecB950E0 vec_b950e0{};  // idx 272 / 0xb950e0
    VecB8D170 vec_b8d170{};  // idx 273 / 0xb8d170
    VecB89EB0 vec_b89eb0{};  // idx 274 / 0xb89eb0
    VecB84580 vec_b84580{};  // idx 275 / 0xb84580
    VecB7B270 vec_b7b270{};  // idx 276 / 0xb7b270
    VecB77E90 vec_b77e90{};  // idx 277 / 0xb77e90
    VecB77910 vec_b77910{};  // idx 278 / 0xb77910
    VecB77740 vec_b77740{};  // idx 279 / 0xb77740
    VecB76950 vec_b76950{};  // idx 280 / 0xb76950
    VecB747f0 vec_b747f0{};  // idx 281 / 0xb747f0
    VecB71F80 vec_b71f80{};  // idx 283 / 0xb71f80
    VecB6E6D0 vec_b6e6d0{};  // idx 284 / 0xb6e6d0
    LeafB692a0 leaf_b692a0{};  // idx 285 / 0xb692a0
    VecB67A70 vec_b67a70{};  // idx 286 / 0xb67a70
    VecB5ECF0 vec_b5ecf0{};  // idx 287 / 0xb5ecf0
    VecB58690 vec_b58690{};  // idx 288 / 0xb58690
    VecB57360 vec_b57360{};  // idx 289 / 0xb57360
    VecB4EF70 vec_b4ef70{};  // idx 291 / 0xb4ef70
    VecB494D0 vec_b494d0{};  // idx 292 / 0xb494d0
    VecB47100 vec_b47100{};  // idx 293 / 0xb47100
    VecB44580 vec_b44580{};  // idx 294 / 0xb44580
    VecB33BD0 vec_b33bd0{};  // idx 295 / 0xb33bd0
    Slot296Stamp slot296_stamp{};      // idx 296 / 0xb2b6b0 (8-byte TSC stamp at 0x334ac18)
    Slot297Table slot297_table{};      // idx 297 / 0xb25470 (400 x 0x118 records at 0x332f650)
    // Shared console-command state: the manager cell [0x2dfcbc8] (null until the engine boot installer 0x242c0), the pending
    // list 0x2dfce50 the ConCmd constructors queue into, and the atexit log (CPU-01-CONCMD-298-301).
    ConCmdRegistry concmd{};
    Slot298Object slot298_shader_type{};  // idx 298 / 0xb0e250 (shaderTypeToLoad, 0x3305d20)
    DebugMenuRegistry debug_menu{};       // parent cell [0x2dfd4b8] (null) + root list 0x2dfd4c0
    Slot299Objects slot299{};             // idx 299 / 0xb0c7a0 (0x3305a30..0x3305d1f)
    Slot300Cleanup slot300_cleanup{};  // idx 300 / 0xacc880 (one inert atexit entry)
    Slot301Object slot301_rumble{};    // idx 301 / 0xac9550 (rumbleEnabled, 0x33054a0)
    Slot302Objects slot302{};          // idx 302 / 0xaaeee0 (ten handles + identity vptr)
    Slot305Pairs slot305{};            // idx 305 / 0xa77b60 (six guarded zero pairs)
    Slot303Objects slot303{};          // idx 303 / 0xa82170 (ProfileCPU 0x32fd228 + debug toggle 0x32fd2f0)
    // idx 306 / 0xa633b0: the 46 `sndx_` objects. The hooks default to the malloc-backed stand-in for the
    // allocator behind the four "MenuSeparator" strings; replace `slot306_hooks` to supply another, or
    // clear `slot306_hooks.separator_alloc.alloc.allocate` to see the fragment fail closed (init_failed).
    Slot306Objects slot306{};
    Slot306Hooks slot306_hooks = default_slot306_hooks();
    Slot306Result slot306_result{};

    // Slot-recovery lanes L1/L2/L4/L5 (2026-10-26): see docs/checkpoints/2026-10-26-L*.md.
    TinyAtexitState tiny_atexit{};  // L1 family A: 19 slots x 2 objects
    Slot404Buffer slot404{};
    Slot406State slot406{};
    Slot517State slot517{};
    Slot519State slot519{};
    Slot541State slot541{};
    Slot553State slot553{};
    Slot562State slot562{};
    Slot563Stamp slot563{};
    // Existing leaf helpers newly wired into the production table (L1 follow-up).
    std::uint32_t slot315_cell = 0;
    std::array<NameHashCell, 3> slot323_cells{};
    std::array<NameHashCell, 2> slot341_cells{};
    Pair128 slot357_cell{};
    std::array<NameHashCell, 3> slot378_cells{};
    std::array<NameHashCell, 1> slot394_cells{};
    Xmm16 slot412_xmm{};
    std::uint64_t slot412_q = 0;
    Xmm16 slot416_xmm{};
    std::uint8_t slot496_cell = 0;
    Bytes12 slot500_cell{};
    std::atomic<std::uint64_t> slot560_cell{0};
    ReleaseFamilyState release_family{};
    ReleaseFamilyHooks release_family_hooks{};  // null release_view -> lifted inert f590
    Slot307Objects slot307{};
    Slot308Objects slot308{};
    Slot309Objects slot309{};
    Slot309Result slot309_result{};
    Slot310Objects slot310{};
    Slot313Objects slot313{};
    Slot317Objects slot317{};
    Slot318Objects slot318{};
    Slot343Objects slot343{}; Slot343Result slot343_result{};
    Slot344Objects slot344{}; Slot344Result slot344_result{};
    Slot345Objects slot345{}; Slot345Result slot345_result{};
    Slot347Objects slot347{}; Slot347Result slot347_result{};
    Slot348Objects slot348{}; Slot348Result slot348_result{};
    Slot350Objects slot350{}; Slot350Result slot350_result{};
    Slot353Objects slot353{}; Slot353Result slot353_result{};
    Slot355Objects slot355{}; Slot355Result slot355_result{};
    Slot356Objects slot356{}; Slot356Result slot356_result{};
    Slot322Objects slot322{};
    Slot325Objects slot325{};
    Slot327Objects slot327{};
    Slot328Objects slot328{};
    Slot330Objects slot330{};
    Slot331Objects slot331{};
    Slot332Objects slot332{};
    Slot333Objects slot333{};
    Slot334Objects slot334{};
    LeafA816d0 leaf_a816d0{};  // idx 304 / 0xa816d0
    Leaf9b75a0 leaf_9b75a0{};  // idx 319 / 0x9b75a0
    Leaf8f3d90 leaf_8f3d90{};  // idx 320 / 0x8f3d90
    Leaf8a0440 leaf_8a0440{};  // idx 321 / 0x8a0440
    Leaf83d800 leaf_83d800{};  // idx 324 / 0x83d800
    Leaf7ce8d0 leaf_7ce8d0{};  // idx 346 / 0x7ce8d0
    Leaf575d40Cell leaf_575d40_source{};  // idx 417 runtime source (original BSS 0x2eea480; producer not recovered)
    Leaf575d40Cell leaf_575d40{};  // idx 417 / 0x575d40 (copies the source)
    Leaf3ca7c0 leaf_3ca7c0{};  // idx 466 / 0x3ca7c0
    Leaf163a40 leaf_163a40{};  // idx 526 / 0x163a40
    Leaf99350 leaf_99350{};  // idx 548 / 0x99350
    Leaf99350Inputs leaf_99350_inputs{};  // idx 548 runtime inputs (original cells 0x2dd9fa8 / 0x2dd9f84; producer not recovered)

    Slot359Objects slot359{}; Slot359Result slot359_result{};
    Slot364Objects slot364{}; Slot364Result slot364_result{};
    Slot365Objects slot365{}; Slot365Result slot365_result{};
    Slot376Objects slot376{}; Slot376Result slot376_result{};
    Slot377Objects slot377{}; Slot377Result slot377_result{};
    Slot383Objects slot383{}; Slot383Result slot383_result{};
    Slot384Objects slot384{}; Slot384Result slot384_result{};
    Slot385Objects slot385{}; Slot385Result slot385_result{};
    Slot386Objects slot386{}; Slot386Result slot386_result{};
    Slot387Objects slot387{}; Slot387Result slot387_result{};
    Slot389Objects slot389{}; Slot389Result slot389_result{};
    Slot393Objects slot393{}; Slot393Result slot393_result{};
    Slot403Objects slot403{}; Slot403Result slot403_result{};
    Slot407Objects slot407{}; Slot407Result slot407_result{};
    Slot413Objects slot413{}; Slot413Result slot413_result{};
    Slot415Objects slot415{}; Slot415Result slot415_result{};
    Slot419Objects slot419{}; Slot419Result slot419_result{};
    Slot421Objects slot421{}; Slot421Result slot421_result{};
    Slot426Objects slot426{}; Slot426Result slot426_result{};
    Slot436Objects slot436{}; Slot436Result slot436_result{};
    Slot439Objects slot439{}; Slot439Result slot439_result{};
    Slot440Objects slot440{}; Slot440Result slot440_result{};
    Slot441Objects slot441{}; Slot441Result slot441_result{};
    Slot456Objects slot456{}; Slot456Result slot456_result{};
    Slot457Objects slot457{}; Slot457Result slot457_result{};
    Slot458Objects slot458{}; Slot458Result slot458_result{};
    Slot459Objects slot459{}; Slot459Result slot459_result{};
    Slot461Objects slot461{}; Slot461Result slot461_result{};
    Slot463Objects slot463{}; Slot463Result slot463_result{};
    Slot465Objects slot465{}; Slot465Result slot465_result{};
    Slot475Objects slot475{}; Slot475Result slot475_result{};
    Slot476Objects slot476{}; Slot476Result slot476_result{};
    Slot478Objects slot478{}; Slot478Result slot478_result{};
    Slot479Objects slot479{}; Slot479Result slot479_result{};
    Slot482Objects slot482{}; Slot482Result slot482_result{};
    Slot486Objects slot486{}; Slot486Result slot486_result{};
    Slot489Objects slot489{}; Slot489Result slot489_result{};
    Slot490Objects slot490{}; Slot490Result slot490_result{};
    lane_m5::AppInfoQueue app_info_queue{};
    Slot491Objects slot491{}; bool slot491_ok{false};
    Slot492Objects slot492{}; Slot492Result slot492_result{};
    Slot499Objects slot499{}; Slot499Result slot499_result{};
    Slot502Objects slot502{}; Slot502Result slot502_result{};
    Slot509Objects slot509{}; Slot509Result slot509_result{};
    Slot516Objects slot516{}; Slot516Result slot516_result{};
    Slot535Objects slot535{}; Slot535Result slot535_result{};
    Slot540Objects slot540{}; Slot540Result slot540_result{};
    Slot543Objects slot543{}; Slot543Result slot543_result{};
    Slot544Objects slot544{}; Slot544Result slot544_result{};
    Slot557Objects slot557{}; Slot557Result slot557_result{};
    Slot558Objects slot558{}; Slot558Result slot558_result{};
    Slot559Objects slot559{}; Slot559Result slot559_result{};
    Slot569Objects slot569{}; Slot569Result slot569_result{};

    std::array<std::byte, kScratchBytes> scratch{};
    std::atomic<std::uint64_t> scratch_slot{0};
    std::atomic<std::uint32_t> flag32{0};

    // Slot 10 / RVA 0x00f09e60 effects. The original published a vtable address point at global
    // 0x3356678 and created a once-guarded recursive + priority-inherit mutex. The user chose
    // StartupState RAII ownership for that mutex; the descriptor is an ordinary native pointer.
    RecursiveMutex descriptor_mutex{};
    bool descriptor_mutex_initialized = false;
    const ClothingPreviewParamDescriptor* descriptor = nullptr;
    bool init_failed = false;

    // Slot 11 / RVA 0x00f09160 effects (ClothingPhysicalMeshParameters). The original published a vptr
    // at global 0x3356068 and created a once-guarded recursive mutex at 0x3356078; DescriptorRegistration
    // owns both here. The lazy schema tree (original flag/table 0x3356070/0x3356080) stays unbuilt and
    // reports schema_not_available. Declaration order matters: the tree borrows the registration's mutex.
    DescriptorRegistration physmesh_registration{};
    LazyParamDefTree physmesh_tree{physmesh_registration.mutex(), &build_clothing_physical_mesh_schema};

    // Slots 12-19 / RVAs 0xf06340 ... 0xefacd0 (registration thunks of unidentified classes). Entry
    // [i] is constructor-table index 12 + i: one published opaque descriptor and its owned recursive +
    // priority-inherit mutex (the original: two adjacent .bss globals, a malloc(0x10) mutex block and
    // an atexit cleanup). See registration_thunks.hpp for what is and is not modelled.
    std::array<DescriptorRegistration, kRegistrationThunkCount> thunk_registrations{};

    // Slots 24-47 (descriptor + mutex + atexit family): entry [i] is constructor-table index 24 + i.
    // Owned by init_registration_thunk_ahead; now on the production path after slot 23.
    std::array<DescriptorRegistration, kAheadRegistrationCount> ahead_registrations{};

    // Slot 21 / RVA 0x00ee1130: byte-exact 128-byte constant table (cell meanings unknown).
    ConstTableEe1130 const_table_ee1130{};

    // Slot 23 / RVA 0xe71990: private name-string fixture + filled 14-record table.
    // Missing/corrupt fixture → init_slot23_leaf_e71990 sets init_failed (fail-visible).
    Slot23NamesFixture slot23_names{};
    LeafE71990 leaf_e71990{};
};

// Load private fixture, verify name_hash for records 0..12, fill leaf_e71990.
// Returns false and sets state.init_failed on any failure.
[[nodiscard]] bool init_slot23_leaf_e71990(StartupState& state) noexcept;

// Original constructor-table length: 573 entries at 0x2dd70e0, indexed BACKWARDS
// (index i is the qword at 0x2dd70e0 - 8*i). tools/lift/check_ctor_slots.py pins the table
// sha256; ctor_original_table.hpp carries the full generated row table.
inline constexpr std::size_t kOriginalCtorCount = 573;

struct CtorSlot {
    std::uint32_t original_rva;               // identity only; never used as an address
    std::uint32_t original_index;             // position in the original 573-entry list (runs 0 -> 572)
    void (*run)(StartupState&) noexcept;      // nullptr == not recovered yet
};

struct CtorRunResult {
    std::size_t completed = 0;
    std::uint32_t blocked_on_rva = 0;         // 0 when every slot ran
    bool callback_failed = false;             // a recovered callback reported a visible failure
    bool ok() const noexcept { return blocked_on_rva == 0 && !callback_failed; }
};

CtorRunResult run_constructors(std::span<const CtorSlot> in_order, StartupState& state) noexcept;

// Recovered subset in original index order. There are no null callbacks: the first original index
// missing from this span is reported by first_unrecovered_constructor_index() in
// ctor_original_table.hpp, which is the explicit, machine-checked startup cliff.
std::span<const CtorSlot> recovered_constructors() noexcept;

// REGISTERED-AHEAD table: constructor-table indices 24..47 (the remaining 24 members of the 34-slot
// descriptor + mutex + atexit family), all with real registration callbacks, in original order.
//
// Convenience span of indices 24-47 (also registered in recovered_constructors). Focused tests may
// run this view alone; production path uses recovered_constructors().
std::span<const CtorSlot> registered_ahead_constructors() noexcept;

}  // namespace sotc
