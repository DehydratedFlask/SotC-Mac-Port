# Lane M1: startup constructor slots 359 364 365 376 377 383 384 385 386 387 (native/lanes/m1.cmake).
# Paths are relative to native/ (this file is include()d from native/CMakeLists.txt).
target_sources(sotc_core PRIVATE
    core/lane_m1_float_item.cpp
    core/ctor_slot_359_785f60.cpp
    core/ctor_slot_364_74a8e0.cpp
    core/ctor_slot_365_747870.cpp
    core/ctor_slot_376_6e4930.cpp
    core/ctor_slot_377_6de050.cpp
    core/ctor_slot_383_6c62b0.cpp
    core/ctor_slot_384_6b9260.cpp
    core/ctor_slot_385_6b4cf0.cpp
    core/ctor_slot_386_6b12f0.cpp
    core/ctor_slot_387_6acf00.cpp)
add_executable(test_ctor_slot_359 tests/test_ctor_slot_359.cpp)
target_link_libraries(test_ctor_slot_359 sotc_core)
add_test(NAME ctor_slot_359 COMMAND test_ctor_slot_359)
add_executable(test_ctor_slot_364 tests/test_ctor_slot_364.cpp)
target_link_libraries(test_ctor_slot_364 sotc_core)
add_test(NAME ctor_slot_364 COMMAND test_ctor_slot_364)
add_executable(test_ctor_slot_365 tests/test_ctor_slot_365.cpp)
target_link_libraries(test_ctor_slot_365 sotc_core)
add_test(NAME ctor_slot_365 COMMAND test_ctor_slot_365)
add_executable(test_ctor_slot_376 tests/test_ctor_slot_376.cpp)
target_link_libraries(test_ctor_slot_376 sotc_core)
add_test(NAME ctor_slot_376 COMMAND test_ctor_slot_376)
add_executable(test_ctor_slot_377 tests/test_ctor_slot_377.cpp)
target_link_libraries(test_ctor_slot_377 sotc_core)
add_test(NAME ctor_slot_377 COMMAND test_ctor_slot_377)
add_executable(test_ctor_slot_383 tests/test_ctor_slot_383.cpp)
target_link_libraries(test_ctor_slot_383 sotc_core)
add_test(NAME ctor_slot_383 COMMAND test_ctor_slot_383)
add_executable(test_ctor_slot_384 tests/test_ctor_slot_384.cpp)
target_link_libraries(test_ctor_slot_384 sotc_core)
add_test(NAME ctor_slot_384 COMMAND test_ctor_slot_384)
add_executable(test_ctor_slot_385 tests/test_ctor_slot_385.cpp)
target_link_libraries(test_ctor_slot_385 sotc_core)
add_test(NAME ctor_slot_385 COMMAND test_ctor_slot_385)
add_executable(test_ctor_slot_386 tests/test_ctor_slot_386.cpp)
target_link_libraries(test_ctor_slot_386 sotc_core)
add_test(NAME ctor_slot_386 COMMAND test_ctor_slot_386)
add_executable(test_ctor_slot_387 tests/test_ctor_slot_387.cpp)
target_link_libraries(test_ctor_slot_387 sotc_core)
add_test(NAME ctor_slot_387 COMMAND test_ctor_slot_387)
