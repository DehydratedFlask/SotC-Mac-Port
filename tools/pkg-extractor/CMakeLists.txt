cmake_minimum_required(VERSION 3.20)
project(SoCPkgExtractor LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(UPSTREAM "${CMAKE_CURRENT_LIST_DIR}/../../reference/shadPS4Plus-pkg")
find_package(ZLIB REQUIRED)
find_path(CRYPTOPP_INCLUDE_DIR cryptopp/aes.h REQUIRED)
find_library(CRYPTOPP_LIBRARY cryptopp REQUIRED)
add_executable(pkg_extractor
  ${UPSTREAM}/extractor/main.cpp
  ${UPSTREAM}/src/core/loader.cpp
  ${UPSTREAM}/src/core/file_format/pkg.cpp
  ${UPSTREAM}/src/core/file_format/psf.cpp
  ${UPSTREAM}/src/core/file_format/pkg_type.cpp
  ${UPSTREAM}/src/core/crypto/crypto.cpp
  ${UPSTREAM}/src/common/io_file.cpp
  ${UPSTREAM}/src/common/assert.cpp
  ${UPSTREAM}/src/common/error.cpp
  ${UPSTREAM}/src/common/path_util.cpp
  ${UPSTREAM}/src/common/string_util.cpp)
target_include_directories(pkg_extractor PRIVATE ${UPSTREAM}/src ${CRYPTOPP_INCLUDE_DIR})
if(APPLE)
  # Darwin's off_t/fseeko are already 64 bit; upstream uses the Linux spelling.
  target_compile_definitions(pkg_extractor PRIVATE fseeko64=fseeko)
endif()
target_link_libraries(pkg_extractor PRIVATE ${CRYPTOPP_LIBRARY} ZLIB::ZLIB "-framework CoreFoundation")
