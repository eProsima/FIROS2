cmake_minimum_required(VERSION 2.8.3)

project(firos2)

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


find_package(ament_cmake REQUIRED)
find_package(fastrtps REQUIRED)
find_package(fastcdr REQUIRED)

add_executable(firosRF src/main.cpp)

add_library(dynamicload include/routing_service/dynamicload/dynamicload.cpp)
add_library(tinyxml2 include/routing_service/tinyxml2/tinyxml2.cpp)
add_library(RSManager include/routing_service/RSManager.cpp)
add_library(RSBridge include/routing_service/RSBridge.cpp)
add_library(GenericPubSubTypes include/routing_service/GenericPubSubTypes.cpp)


target_link_libraries(dynamicload  ${CMAKE_DL_LIBS})
target_link_libraries(GenericPubSubTypes fastrtps fastcdr)
target_link_libraries(RSBridge GenericPubSubTypes dynamicload)
target_link_libraries(RSManager RSBridge tinyxml2)

target_link_libraries(firosRF RSManager)

install(TARGETS firosRF DESTINATION bin)

ament_package()
