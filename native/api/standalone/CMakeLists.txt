# Standalone top level for the ABI library; does not touch native/CMakeLists.txt.
#   cmake -S native/api/standalone -B local/build/native-agent-c [-DSOTC_API_LINK_CORE=OFF]
#   cmake --build local/build/native-agent-c -j 8 && ctest --test-dir local/build/native-agent-c
cmake_minimum_required(VERSION 3.20)
project(sotc_api_standalone LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES arm64 CACHE STRING "native Apple Silicon only" FORCE)
add_compile_options(-Wall -Wextra -Werror -ffp-contract=off)
option(SOTC_SANITIZE "ASan+UBSan for tests" ON)
if(SOTC_SANITIZE)
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address,undefined)
endif()
option(SOTC_API_LINK_CORE "link native/core (runs the constructor-runner prefix)" ON)
enable_testing()
if(SOTC_API_LINK_CORE)
  # Builds native/CMakeLists.txt as a read-only subproject; only sotc_core is needed by the tests.
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../.. sotc_native EXCLUDE_FROM_ALL)
endif()
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/.. sotc_api)
