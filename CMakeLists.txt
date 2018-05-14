cmake_minimum_required(VERSION 2.8)

project (symfirm)
set (CMAKE_CXX_STANDARD 17)

option (BUILD_EXAMPLE "Build examples" ON)

add_library(symfirm     
    include/symfirm/fpsx.h
    include/symfirm/rofs.h
    src/fpsx.cpp
    src/rofs.cpp
    src/utils.h)

target_include_directories(symfirm PUBLIC include)

if (BUILD_EXAMPLE)
    add_executable(rofsdump example/rofsdump.cpp)
    add_executable(fpsxdump example/fpsxdump.cpp)
    target_link_libraries(rofsdump PUBLIC symfirm)
    target_link_libraries(fpsxdump PUBLIC symfirm)
endif()