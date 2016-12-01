project(user_lib)
cmake_minimum_required(VERSION 2.9)

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

find_package(fastcdr)
find_package(fastrtps)

add_definitions(-DEPROSIMA_USER_DLL_EXPORT  -DBUILD_SHARED_LIBS)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(BUILD_SHARED_LIBS TRUE)

add_library(testlib SHARED  Imu_.cpp
                            Imu_PubSubTypes.cpp
                            Header_.cpp
                            Vector3_.cpp
                            Quaternion_.cpp
                            Output.cpp
                            OutputPubSubTypes.cpp
                            testlib.cpp
)

target_link_libraries(testlib fastrtps fastcdr ${CMAKE_DL_LIBS})
