# Lane M4 fragment (startup constructor slots 463 465 475 476 478 479 482 486 489 490). Paths are relative to native/.
target_sources(sotc_core PRIVATE
  core/lane_m4_shapes.cpp
  core/ctor_slot_463_3e0040.cpp
  core/ctor_slot_465_3cc9e0.cpp
  core/ctor_slot_475_33cc60.cpp
  core/ctor_slot_476_329650.cpp
  core/ctor_slot_478_31eb00.cpp
  core/ctor_slot_479_31b660.cpp
  core/ctor_slot_482_2fb2a0.cpp
  core/ctor_slot_486_2d5e00.cpp
  core/ctor_slot_489_2acc10.cpp
  core/ctor_slot_490_296d90.cpp
)
add_executable(test_ctor_slot_463 tests/test_ctor_slot_463.cpp)
target_link_libraries(test_ctor_slot_463 sotc_core)
add_test(NAME ctor_slot_463 COMMAND test_ctor_slot_463)
add_executable(test_ctor_slot_465 tests/test_ctor_slot_465.cpp)
target_link_libraries(test_ctor_slot_465 sotc_core)
add_test(NAME ctor_slot_465 COMMAND test_ctor_slot_465)
add_executable(test_ctor_slot_475 tests/test_ctor_slot_475.cpp)
target_link_libraries(test_ctor_slot_475 sotc_core)
add_test(NAME ctor_slot_475 COMMAND test_ctor_slot_475)
add_executable(test_ctor_slot_476 tests/test_ctor_slot_476.cpp)
target_link_libraries(test_ctor_slot_476 sotc_core)
add_test(NAME ctor_slot_476 COMMAND test_ctor_slot_476)
add_executable(test_ctor_slot_478 tests/test_ctor_slot_478.cpp)
target_link_libraries(test_ctor_slot_478 sotc_core)
add_test(NAME ctor_slot_478 COMMAND test_ctor_slot_478)
add_executable(test_ctor_slot_479 tests/test_ctor_slot_479.cpp)
target_link_libraries(test_ctor_slot_479 sotc_core)
add_test(NAME ctor_slot_479 COMMAND test_ctor_slot_479)
add_executable(test_ctor_slot_482 tests/test_ctor_slot_482.cpp)
target_link_libraries(test_ctor_slot_482 sotc_core)
add_test(NAME ctor_slot_482 COMMAND test_ctor_slot_482)
add_executable(test_ctor_slot_486 tests/test_ctor_slot_486.cpp)
target_link_libraries(test_ctor_slot_486 sotc_core)
add_test(NAME ctor_slot_486 COMMAND test_ctor_slot_486)
add_executable(test_ctor_slot_489 tests/test_ctor_slot_489.cpp)
target_link_libraries(test_ctor_slot_489 sotc_core)
add_test(NAME ctor_slot_489 COMMAND test_ctor_slot_489)
add_executable(test_ctor_slot_490 tests/test_ctor_slot_490.cpp)
target_link_libraries(test_ctor_slot_490 sotc_core)
add_test(NAME ctor_slot_490 COMMAND test_ctor_slot_490)
