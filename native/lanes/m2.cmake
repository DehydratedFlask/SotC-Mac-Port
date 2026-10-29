# Lane M2 fragment (startup constructor slots 389 393 403 407 413 415 419 421). Paths are relative to native/.
# Included by native/CMakeLists.txt when configured with -DSOTC_LANES=m2.
target_sources(sotc_core PRIVATE
    core/lane_m2_shapes.cpp
    core/ctor_slot_389_695a50.cpp
    core/ctor_slot_393_66cc00.cpp
    core/ctor_slot_403_5f0160.cpp
    core/ctor_slot_407_5d41c0.cpp
    core/ctor_slot_413_57c810.cpp
    core/ctor_slot_415_578b60.cpp
    core/ctor_slot_419_56fae0.cpp
    core/ctor_slot_421_56da20.cpp)

add_executable(test_ctor_slot_389 tests/test_ctor_slot_389.cpp)
target_link_libraries(test_ctor_slot_389 sotc_core)
add_test(NAME ctor_slot_389 COMMAND test_ctor_slot_389)

add_executable(test_ctor_slot_393 tests/test_ctor_slot_393.cpp)
target_link_libraries(test_ctor_slot_393 sotc_core)
add_test(NAME ctor_slot_393 COMMAND test_ctor_slot_393)

add_executable(test_ctor_slot_403 tests/test_ctor_slot_403.cpp)
target_link_libraries(test_ctor_slot_403 sotc_core)
add_test(NAME ctor_slot_403 COMMAND test_ctor_slot_403)

add_executable(test_ctor_slot_407 tests/test_ctor_slot_407.cpp)
target_link_libraries(test_ctor_slot_407 sotc_core)
add_test(NAME ctor_slot_407 COMMAND test_ctor_slot_407)

add_executable(test_ctor_slot_413 tests/test_ctor_slot_413.cpp)
target_link_libraries(test_ctor_slot_413 sotc_core)
add_test(NAME ctor_slot_413 COMMAND test_ctor_slot_413)

add_executable(test_ctor_slot_415 tests/test_ctor_slot_415.cpp)
target_link_libraries(test_ctor_slot_415 sotc_core)
add_test(NAME ctor_slot_415 COMMAND test_ctor_slot_415)

add_executable(test_ctor_slot_419 tests/test_ctor_slot_419.cpp)
target_link_libraries(test_ctor_slot_419 sotc_core)
add_test(NAME ctor_slot_419 COMMAND test_ctor_slot_419)

add_executable(test_ctor_slot_421 tests/test_ctor_slot_421.cpp)
target_link_libraries(test_ctor_slot_421 sotc_core)
add_test(NAME ctor_slot_421 COMMAND test_ctor_slot_421)
