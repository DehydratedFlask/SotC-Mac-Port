# Lane M6: startup constructor slots 544 557 558 559 569 448 379 518 507 539 (native/lanes/m6.cmake).
# Paths are relative to native/ (this file is include()d from native/CMakeLists.txt).
target_sources(sotc_core PRIVATE
    core/lane_m6_shapes.cpp
    core/ctor_slot_544_d1b20.cpp
    core/ctor_slot_557_47490.cpp
    core/ctor_slot_558_428a0.cpp
    core/ctor_slot_559_3f2d0.cpp
    core/ctor_slot_569_1cc80.cpp)
add_executable(test_ctor_slot_544 tests/test_ctor_slot_544.cpp)
target_link_libraries(test_ctor_slot_544 sotc_core)
add_test(NAME ctor_slot_544 COMMAND test_ctor_slot_544)
add_executable(test_ctor_slot_557 tests/test_ctor_slot_557.cpp)
target_link_libraries(test_ctor_slot_557 sotc_core)
add_test(NAME ctor_slot_557 COMMAND test_ctor_slot_557)
add_executable(test_ctor_slot_558 tests/test_ctor_slot_558.cpp)
target_link_libraries(test_ctor_slot_558 sotc_core)
add_test(NAME ctor_slot_558 COMMAND test_ctor_slot_558)
add_executable(test_ctor_slot_559 tests/test_ctor_slot_559.cpp)
target_link_libraries(test_ctor_slot_559 sotc_core)
add_test(NAME ctor_slot_559 COMMAND test_ctor_slot_559)
add_executable(test_ctor_slot_569 tests/test_ctor_slot_569.cpp)
target_link_libraries(test_ctor_slot_569 sotc_core)
add_test(NAME ctor_slot_569 COMMAND test_ctor_slot_569)
