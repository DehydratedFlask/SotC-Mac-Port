# Lane M6: verified constructor fixtures 448 507 539 544 557 558 559 569; 379/518 remain excluded.
# Paths are relative to native/ (this file is include()d from native/CMakeLists.txt).
target_sources(sotc_core PRIVATE
    core/lane_m6_shapes.cpp
    core/ctor_slot_448_4b8680.cpp
    core/ctor_slot_507_22f3c0.cpp
    core/ctor_slot_539_121940.cpp
    core/ctor_slot_544_d1b20.cpp
    core/ctor_slot_557_47490.cpp
    core/ctor_slot_558_428a0.cpp
    core/ctor_slot_559_3f2d0.cpp
    core/ctor_slot_569_1cc80.cpp)
add_executable(test_ctor_slot_544 tests/test_ctor_slot_544.cpp)
target_link_libraries(test_ctor_slot_544 sotc_core)
add_test(NAME ctor_slot_544 COMMAND test_ctor_slot_544)
add_executable(test_ctor_slot_448 tests/test_ctor_slot_448.cpp)
target_link_libraries(test_ctor_slot_448 sotc_core)
add_test(NAME ctor_slot_448 COMMAND test_ctor_slot_448)
add_executable(test_ctor_slot_507 tests/test_ctor_slot_507.cpp)
target_link_libraries(test_ctor_slot_507 sotc_core)
add_test(NAME ctor_slot_507 COMMAND test_ctor_slot_507)
add_executable(test_ctor_slot_539 tests/test_ctor_slot_539.cpp)
target_link_libraries(test_ctor_slot_539 sotc_core)
add_test(NAME ctor_slot_539 COMMAND test_ctor_slot_539)
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
