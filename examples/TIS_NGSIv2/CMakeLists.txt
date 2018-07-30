project(TIS_NGSIv2)
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

# cURLpp as thirdparty
set(CURLPP_DIR ../../thirdparty/curlpp)
file(GLOB_RECURSE CURLPP_HEADERS ${CURLPP_DIR}/include/*)
file(GLOB_RECURSE CURLPP_SOURCES ${CURLPP_DIR}/src/*)

if(WIN32)
    set(CURL_BUILD_DIR ../../thirdparty/curl/builds/libcurl-vc14-x64-release-dll-ipv6-sspi-winssl)

    LINK_DIRECTORIES(${CURL_BUILD_DIR}/lib)
    LINK_DIRECTORIES(${CURL_BUILD_DIR}/bin)

    set(liblist libcurl)
else()
    set(liblist -L/usr/lib/x86_64-linux-gnu curl)
endif()

if(WIN32)
    add_executable(cb_create_robot cb_create_robot.cpp ${CURLPP_HEADERS} ${CURLPP_SOURCES})
    add_executable(cb_get_robot cb_get_robot.cpp ${CURLPP_HEADERS} ${CURLPP_SOURCES})
    add_executable(cb_update_attr cb_update_attr.cpp ${CURLPP_HEADERS} ${CURLPP_SOURCES})
    target_include_directories(cb_create_robot PUBLIC ${CURLPP_DIR}/include ${CURL_BUILD_DIR}/include)
    target_include_directories(cb_get_robot PUBLIC ${CURLPP_DIR}/include ${CURL_BUILD_DIR}/include)
    target_include_directories(cb_update_attr PUBLIC ${CURLPP_DIR}/include ${CURL_BUILD_DIR}/include)
    configure_file(${CURL_BUILD_DIR}/bin/libcurl.dll ${CMAKE_CURRENT_BINARY_DIR}/Release/libcurl.dll COPYONLY)
else()
    add_executable(cb_create_robot cb_create_robot.cpp ${CURLPP_HEADERS} ${CURLPP_SOURCES})
    add_executable(cb_get_robot cb_get_robot.cpp ${CURLPP_HEADERS} ${CURLPP_SOURCES})
    add_executable(cb_update_attr cb_update_attr.cpp ${CURLPP_HEADERS} ${CURLPP_SOURCES})
    target_include_directories(cb_create_robot PUBLIC ${CURLPP_DIR}/include)
    target_include_directories(cb_get_robot PUBLIC ${CURLPP_DIR}/include)
    target_include_directories(cb_update_attr PUBLIC ${CURLPP_DIR}/include)
endif()

add_executable(RobotExample RobotExample_main.cpp
    RobotExamplePublisher.cpp RobotExampleSubscriber.cpp
    RobotPosition.cxx RobotPositionPubSubTypes.cxx
    RobotRcv.cxx RobotRcvPubSubTypes.cxx
    RobotSnd.cxx RobotSndPubSubTypes.cxx)

add_library(userlib SHARED ${USER_LIB_SOURCES_CXX} userlib.cpp
    RobotPosition.cxx RobotPositionPubSubTypes.cxx
    RobotRcv.cxx RobotRcvPubSubTypes.cxx
    RobotSnd.cxx RobotSndPubSubTypes.cxx
    ../../src/jsoncpp.cpp
    ../../src/NGSIv2/idl/JsonNGSIv2.cxx
    ../../src/NGSIv2/idl/JsonNGSIv2PubSubTypes.cxx)

#add_library(userlib2 SHARED ${USER_LIB_SOURCES_CXX} userlib2.cpp jsoncpp.cpp ...)

target_include_directories(userlib PUBLIC ../../src)

target_link_libraries(userlib fastrtps fastcdr ${CMAKE_DL_LIBS})
#target_link_libraries(userlib2 fastrtps fastcdr ${CMAKE_DL_LIBS})
target_link_libraries(cb_create_robot  ${liblist})
target_link_libraries(cb_get_robot  ${liblist})
target_link_libraries(cb_update_attr  ${liblist})
target_link_libraries(RobotExample fastrtps fastcdr ${liblist})
