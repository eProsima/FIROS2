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

if(WIN32)
    set(CURL_BUILD_DIR ../../thirdparty/curl/builds/libcurl-vc14-x64-release-dll-ipv6-sspi-winssl)
    set(CURLPP_DIR ../../thirdparty/curlpp)
    
    LINK_DIRECTORIES(${CURL_BUILD_DIR}/lib)
    LINK_DIRECTORIES(${CURL_BUILD_DIR}/bin)

    # cURLpp as thirdparty
    file(GLOB_RECURSE CURLPP_HEADERS ${CURLPP_DIR}/include/*)
    file(GLOB_RECURSE CURLPP_SOURCES ${CURLPP_DIR}/src/*)

    set(liblist libcurl)
else()
    set(liblist -L/usr/lib/x86_64-linux-gnu curlpp curl)
endif()

file(GLOB USER_LIB_SOURCES_CXX "*.cxx")
file(GLOB USER_LIB_SOURCES_CPP "userlib.cpp")

add_library(userlib SHARED ${USER_LIB_SOURCES_CXX} ${USER_LIB_SOURCES_CPP}
    ../../src/NGSIv2/idl/JsonNGSIv2.cxx
    ../../src/NGSIv2/idl/JsonNGSIv2PubSubTypes.cxx
    ../../src/jsoncpp.cpp)

if(WIN32)
    add_executable(cb_create_helloworld cb_create_helloworld.cpp ${CURLPP_HEADERS} ${CURLPP_SOURCES})
    add_executable(cb_update_attr cb_update_attr.cpp ${CURLPP_HEADERS} ${CURLPP_SOURCES})
    target_include_directories(cb_create_helloworld PUBLIC ${CURLPP_DIR}/include ${CURL_BUILD_DIR}/include)
    target_include_directories(cb_update_attr PUBLIC ${CURLPP_DIR}/include ${CURL_BUILD_DIR}/include)
    configure_file(${CURL_BUILD_DIR}/bin/libcurl.dll ${CMAKE_CURRENT_BINARY_DIR}/Release/libcurl.dll COPYONLY)
else()
    add_executable(cb_create_helloworld cb_create_helloworld.cpp)
    add_executable(cb_update_attr cb_update_attr.cpp)
endif()

target_include_directories(userlib PUBLIC ../../src)

target_link_libraries(userlib fastrtps fastcdr ${CMAKE_DL_LIBS})
target_link_libraries(cb_create_helloworld ${liblist})
target_link_libraries(cb_update_attr ${liblist})
