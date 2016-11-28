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

add_library(tinyxml2 include/tinyxml2/tinyxml2.cpp)
add_library(RSManager include/routing_service/RSManager.cpp)
add_library(DummyPubSub include/routing_service/DummyPubSub.cpp)
add_library(DummyPubSubTypes include/routing_service/DummyPubSubTypes.cpp)

target_link_libraries(DummyPubSubTypes fastrtps fastcdr)
target_link_libraries(DummyPubSub DummyPubSubTypes)
target_link_libraries(RSManager DummyPubSub)

target_link_libraries(	firosRF
						RSManager
                        tinyxml2
                        DummyPubSub
						${CMAKE_DL_LIBS}
                     )

install(TARGETS firosRF DESTINATION bin)

ament_package()
