cmake_minimum_required(VERSION 3.15)
project(source_id)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

include_directories(sourceid ${CMAKE_SOURCE_DIR}/public)

add_library(sourceid ${CMAKE_SOURCE_DIR}/private/source_id.cpp)

find_package(FLEX REQUIRED)
FLEX_TARGET(dolex ${CMAKE_SOURCE_DIR}/test/preprocess.lex ${CMAKE_SOURCE_DIR}/test/preprocess.cpp)

add_executable(sourceidtest ${CMAKE_SOURCE_DIR}/test/test.cpp
                            ${CMAKE_SOURCE_DIR}/test/preprocess.cpp)
target_compile_definitions(sourceidtest PRIVATE
    BASE_DIR="${CMAKE_SOURCE_DIR}")
target_link_libraries(sourceidtest sourceid)
