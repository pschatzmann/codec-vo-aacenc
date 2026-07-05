cmake_minimum_required(VERSION 3.16)

project(VOAACEncoder LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

file(GLOB VOAAC_C_SOURCES CONFIGURE_DEPENDS src/vo-aacenc/*.c)

add_library(VOAACEncoder STATIC
  ${VOAAC_C_SOURCES}
)

target_include_directories(VOAACEncoder PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Keep compatibility with existing C code assumptions.
target_compile_definitions(VOAACEncoder PRIVATE
  ARMV5E
)
