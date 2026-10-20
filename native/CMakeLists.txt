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
  core/scratch_reset.cpp
  core/startup_ctors.cpp
  core/trig_constants.cpp
  core/addr.cpp
  core/allocator_region.cpp
  core/host_map.cpp
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
