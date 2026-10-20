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
  core/vec_neg036.cpp
  core/xyz_mask_copy.cpp
  core/vec_dd41e0.cpp
  core/clear_flag32.cpp
  core/clear_flag64.cpp
  core/vec_b52110.cpp
  core/vec_b743c0.cpp
  core/vec_ef85e0.cpp
  core/vec_e085e0.cpp
  core/store_zero32.cpp
  core/zero_xmm16.cpp
  core/zero_xmm_qword.cpp
  core/init_f4fd40.cpp
  core/init_f4eba0.cpp
  core/init_f4b540.cpp
  core/vec_e026b0.cpp)
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
add_executable(test_vec_e085e0 tests/test_vec_e085e0.cpp)
target_link_libraries(test_vec_e085e0 sotc_core)
add_test(NAME vec_e085e0 COMMAND test_vec_e085e0)
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
