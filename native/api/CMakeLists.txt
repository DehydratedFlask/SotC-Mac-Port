# native/api - versioned C ABI of the native game core (APP-01-CORE-ABI-01).
#
# Use from native/CMakeLists.txt with ONE line (after the sotc_core target is defined):
#     add_subdirectory(api)
# When the sotc_core target exists this library links it and reports real constructor-runner
# facts; otherwise it still builds and reports those facts as NOT_LINKED.
# Standalone: cmake -S native/api/standalone -B <dir>   (see standalone/CMakeLists.txt)
if(TARGET sotc_api)
  return()  # already added (standalone top level + a later add_subdirectory(api))
endif()

add_library(sotc_api STATIC sotc_core.cpp)
target_include_directories(sotc_api PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(sotc_api PROPERTIES CXX_VISIBILITY_PRESET default)
if(TARGET sotc_core)
  target_link_libraries(sotc_api PUBLIC sotc_core)
  target_compile_definitions(sotc_api PRIVATE SOTC_API_WITH_CORE_LIB=1)
else()
  target_compile_definitions(sotc_api PRIVATE SOTC_API_WITH_CORE_LIB=0)
endif()

enable_testing()
add_executable(test_sotc_core_api tests/test_sotc_core_api.cpp)
target_link_libraries(test_sotc_core_api sotc_api)
if(TARGET sotc_core)
  target_compile_definitions(test_sotc_core_api PRIVATE SOTC_TEST_CORE_LINKED=1)
  target_include_directories(test_sotc_core_api PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
else()
  target_compile_definitions(test_sotc_core_api PRIVATE SOTC_TEST_CORE_LINKED=0)
endif()
add_test(NAME sotc_core_api COMMAND test_sotc_core_api)

# Proves the header is plain C99 (needs the C language enabled by the top-level project).
get_property(_sotc_langs GLOBAL PROPERTY ENABLED_LANGUAGES)
if("C" IN_LIST _sotc_langs)
  add_executable(test_sotc_core_c tests/test_sotc_core_c.c)
  target_link_libraries(test_sotc_core_c sotc_api)
  set_target_properties(test_sotc_core_c PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED ON LINKER_LANGUAGE CXX)
  add_test(NAME sotc_core_c COMMAND test_sotc_core_c)
endif()
