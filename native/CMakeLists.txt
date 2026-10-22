cmake_minimum_required(VERSION 3.20)
project(sotc_native LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES arm64 CACHE STRING "native Apple Silicon only" FORCE)
add_compile_options(-Wall -Wextra -Werror -ffp-contract=off)   # recovered float code must not be fused
option(SOTC_SANITIZE "ASan+UBSan for tests" ON)
if(SOTC_SANITIZE)
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address,undefined)
endif()
add_library(sotc_core STATIC
  core/math_constants.cpp
  core/guarded_dd91b0.cpp
  core/guarded_dd7e30.cpp
  core/guarded_dd4210.cpp
  core/guarded_d39b00.cpp
  core/guarded_d366d0.cpp
  core/guarded_d33b50.cpp
  core/avx_d8fa40.cpp
  core/avx_d86ed0.cpp
  core/avx_d5a280.cpp
  core/leaf_cab080.cpp
  core/leaf_ca18d0.cpp
  core/leaf_c94ae0.cpp
  core/leaf_b692a0.cpp
  core/leaf_c811b0.cpp
  core/leaf_8a0440.cpp
  core/leaf_c05980.cpp
  core/leaf_c6f520.cpp
  core/leaf_c6e9d0.cpp
  core/leaf_a816d0.cpp
  core/leaf_3ca7c0.cpp
  core/leaf_c0d380.cpp
  core/leaf_c675f0.cpp
  core/leaf_c16b30.cpp
  core/leaf_8f3d90.cpp
  core/leaf_163a40.cpp
  core/leaf_9b75a0.cpp
  core/leaf_575d40.cpp
  core/leaf_99350.cpp
  core/leaf_83d800.cpp
  core/leaf_7ce8d0.cpp
  core/leaf_e71990.cpp
  core/slot23_names_fixture.cpp
  core/leaf_df94c0.cpp
  core/leaf_de0160.cpp
  core/fnv_cstr_161fb0.cpp
  core/fnv_path_cstr_162020.cpp
  core/hashed_name_ctors.cpp
  core/guarded_static_helpers.cpp
  core/atomic_add_ref_14ccb0.cpp
  core/atomic_xadd_b30cf0.cpp
  core/store_value_clear_flag_158340.cpp
  core/fnv_span_161f80.cpp
  core/hashed_string_14c3a0.cpp
  core/registry_object_14c9a0.cpp
  core/registry_object_2efd160.cpp
  core/registry_sorted_insert.cpp
  core/inputtable_names_fixture.cpp
  core/registry_fill_52a220.cpp
  core/registry_init_531d60.cpp
  core/appmain_init_registry_slice.cpp
  core/init_531d60_trivial_leaves.cpp
  core/appmain_init_remainder_slice.cpp
  core/xchg_store_u32_357f0.cpp
  core/base_initialize_255130.cpp
  core/appmain_init_base_initialize_slice.cpp
  core/init_flag_object_335a0.cpp
  core/prologue_255180.cpp
  core/store_tsc_323d0.cpp
  core/ensure_capacity_3c2f70.cpp
  core/init_sub_3be930.cpp
  core/ctor_40_1ab550.cpp
  core/prologue_gates_2552d4.cpp
  core/vslot_30_534130.cpp
  core/mid_255340.cpp
  core/singleton_f08_531d84.cpp
  core/vslot_40_533100_header.cpp
  core/lookup_1fc80.cpp
  core/vslot_40_533100_post.cpp
  core/vslot_40_533100_miss.cpp
  core/vslot_40_533100_join.cpp
  core/vslot_40_533100_gfx_2ee2d00.cpp
  core/vslot_40_533100_shadowmodels.cpp
  core/vslot_40_533100_shadow_attach.cpp
  core/vslot_40_533100_shadowskels.cpp
  core/vslot_40_533100_skels_attach.cpp
  core/vslot_40_533100_shadowanims.cpp
  core/vslot_40_533100_anims_attach.cpp
  core/vslot_40_533100_shadowbanks.cpp
  core/vslot_40_533100_banks_attach.cpp
  core/vslot_40_533100_shadowstreams.cpp
  core/vslot_40_533100_streams_attach.cpp
  core/vslot_40_533100_shadowrandomevents.cpp
  core/vslot_40_533100_randomevents_attach.cpp
  core/vslot_40_533100_shadowbossmusic.cpp
  core/vslot_40_533100_bossmusic_attach.cpp
  core/vslot_40_533100_cp3characters.cpp
  core/vslot_40_533100_cp3characters_attach.cpp
  core/vslot_40_533100_cp3shadowprecache.cpp
  core/vslot_40_533100_cp3shadowprecache_attach.cpp
  core/vslot_40_533100_tail_140d90.cpp
  core/vslot_40_533100_stackchk.cpp
  core/init_531d60_post_base_499f80.cpp
  core/init_531d60_post_base_56dcb0.cpp
  core/init_531d60_post_base_5c64c0.cpp
  core/init_531d60_post_base_1290.cpp
  core/init_531d60_post_base_56db70.cpp
  core/init_531d60_post_base_5b3d00.cpp
  core/init_531d60_post_base_5b3cf0.cpp
  core/init_531d60_post_base_avx_bss.cpp
  core/init_531d60_post_base_7f7220.cpp
  core/init_531d60_post_base_56fad0.cpp
  core/init_531d60_post_base_f90310.cpp
  core/init_531d60_post_base_ymm_bss.cpp
  core/init_531d60_post_base_ymm_intercell.cpp
  core/init_531d60_post_base_xmm_seed.cpp
  core/init_531d60_post_base_ymm_reload.cpp
  core/init_531d60_post_base_26800_f590.cpp
  core/init_531d60_post_base_sbj.cpp
  core/init_531d60_post_base_sboj.cpp
  core/init_531d60_post_base_52eb50.cpp
  core/init_531d60_post_base_lea_vmovq.cpp
  core/init_531d60_post_base_532ca0.cpp
  core/init_531d60_post_base_vcall18.cpp
  core/init_531d60_post_base_532ca0_u2.cpp
  core/init_531d60_post_base_vcall18_u2.cpp
  core/init_531d60_post_base_532ca0_u3.cpp
  core/init_531d60_post_base_vcall18_u3.cpp
  core/init_531d60_post_base_532ca0_u4.cpp
  core/init_531d60_post_base_vcall18_u4.cpp
  core/ctor_98a90_3be050.cpp
  core/appmain_init_prologue_slice.cpp
  core/appmain_init_prologue_tail_slice.cpp
  core/appmain_init_ctor_98a90_slice.cpp
  core/scratch_reset.cpp
  core/save_checksum.cpp
  core/name_hash.cpp
  core/save_chunk_header.cpp
  core/save_stream.cpp
  core/save_chunk_codec.cpp
  core/descriptor_registration.cpp
  core/const_table_ee1130.cpp
  core/input_table_551140.cpp
  core/recursive_mutex.cpp
  core/startup_ctors.cpp
  core/clothing_preview_param.cpp
  core/lazy_param_def_tree.cpp
  core/clothing_physical_mesh_parameters.cpp
  core/registration_thunks.cpp
  core/trig_constants.cpp
  core/addr.cpp
  core/allocator_region.cpp
  core/host_map.cpp
  core/page_owner_map.cpp
  core/page_allocator.cpp
  core/allocator_front.cpp
  core/free_list_carve.cpp
  core/region_split.cpp
  core/region_list.cpp
  core/arena_high_water.cpp
  core/arena_accounting.cpp
  core/exact_fit_finalize.cpp
  core/arena_page_debit.cpp
  core/vec_neg036.cpp
  core/xyz_mask_copy.cpp
  core/vec_dd41e0.cpp
  core/clear_flag32.cpp
  core/clear_flag64.cpp
  core/vec_b52110.cpp
  core/vec_b743c0.cpp
  core/vec_b747f0.cpp
  core/vec_b76950.cpp
  core/vec_ef85e0.cpp
  core/vec_e0b180.cpp
  core/vec_e085e0.cpp
  core/vec_e05320.cpp
  core/store_zero32.cpp
  core/zero_xmm16.cpp
  core/zero_xmm_qword.cpp
  core/init_f4fd40.cpp
  core/init_f4eba0.cpp
  core/init_f4b540.cpp
  core/init_ea52f0.cpp
  core/vec_e026b0.cpp
  core/vec_de6d20.cpp
  core/vec_dfd5f0.cpp
  core/vec_de1ad0.cpp
  core/vec_ddef00.cpp
  core/vec_de7fb0.cpp
  core/vec_ddab00.cpp
  core/vec_dddc90.cpp
  core/vec_dda990.cpp
  core/vec_dc4180.cpp
  core/vec_dc2d50.cpp
  core/vec_db3a70.cpp
  core/vec_db93a0.cpp
  core/vec_dbb280.cpp
  core/vec_db5dd0.cpp
  core/vec_db7e60.cpp
  core/vec_db3960.cpp
  core/vec_d90310.cpp
  core/vec_d94cb0.cpp
  core/vec_d853a0.cpp
  core/vec_d80e60.cpp
  core/vec_dac410.cpp
  core/vec_d7fe90.cpp
  core/vec_d84ee0.cpp
  core/vec_d7e640.cpp
  core/vec_d7fe40.cpp
  core/vec_d7c9d0.cpp
  core/vec_d7c4d0.cpp
  core/vec_d7c380.cpp
  core/vec_d7bc60.cpp
  core/vec_d7be60.cpp
  core/vec_d7b930.cpp
  core/vec_d6e0f0.cpp
  core/vec_d6dbf0.cpp
  core/vec_d6d890.cpp
  core/vec_d6d280.cpp
  core/vec_d6a520.cpp
  core/vec_d62b00.cpp
  core/vec_d62690.cpp
  core/vec_d623c0.cpp
  core/vec_d5fbe0.cpp
  core/vec_d47880.cpp
  core/vec_d47420.cpp
  core/vec_d47270.cpp
  core/vec_d42b60.cpp
  core/vec_d40ea0.cpp
  core/vec_d405a0.cpp
  core/vec_d3b790.cpp
  core/vec_d40010.cpp
  core/vec_d32b70.cpp
  core/vec_d3a6d0.cpp
  core/vec_d2ffe0.cpp
  core/vec_d32720.cpp
  core/vec_d2cb30.cpp
  core/vec_d19bb0.cpp
  core/vec_d2c7e0.cpp
  core/vec_d17c10.cpp
  core/vec_d19af0.cpp
  core/vec_d20590.cpp
  core/vec_d19930.cpp
  core/vec_d13300.cpp
  core/vec_d0bde0.cpp
  core/vec_d0a250.cpp
  core/vec_d09160.cpp
  core/vec_d082e0.cpp
  core/vec_d0bcd0.cpp
  core/vec_d08240.cpp
  core/vec_d06dd0.cpp
  core/vec_d00600.cpp
  core/vec_cfcc60.cpp
  core/vec_cfcbc0.cpp
  core/vec_d04ce0.cpp
  core/vec_cf44a0.cpp
  core/vec_cee8a0.cpp
  core/vec_ceb890.cpp
  core/vec_ceb2c0.cpp
  core/vec_ceb1c0.cpp
  core/vec_ceb070.cpp
  core/vec_ce99c0.cpp
  core/vec_ce74a0.cpp
  core/vec_ce7300.cpp
  core/vec_cdc760.cpp
  core/vec_ce57b0.cpp
  core/vec_ce0890.cpp
  core/vec_ce6fb0.cpp
  core/vec_ce8440.cpp
  core/vec_ce67b0.cpp
  core/vec_ce6130.cpp
  core/vec_ce38d0.cpp
  core/vec_cde740.cpp
  core/vec_cd4d60.cpp
  core/vec_cd0420.cpp
  core/vec_cc8ac0.cpp
  core/vec_ccd0f0.cpp
  core/vec_cceaa0.cpp
  core/vec_cc8330.cpp
  core/vec_cc5a10.cpp
  core/vec_cc07d0.cpp
  core/vec_caeed0.cpp
  core/vec_cb5cb0.cpp
  core/vec_cbb2c0.cpp
  core/vec_cae740.cpp
  core/vec_cae240.cpp
  core/vec_cad450.cpp
  core/vec_ca4dc0.cpp
  core/vec_c9e8e0.cpp
  core/vec_c91b10.cpp
  core/vec_c891e0.cpp
  core/vec_c7de60.cpp
  core/vec_c7c310.cpp
  core/vec_c786c0.cpp
  core/vec_c77070.cpp
  core/vec_c741c0.cpp
  core/vec_c70e20.cpp
  core/vec_c6fdf0.cpp
  core/vec_c6d910.cpp
  core/vec_c6bad0.cpp
  core/vec_c69f70.cpp
  core/vec_c686e0.cpp
  core/vec_c66580.cpp
  core/vec_c65f80.cpp
  core/vec_c65da0.cpp
  core/vec_c64060.cpp
  core/vec_c61670.cpp
  core/vec_c5f6f0.cpp
  core/vec_c5c320.cpp
  core/vec_c5c1a0.cpp
  core/vec_c5c040.cpp
  core/vec_c5a400.cpp
  core/vec_c5a100.cpp
  core/vec_c555c0.cpp
  core/vec_c54d20.cpp
  core/vec_c54830.cpp
  core/vec_c53db0.cpp
  core/vec_c4e5e0.cpp
  core/vec_c4aa80.cpp
  core/vec_c47e00.cpp
  core/vec_c451d0.cpp
  core/vec_c44830.cpp
  core/vec_c42b00.cpp
  core/vec_c3eaa0.cpp
  core/vec_c3cf00.cpp
  core/vec_c37020.cpp
  core/vec_c36a80.cpp
  core/vec_c23650.cpp
  core/vec_c22300.cpp
  core/vec_c20710.cpp
  core/vec_c1a9a0.cpp
  core/vec_c191a0.cpp
  core/vec_c17a70.cpp
  core/vec_c152c0.cpp
  core/vec_c14f60.cpp
  core/vec_c0d720.cpp
  core/vec_c0bea0.cpp
  core/vec_bd7a80.cpp
  core/vec_be4ff0.cpp
  core/vec_be50a0.cpp
  core/vec_be5270.cpp
  core/vec_be5410.cpp
  core/vec_c060a0.cpp
  core/vec_c0bbd0.cpp
  core/vec_c0bb00.cpp
  core/vec_c0baa0.cpp
  core/vec_c0aae0.cpp
  core/vec_c09070.cpp
  core/vec_e094e0.cpp
  core/vec_dbfa60.cpp
  core/vec_d85490.cpp
  core/vec_bd7950.cpp
  core/vec_bd60a0.cpp
  core/vec_bd4dd0.cpp
  core/vec_bd32f0.cpp
  core/vec_bd3190.cpp
  core/vec_bd06c0.cpp
  core/vec_bceda0.cpp
  core/vec_bccc00.cpp
  core/vec_bcc960.cpp
  core/vec_bcc4e0.cpp
  core/vec_bcc3f0.cpp
  core/vec_bc8210.cpp
  core/vec_bc7280.cpp
  core/vec_bc5b00.cpp
  core/vec_bc2800.cpp
  core/vec_bc07f0.cpp
  core/vec_bc0670.cpp
  core/vec_bbf580.cpp
  core/vec_bbd5e0.cpp
  core/vec_bbba10.cpp
  core/vec_bb9740.cpp
  core/vec_bb82d0.cpp
  core/vec_bb7990.cpp
  core/vec_bacfb0.cpp
  core/vec_bace80.cpp
  core/vec_bace50.cpp
  core/vec_bacb30.cpp
  core/vec_bab950.cpp
  core/vec_ba3ef0.cpp
  core/vec_b950e0.cpp
  core/vec_b8d170.cpp
  core/vec_b89eb0.cpp
  core/vec_b84580.cpp
  core/vec_b7b270.cpp
  core/vec_b77e90.cpp
  core/vec_b77910.cpp
  core/vec_b77740.cpp
  core/vec_b71f80.cpp
  core/vec_b6e6d0.cpp
  core/vec_b67a70.cpp
  core/vec_b5ecf0.cpp
  core/vec_b58690.cpp
  core/vec_b57360.cpp
  core/vec_b4ef70.cpp
  core/vec_b494d0.cpp
  core/vec_b47100.cpp
  core/vec_b44580.cpp
  core/vec_b33bd0.cpp
)
target_include_directories(sotc_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
enable_testing()
add_executable(test_math_constants tests/test_math_constants.cpp)
target_link_libraries(test_math_constants sotc_core)
add_test(NAME math_constants COMMAND test_math_constants)
add_executable(test_scratch_reset tests/test_scratch_reset.cpp)
target_link_libraries(test_scratch_reset sotc_core)
add_test(NAME scratch_reset COMMAND test_scratch_reset)
add_executable(test_startup_ctors tests/test_startup_ctors.cpp)
target_link_libraries(test_startup_ctors sotc_core)
add_test(NAME startup_ctors COMMAND test_startup_ctors)
set_tests_properties(startup_ctors PROPERTIES
  ENVIRONMENT "SOTC_SLOT23_NAMES_FIXTURE=${CMAKE_SOURCE_DIR}/../local/fixtures/slot23_e71990_names.bin")
add_executable(test_slot23_names_fixture tests/test_slot23_names_fixture.cpp)
target_link_libraries(test_slot23_names_fixture sotc_core)
add_test(NAME slot23_names_fixture COMMAND test_slot23_names_fixture)
set_tests_properties(slot23_names_fixture PROPERTIES
  ENVIRONMENT "SOTC_SLOT23_NAMES_FIXTURE=${CMAKE_SOURCE_DIR}/../local/fixtures/slot23_e71990_names.bin")

add_executable(test_clothing_preview_param tests/test_clothing_preview_param.cpp)
target_link_libraries(test_clothing_preview_param sotc_core)
add_test(NAME clothing_preview_param COMMAND test_clothing_preview_param)
add_executable(test_lazy_param_def_tree tests/test_lazy_param_def_tree.cpp)
target_link_libraries(test_lazy_param_def_tree sotc_core)
add_test(NAME lazy_param_def_tree COMMAND test_lazy_param_def_tree)
add_executable(test_clothing_physical_mesh_parameters tests/test_clothing_physical_mesh_parameters.cpp)
target_link_libraries(test_clothing_physical_mesh_parameters sotc_core)
add_test(NAME clothing_physical_mesh_parameters COMMAND test_clothing_physical_mesh_parameters)
add_executable(test_registration_thunks tests/test_registration_thunks.cpp)
target_link_libraries(test_registration_thunks sotc_core)
add_test(NAME registration_thunks COMMAND test_registration_thunks)
add_executable(test_registration_thunks_ahead tests/test_registration_thunks_ahead.cpp)
target_link_libraries(test_registration_thunks_ahead sotc_core)
add_test(NAME registration_thunks_ahead COMMAND test_registration_thunks_ahead)
set_tests_properties(registration_thunks_ahead PROPERTIES
  ENVIRONMENT "SOTC_SLOT23_NAMES_FIXTURE=${CMAKE_SOURCE_DIR}/../local/fixtures/slot23_e71990_names.bin")
add_executable(test_trig_constants tests/test_trig_constants.cpp)
target_link_libraries(test_trig_constants sotc_core)
add_test(NAME trig_constants COMMAND test_trig_constants)
add_executable(test_normal_threshold tests/test_normal_threshold.cpp)
target_link_libraries(test_normal_threshold sotc_core)
add_test(NAME normal_threshold COMMAND test_normal_threshold)
add_executable(test_addr tests/test_addr.cpp)
target_link_libraries(test_addr sotc_core)
add_test(NAME addr COMMAND test_addr)
add_executable(test_allocator_region tests/test_allocator_region.cpp)
target_link_libraries(test_allocator_region sotc_core)
add_test(NAME allocator_region COMMAND test_allocator_region)
add_executable(test_arena_accounting tests/test_arena_accounting.cpp)
target_link_libraries(test_arena_accounting sotc_core)
add_test(NAME arena_accounting COMMAND test_arena_accounting)
add_executable(test_vec_neg036 tests/test_vec_neg036.cpp)
target_link_libraries(test_vec_neg036 sotc_core)
add_test(NAME vec_neg036 COMMAND test_vec_neg036)
add_executable(test_xyz_mask_copy tests/test_xyz_mask_copy.cpp)
target_link_libraries(test_xyz_mask_copy sotc_core)
add_test(NAME xyz_mask_copy COMMAND test_xyz_mask_copy)
add_executable(test_vec_dd41e0 tests/test_vec_dd41e0.cpp)
target_link_libraries(test_vec_dd41e0 sotc_core)
add_test(NAME vec_dd41e0 COMMAND test_vec_dd41e0)
add_executable(test_clear_flag32 tests/test_clear_flag32.cpp)
target_link_libraries(test_clear_flag32 sotc_core)
add_test(NAME clear_flag32 COMMAND test_clear_flag32)
add_executable(test_clear_flag64 tests/test_clear_flag64.cpp)
target_link_libraries(test_clear_flag64 sotc_core)
add_test(NAME clear_flag64 COMMAND test_clear_flag64)
add_executable(test_vec_b52110 tests/test_vec_b52110.cpp)
target_link_libraries(test_vec_b52110 sotc_core)
add_test(NAME vec_b52110 COMMAND test_vec_b52110)
add_executable(test_vec_b743c0 tests/test_vec_b743c0.cpp)
target_link_libraries(test_vec_b743c0 sotc_core)
add_test(NAME vec_b743c0 COMMAND test_vec_b743c0)
add_executable(test_vec_b747f0 tests/test_vec_b747f0.cpp)
target_link_libraries(test_vec_b747f0 sotc_core)
add_test(NAME vec_b747f0 COMMAND test_vec_b747f0)
add_executable(test_vec_b76950 tests/test_vec_b76950.cpp)
target_link_libraries(test_vec_b76950 sotc_core)
add_test(NAME vec_b76950 COMMAND test_vec_b76950)
add_executable(test_vec_ef85e0 tests/test_vec_ef85e0.cpp)
target_link_libraries(test_vec_ef85e0 sotc_core)
add_test(NAME vec_ef85e0 COMMAND test_vec_ef85e0)
add_executable(test_vec_e0b180 tests/test_vec_e0b180.cpp)
target_link_libraries(test_vec_e0b180 sotc_core)
add_test(NAME vec_e0b180 COMMAND test_vec_e0b180)
add_executable(test_vec_e085e0 tests/test_vec_e085e0.cpp)
target_link_libraries(test_vec_e085e0 sotc_core)
add_test(NAME vec_e085e0 COMMAND test_vec_e085e0)
add_executable(test_vec_e05320 tests/test_vec_e05320.cpp)
target_link_libraries(test_vec_e05320 sotc_core)
add_test(NAME vec_e05320 COMMAND test_vec_e05320)
add_executable(test_store_zero32 tests/test_store_zero32.cpp)
target_link_libraries(test_store_zero32 sotc_core)
add_test(NAME store_zero32 COMMAND test_store_zero32)
add_executable(test_zero_xmm16 tests/test_zero_xmm16.cpp)
target_link_libraries(test_zero_xmm16 sotc_core)
add_test(NAME zero_xmm16 COMMAND test_zero_xmm16)
add_executable(test_init_f4fd40 tests/test_init_f4fd40.cpp)
target_link_libraries(test_init_f4fd40 sotc_core)
add_test(NAME init_f4fd40 COMMAND test_init_f4fd40)
add_executable(test_init_f4eba0 tests/test_init_f4eba0.cpp)
target_link_libraries(test_init_f4eba0 sotc_core)
add_test(NAME init_f4eba0 COMMAND test_init_f4eba0)
add_executable(test_init_f4b540 tests/test_init_f4b540.cpp)
target_link_libraries(test_init_f4b540 sotc_core)
add_test(NAME init_f4b540 COMMAND test_init_f4b540)
add_executable(test_init_ea52f0 tests/test_init_ea52f0.cpp)
target_link_libraries(test_init_ea52f0 sotc_core)
add_test(NAME init_ea52f0 COMMAND test_init_ea52f0)
add_executable(test_zero_xmm_qword tests/test_zero_xmm_qword.cpp)
target_link_libraries(test_zero_xmm_qword sotc_core)
add_test(NAME zero_xmm_qword COMMAND test_zero_xmm_qword)
add_executable(test_vec_e026b0 tests/test_vec_e026b0.cpp)
target_link_libraries(test_vec_e026b0 sotc_core)
add_test(NAME vec_e026b0 COMMAND test_vec_e026b0)
add_executable(test_vec_de6d20 tests/test_vec_de6d20.cpp)
target_link_libraries(test_vec_de6d20 sotc_core)
add_test(NAME vec_de6d20 COMMAND test_vec_de6d20)
add_executable(test_vec_dfd5f0 tests/test_vec_dfd5f0.cpp)
target_link_libraries(test_vec_dfd5f0 sotc_core)
add_test(NAME vec_dfd5f0 COMMAND test_vec_dfd5f0)
add_executable(test_vec_de1ad0 tests/test_vec_de1ad0.cpp)
target_link_libraries(test_vec_de1ad0 sotc_core)
add_test(NAME vec_de1ad0 COMMAND test_vec_de1ad0)
add_executable(test_host_map tests/test_host_map.cpp)
target_link_libraries(test_host_map sotc_core)
add_test(NAME host_map COMMAND test_host_map)
add_executable(test_vec_ddef00 tests/test_vec_ddef00.cpp)
target_link_libraries(test_vec_ddef00 sotc_core)
add_test(NAME vec_ddef00 COMMAND test_vec_ddef00)

add_executable(test_vec_de7fb0 tests/test_vec_de7fb0.cpp)
target_link_libraries(test_vec_de7fb0 sotc_core)
add_test(NAME vec_de7fb0 COMMAND test_vec_de7fb0)
add_executable(test_vec_dddc90 tests/test_vec_dddc90.cpp)
target_link_libraries(test_vec_dddc90 sotc_core)
add_test(NAME vec_dddc90 COMMAND test_vec_dddc90)
add_executable(test_vec_ddab00 tests/test_vec_ddab00.cpp)
target_link_libraries(test_vec_ddab00 sotc_core)
add_test(NAME vec_ddab00 COMMAND test_vec_ddab00)
add_executable(test_vec_dda990 tests/test_vec_dda990.cpp)
target_link_libraries(test_vec_dda990 sotc_core)
add_test(NAME vec_dda990 COMMAND test_vec_dda990)
add_executable(test_vec_dc4180 tests/test_vec_dc4180.cpp)
target_link_libraries(test_vec_dc4180 sotc_core)
add_test(NAME vec_dc4180 COMMAND test_vec_dc4180)
add_executable(test_free_list_carve tests/test_free_list_carve.cpp)
target_link_libraries(test_free_list_carve sotc_core)
add_test(NAME free_list_carve COMMAND test_free_list_carve)

add_executable(test_vec_dc2d50 tests/test_vec_dc2d50.cpp)
target_link_libraries(test_vec_dc2d50 sotc_core)
add_test(NAME vec_dc2d50 COMMAND test_vec_dc2d50)
add_executable(test_vec_db3a70 tests/test_vec_db3a70.cpp)
target_link_libraries(test_vec_db3a70 sotc_core)
add_test(NAME vec_db3a70 COMMAND test_vec_db3a70)
add_executable(test_vec_db93a0 tests/test_vec_db93a0.cpp)
target_link_libraries(test_vec_db93a0 sotc_core)
add_test(NAME vec_db93a0 COMMAND test_vec_db93a0)

add_executable(test_vec_dbb280 tests/test_vec_dbb280.cpp)
target_link_libraries(test_vec_dbb280 sotc_core)
add_test(NAME vec_dbb280 COMMAND test_vec_dbb280)
add_executable(test_vec_db5dd0 tests/test_vec_db5dd0.cpp)
target_link_libraries(test_vec_db5dd0 sotc_core)
add_test(NAME vec_db5dd0 COMMAND test_vec_db5dd0)

add_executable(test_vec_db7e60 tests/test_vec_db7e60.cpp)
target_link_libraries(test_vec_db7e60 sotc_core)
add_test(NAME vec_db7e60 COMMAND test_vec_db7e60)

add_executable(test_vec_db3960 tests/test_vec_db3960.cpp)
target_link_libraries(test_vec_db3960 sotc_core)
add_test(NAME vec_db3960 COMMAND test_vec_db3960)
add_executable(test_region_split tests/test_region_split.cpp)
target_link_libraries(test_region_split sotc_core)
add_test(NAME region_split COMMAND test_region_split)

add_executable(test_vec_d90310 tests/test_vec_d90310.cpp)
target_link_libraries(test_vec_d90310 sotc_core)
add_test(NAME vec_d90310 COMMAND test_vec_d90310)
add_executable(test_vec_d94cb0 tests/test_vec_d94cb0.cpp)
target_link_libraries(test_vec_d94cb0 sotc_core)
add_test(NAME vec_d94cb0 COMMAND test_vec_d94cb0)

add_executable(test_vec_d853a0 tests/test_vec_d853a0.cpp)
target_link_libraries(test_vec_d853a0 sotc_core)
add_test(NAME vec_d853a0 COMMAND test_vec_d853a0)
add_executable(test_region_list tests/test_region_list.cpp)
target_link_libraries(test_region_list sotc_core)
add_test(NAME region_list COMMAND test_region_list)

add_executable(test_arena_high_water tests/test_arena_high_water.cpp)
target_link_libraries(test_arena_high_water sotc_core)
add_test(NAME arena_high_water COMMAND test_arena_high_water)

add_executable(test_exact_fit_finalize tests/test_exact_fit_finalize.cpp)
target_link_libraries(test_exact_fit_finalize sotc_core)
add_test(NAME exact_fit_finalize COMMAND test_exact_fit_finalize)

add_executable(test_arena_page_debit tests/test_arena_page_debit.cpp)
target_link_libraries(test_arena_page_debit sotc_core)
add_test(NAME arena_page_debit COMMAND test_arena_page_debit)
add_executable(test_vec_d80e60 tests/test_vec_d80e60.cpp)
target_link_libraries(test_vec_d80e60 sotc_core)
add_test(NAME vec_d80e60 COMMAND test_vec_d80e60)
add_executable(test_vec_dac410 tests/test_vec_dac410.cpp)
target_link_libraries(test_vec_dac410 sotc_core)
add_test(NAME vec_dac410 COMMAND test_vec_dac410)

add_executable(test_vec_d7fe90 tests/test_vec_d7fe90.cpp)
target_link_libraries(test_vec_d7fe90 sotc_core)
add_test(NAME vec_d7fe90 COMMAND test_vec_d7fe90)
add_executable(test_vec_d84ee0 tests/test_vec_d84ee0.cpp)
target_link_libraries(test_vec_d84ee0 sotc_core)
add_test(NAME vec_d84ee0 COMMAND test_vec_d84ee0)

add_executable(test_vec_d7e640 tests/test_vec_d7e640.cpp)
target_link_libraries(test_vec_d7e640 sotc_core)
add_test(NAME vec_d7e640 COMMAND test_vec_d7e640)

add_executable(test_vec_d7fe40 tests/test_vec_d7fe40.cpp)
target_link_libraries(test_vec_d7fe40 sotc_core)
add_test(NAME vec_d7fe40 COMMAND test_vec_d7fe40)

add_executable(test_vec_d7c9d0 tests/test_vec_d7c9d0.cpp)
target_link_libraries(test_vec_d7c9d0 sotc_core)
add_test(NAME vec_d7c9d0 COMMAND test_vec_d7c9d0)

add_executable(test_vec_d7c380 tests/test_vec_d7c380.cpp)
target_link_libraries(test_vec_d7c380 sotc_core)
add_test(NAME vec_d7c380 COMMAND test_vec_d7c380)
add_executable(test_vec_d7c4d0 tests/test_vec_d7c4d0.cpp)
target_link_libraries(test_vec_d7c4d0 sotc_core)
add_test(NAME vec_d7c4d0 COMMAND test_vec_d7c4d0)

add_executable(test_vec_d7bc60 tests/test_vec_d7bc60.cpp)
target_link_libraries(test_vec_d7bc60 sotc_core)
add_test(NAME vec_d7bc60 COMMAND test_vec_d7bc60)

add_executable(test_vec_d7be60 tests/test_vec_d7be60.cpp)
target_link_libraries(test_vec_d7be60 sotc_core)
add_test(NAME vec_d7be60 COMMAND test_vec_d7be60)
add_executable(test_vec_d7b930 tests/test_vec_d7b930.cpp)
target_link_libraries(test_vec_d7b930 sotc_core)
add_test(NAME vec_d7b930 COMMAND test_vec_d7b930)

add_executable(test_vec_d6e0f0 tests/test_vec_d6e0f0.cpp)
target_link_libraries(test_vec_d6e0f0 sotc_core)
add_test(NAME vec_d6e0f0 COMMAND test_vec_d6e0f0)

add_executable(test_vec_d6dbf0 tests/test_vec_d6dbf0.cpp)
target_link_libraries(test_vec_d6dbf0 sotc_core)
add_test(NAME vec_d6dbf0 COMMAND test_vec_d6dbf0)

add_executable(test_vec_d6d890 tests/test_vec_d6d890.cpp)
target_link_libraries(test_vec_d6d890 sotc_core)
add_test(NAME vec_d6d890 COMMAND test_vec_d6d890)

add_executable(test_vec_d6d280 tests/test_vec_d6d280.cpp)
target_link_libraries(test_vec_d6d280 sotc_core)
add_test(NAME vec_d6d280 COMMAND test_vec_d6d280)

add_executable(test_vec_d6a520 tests/test_vec_d6a520.cpp)
target_link_libraries(test_vec_d6a520 sotc_core)
add_test(NAME vec_d6a520 COMMAND test_vec_d6a520)

add_executable(test_vec_d62b00 tests/test_vec_d62b00.cpp)
target_link_libraries(test_vec_d62b00 sotc_core)
add_test(NAME vec_d62b00 COMMAND test_vec_d62b00)

add_executable(test_vec_d62690 tests/test_vec_d62690.cpp)
target_link_libraries(test_vec_d62690 sotc_core)
add_test(NAME vec_d62690 COMMAND test_vec_d62690)

add_executable(test_vec_d5fbe0 tests/test_vec_d5fbe0.cpp)
target_link_libraries(test_vec_d5fbe0 sotc_core)
add_test(NAME vec_d5fbe0 COMMAND test_vec_d5fbe0)

add_executable(test_vec_d47880 tests/test_vec_d47880.cpp)
target_link_libraries(test_vec_d47880 sotc_core)
add_test(NAME vec_d47880 COMMAND test_vec_d47880)

add_executable(test_vec_d47420 tests/test_vec_d47420.cpp)
target_link_libraries(test_vec_d47420 sotc_core)
add_test(NAME vec_d47420 COMMAND test_vec_d47420)

add_executable(test_vec_d47270 tests/test_vec_d47270.cpp)
target_link_libraries(test_vec_d47270 sotc_core)
add_test(NAME vec_d47270 COMMAND test_vec_d47270)

add_executable(test_vec_d42b60 tests/test_vec_d42b60.cpp)
target_link_libraries(test_vec_d42b60 sotc_core)
add_test(NAME vec_d42b60 COMMAND test_vec_d42b60)

add_executable(test_vec_d40ea0 tests/test_vec_d40ea0.cpp)
target_link_libraries(test_vec_d40ea0 sotc_core)
add_test(NAME vec_d40ea0 COMMAND test_vec_d40ea0)

add_executable(test_vec_d405a0 tests/test_vec_d405a0.cpp)
target_link_libraries(test_vec_d405a0 sotc_core)
add_test(NAME vec_d405a0 COMMAND test_vec_d405a0)

add_executable(test_vec_d623c0 tests/test_vec_d623c0.cpp)
target_link_libraries(test_vec_d623c0 sotc_core)
add_test(NAME vec_d623c0 COMMAND test_vec_d623c0)

add_executable(test_vec_d3b790 tests/test_vec_d3b790.cpp)
target_link_libraries(test_vec_d3b790 sotc_core)
add_test(NAME vec_d3b790 COMMAND test_vec_d3b790)

add_executable(test_vec_d40010 tests/test_vec_d40010.cpp)
target_link_libraries(test_vec_d40010 sotc_core)
add_test(NAME vec_d40010 COMMAND test_vec_d40010)

add_executable(test_vec_d32b70 tests/test_vec_d32b70.cpp)
target_link_libraries(test_vec_d32b70 sotc_core)
add_test(NAME vec_d32b70 COMMAND test_vec_d32b70)

add_executable(test_vec_d3a6d0 tests/test_vec_d3a6d0.cpp)
target_link_libraries(test_vec_d3a6d0 sotc_core)
add_test(NAME vec_d3a6d0 COMMAND test_vec_d3a6d0)

add_executable(test_vec_d2ffe0 tests/test_vec_d2ffe0.cpp)
target_link_libraries(test_vec_d2ffe0 sotc_core)
add_test(NAME vec_d2ffe0 COMMAND test_vec_d2ffe0)

add_executable(test_vec_d32720 tests/test_vec_d32720.cpp)
target_link_libraries(test_vec_d32720 sotc_core)
add_test(NAME vec_d32720 COMMAND test_vec_d32720)

add_executable(test_vec_d2cb30 tests/test_vec_d2cb30.cpp)
target_link_libraries(test_vec_d2cb30 sotc_core)
add_test(NAME vec_d2cb30 COMMAND test_vec_d2cb30)

add_executable(test_vec_d19bb0 tests/test_vec_d19bb0.cpp)
target_link_libraries(test_vec_d19bb0 sotc_core)
add_test(NAME vec_d19bb0 COMMAND test_vec_d19bb0)

add_executable(test_vec_d2c7e0 tests/test_vec_d2c7e0.cpp)
target_link_libraries(test_vec_d2c7e0 sotc_core)
add_test(NAME vec_d2c7e0 COMMAND test_vec_d2c7e0)

add_executable(test_vec_d17c10 tests/test_vec_d17c10.cpp)
target_link_libraries(test_vec_d17c10 sotc_core)
add_test(NAME vec_d17c10 COMMAND test_vec_d17c10)

add_executable(test_vec_d19af0 tests/test_vec_d19af0.cpp)
target_link_libraries(test_vec_d19af0 sotc_core)
add_test(NAME vec_d19af0 COMMAND test_vec_d19af0)
add_executable(test_vec_d20590 tests/test_vec_d20590.cpp)
target_link_libraries(test_vec_d20590 sotc_core)
add_test(NAME vec_d20590 COMMAND test_vec_d20590)

add_executable(test_vec_d19930 tests/test_vec_d19930.cpp)
target_link_libraries(test_vec_d19930 sotc_core)
add_test(NAME vec_d19930 COMMAND test_vec_d19930)

add_executable(test_vec_d13300 tests/test_vec_d13300.cpp)
target_link_libraries(test_vec_d13300 sotc_core)
add_test(NAME vec_d13300 COMMAND test_vec_d13300)

add_executable(test_vec_d0bde0 tests/test_vec_d0bde0.cpp)
target_link_libraries(test_vec_d0bde0 sotc_core)
add_test(NAME vec_d0bde0 COMMAND test_vec_d0bde0)

add_executable(test_vec_d0a250 tests/test_vec_d0a250.cpp)
target_link_libraries(test_vec_d0a250 sotc_core)
add_test(NAME vec_d0a250 COMMAND test_vec_d0a250)

add_executable(test_vec_d09160 tests/test_vec_d09160.cpp)
target_link_libraries(test_vec_d09160 sotc_core)
add_test(NAME vec_d09160 COMMAND test_vec_d09160)

add_executable(test_vec_d082e0 tests/test_vec_d082e0.cpp)
target_link_libraries(test_vec_d082e0 sotc_core)
add_test(NAME vec_d082e0 COMMAND test_vec_d082e0)
add_executable(test_vec_d0bcd0 tests/test_vec_d0bcd0.cpp)
target_link_libraries(test_vec_d0bcd0 sotc_core)
add_test(NAME vec_d0bcd0 COMMAND test_vec_d0bcd0)

add_executable(test_vec_d08240 tests/test_vec_d08240.cpp)
target_link_libraries(test_vec_d08240 sotc_core)
add_test(NAME vec_d08240 COMMAND test_vec_d08240)

add_executable(test_vec_d06dd0 tests/test_vec_d06dd0.cpp)
target_link_libraries(test_vec_d06dd0 sotc_core)
add_test(NAME vec_d06dd0 COMMAND test_vec_d06dd0)

add_executable(test_vec_d00600 tests/test_vec_d00600.cpp)
target_link_libraries(test_vec_d00600 sotc_core)
add_test(NAME vec_d00600 COMMAND test_vec_d00600)

add_executable(test_vec_cfcc60 tests/test_vec_cfcc60.cpp)
target_link_libraries(test_vec_cfcc60 sotc_core)
add_test(NAME vec_cfcc60 COMMAND test_vec_cfcc60)

add_executable(test_vec_cfcbc0 tests/test_vec_cfcbc0.cpp)
target_link_libraries(test_vec_cfcbc0 sotc_core)
add_test(NAME vec_cfcbc0 COMMAND test_vec_cfcbc0)

add_executable(test_vec_d04ce0 tests/test_vec_d04ce0.cpp)
target_link_libraries(test_vec_d04ce0 sotc_core)
add_test(NAME vec_d04ce0 COMMAND test_vec_d04ce0)

add_executable(test_vec_cf44a0 tests/test_vec_cf44a0.cpp)
target_link_libraries(test_vec_cf44a0 sotc_core)
add_test(NAME vec_cf44a0 COMMAND test_vec_cf44a0)
add_executable(test_vec_cee8a0 tests/test_vec_cee8a0.cpp)
target_link_libraries(test_vec_cee8a0 sotc_core)
add_test(NAME vec_cee8a0 COMMAND test_vec_cee8a0)

add_executable(test_vec_ceb890 tests/test_vec_ceb890.cpp)
target_link_libraries(test_vec_ceb890 sotc_core)
add_test(NAME vec_ceb890 COMMAND test_vec_ceb890)

add_executable(test_vec_ceb2c0 tests/test_vec_ceb2c0.cpp)
target_link_libraries(test_vec_ceb2c0 sotc_core)
add_test(NAME vec_ceb2c0 COMMAND test_vec_ceb2c0)

add_executable(test_vec_ceb1c0 tests/test_vec_ceb1c0.cpp)
target_link_libraries(test_vec_ceb1c0 sotc_core)
add_test(NAME vec_ceb1c0 COMMAND test_vec_ceb1c0)

add_executable(test_vec_ceb070 tests/test_vec_ceb070.cpp)
target_link_libraries(test_vec_ceb070 sotc_core)
add_test(NAME vec_ceb070 COMMAND test_vec_ceb070)

add_executable(test_vec_ce99c0 tests/test_vec_ce99c0.cpp)
target_link_libraries(test_vec_ce99c0 sotc_core)
add_test(NAME vec_ce99c0 COMMAND test_vec_ce99c0)

add_executable(test_vec_ce74a0 tests/test_vec_ce74a0.cpp)
target_link_libraries(test_vec_ce74a0 sotc_core)
add_test(NAME vec_ce74a0 COMMAND test_vec_ce74a0)

add_executable(test_vec_ce7300 tests/test_vec_ce7300.cpp)
target_link_libraries(test_vec_ce7300 sotc_core)
add_test(NAME vec_ce7300 COMMAND test_vec_ce7300)

add_executable(test_vec_cdc760 tests/test_vec_cdc760.cpp)
target_link_libraries(test_vec_cdc760 sotc_core)
add_test(NAME vec_cdc760 COMMAND test_vec_cdc760)

add_executable(test_vec_ce57b0 tests/test_vec_ce57b0.cpp)
target_link_libraries(test_vec_ce57b0 sotc_core)
add_test(NAME vec_ce57b0 COMMAND test_vec_ce57b0)

add_executable(test_vec_ce0890 tests/test_vec_ce0890.cpp)
target_link_libraries(test_vec_ce0890 sotc_core)
add_test(NAME vec_ce0890 COMMAND test_vec_ce0890)

add_executable(test_vec_ce6fb0 tests/test_vec_ce6fb0.cpp)
target_link_libraries(test_vec_ce6fb0 sotc_core)
add_test(NAME vec_ce6fb0 COMMAND test_vec_ce6fb0)

add_executable(test_vec_ce8440 tests/test_vec_ce8440.cpp)
target_link_libraries(test_vec_ce8440 sotc_core)
add_test(NAME vec_ce8440 COMMAND test_vec_ce8440)

add_executable(test_vec_ce67b0 tests/test_vec_ce67b0.cpp)
target_link_libraries(test_vec_ce67b0 sotc_core)
add_test(NAME vec_ce67b0 COMMAND test_vec_ce67b0)

add_executable(test_vec_ce6130 tests/test_vec_ce6130.cpp)
target_link_libraries(test_vec_ce6130 sotc_core)
add_test(NAME vec_ce6130 COMMAND test_vec_ce6130)

add_executable(test_vec_ce38d0 tests/test_vec_ce38d0.cpp)
target_link_libraries(test_vec_ce38d0 sotc_core)
add_test(NAME vec_ce38d0 COMMAND test_vec_ce38d0)

add_executable(test_vec_cde740 tests/test_vec_cde740.cpp)
target_link_libraries(test_vec_cde740 sotc_core)
add_test(NAME vec_cde740 COMMAND test_vec_cde740)

add_executable(test_vec_cd4d60 tests/test_vec_cd4d60.cpp)
target_link_libraries(test_vec_cd4d60 sotc_core)
add_test(NAME vec_cd4d60 COMMAND test_vec_cd4d60)

add_executable(test_vec_cd0420 tests/test_vec_cd0420.cpp)
target_link_libraries(test_vec_cd0420 sotc_core)
add_test(NAME vec_cd0420 COMMAND test_vec_cd0420)

add_executable(test_vec_cceaa0 tests/test_vec_cceaa0.cpp)
target_link_libraries(test_vec_cceaa0 sotc_core)
add_test(NAME vec_cceaa0 COMMAND test_vec_cceaa0)

add_executable(test_vec_ccd0f0 tests/test_vec_ccd0f0.cpp)
target_link_libraries(test_vec_ccd0f0 sotc_core)
add_test(NAME vec_ccd0f0 COMMAND test_vec_ccd0f0)

add_executable(test_vec_cc8ac0 tests/test_vec_cc8ac0.cpp)
target_link_libraries(test_vec_cc8ac0 sotc_core)
add_test(NAME vec_cc8ac0 COMMAND test_vec_cc8ac0)

add_executable(test_vec_cc8330 tests/test_vec_cc8330.cpp)
target_link_libraries(test_vec_cc8330 sotc_core)
add_test(NAME vec_cc8330 COMMAND test_vec_cc8330)

add_executable(test_vec_cc5a10 tests/test_vec_cc5a10.cpp)
target_link_libraries(test_vec_cc5a10 sotc_core)
add_test(NAME vec_cc5a10 COMMAND test_vec_cc5a10)

add_executable(test_vec_cc07d0 tests/test_vec_cc07d0.cpp)
target_link_libraries(test_vec_cc07d0 sotc_core)
add_test(NAME vec_cc07d0 COMMAND test_vec_cc07d0)

add_executable(test_vec_cbb2c0 tests/test_vec_cbb2c0.cpp)
target_link_libraries(test_vec_cbb2c0 sotc_core)
add_test(NAME vec_cbb2c0 COMMAND test_vec_cbb2c0)

add_executable(test_vec_cb5cb0 tests/test_vec_cb5cb0.cpp)
target_link_libraries(test_vec_cb5cb0 sotc_core)
add_test(NAME vec_cb5cb0 COMMAND test_vec_cb5cb0)

add_executable(test_vec_caeed0 tests/test_vec_caeed0.cpp)
target_link_libraries(test_vec_caeed0 sotc_core)
add_test(NAME vec_caeed0 COMMAND test_vec_caeed0)

add_executable(test_vec_cae740 tests/test_vec_cae740.cpp)
target_link_libraries(test_vec_cae740 sotc_core)
add_test(NAME vec_cae740 COMMAND test_vec_cae740)

add_executable(test_vec_cae240 tests/test_vec_cae240.cpp)
target_link_libraries(test_vec_cae240 sotc_core)
add_test(NAME vec_cae240 COMMAND test_vec_cae240)

add_executable(test_vec_cad450 tests/test_vec_cad450.cpp)
target_link_libraries(test_vec_cad450 sotc_core)
add_test(NAME vec_cad450 COMMAND test_vec_cad450)

add_executable(test_vec_ca4dc0 tests/test_vec_ca4dc0.cpp)
target_link_libraries(test_vec_ca4dc0 sotc_core)
add_test(NAME vec_ca4dc0 COMMAND test_vec_ca4dc0)

add_executable(test_vec_c9e8e0 tests/test_vec_c9e8e0.cpp)
target_link_libraries(test_vec_c9e8e0 sotc_core)
add_test(NAME vec_c9e8e0 COMMAND test_vec_c9e8e0)

add_executable(test_vec_c91b10 tests/test_vec_c91b10.cpp)
target_link_libraries(test_vec_c91b10 sotc_core)
add_test(NAME vec_c91b10 COMMAND test_vec_c91b10)

add_executable(test_vec_c891e0 tests/test_vec_c891e0.cpp)
target_link_libraries(test_vec_c891e0 sotc_core)
add_test(NAME vec_c891e0 COMMAND test_vec_c891e0)

add_executable(test_vec_c7de60 tests/test_vec_c7de60.cpp)
target_link_libraries(test_vec_c7de60 sotc_core)
add_test(NAME vec_c7de60 COMMAND test_vec_c7de60)

add_executable(test_vec_c7c310 tests/test_vec_c7c310.cpp)
target_link_libraries(test_vec_c7c310 sotc_core)
add_test(NAME vec_c7c310 COMMAND test_vec_c7c310)

add_executable(test_vec_c786c0 tests/test_vec_c786c0.cpp)
target_link_libraries(test_vec_c786c0 sotc_core)
add_test(NAME vec_c786c0 COMMAND test_vec_c786c0)

add_executable(test_vec_c77070 tests/test_vec_c77070.cpp)
target_link_libraries(test_vec_c77070 sotc_core)
add_test(NAME vec_c77070 COMMAND test_vec_c77070)

add_executable(test_vec_c741c0 tests/test_vec_c741c0.cpp)
target_link_libraries(test_vec_c741c0 sotc_core)
add_test(NAME vec_c741c0 COMMAND test_vec_c741c0)

add_executable(test_vec_c70e20 tests/test_vec_c70e20.cpp)
target_link_libraries(test_vec_c70e20 sotc_core)
add_test(NAME vec_c70e20 COMMAND test_vec_c70e20)

add_executable(test_vec_c6fdf0 tests/test_vec_c6fdf0.cpp)
target_link_libraries(test_vec_c6fdf0 sotc_core)
add_test(NAME vec_c6fdf0 COMMAND test_vec_c6fdf0)

add_executable(test_vec_c6d910 tests/test_vec_c6d910.cpp)
target_link_libraries(test_vec_c6d910 sotc_core)
add_test(NAME vec_c6d910 COMMAND test_vec_c6d910)

add_executable(test_vec_c6bad0 tests/test_vec_c6bad0.cpp)
target_link_libraries(test_vec_c6bad0 sotc_core)
add_test(NAME vec_c6bad0 COMMAND test_vec_c6bad0)

add_executable(test_vec_c69f70 tests/test_vec_c69f70.cpp)
target_link_libraries(test_vec_c69f70 sotc_core)
add_test(NAME vec_c69f70 COMMAND test_vec_c69f70)

add_executable(test_vec_c686e0 tests/test_vec_c686e0.cpp)
target_link_libraries(test_vec_c686e0 sotc_core)
add_test(NAME vec_c686e0 COMMAND test_vec_c686e0)

add_executable(test_vec_c66580 tests/test_vec_c66580.cpp)
target_link_libraries(test_vec_c66580 sotc_core)
add_test(NAME vec_c66580 COMMAND test_vec_c66580)

add_executable(test_vec_c65f80 tests/test_vec_c65f80.cpp)
target_link_libraries(test_vec_c65f80 sotc_core)
add_test(NAME vec_c65f80 COMMAND test_vec_c65f80)

add_executable(test_vec_c65da0 tests/test_vec_c65da0.cpp)
target_link_libraries(test_vec_c65da0 sotc_core)
add_test(NAME vec_c65da0 COMMAND test_vec_c65da0)

add_executable(test_vec_c64060 tests/test_vec_c64060.cpp)
target_link_libraries(test_vec_c64060 sotc_core)
add_test(NAME vec_c64060 COMMAND test_vec_c64060)

add_executable(test_vec_c61670 tests/test_vec_c61670.cpp)
target_link_libraries(test_vec_c61670 sotc_core)
add_test(NAME vec_c61670 COMMAND test_vec_c61670)

add_executable(test_vec_c5f6f0 tests/test_vec_c5f6f0.cpp)
target_link_libraries(test_vec_c5f6f0 sotc_core)
add_test(NAME vec_c5f6f0 COMMAND test_vec_c5f6f0)

add_executable(test_vec_c5c320 tests/test_vec_c5c320.cpp)
target_link_libraries(test_vec_c5c320 sotc_core)
add_test(NAME vec_c5c320 COMMAND test_vec_c5c320)

add_executable(test_vec_c5c1a0 tests/test_vec_c5c1a0.cpp)
target_link_libraries(test_vec_c5c1a0 sotc_core)
add_test(NAME vec_c5c1a0 COMMAND test_vec_c5c1a0)

add_executable(test_vec_c5c040 tests/test_vec_c5c040.cpp)
target_link_libraries(test_vec_c5c040 sotc_core)
add_test(NAME vec_c5c040 COMMAND test_vec_c5c040)

add_executable(test_vec_c5a400 tests/test_vec_c5a400.cpp)
target_link_libraries(test_vec_c5a400 sotc_core)
add_test(NAME vec_c5a400 COMMAND test_vec_c5a400)

add_executable(test_vec_c5a100 tests/test_vec_c5a100.cpp)
target_link_libraries(test_vec_c5a100 sotc_core)
add_test(NAME vec_c5a100 COMMAND test_vec_c5a100)

add_executable(test_vec_c555c0 tests/test_vec_c555c0.cpp)
target_link_libraries(test_vec_c555c0 sotc_core)
add_test(NAME vec_c555c0 COMMAND test_vec_c555c0)

add_executable(test_vec_c54d20 tests/test_vec_c54d20.cpp)
target_link_libraries(test_vec_c54d20 sotc_core)
add_test(NAME vec_c54d20 COMMAND test_vec_c54d20)

add_executable(test_vec_c54830 tests/test_vec_c54830.cpp)
target_link_libraries(test_vec_c54830 sotc_core)
add_test(NAME vec_c54830 COMMAND test_vec_c54830)

add_executable(test_vec_c53db0 tests/test_vec_c53db0.cpp)
target_link_libraries(test_vec_c53db0 sotc_core)
add_test(NAME vec_c53db0 COMMAND test_vec_c53db0)

add_executable(test_vec_c4e5e0 tests/test_vec_c4e5e0.cpp)
target_link_libraries(test_vec_c4e5e0 sotc_core)
add_test(NAME vec_c4e5e0 COMMAND test_vec_c4e5e0)

add_executable(test_vec_c4aa80 tests/test_vec_c4aa80.cpp)
target_link_libraries(test_vec_c4aa80 sotc_core)
add_test(NAME vec_c4aa80 COMMAND test_vec_c4aa80)

add_executable(test_vec_c47e00 tests/test_vec_c47e00.cpp)
target_link_libraries(test_vec_c47e00 sotc_core)
add_test(NAME vec_c47e00 COMMAND test_vec_c47e00)

add_executable(test_vec_c451d0 tests/test_vec_c451d0.cpp)
target_link_libraries(test_vec_c451d0 sotc_core)
add_test(NAME vec_c451d0 COMMAND test_vec_c451d0)

add_executable(test_vec_c44830 tests/test_vec_c44830.cpp)
target_link_libraries(test_vec_c44830 sotc_core)
add_test(NAME vec_c44830 COMMAND test_vec_c44830)

add_executable(test_vec_c42b00 tests/test_vec_c42b00.cpp)
target_link_libraries(test_vec_c42b00 sotc_core)
add_test(NAME vec_c42b00 COMMAND test_vec_c42b00)

add_executable(test_vec_c3eaa0 tests/test_vec_c3eaa0.cpp)
target_link_libraries(test_vec_c3eaa0 sotc_core)
add_test(NAME vec_c3eaa0 COMMAND test_vec_c3eaa0)

add_executable(test_vec_c3cf00 tests/test_vec_c3cf00.cpp)
target_link_libraries(test_vec_c3cf00 sotc_core)
add_test(NAME vec_c3cf00 COMMAND test_vec_c3cf00)

add_executable(test_vec_c37020 tests/test_vec_c37020.cpp)
target_link_libraries(test_vec_c37020 sotc_core)
add_test(NAME vec_c37020 COMMAND test_vec_c37020)

add_executable(test_vec_c36a80 tests/test_vec_c36a80.cpp)
target_link_libraries(test_vec_c36a80 sotc_core)
add_test(NAME vec_c36a80 COMMAND test_vec_c36a80)

add_executable(test_vec_c23650 tests/test_vec_c23650.cpp)
target_link_libraries(test_vec_c23650 sotc_core)
add_test(NAME vec_c23650 COMMAND test_vec_c23650)

add_executable(test_vec_c22300 tests/test_vec_c22300.cpp)
target_link_libraries(test_vec_c22300 sotc_core)
add_test(NAME vec_c22300 COMMAND test_vec_c22300)

add_executable(test_vec_c20710 tests/test_vec_c20710.cpp)
target_link_libraries(test_vec_c20710 sotc_core)
add_test(NAME vec_c20710 COMMAND test_vec_c20710)

add_executable(test_vec_c1a9a0 tests/test_vec_c1a9a0.cpp)
target_link_libraries(test_vec_c1a9a0 sotc_core)
add_test(NAME vec_c1a9a0 COMMAND test_vec_c1a9a0)

add_executable(test_vec_c191a0 tests/test_vec_c191a0.cpp)
target_link_libraries(test_vec_c191a0 sotc_core)
add_test(NAME vec_c191a0 COMMAND test_vec_c191a0)

add_executable(test_vec_c17a70 tests/test_vec_c17a70.cpp)
target_link_libraries(test_vec_c17a70 sotc_core)
add_test(NAME vec_c17a70 COMMAND test_vec_c17a70)

add_executable(test_vec_c152c0 tests/test_vec_c152c0.cpp)
target_link_libraries(test_vec_c152c0 sotc_core)
add_test(NAME vec_c152c0 COMMAND test_vec_c152c0)

add_executable(test_vec_c14f60 tests/test_vec_c14f60.cpp)
target_link_libraries(test_vec_c14f60 sotc_core)
add_test(NAME vec_c14f60 COMMAND test_vec_c14f60)

add_executable(test_vec_c0d720 tests/test_vec_c0d720.cpp)
target_link_libraries(test_vec_c0d720 sotc_core)
add_test(NAME vec_c0d720 COMMAND test_vec_c0d720)

add_executable(test_vec_c0bea0 tests/test_vec_c0bea0.cpp)
target_link_libraries(test_vec_c0bea0 sotc_core)
add_test(NAME vec_c0bea0 COMMAND test_vec_c0bea0)

add_executable(test_vec_c0bbd0 tests/test_vec_c0bbd0.cpp)
target_link_libraries(test_vec_c0bbd0 sotc_core)
add_test(NAME vec_c0bbd0 COMMAND test_vec_c0bbd0)

add_executable(test_vec_c0bb00 tests/test_vec_c0bb00.cpp)
target_link_libraries(test_vec_c0bb00 sotc_core)
add_test(NAME vec_c0bb00 COMMAND test_vec_c0bb00)

add_executable(test_vec_c0baa0 tests/test_vec_c0baa0.cpp)
target_link_libraries(test_vec_c0baa0 sotc_core)
add_test(NAME vec_c0baa0 COMMAND test_vec_c0baa0)

add_executable(test_vec_c0aae0 tests/test_vec_c0aae0.cpp)
target_link_libraries(test_vec_c0aae0 sotc_core)
add_test(NAME vec_c0aae0 COMMAND test_vec_c0aae0)

add_executable(test_vec_c09070 tests/test_vec_c09070.cpp)
target_link_libraries(test_vec_c09070 sotc_core)
add_test(NAME vec_c09070 COMMAND test_vec_c09070)

add_executable(test_vec_c060a0 tests/test_vec_c060a0.cpp)
target_link_libraries(test_vec_c060a0 sotc_core)
add_test(NAME vec_c060a0 COMMAND test_vec_c060a0)

add_executable(test_vec_bd4dd0 tests/test_vec_bd4dd0.cpp)
target_link_libraries(test_vec_bd4dd0 sotc_core)
add_test(NAME vec_bd4dd0 COMMAND test_vec_bd4dd0)

add_executable(test_vec_bd60a0 tests/test_vec_bd60a0.cpp)
target_link_libraries(test_vec_bd60a0 sotc_core)
add_test(NAME vec_bd60a0 COMMAND test_vec_bd60a0)

add_executable(test_vec_bd7950 tests/test_vec_bd7950.cpp)
target_link_libraries(test_vec_bd7950 sotc_core)
add_test(NAME vec_bd7950 COMMAND test_vec_bd7950)

add_executable(test_vec_bd32f0 tests/test_vec_bd32f0.cpp)
target_link_libraries(test_vec_bd32f0 sotc_core)
add_test(NAME vec_bd32f0 COMMAND test_vec_bd32f0)

add_executable(test_vec_bd3190 tests/test_vec_bd3190.cpp)
target_link_libraries(test_vec_bd3190 sotc_core)
add_test(NAME vec_bd3190 COMMAND test_vec_bd3190)

add_executable(test_vec_bd06c0 tests/test_vec_bd06c0.cpp)
target_link_libraries(test_vec_bd06c0 sotc_core)
add_test(NAME vec_bd06c0 COMMAND test_vec_bd06c0)

add_executable(test_vec_bceda0 tests/test_vec_bceda0.cpp)
target_link_libraries(test_vec_bceda0 sotc_core)
add_test(NAME vec_bceda0 COMMAND test_vec_bceda0)

add_executable(test_vec_bccc00 tests/test_vec_bccc00.cpp)
target_link_libraries(test_vec_bccc00 sotc_core)
add_test(NAME vec_bccc00 COMMAND test_vec_bccc00)

add_executable(test_vec_bcc960 tests/test_vec_bcc960.cpp)
target_link_libraries(test_vec_bcc960 sotc_core)
add_test(NAME vec_bcc960 COMMAND test_vec_bcc960)

add_executable(test_vec_bcc4e0 tests/test_vec_bcc4e0.cpp)
target_link_libraries(test_vec_bcc4e0 sotc_core)
add_test(NAME vec_bcc4e0 COMMAND test_vec_bcc4e0)

add_executable(test_vec_bcc3f0 tests/test_vec_bcc3f0.cpp)
target_link_libraries(test_vec_bcc3f0 sotc_core)
add_test(NAME vec_bcc3f0 COMMAND test_vec_bcc3f0)

add_executable(test_vec_bc8210 tests/test_vec_bc8210.cpp)
target_link_libraries(test_vec_bc8210 sotc_core)
add_test(NAME vec_bc8210 COMMAND test_vec_bc8210)

add_executable(test_vec_bc7280 tests/test_vec_bc7280.cpp)
target_link_libraries(test_vec_bc7280 sotc_core)
add_test(NAME vec_bc7280 COMMAND test_vec_bc7280)

add_executable(test_vec_bc5b00 tests/test_vec_bc5b00.cpp)
target_link_libraries(test_vec_bc5b00 sotc_core)
add_test(NAME vec_bc5b00 COMMAND test_vec_bc5b00)

add_executable(test_vec_bc2800 tests/test_vec_bc2800.cpp)
target_link_libraries(test_vec_bc2800 sotc_core)
add_test(NAME vec_bc2800 COMMAND test_vec_bc2800)

add_executable(test_vec_d85490 tests/test_vec_d85490.cpp)
target_link_libraries(test_vec_d85490 sotc_core)
add_test(NAME vec_d85490 COMMAND test_vec_d85490)

add_executable(test_vec_dbfa60 tests/test_vec_dbfa60.cpp)
target_link_libraries(test_vec_dbfa60 sotc_core)
add_test(NAME vec_dbfa60 COMMAND test_vec_dbfa60)

add_executable(test_vec_e094e0 tests/test_vec_e094e0.cpp)
target_link_libraries(test_vec_e094e0 sotc_core)
add_test(NAME vec_e094e0 COMMAND test_vec_e094e0)

add_executable(test_vec_be5410 tests/test_vec_be5410.cpp)
target_link_libraries(test_vec_be5410 sotc_core)
add_test(NAME vec_be5410 COMMAND test_vec_be5410)

add_executable(test_vec_be5270 tests/test_vec_be5270.cpp)
target_link_libraries(test_vec_be5270 sotc_core)
add_test(NAME vec_be5270 COMMAND test_vec_be5270)

add_executable(test_vec_be50a0 tests/test_vec_be50a0.cpp)
target_link_libraries(test_vec_be50a0 sotc_core)
add_test(NAME vec_be50a0 COMMAND test_vec_be50a0)

add_executable(test_vec_be4ff0 tests/test_vec_be4ff0.cpp)
target_link_libraries(test_vec_be4ff0 sotc_core)
add_test(NAME vec_be4ff0 COMMAND test_vec_be4ff0)

add_executable(test_vec_bd7a80 tests/test_vec_bd7a80.cpp)
target_link_libraries(test_vec_bd7a80 sotc_core)
add_test(NAME vec_bd7a80 COMMAND test_vec_bd7a80)

add_executable(test_vec_bc07f0 tests/test_vec_bc07f0.cpp)
target_link_libraries(test_vec_bc07f0 sotc_core)
add_test(NAME vec_bc07f0 COMMAND test_vec_bc07f0)

add_executable(test_vec_bc0670 tests/test_vec_bc0670.cpp)
target_link_libraries(test_vec_bc0670 sotc_core)
add_test(NAME vec_bc0670 COMMAND test_vec_bc0670)

add_executable(test_vec_bbf580 tests/test_vec_bbf580.cpp)
target_link_libraries(test_vec_bbf580 sotc_core)
add_test(NAME vec_bbf580 COMMAND test_vec_bbf580)

add_executable(test_vec_bbd5e0 tests/test_vec_bbd5e0.cpp)
target_link_libraries(test_vec_bbd5e0 sotc_core)
add_test(NAME vec_bbd5e0 COMMAND test_vec_bbd5e0)

add_executable(test_vec_bbba10 tests/test_vec_bbba10.cpp)
target_link_libraries(test_vec_bbba10 sotc_core)
add_test(NAME vec_bbba10 COMMAND test_vec_bbba10)

add_executable(test_vec_bb9740 tests/test_vec_bb9740.cpp)
target_link_libraries(test_vec_bb9740 sotc_core)
add_test(NAME vec_bb9740 COMMAND test_vec_bb9740)

add_executable(test_save_checksum tests/test_save_checksum.cpp)
target_link_libraries(test_save_checksum sotc_core)
add_test(NAME save_checksum COMMAND test_save_checksum)

add_executable(test_recursive_mutex tests/test_recursive_mutex.cpp)
target_link_libraries(test_recursive_mutex sotc_core)
add_test(NAME recursive_mutex COMMAND test_recursive_mutex)

add_executable(test_vec_bb82d0 tests/test_vec_bb82d0.cpp)
target_link_libraries(test_vec_bb82d0 sotc_core)
add_test(NAME vec_bb82d0 COMMAND test_vec_bb82d0)

add_executable(test_vec_bb7990 tests/test_vec_bb7990.cpp)
target_link_libraries(test_vec_bb7990 sotc_core)
add_test(NAME vec_bb7990 COMMAND test_vec_bb7990)

add_executable(test_vec_bacfb0 tests/test_vec_bacfb0.cpp)
target_link_libraries(test_vec_bacfb0 sotc_core)
add_test(NAME vec_bacfb0 COMMAND test_vec_bacfb0)

add_executable(test_vec_bace80 tests/test_vec_bace80.cpp)
target_link_libraries(test_vec_bace80 sotc_core)
add_test(NAME vec_bace80 COMMAND test_vec_bace80)

add_executable(test_vec_bace50 tests/test_vec_bace50.cpp)
target_link_libraries(test_vec_bace50 sotc_core)
add_test(NAME vec_bace50 COMMAND test_vec_bace50)

add_executable(test_vec_bacb30 tests/test_vec_bacb30.cpp)
target_link_libraries(test_vec_bacb30 sotc_core)
add_test(NAME vec_bacb30 COMMAND test_vec_bacb30)

add_executable(test_vec_bab950 tests/test_vec_bab950.cpp)
target_link_libraries(test_vec_bab950 sotc_core)
add_test(NAME vec_bab950 COMMAND test_vec_bab950)

add_executable(test_vec_ba3ef0 tests/test_vec_ba3ef0.cpp)
target_link_libraries(test_vec_ba3ef0 sotc_core)
add_test(NAME vec_ba3ef0 COMMAND test_vec_ba3ef0)

add_executable(test_vec_b950e0 tests/test_vec_b950e0.cpp)
target_link_libraries(test_vec_b950e0 sotc_core)
add_test(NAME vec_b950e0 COMMAND test_vec_b950e0)

add_executable(test_vec_b8d170 tests/test_vec_b8d170.cpp)
target_link_libraries(test_vec_b8d170 sotc_core)
add_test(NAME vec_b8d170 COMMAND test_vec_b8d170)

add_executable(test_vec_b89eb0 tests/test_vec_b89eb0.cpp)
target_link_libraries(test_vec_b89eb0 sotc_core)
add_test(NAME vec_b89eb0 COMMAND test_vec_b89eb0)

add_executable(test_vec_b84580 tests/test_vec_b84580.cpp)
target_link_libraries(test_vec_b84580 sotc_core)
add_test(NAME vec_b84580 COMMAND test_vec_b84580)

add_executable(test_vec_b7b270 tests/test_vec_b7b270.cpp)
target_link_libraries(test_vec_b7b270 sotc_core)
add_test(NAME vec_b7b270 COMMAND test_vec_b7b270)

add_executable(test_vec_b77e90 tests/test_vec_b77e90.cpp)
target_link_libraries(test_vec_b77e90 sotc_core)
add_test(NAME vec_b77e90 COMMAND test_vec_b77e90)

add_executable(test_vec_b77910 tests/test_vec_b77910.cpp)
target_link_libraries(test_vec_b77910 sotc_core)
add_test(NAME vec_b77910 COMMAND test_vec_b77910)

add_executable(test_vec_b77740 tests/test_vec_b77740.cpp)
target_link_libraries(test_vec_b77740 sotc_core)
add_test(NAME vec_b77740 COMMAND test_vec_b77740)

add_executable(test_vec_b71f80 tests/test_vec_b71f80.cpp)
target_link_libraries(test_vec_b71f80 sotc_core)
add_test(NAME vec_b71f80 COMMAND test_vec_b71f80)

add_executable(test_vec_b6e6d0 tests/test_vec_b6e6d0.cpp)
target_link_libraries(test_vec_b6e6d0 sotc_core)
add_test(NAME vec_b6e6d0 COMMAND test_vec_b6e6d0)

add_executable(test_vec_b67a70 tests/test_vec_b67a70.cpp)
target_link_libraries(test_vec_b67a70 sotc_core)
add_test(NAME vec_b67a70 COMMAND test_vec_b67a70)

add_executable(test_vec_b5ecf0 tests/test_vec_b5ecf0.cpp)
target_link_libraries(test_vec_b5ecf0 sotc_core)
add_test(NAME vec_b5ecf0 COMMAND test_vec_b5ecf0)

add_executable(test_vec_b58690 tests/test_vec_b58690.cpp)
target_link_libraries(test_vec_b58690 sotc_core)
add_test(NAME vec_b58690 COMMAND test_vec_b58690)

add_executable(test_vec_b57360 tests/test_vec_b57360.cpp)
target_link_libraries(test_vec_b57360 sotc_core)
add_test(NAME vec_b57360 COMMAND test_vec_b57360)

add_executable(test_vec_b4ef70 tests/test_vec_b4ef70.cpp)
target_link_libraries(test_vec_b4ef70 sotc_core)
add_test(NAME vec_b4ef70 COMMAND test_vec_b4ef70)

add_executable(test_vec_b494d0 tests/test_vec_b494d0.cpp)
target_link_libraries(test_vec_b494d0 sotc_core)
add_test(NAME vec_b494d0 COMMAND test_vec_b494d0)

add_executable(test_vec_b47100 tests/test_vec_b47100.cpp)
target_link_libraries(test_vec_b47100 sotc_core)
add_test(NAME vec_b47100 COMMAND test_vec_b47100)

add_executable(test_vec_b44580 tests/test_vec_b44580.cpp)
target_link_libraries(test_vec_b44580 sotc_core)
add_test(NAME vec_b44580 COMMAND test_vec_b44580)

add_executable(test_vec_b33bd0 tests/test_vec_b33bd0.cpp)
target_link_libraries(test_vec_b33bd0 sotc_core)
add_test(NAME vec_b33bd0 COMMAND test_vec_b33bd0)

add_executable(test_guarded_dd91b0 tests/test_guarded_dd91b0.cpp)
target_link_libraries(test_guarded_dd91b0 sotc_core)
add_test(NAME guarded_dd91b0 COMMAND test_guarded_dd91b0)

add_executable(test_guarded_dd7e30 tests/test_guarded_dd7e30.cpp)
target_link_libraries(test_guarded_dd7e30 sotc_core)
add_test(NAME guarded_dd7e30 COMMAND test_guarded_dd7e30)

add_executable(test_guarded_dd4210 tests/test_guarded_dd4210.cpp)
target_link_libraries(test_guarded_dd4210 sotc_core)
add_test(NAME guarded_dd4210 COMMAND test_guarded_dd4210)

add_executable(test_guarded_d39b00 tests/test_guarded_d39b00.cpp)
target_link_libraries(test_guarded_d39b00 sotc_core)
add_test(NAME guarded_d39b00 COMMAND test_guarded_d39b00)

add_executable(test_guarded_d366d0 tests/test_guarded_d366d0.cpp)
target_link_libraries(test_guarded_d366d0 sotc_core)
add_test(NAME guarded_d366d0 COMMAND test_guarded_d366d0)

add_executable(test_guarded_d33b50 tests/test_guarded_d33b50.cpp)
target_link_libraries(test_guarded_d33b50 sotc_core)
add_test(NAME guarded_d33b50 COMMAND test_guarded_d33b50)

add_executable(test_avx_d8fa40 tests/test_avx_d8fa40.cpp)
target_link_libraries(test_avx_d8fa40 sotc_core)
add_test(NAME avx_d8fa40 COMMAND test_avx_d8fa40)

add_executable(test_avx_d86ed0 tests/test_avx_d86ed0.cpp)
target_link_libraries(test_avx_d86ed0 sotc_core)
add_test(NAME avx_d86ed0 COMMAND test_avx_d86ed0)

add_executable(test_avx_d5a280 tests/test_avx_d5a280.cpp)
target_link_libraries(test_avx_d5a280 sotc_core)
add_test(NAME avx_d5a280 COMMAND test_avx_d5a280)

add_executable(test_leaf_cab080 tests/test_leaf_cab080.cpp)
target_link_libraries(test_leaf_cab080 sotc_core)
add_test(NAME leaf_cab080 COMMAND test_leaf_cab080)

add_executable(test_leaf_ca18d0 tests/test_leaf_ca18d0.cpp)
target_link_libraries(test_leaf_ca18d0 sotc_core)
add_test(NAME leaf_ca18d0 COMMAND test_leaf_ca18d0)

add_executable(test_leaf_c94ae0 tests/test_leaf_c94ae0.cpp)
target_link_libraries(test_leaf_c94ae0 sotc_core)
add_test(NAME leaf_c94ae0 COMMAND test_leaf_c94ae0)

add_executable(test_leaf_b692a0 tests/test_leaf_b692a0.cpp)
target_link_libraries(test_leaf_b692a0 sotc_core)
add_test(NAME leaf_b692a0 COMMAND test_leaf_b692a0)

add_executable(test_leaf_c811b0 tests/test_leaf_c811b0.cpp)
target_link_libraries(test_leaf_c811b0 sotc_core)
add_test(NAME leaf_c811b0 COMMAND test_leaf_c811b0)

add_executable(test_leaf_8a0440 tests/test_leaf_8a0440.cpp)
target_link_libraries(test_leaf_8a0440 sotc_core)
add_test(NAME leaf_8a0440 COMMAND test_leaf_8a0440)

add_executable(test_leaf_c05980 tests/test_leaf_c05980.cpp)
target_link_libraries(test_leaf_c05980 sotc_core)
add_test(NAME leaf_c05980 COMMAND test_leaf_c05980)

add_executable(test_leaf_c6f520 tests/test_leaf_c6f520.cpp)
target_link_libraries(test_leaf_c6f520 sotc_core)
add_test(NAME leaf_c6f520 COMMAND test_leaf_c6f520)

add_executable(test_leaf_c6e9d0 tests/test_leaf_c6e9d0.cpp)
target_link_libraries(test_leaf_c6e9d0 sotc_core)
add_test(NAME leaf_c6e9d0 COMMAND test_leaf_c6e9d0)

add_executable(test_leaf_a816d0 tests/test_leaf_a816d0.cpp)
target_link_libraries(test_leaf_a816d0 sotc_core)
add_test(NAME leaf_a816d0 COMMAND test_leaf_a816d0)

add_executable(test_leaf_3ca7c0 tests/test_leaf_3ca7c0.cpp)
target_link_libraries(test_leaf_3ca7c0 sotc_core)
add_test(NAME leaf_3ca7c0 COMMAND test_leaf_3ca7c0)

add_executable(test_leaf_c0d380 tests/test_leaf_c0d380.cpp)
target_link_libraries(test_leaf_c0d380 sotc_core)
add_test(NAME leaf_c0d380 COMMAND test_leaf_c0d380)

add_executable(test_leaf_c675f0 tests/test_leaf_c675f0.cpp)
target_link_libraries(test_leaf_c675f0 sotc_core)
add_test(NAME leaf_c675f0 COMMAND test_leaf_c675f0)

add_executable(test_leaf_c16b30 tests/test_leaf_c16b30.cpp)
target_link_libraries(test_leaf_c16b30 sotc_core)
add_test(NAME leaf_c16b30 COMMAND test_leaf_c16b30)

add_executable(test_leaf_8f3d90 tests/test_leaf_8f3d90.cpp)
target_link_libraries(test_leaf_8f3d90 sotc_core)
add_test(NAME leaf_8f3d90 COMMAND test_leaf_8f3d90)

add_executable(test_leaf_163a40 tests/test_leaf_163a40.cpp)
target_link_libraries(test_leaf_163a40 sotc_core)
add_test(NAME leaf_163a40 COMMAND test_leaf_163a40)

add_executable(test_leaf_9b75a0 tests/test_leaf_9b75a0.cpp)
target_link_libraries(test_leaf_9b75a0 sotc_core)
add_test(NAME leaf_9b75a0 COMMAND test_leaf_9b75a0)

add_executable(test_leaf_575d40 tests/test_leaf_575d40.cpp)
target_link_libraries(test_leaf_575d40 sotc_core)
add_test(NAME leaf_575d40 COMMAND test_leaf_575d40)

add_executable(test_leaf_99350 tests/test_leaf_99350.cpp)
target_link_libraries(test_leaf_99350 sotc_core)
add_test(NAME leaf_99350 COMMAND test_leaf_99350)

add_executable(test_leaf_83d800 tests/test_leaf_83d800.cpp)
target_link_libraries(test_leaf_83d800 sotc_core)
add_test(NAME leaf_83d800 COMMAND test_leaf_83d800)

add_executable(test_leaf_7ce8d0 tests/test_leaf_7ce8d0.cpp)
target_link_libraries(test_leaf_7ce8d0 sotc_core)
add_test(NAME leaf_7ce8d0 COMMAND test_leaf_7ce8d0)

add_executable(test_leaf_e71990 tests/test_leaf_e71990.cpp)
target_link_libraries(test_leaf_e71990 sotc_core)
add_test(NAME leaf_e71990 COMMAND test_leaf_e71990)

add_executable(test_leaf_df94c0 tests/test_leaf_df94c0.cpp)
target_link_libraries(test_leaf_df94c0 sotc_core)
add_test(NAME leaf_df94c0 COMMAND test_leaf_df94c0)

add_executable(test_leaf_de0160 tests/test_leaf_de0160.cpp)
target_link_libraries(test_leaf_de0160 sotc_core)
add_test(NAME leaf_de0160 COMMAND test_leaf_de0160)

add_executable(test_fnv_cstr tests/test_fnv_cstr.cpp)
target_link_libraries(test_fnv_cstr sotc_core)
add_test(NAME fnv_cstr COMMAND test_fnv_cstr)

add_executable(test_fnv_path_cstr_162020 tests/test_fnv_path_cstr_162020.cpp)
target_link_libraries(test_fnv_path_cstr_162020 sotc_core)
add_test(NAME fnv_path_cstr_162020 COMMAND test_fnv_path_cstr_162020)

add_executable(test_hashed_name_ctors tests/test_hashed_name_ctors.cpp)
target_link_libraries(test_hashed_name_ctors sotc_core)
add_test(NAME hashed_name_ctors COMMAND test_hashed_name_ctors)

add_executable(test_guarded_static_helpers tests/test_guarded_static_helpers.cpp)
target_link_libraries(test_guarded_static_helpers sotc_core)
add_test(NAME guarded_static_helpers COMMAND test_guarded_static_helpers)
add_executable(test_const_table_ee1130 tests/test_const_table_ee1130.cpp)
target_link_libraries(test_const_table_ee1130 sotc_core)
add_test(NAME const_table_ee1130 COMMAND test_const_table_ee1130)

add_executable(test_input_table_551140 tests/test_input_table_551140.cpp)
target_link_libraries(test_input_table_551140 sotc_core)
add_test(NAME input_table_551140 COMMAND test_input_table_551140)

add_executable(test_descriptor_registration tests/test_descriptor_registration.cpp)
target_link_libraries(test_descriptor_registration sotc_core)
add_test(NAME descriptor_registration COMMAND test_descriptor_registration)

add_executable(test_save_chunk_header tests/test_save_chunk_header.cpp)
target_link_libraries(test_save_chunk_header sotc_core)
add_test(NAME save_chunk_header COMMAND test_save_chunk_header)

add_executable(test_save_checksum_edges tests/test_save_checksum_edges.cpp)
target_link_libraries(test_save_checksum_edges sotc_core)
add_test(NAME save_checksum_edges COMMAND test_save_checksum_edges)

add_executable(test_name_hash tests/test_name_hash.cpp)
target_link_libraries(test_name_hash sotc_core)
add_test(NAME name_hash COMMAND test_name_hash)

add_executable(test_save_stream tests/test_save_stream.cpp)
target_link_libraries(test_save_stream sotc_core)
add_test(NAME save_stream COMMAND test_save_stream)

add_executable(test_save_stream_file tests/test_save_stream_file.cpp)
target_link_libraries(test_save_stream_file sotc_core)
add_test(NAME save_stream_file COMMAND test_save_stream_file)

add_executable(test_save_chunk_codec tests/test_save_chunk_codec.cpp)
target_link_libraries(test_save_chunk_codec sotc_core)
add_test(NAME save_chunk_codec COMMAND test_save_chunk_codec)

add_executable(test_atomic_add_ref_14ccb0 tests/test_atomic_add_ref_14ccb0.cpp)
target_link_libraries(test_atomic_add_ref_14ccb0 sotc_core)
add_test(NAME atomic_add_ref_14ccb0 COMMAND test_atomic_add_ref_14ccb0)

add_executable(test_atomic_xadd_b30cf0 tests/test_atomic_xadd_b30cf0.cpp)
target_link_libraries(test_atomic_xadd_b30cf0 sotc_core)
add_test(NAME atomic_xadd_b30cf0 COMMAND test_atomic_xadd_b30cf0)

add_executable(test_store_value_clear_flag_158340 tests/test_store_value_clear_flag_158340.cpp)
target_link_libraries(test_store_value_clear_flag_158340 sotc_core)
add_test(NAME store_value_clear_flag_158340 COMMAND test_store_value_clear_flag_158340)

add_executable(test_fnv_span_161f80 tests/test_fnv_span_161f80.cpp)
target_link_libraries(test_fnv_span_161f80 sotc_core)
add_test(NAME fnv_span_161f80 COMMAND test_fnv_span_161f80)

add_executable(test_hashed_string_14c3a0 tests/test_hashed_string_14c3a0.cpp)
target_link_libraries(test_hashed_string_14c3a0 sotc_core)
add_test(NAME hashed_string_14c3a0 COMMAND test_hashed_string_14c3a0)

add_executable(test_registry_object_14c9a0 tests/test_registry_object_14c9a0.cpp)
target_link_libraries(test_registry_object_14c9a0 sotc_core)
add_test(NAME registry_object_14c9a0 COMMAND test_registry_object_14c9a0)

add_executable(test_registry_object_2efd160 tests/test_registry_object_2efd160.cpp)
target_link_libraries(test_registry_object_2efd160 sotc_core)
add_test(NAME registry_object_2efd160 COMMAND test_registry_object_2efd160)

add_executable(test_registry_sorted_insert tests/test_registry_sorted_insert.cpp)
target_link_libraries(test_registry_sorted_insert sotc_core)
add_test(NAME registry_sorted_insert COMMAND test_registry_sorted_insert)

add_executable(test_inputtable_names_fixture tests/test_inputtable_names_fixture.cpp)
target_link_libraries(test_inputtable_names_fixture sotc_core)
add_test(NAME inputtable_names_fixture COMMAND test_inputtable_names_fixture)
set_tests_properties(inputtable_names_fixture PROPERTIES
  ENVIRONMENT "SOTC_INPUTTABLE_NAMES_FIXTURE=${CMAKE_SOURCE_DIR}/../local/fixtures/inputtable_551140_names.bin")

add_executable(test_registry_fill_52a220 tests/test_registry_fill_52a220.cpp)
target_link_libraries(test_registry_fill_52a220 sotc_core)
add_test(NAME registry_fill_52a220 COMMAND test_registry_fill_52a220)
set_tests_properties(registry_fill_52a220 PROPERTIES
  ENVIRONMENT "SOTC_INPUTTABLE_NAMES_FIXTURE=${CMAKE_SOURCE_DIR}/../local/fixtures/inputtable_551140_names.bin")

add_executable(test_registry_init_531d60 tests/test_registry_init_531d60.cpp)
target_link_libraries(test_registry_init_531d60 sotc_core)
add_test(NAME registry_init_531d60 COMMAND test_registry_init_531d60)
set_tests_properties(registry_init_531d60 PROPERTIES
  ENVIRONMENT "SOTC_INPUTTABLE_NAMES_FIXTURE=${CMAKE_SOURCE_DIR}/../local/fixtures/inputtable_551140_names.bin")

add_executable(test_appmain_init_registry_slice tests/test_appmain_init_registry_slice.cpp)
target_link_libraries(test_appmain_init_registry_slice sotc_core)
add_test(NAME appmain_init_registry_slice COMMAND test_appmain_init_registry_slice)
set_tests_properties(appmain_init_registry_slice PROPERTIES
  ENVIRONMENT "SOTC_INPUTTABLE_NAMES_FIXTURE=${CMAKE_SOURCE_DIR}/../local/fixtures/inputtable_551140_names.bin")

add_executable(test_appmain_init_remainder_slice tests/test_appmain_init_remainder_slice.cpp)
target_link_libraries(test_appmain_init_remainder_slice sotc_core)
add_test(NAME appmain_init_remainder_slice COMMAND test_appmain_init_remainder_slice)
set_tests_properties(appmain_init_remainder_slice PROPERTIES
  ENVIRONMENT "SOTC_INPUTTABLE_NAMES_FIXTURE=${CMAKE_SOURCE_DIR}/../local/fixtures/inputtable_551140_names.bin")

add_executable(test_appmain_init_base_initialize_slice tests/test_appmain_init_base_initialize_slice.cpp)
target_link_libraries(test_appmain_init_base_initialize_slice sotc_core)
add_test(NAME appmain_init_base_initialize_slice COMMAND test_appmain_init_base_initialize_slice)
set_tests_properties(appmain_init_base_initialize_slice PROPERTIES
  ENVIRONMENT "SOTC_INPUTTABLE_NAMES_FIXTURE=${CMAKE_SOURCE_DIR}/../local/fixtures/inputtable_551140_names.bin")

add_executable(test_appmain_init_prologue_slice tests/test_appmain_init_prologue_slice.cpp)
target_link_libraries(test_appmain_init_prologue_slice sotc_core)
add_test(NAME appmain_init_prologue_slice COMMAND test_appmain_init_prologue_slice)
set_tests_properties(appmain_init_prologue_slice PROPERTIES
  ENVIRONMENT "SOTC_INPUTTABLE_NAMES_FIXTURE=${CMAKE_SOURCE_DIR}/../local/fixtures/inputtable_551140_names.bin")

add_executable(test_appmain_init_prologue_tail_slice tests/test_appmain_init_prologue_tail_slice.cpp)
target_link_libraries(test_appmain_init_prologue_tail_slice sotc_core)
add_test(NAME appmain_init_prologue_tail_slice COMMAND test_appmain_init_prologue_tail_slice)
set_tests_properties(appmain_init_prologue_tail_slice PROPERTIES
  ENVIRONMENT "SOTC_INPUTTABLE_NAMES_FIXTURE=${CMAKE_SOURCE_DIR}/../local/fixtures/inputtable_551140_names.bin")

add_executable(test_appmain_init_ctor_98a90_slice tests/test_appmain_init_ctor_98a90_slice.cpp)
target_link_libraries(test_appmain_init_ctor_98a90_slice sotc_core)
add_test(NAME appmain_init_ctor_98a90_slice COMMAND test_appmain_init_ctor_98a90_slice)
set_tests_properties(appmain_init_ctor_98a90_slice PROPERTIES
  ENVIRONMENT "SOTC_INPUTTABLE_NAMES_FIXTURE=${CMAKE_SOURCE_DIR}/../local/fixtures/inputtable_551140_names.bin")

add_executable(test_page_owner_map tests/test_page_owner_map.cpp)
target_link_libraries(test_page_owner_map sotc_core)
add_test(NAME page_owner_map COMMAND test_page_owner_map)

add_executable(test_page_allocator tests/test_page_allocator.cpp)
target_link_libraries(test_page_allocator sotc_core)
add_test(NAME page_allocator COMMAND test_page_allocator)

add_executable(test_allocator_front tests/test_allocator_front.cpp)
target_link_libraries(test_allocator_front sotc_core)
add_test(NAME allocator_front COMMAND test_allocator_front)

add_subdirectory(api)
