cmake_minimum_required(VERSION 2.8.12)
project(cache)

# 设置 C++11 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_LDFLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${CMAKE_LDFLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${CMAKE_LDFLAGS}")

add_definitions(${CMAKE_CFLAGS})
add_definitions(${CMAKE_MACRO})

# 添加头文件搜索路径
include_directories(./include)
#include_directories("./src")

# 添加动态库的源文件
aux_source_directory(src/ CACHE_FILE)
add_library(cache SHARED ${CACHE_FILE})

# 生成文件到指定位置
set(LIBRARY_OUTPUT_PATH lib/)

#add_executable(MyExecutable test/test.cc cache)
