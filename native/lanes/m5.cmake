# Lane M5 fragment (startup constructor slots 491 492 499 502 509 516 535 540 543). Paths are relative to native/.
target_sources(sotc_core PRIVATE
  core/lane_m5_app_entry.cpp
  core/ctor_slot_491_295f90.cpp
  core/ctor_slot_492_278640.cpp
  core/ctor_slot_499_2500e0.cpp
  core/ctor_slot_502_249270.cpp
  core/ctor_slot_509_2196f0.cpp
  core/ctor_slot_516_1b97b0.cpp
  core/ctor_slot_535_138e20.cpp
  core/ctor_slot_540_10e390.cpp
  core/ctor_slot_543_fae50.cpp
)
add_executable(test_ctor_slot_491 tests/test_ctor_slot_491.cpp)
target_link_libraries(test_ctor_slot_491 sotc_core)
add_test(NAME ctor_slot_491 COMMAND test_ctor_slot_491)
add_executable(test_ctor_slot_492 tests/test_ctor_slot_492.cpp)
target_link_libraries(test_ctor_slot_492 sotc_core)
add_test(NAME ctor_slot_492 COMMAND test_ctor_slot_492)
add_executable(test_ctor_slot_499 tests/test_ctor_slot_499.cpp)
target_link_libraries(test_ctor_slot_499 sotc_core)
add_test(NAME ctor_slot_499 COMMAND test_ctor_slot_499)
add_executable(test_ctor_slot_502 tests/test_ctor_slot_502.cpp)
target_link_libraries(test_ctor_slot_502 sotc_core)
add_test(NAME ctor_slot_502 COMMAND test_ctor_slot_502)
add_executable(test_ctor_slot_509 tests/test_ctor_slot_509.cpp)
target_link_libraries(test_ctor_slot_509 sotc_core)
add_test(NAME ctor_slot_509 COMMAND test_ctor_slot_509)
add_executable(test_ctor_slot_516 tests/test_ctor_slot_516.cpp)
target_link_libraries(test_ctor_slot_516 sotc_core)
add_test(NAME ctor_slot_516 COMMAND test_ctor_slot_516)
add_executable(test_ctor_slot_535 tests/test_ctor_slot_535.cpp)
target_link_libraries(test_ctor_slot_535 sotc_core)
add_test(NAME ctor_slot_535 COMMAND test_ctor_slot_535)
add_executable(test_ctor_slot_540 tests/test_ctor_slot_540.cpp)
target_link_libraries(test_ctor_slot_540 sotc_core)
add_test(NAME ctor_slot_540 COMMAND test_ctor_slot_540)
add_executable(test_ctor_slot_543 tests/test_ctor_slot_543.cpp)
target_link_libraries(test_ctor_slot_543 sotc_core)
add_test(NAME ctor_slot_543 COMMAND test_ctor_slot_543)
