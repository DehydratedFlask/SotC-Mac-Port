# Lane m3: startup constructor slots 426 436 439 440 441 456 457 458 459 461 (paths relative to native/).
# Included by native/CMakeLists.txt through the lane-fragment glob; build alone with -DSOTC_LANES=m3.
target_sources(sotc_core PRIVATE
  core/ctor_slot_426_53fb50.cpp
  core/ctor_slot_436_50dbe0.cpp
  core/ctor_slot_439_4f7b70.cpp
  core/ctor_slot_440_4f0c60.cpp
  core/ctor_slot_441_4e68c0.cpp
  core/ctor_slot_456_471950.cpp
  core/ctor_slot_457_45c440.cpp
  core/ctor_slot_458_45ace0.cpp
  core/ctor_slot_459_43f460.cpp
  core/ctor_slot_461_419480.cpp
)
add_executable(test_ctor_slot_426 tests/test_ctor_slot_426.cpp)
target_link_libraries(test_ctor_slot_426 sotc_core)
add_test(NAME ctor_slot_426 COMMAND test_ctor_slot_426)
add_executable(test_ctor_slot_436 tests/test_ctor_slot_436.cpp)
target_link_libraries(test_ctor_slot_436 sotc_core)
add_test(NAME ctor_slot_436 COMMAND test_ctor_slot_436)
add_executable(test_ctor_slot_439 tests/test_ctor_slot_439.cpp)
target_link_libraries(test_ctor_slot_439 sotc_core)
add_test(NAME ctor_slot_439 COMMAND test_ctor_slot_439)
add_executable(test_ctor_slot_440 tests/test_ctor_slot_440.cpp)
target_link_libraries(test_ctor_slot_440 sotc_core)
add_test(NAME ctor_slot_440 COMMAND test_ctor_slot_440)
add_executable(test_ctor_slot_441 tests/test_ctor_slot_441.cpp)
target_link_libraries(test_ctor_slot_441 sotc_core)
add_test(NAME ctor_slot_441 COMMAND test_ctor_slot_441)
add_executable(test_ctor_slot_456 tests/test_ctor_slot_456.cpp)
target_link_libraries(test_ctor_slot_456 sotc_core)
add_test(NAME ctor_slot_456 COMMAND test_ctor_slot_456)
add_executable(test_ctor_slot_457 tests/test_ctor_slot_457.cpp)
target_link_libraries(test_ctor_slot_457 sotc_core)
add_test(NAME ctor_slot_457 COMMAND test_ctor_slot_457)
add_executable(test_ctor_slot_458 tests/test_ctor_slot_458.cpp)
target_link_libraries(test_ctor_slot_458 sotc_core)
add_test(NAME ctor_slot_458 COMMAND test_ctor_slot_458)
add_executable(test_ctor_slot_459 tests/test_ctor_slot_459.cpp)
target_link_libraries(test_ctor_slot_459 sotc_core)
add_test(NAME ctor_slot_459 COMMAND test_ctor_slot_459)
add_executable(test_ctor_slot_461 tests/test_ctor_slot_461.cpp)
target_link_libraries(test_ctor_slot_461 sotc_core)
add_test(NAME ctor_slot_461 COMMAND test_ctor_slot_461)
