project(user_lib)
cmake_minimum_required(VERSION 2.8)

include(CheckCXXCompilerFlag)
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG OR
        CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    check_cxx_compiler_flag(--std=c++11 SUPPORTS_CXX11)
    if(SUPPORTS_CXX11)
        add_compile_options(--std=c++11)
    else()
        message(FATAL_ERROR "Compiler doesn't support C++11")
    endif()
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(Imu SHARED Imu_.cpp)
add_library(ImuPubSubTypes SHARED Imu_PubSubTypes.cpp)
add_library(Header SHARED Header_.cpp)
add_library(Vector3 SHARED Vector3_.cpp)
add_library(Quaternion SHARED Quaternion_.cpp)
add_library(Output SHARED Output.cpp)
add_library(OutputPubSubTypes SHARED OutputPubSubTypes.cpp)
add_library(testlib SHARED testlib.cpp)

target_link_libraries(Imu fastcdr Header Vector3 Quaternion)
target_link_libraries(ImuPubSubTypes Imu)
target_link_libraries(OutputPubSubTypes fastcdr Output)
target_link_libraries(testlib fastcdr ImuPubSubTypes OutputPubSubTypes ${CMAKE_DL_LIBS})

set_property(TARGET testlib PROPERTY POSITION_INDEPENDENT_CODE ON)
