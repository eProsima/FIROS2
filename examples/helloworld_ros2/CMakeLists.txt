project(helloworld_ros2)
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

file(GLOB USER_LIB_SOURCES_CXX "*.cxx")
file(GLOB USER_LIB_SOURCES_CPP "userlib.cpp")

add_library(userlib_ros2 SHARED ${USER_LIB_SOURCES_CXX} ${USER_LIB_SOURCES_CPP}
    ../../src/NGSIv2/idl/JsonNGSIv2.cxx
    ../../src/NGSIv2/idl/JsonNGSIv2PubSubTypes.cxx
    ../../src/jsoncpp.cpp)

if(WIN32)
    add_executable(cb_create_helloworld_ros2 cb_create_helloworld.cpp ${CURLPP_HEADERS} ${CURLPP_SOURCES})
    add_executable(cb_update_attr_ros2 cb_update_attr.cpp ${CURLPP_HEADERS} ${CURLPP_SOURCES})
    target_include_directories(cb_create_helloworld_ros2 PUBLIC ${CURLPP_DIR}/include ${CURL_BUILD_DIR}/include)
    target_include_directories(cb_update_attr_ros2 PUBLIC ${CURLPP_DIR}/include ${CURL_BUILD_DIR}/include)
    configure_file(${CURL_BUILD_DIR}/bin/libcurl.dll ${CMAKE_CURRENT_BINARY_DIR}/Release/libcurl.dll COPYONLY)
else()
    add_executable(cb_create_helloworld_ros2 cb_create_helloworld.cpp ${CURLPP_HEADERS} ${CURLPP_SOURCES})
    add_executable(cb_update_attr_ros2 cb_update_attr.cpp ${CURLPP_HEADERS} ${CURLPP_SOURCES})
    target_include_directories(cb_create_helloworld_ros2 PUBLIC ${CURLPP_DIR}/include)
    target_include_directories(cb_update_attr_ros2 PUBLIC ${CURLPP_DIR}/include)
endif()

target_include_directories(userlib_ros2 PUBLIC ../../src)

target_link_libraries(userlib_ros2 fastrtps fastcdr ${CMAKE_DL_LIBS})
target_link_libraries(cb_create_helloworld_ros2 ${liblist})
target_link_libraries(cb_update_attr_ros2 ${liblist})

set_target_properties(userlib_ros2 PROPERTIES OUTPUT_NAME "userlib")
set_target_properties(cb_create_helloworld_ros2 PROPERTIES OUTPUT_NAME "cb_create_robot")
set_target_properties(cb_update_attr_ros2 PROPERTIES OUTPUT_NAME "cb_update_attr")

install(TARGETS cb_update_attr_ros2
    RUNTIME DESTINATION examples/${PROJECT_NAME}/build$<$<BOOL:${WIN32}>:/${CMAKE_BUILD_TYPE}>
    )

install(TARGETS cb_create_helloworld_ros2
    RUNTIME DESTINATION examples/${PROJECT_NAME}/build$<$<BOOL:${WIN32}>:/${CMAKE_BUILD_TYPE}>
    )

install(TARGETS userlib_ros2
    LIBRARY DESTINATION examples/${PROJECT_NAME}/build$<$<BOOL:${WIN32}>:/${CMAKE_BUILD_TYPE}>
    )

install(FILES
    config$<$<BOOL:${WIN32}>:_win>.xml
    DESTINATION examples/${PROJECT_NAME}
    )

if(WIN32)
    install(FILES
    ${CURL_BUILD_DIR}/bin/libcurl.dll
        DESTINATION examples/${PROJECT_NAME}/build$<$<BOOL:${WIN32}>:/${CMAKE_BUILD_TYPE}>
        )
endif()
