cmake_minimum_required(VERSION 2.8.3)

###############################################################################
# Version information                                                         #
###############################################################################

#If not set, copy over product version from configure.ac
file(STRINGS
    configure.ac
    LIB_MAJOR_VERSION_TMP
    REGEX "^m4_define\\(\\[?version_major\\]?, *\\[?[0-9]+\\]?\\)"
    )
file(STRINGS
    configure.ac
    LIB_MINOR_VERSION_TMP
    REGEX "^m4_define\\(\\[?version_minor\\]?, *\\[?[0-9]+\\]?\\)"
    )
file(STRINGS
    configure.ac
    LIB_PATCH_VERSION_TMP
    REGEX "^m4_define\\(\\[?version_micro\\]?, *\\[?[0-9]+\\]?\\)"
    )

string(REGEX REPLACE "m4_define\\(\\[?version_major\\]?, *\\[?([0-9]+)\\]?\\)"
    "\\1"
    LIB_MAJOR_VERSION
    ${LIB_MAJOR_VERSION_TMP}
    )
string(REGEX REPLACE "m4_define\\(\\[?version_minor\\]?, *\\[?([0-9]+)\\]?\\)"
    "\\1"
    LIB_MINOR_VERSION
    ${LIB_MINOR_VERSION_TMP}
    )
string(REGEX REPLACE "m4_define\\(\\[?version_micro\\]?, *\\[?([0-9]+)\\]?\\)"
    "\\1"
    LIB_PATCH_VERSION
    ${LIB_PATCH_VERSION_TMP}
    )

set(LIB_VERSION_STR "${LIB_MAJOR_VERSION}.${LIB_MINOR_VERSION}.${LIB_PATCH_VERSION}")

###############################################################################
# Project                                                                     #
###############################################################################
if(CMAKE_VERSION VERSION_LESS 3.0)
    project("firos2" C CXX)
    set(PROJECT_VERSION_MAJOR "${LIB_MAJOR_VERSION}")
    set(PROJECT_VERSION_MINOR "${LIB_MINOR_VERSION}")
    set(PROJECT_VERSION_PATCH "${LIB_PATCH_VERSION}")
    set(PROJECT_VERSION "${LIB_VERSION_STR}")
else()
    cmake_policy(SET CMP0048 NEW)
    project("firos2" VERSION "${LIB_VERSION_STR}" LANGUAGES C CXX)
endif()
set(PROJECT_NAME_STYLED "FIROS2")
set(PROJECT_NAME_LARGE "FIROS2")
set(PROJECT_NAME_UPPER "FIROS2")
set(NGSIV2LIB_NAME "isbridgengsiv2lib")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(CheckCXXCompilerFlag)
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG OR
        CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    check_cxx_compiler_flag(--std=c++14 SUPPORTS_CXX11)
    if(SUPPORTS_CXX11)
        add_compile_options(--std=c++14)
    else()
        message(FATAL_ERROR "Compiler doesn't support C++11")
    endif()
endif()

#set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
#set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

cmake_policy(SET CMP0057 NEW)

include(${PROJECT_SOURCE_DIR}/cmake/common/eprosima_libraries.cmake)

find_package(fastcdr REQUIRED)
find_package(fastrtps REQUIRED)

SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF("${isSystemDir}" STREQUAL "-1")


set(BIN_INSTALL_DIR bin/ CACHE PATH "Installation directory for binaries")
set(INCLUDE_INSTALL_DIR include/ CACHE PATH "Installation directory for C++ headers")
set(INCLUDE_SOURCE_DIR src/ CACHE PATH "Installation directory for C++ sources")
set(LIB_INSTALL_DIR lib/ CACHE PATH "Installation directory for libraries")
set(DATA_INSTALL_DIR share/ CACHE PATH "Installation directory for data")

if(CMAKE_CL_64)
    set(CURL_BUILD_DIR thirdparty/curl/builds/libcurl-vc14-x64-release-dll-ipv6-sspi-winssl)
else()
    set(CURL_BUILD_DIR thirdparty/curl/builds/libcurl-vc14-x86-release-dll-ipv6-sspi-winssl)
endif()

add_definitions(-DBUILD_SHARED_LIBS -Disbridgengsiv2lib_EXPORTS)
add_definitions(-DBUILD_SHARED_LIBS -Disdynbridgengsiv2lib_EXPORTS)
add_definitions(-D${PROJECT_NAME_UPPER}_SOURCE)

IF (NOT DEFINED LOG_LEVEL_DEBUG)
    SET(LOG_LEVEL_DEBUG OFF)
ENDIF()
ADD_DEFINITIONS(-DLOG_LEVEL_DEBUG=${LOG_LEVEL_DEBUG})

IF (NOT DEFINED LOG_LEVEL_INFO)
    SET(LOG_LEVEL_INFO OFF)
ENDIF()
ADD_DEFINITIONS(-DLOG_LEVEL_INFO=${LOG_LEVEL_INFO})

IF (NOT DEFINED LOG_LEVEL_WARN)
    SET(LOG_LEVEL_WARN OFF)
ENDIF()
ADD_DEFINITIONS(-DLOG_LEVEL_WARN=${LOG_LEVEL_WARN})

IF (NOT DEFINED LOG_LEVEL_ERROR)
    SET(LOG_LEVEL_ERROR ON)
ENDIF()
ADD_DEFINITIONS(-DLOG_LEVEL_ERROR=${LOG_LEVEL_ERROR})

configure_file(thirdparty/integration-services/src/log/ISLog.h.in ${PROJECT_SOURCE_DIR}/thirdparty/integration-services/src/log/ISLog.h)
configure_file(${PROJECT_SOURCE_DIR}/src/config.h.in
    ${PROJECT_BINARY_DIR}/include/${PROJECT_NAME}/config.h)

include(${PROJECT_SOURCE_DIR}/cmake/dev/set_sources.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/dev/generate_msvc_libraries.cmake)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(BUILD_SHARED_LIBS TRUE)

set(CURLPP_DIR thirdparty/curlpp)
LINK_DIRECTORIES(${CURL_BUILD_DIR}/lib)
LINK_DIRECTORIES(${CURL_BUILD_DIR}/bin)

# cURLpp as thirdparty
#file(GLOB_RECURSE CURLPP_HEADERS ${CURLPP_DIR}/include/*)
#file(GLOB_RECURSE CURLPP_SOURCES ${CURLPP_DIR}/src/*)
set(CURLPP_HEADERS
    ${CURLPP_DIR}/include/curlpp/cURLpp.hpp
    ${CURLPP_DIR}/include/curlpp/Easy.hpp
    ${CURLPP_DIR}/include/curlpp/Easy.inl
    ${CURLPP_DIR}/include/curlpp/Exception.hpp
    ${CURLPP_DIR}/include/curlpp/Form.hpp
    ${CURLPP_DIR}/include/curlpp/Info.hpp
    ${CURLPP_DIR}/include/curlpp/Info.inl
    ${CURLPP_DIR}/include/curlpp/Infos.hpp
    ${CURLPP_DIR}/include/curlpp/Multi.hpp
    ${CURLPP_DIR}/include/curlpp/Option.hpp
    ${CURLPP_DIR}/include/curlpp/Option.inl
    ${CURLPP_DIR}/include/curlpp/OptionBase.hpp
    ${CURLPP_DIR}/include/curlpp/Options.hpp
    ${CURLPP_DIR}/include/curlpp/Types.hpp
    ${CURLPP_DIR}/include/curlpp/internal/CurlHandle.hpp
    ${CURLPP_DIR}/include/curlpp/internal/CurlHandle.inl
    ${CURLPP_DIR}/include/curlpp/internal/OptionContainer.hpp
    ${CURLPP_DIR}/include/curlpp/internal/OptionContainer.inl
    ${CURLPP_DIR}/include/curlpp/internal/OptionContainerType.hpp
    ${CURLPP_DIR}/include/curlpp/internal/OptionList.hpp
    ${CURLPP_DIR}/include/curlpp/internal/OptionSetter.hpp
    ${CURLPP_DIR}/include/curlpp/internal/OptionSetter.inl
    ${CURLPP_DIR}/include/curlpp/internal/SList.hpp
    ${CURLPP_DIR}/include/utilspp/clone_ptr.hpp
    ${CURLPP_DIR}/include/utilspp/EmptyType.hpp
    ${CURLPP_DIR}/include/utilspp/NonCopyable.hpp
    ${CURLPP_DIR}/include/utilspp/NullType.hpp
    ${CURLPP_DIR}/include/utilspp/SmartPtr.hpp
    ${CURLPP_DIR}/include/utilspp/ThreadingFactoryMutex.hpp
    ${CURLPP_DIR}/include/utilspp/ThreadingFactoryMutex.inl
    ${CURLPP_DIR}/include/utilspp/ThreadingSingle.hpp
    ${CURLPP_DIR}/include/utilspp/ThreadingSingle.inl
    ${CURLPP_DIR}/include/utilspp/TypeList.hpp
    ${CURLPP_DIR}/include/utilspp/TypeTrait.hpp)

set(CURLPP_SOURCES
    ${CURLPP_DIR}/src/curlpp/cURLpp.cpp
    ${CURLPP_DIR}/src/curlpp/Easy.cpp
    ${CURLPP_DIR}/src/curlpp/Exception.cpp
    ${CURLPP_DIR}/src/curlpp/Form.cpp
    ${CURLPP_DIR}/src/curlpp/Info.cpp
    ${CURLPP_DIR}/src/curlpp/Multi.cpp
    ${CURLPP_DIR}/src/curlpp/OptionBase.cpp
    ${CURLPP_DIR}/src/curlpp/Options.cpp
    ${CURLPP_DIR}/src/curlpp/internal/CurlHandle.cpp
    ${CURLPP_DIR}/src/curlpp/internal/OptionList.cpp
    ${CURLPP_DIR}/src/curlpp/internal/OptionSetter.cpp
    ${CURLPP_DIR}/src/curlpp/internal/SList.cpp)

set(CURL_FILES ${CURLPP_HEADERS} ${CURLPP_SOURCES})
set(IS_LIB_FILES thirdparty/integration-services/src/ISManager.cpp thirdparty/integration-services/src/ISManager.h
    thirdparty/integration-services/src/ISBridge.h
    thirdparty/integration-services/src/log/ISLog.h
    thirdparty/integration-services/src/RTPSBridge.cpp thirdparty/integration-services/src/RTPSBridge.h
    thirdparty/integration-services/src/RTPSPublisher.cpp thirdparty/integration-services/src/RTPSPublisher.h
    thirdparty/integration-services/src/RTPSSubscriber.cpp thirdparty/integration-services/src/RTPSSubscriber.h
    thirdparty/integration-services/src/dynamicload/dynamicload.cpp thirdparty/integration-services/src/dynamicload/dynamicload.h
    thirdparty/integration-services/src/GenericPubSubTypes.cpp thirdparty/integration-services/src/GenericPubSubTypes.h
    thirdparty/integration-services/src/xmlUtils.h)

set(NGSIv2_FILES src/NGSIv2/NGSIv2Params.cpp src/NGSIv2/NGSIv2Params.h
        src/NGSIv2/NGSIv2ISBridge.cpp src/NGSIv2/NGSIv2ISBridge.h
        src/NGSIv2/NGSIv2Publisher.cpp src/NGSIv2/NGSIv2Publisher.h
        src/NGSIv2/NGSIv2Subscriber.cpp src/NGSIv2/NGSIv2Subscriber.h
        src/NGSIv2/NGSIv2Utils.cpp src/NGSIv2/NGSIv2Utils.h
        src/jsoncpp.cpp
        src/NGSIv2/rtpsngsiv2lib.cpp
        src/NGSIv2/idl/JsonNGSIv2.cxx src/NGSIv2/idl/JsonNGSIv2.h
        src/NGSIv2/idl/JsonNGSIv2PubSubTypes.cxx src/NGSIv2/idl/JsonNGSIv2PubSubTypes.h)

set(DYN_NGSIv2_FILES src/DynNGSIv2/NGSIv2Params.cpp src/DynNGSIv2/NGSIv2Params.h
        src/DynNGSIv2/DynNGSIv2ISBridge.cpp src/DynNGSIv2/DynNGSIv2ISBridge.h
        src/DynNGSIv2/DynNGSIv2Publisher.cpp src/DynNGSIv2/DynNGSIv2Publisher.h
        src/DynNGSIv2/DynNGSIv2Subscriber.cpp src/DynNGSIv2/DynNGSIv2Subscriber.h
        src/DynNGSIv2/NGSIv2Utils.cpp src/DynNGSIv2/NGSIv2Utils.h
        src/jsoncpp.cpp
        src/DynNGSIv2/rtpsngsiv2lib.cpp)

set(THIRDPARTY_INCLUDE thirdparty/tinyxml2 thirdparty/curlpp/include thirdparty/asio/asio/include)

if(WIN32)
    add_executable(firos2 src/main.cpp
        ${IS_LIB_FILES}
        ${CURL_FILES}
        thirdparty/tinyxml2/tinyxml2.cpp thirdparty/tinyxml2/tinyxml2.h)
    source_group("is_lib" FILES ${IS_LIB_FILES})
    source_group("curl" FILES ${CURL_FILES})

    add_library(isbridgengsiv2lib
        ${NGSIv2_FILES}
        ${IS_LIB_FILES}
        ${CURL_FILES}
        thirdparty/tinyxml2/tinyxml2.cpp thirdparty/tinyxml2/tinyxml2.h)

    add_library(isdynbridgengsiv2lib
        ${DYN_NGSIv2_FILES}
        ${IS_LIB_FILES}
        ${CURL_FILES}
        thirdparty/tinyxml2/tinyxml2.cpp thirdparty/tinyxml2/tinyxml2.h)

    source_group("curl" FILES ${CURL_FILES})
    source_group("is_lib" FILES ${IS_LIB_FILES})

    # Include thirdparty (ASIO only in WIN32)
    add_definitions(-DASIO_STANDALONE)
    target_include_directories(firos2 PRIVATE ${THIRDPARTY_INCLUDE} ${CURL_BUILD_DIR}/include)
    target_include_directories(isbridgengsiv2lib PRIVATE ${THIRDPARTY_INCLUDE} ${CURL_BUILD_DIR}/include src)
    target_include_directories(isdynbridgengsiv2lib PRIVATE ${THIRDPARTY_INCLUDE} ${CURL_BUILD_DIR}/include src)

    #set(liblist fastrtps_static fastcdr_static ${CMAKE_DL_LIBS} IPHLPAPI shlwapi libcurl_a)
    set(liblist fastrtps fastcdr ${CMAKE_DL_LIBS} IPHLPAPI shlwapi libcurl)
else()
    add_executable(firos2 src/main.cpp
        ${IS_LIB_FILES}
        ${CURL_FILES}
        thirdparty/tinyxml2/tinyxml2.cpp thirdparty/tinyxml2/tinyxml2.h)

    add_library(isbridgengsiv2lib SHARED
        ${NGSIv2_FILES}
        ${IS_LIB_FILES}
        ${CURL_FILES}
        thirdparty/tinyxml2/tinyxml2.cpp thirdparty/tinyxml2/tinyxml2.h)

    add_library(isdynbridgengsiv2lib SHARED
        ${DYN_NGSIv2_FILES}
        ${IS_LIB_FILES}
        ${CURL_FILES}
        thirdparty/tinyxml2/tinyxml2.cpp thirdparty/tinyxml2/tinyxml2.h)

    target_include_directories(firos2 PRIVATE ${THIRDPARTY_INCLUDE})
    target_include_directories(isbridgengsiv2lib PRIVATE ${THIRDPARTY_INCLUDE} src)
    target_include_directories(isdynbridgengsiv2lib PRIVATE ${THIRDPARTY_INCLUDE} src)

    set(liblist fastrtps fastcdr -L/usr/lib/x86_64-linux-gnu curl)
endif()

target_link_libraries(firos2 ${CMAKE_DL_LIBS} ${liblist})

target_link_libraries(isbridgengsiv2lib
    ${liblist}
    ${CMAKE_DL_LIBS})

target_link_libraries(isdynbridgengsiv2lib
    ${liblist}
    ${CMAKE_DL_LIBS})

set(CPACK_COMPONENT_HEADERS_DISPLAY_NAME "C++ Headers" PARENT_SCOPE)
set(CPACK_COMPONENT_HEADERS_DESCRIPTION "eProsima ${PROJECT_NAME_LARGE} C++ Headers" PARENT_SCOPE)


# Add sources to Makefile.am
#set_sources(
#    ${NGSIv2_FILES}
#    ${IS_LIB_FILES}
#    ${CURL_FILES}
#    thirdparty/tinyxml2/tinyxml2.cpp thirdparty/tinyxml2/tinyxml2.h)

set(firos2_SOURCES ${firos2_SOURCES}
    ${IS_LIB_FILES}
    ${CURL_FILES}
    thirdparty/tinyxml2/tinyxml2.cpp thirdparty/tinyxml2/tinyxml2.h)
set(isbridgengsiv2lib_SOURCES ${isbridgengsiv2lib_SOURCES}
    ${NGSIv2_FILES}
    ${IS_LIB_FILES}
    ${CURL_FILES}
    thirdparty/tinyxml2/tinyxml2.cpp thirdparty/tinyxml2/tinyxml2.h)
set(isdynbridgengsiv2lib_SOURCES ${isdynbridgengsiv2lib_SOURCES}
    ${DYN_NGSIv2_FILES}
    ${IS_LIB_FILES}
    ${CURL_FILES}
    thirdparty/tinyxml2/tinyxml2.cpp thirdparty/tinyxml2/tinyxml2.h)
set_property(GLOBAL PROPERTY firos2_SOURCES_PROPERTY ${firos2_SOURCES})
set_property(GLOBAL PROPERTY isbridgengsiv2lib_SOURCES_PROPERTY ${isbridgengsiv2lib_SOURCES})
set_property(GLOBAL PROPERTY isdynbridgengsiv2lib_SOURCES_PROPERTY ${isdynbridgengsiv2lib_SOURCES})

###############################################################################
# Test system configuration
###############################################################################
include(${PROJECT_SOURCE_DIR}/cmake/common/check_configuration.cmake)
check_stdcxx()
check_compile_feature()
check_endianness()

###############################################################################
# Check MSVC architecture
###############################################################################
if(MSVC OR MSVC_IDE)
    check_msvc_arch()
endif()

# Create libraries
if(MSVC OR MSVC_IDE)
    if(EPROSIMA_INSTALLER)
        generate_msvc_libraries(${MSVC_ARCH})
    else()
        set_target_properties(${NGSIV2LIB_NAME} PROPERTIES RELEASE_POSTFIX -${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
        set_target_properties(${NGSIV2LIB_NAME} PROPERTIES RELWITHDEBINFO_POSTFIX -${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
        set_target_properties(${NGSIV2LIB_NAME} PROPERTIES DEBUG_POSTFIX d-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})

        if("${CMAKE_SYSTEM_NAME}" STREQUAL "WindowsStore")
            set_target_properties(${NGSIV2LIB_NAME} PROPERTIES VS_WINRT_COMPONENT "true")
        endif()
    endif()
endif()


if(EPROSIMA_INSTALLER)
    install(
        FILES
            src/jsoncpp.cpp
            src/main.cpp
        COMPONENT sources
        DESTINATION ${INCLUDE_SOURCE_DIR}/src)

    install(
        FILES
            src/NGSIv2/NGSIv2Params.cpp src/NGSIv2/NGSIv2Params.h
            src/NGSIv2/NGSIv2ISBridge.cpp src/NGSIv2/NGSIv2ISBridge.h
            src/NGSIv2/NGSIv2Publisher.cpp src/NGSIv2/NGSIv2Publisher.h
            src/NGSIv2/NGSIv2Subscriber.cpp src/NGSIv2/NGSIv2Subscriber.h
            src/NGSIv2/NGSIv2Utils.cpp src/NGSIv2/NGSIv2Utils.h
            src/NGSIv2/rtpsngsiv2lib.cpp
        COMPONENT sources
        DESTINATION ${INCLUDE_SOURCE_DIR}/src/NGSIv2)

    install(
        FILES
            src/NGSIv2/idl/JsonNGSIv2.cxx src/NGSIv2/idl/JsonNGSIv2.h
            src/NGSIv2/idl/JsonNGSIv2PubSubTypes.cxx src/NGSIv2/idl/JsonNGSIv2PubSubTypes.h
        COMPONENT sources
        DESTINATION ${INCLUDE_SOURCE_DIR}/src/NGSIv2/idl)

    install(
        FILES
            thirdparty/integration-services/src/ISManager.cpp thirdparty/integration-services/src/ISManager.h
            thirdparty/integration-services/src/ISBridge.h
            thirdparty/integration-services/src/RTPSBridge.cpp thirdparty/integration-services/src/RTPSBridge.h
            thirdparty/integration-services/src/RTPSPublisher.cpp thirdparty/integration-services/src/RTPSPublisher.h
            thirdparty/integration-services/src/RTPSSubscriber.cpp thirdparty/integration-services/src/RTPSSubscriber.h
            thirdparty/integration-services/src/GenericPubSubTypes.cpp thirdparty/integration-services/src/GenericPubSubTypes.h
            thirdparty/integration-services/src/xmlUtils.h
        COMPONENT sources
        DESTINATION ${INCLUDE_SOURCE_DIR}/thirdparty/integration-services/src)

    install(
        FILES
            thirdparty/integration-services/src/log/ISLog.h
        COMPONENT sources
        DESTINATION ${INCLUDE_SOURCE_DIR}/thirdparty/integration-services/src/log)

    install(
        FILES
            thirdparty/integration-services/src/dynamicload/dynamicload.cpp
            thirdparty/integration-services/src/dynamicload/dynamicload.h
        COMPONENT sources
        DESTINATION ${INCLUDE_SOURCE_DIR}/thirdparty/integration-services/src/dynamicload)

    install(
        FILES
            thirdparty/tinyxml2/tinyxml2.cpp thirdparty/tinyxml2/tinyxml2.h
        COMPONENT sources
        DESTINATION ${INCLUDE_SOURCE_DIR}/thirdparty/tinyxml2)

    install(
        DIRECTORY ${CURLPP_DIR}
        DESTINATION ${INCLUDE_SOURCE_DIR}
        COMPONENT sources
    )
    install(
        FILES
            ${CURLPP_DIR}/include/curlpp/cURLpp.hpp
            ${CURLPP_DIR}/include/curlpp/Easy.hpp
            ${CURLPP_DIR}/include/curlpp/Easy.inl
            ${CURLPP_DIR}/include/curlpp/Exception.hpp
            ${CURLPP_DIR}/include/curlpp/Form.hpp
            ${CURLPP_DIR}/include/curlpp/Info.hpp
            ${CURLPP_DIR}/include/curlpp/Info.inl
            ${CURLPP_DIR}/include/curlpp/Infos.hpp
            ${CURLPP_DIR}/include/curlpp/Multi.hpp
            ${CURLPP_DIR}/include/curlpp/Option.hpp
            ${CURLPP_DIR}/include/curlpp/Option.inl
            ${CURLPP_DIR}/include/curlpp/OptionBase.hpp
            ${CURLPP_DIR}/include/curlpp/Options.hpp
            ${CURLPP_DIR}/include/curlpp/Types.hpp
        COMPONENT sources
        DESTINATION ${INCLUDE_SOURCE_DIR}/${CURLPP_DIR}/include/curlpp)

    install(
        FILES
            ${CURLPP_DIR}/include/curlpp/internal/CurlHandle.hpp
            ${CURLPP_DIR}/include/curlpp/internal/CurlHandle.inl
            ${CURLPP_DIR}/include/curlpp/internal/OptionContainer.hpp
            ${CURLPP_DIR}/include/curlpp/internal/OptionContainer.inl
            ${CURLPP_DIR}/include/curlpp/internal/OptionContainerType.hpp
            ${CURLPP_DIR}/include/curlpp/internal/OptionList.hpp
            ${CURLPP_DIR}/include/curlpp/internal/OptionSetter.hpp
            ${CURLPP_DIR}/include/curlpp/internal/OptionSetter.inl
            ${CURLPP_DIR}/include/curlpp/internal/SList.hpp
        COMPONENT sources
        DESTINATION ${INCLUDE_SOURCE_DIR}/${CURLPP_DIR}/include/curlpp/internal)

    install(
        FILES
            ${CURLPP_DIR}/include/utilspp/clone_ptr.hpp
            ${CURLPP_DIR}/include/utilspp/EmptyType.hpp
            ${CURLPP_DIR}/include/utilspp/NonCopyable.hpp
            ${CURLPP_DIR}/include/utilspp/NullType.hpp
            ${CURLPP_DIR}/include/utilspp/SmartPtr.hpp
            ${CURLPP_DIR}/include/utilspp/ThreadingFactoryMutex.hpp
            ${CURLPP_DIR}/include/utilspp/ThreadingFactoryMutex.inl
            ${CURLPP_DIR}/include/utilspp/ThreadingSingle.hpp
            ${CURLPP_DIR}/include/utilspp/ThreadingSingle.inl
            ${CURLPP_DIR}/include/utilspp/TypeList.hpp
            ${CURLPP_DIR}/include/utilspp/TypeTrait.hpp
        COMPONENT sources
        DESTINATION ${INCLUDE_SOURCE_DIR}/${CURLPP_DIR}/include/utilspp)

    install(
        FILES
            ${CURLPP_DIR}/src/curlpp/cURLpp.cpp
            ${CURLPP_DIR}/src/curlpp/Easy.cpp
            ${CURLPP_DIR}/src/curlpp/Exception.cpp
            ${CURLPP_DIR}/src/curlpp/Form.cpp
            ${CURLPP_DIR}/src/curlpp/Info.cpp
            ${CURLPP_DIR}/src/curlpp/Multi.cpp
            ${CURLPP_DIR}/src/curlpp/OptionBase.cpp
            ${CURLPP_DIR}/src/curlpp/Options.cpp
        COMPONENT sources
        DESTINATION ${INCLUDE_SOURCE_DIR}/${CURLPP_DIR}/src/curlpp)

    install(
        FILES
            ${CURLPP_DIR}/src/curlpp/internal/CurlHandle.cpp
            ${CURLPP_DIR}/src/curlpp/internal/OptionList.cpp
            ${CURLPP_DIR}/src/curlpp/internal/OptionSetter.cpp
            ${CURLPP_DIR}/src/curlpp/internal/SList.cpp
        COMPONENT sources
        DESTINATION ${INCLUDE_SOURCE_DIR}/${CURLPP_DIR}/src/curlpp/internal)

    # Install licenses
    install(FILES ${PROJECT_SOURCE_DIR}/LICENSE
        DESTINATION ./
        COMPONENT licenses
        )
    set(CPACK_COMPONENT_LICENSES_HIDDEN 1)

    install(DIRECTORY ${PROJECT_BINARY_DIR}/autotools/
        DESTINATION .
        USE_SOURCE_PERMISSIONS
        OPTIONAL
        COMPONENT sources
        )
endif()

# Install examples
install(DIRECTORY ${PROJECT_SOURCE_DIR}/examples
    DESTINATION ./
    COMPONENT examples
    PATTERN "examples/CMakeLists.txt" EXCLUDE
    )
set(CPACK_COMPONENT_EXAMPLES_DISPLAY_NAME "Examples")
set(CPACK_COMPONENT_EXAMPLES_DESCRIPTION "eProsima ${PROJECT_NAME_LARGE} examples")

if(MSVC OR MSVC_IDE)
    install(
        TARGETS isbridgengsiv2lib
        EXPORT ${PROJECT_NAME}Targets
        COMPONENT libraries_${MSVC_ARCH}
        DESTINATION ${LIB_INSTALL_DIR})
    install(
        TARGETS isdynbridgengsiv2lib
        EXPORT ${PROJECT_NAME}Targets
        COMPONENT libraries_${MSVC_ARCH}
        DESTINATION ${LIB_INSTALL_DIR})
else()
    install(
        TARGETS isbridgengsiv2lib
        EXPORT ${PROJECT_NAME}Targets
        COMPONENT libraries}
        DESTINATION ${LIB_INSTALL_DIR})
    install(
        TARGETS isdynbridgengsiv2lib
        EXPORT ${PROJECT_NAME}Targets
        COMPONENT libraries}
        DESTINATION ${LIB_INSTALL_DIR})
endif()

export(
    TARGETS isbridgengsiv2lib
    FILE ${BIN_INSTALL_DIR}cmake/config/${PROJECT_NAME}Targets.cmake)
export(
    TARGETS isdynbridgengsiv2lib
    FILE ${BIN_INSTALL_DIR}cmake/config/${PROJECT_NAME}Targets.cmake)

install(
    EXPORT ${PROJECT_NAME}Targets
    DESTINATION lib/${PROJECT_NAME}/cmake
    COMPONENT cmake)

install(
    TARGETS firos2
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    COMPONENT libraries_${MSVC_ARCH})

set(CPACK_COMPONENTS_ALL headers sources licenses examples)

if(WIN32)
    install(
        FILES ${CURL_BUILD_DIR}/bin/libcurl.dll
        DESTINATION bin
        COMPONENT libraries_${MSVC_ARCH}
        )
endif()

if(MSVC OR MSVC_IDE)
    set(CPACK_COMPONENTS_ALL ${CPACK_COMPONENTS_ALL} libraries_${MSVC_ARCH})
else()
    set(CPACK_COMPONENTS_ALL ${CPACK_COMPONENTS_ALL} libraries)
endif()

include(${PROJECT_SOURCE_DIR}/cmake/packaging/eProsimaPackaging.cmake)

# Install libraries
if(MSVC OR MSVC_IDE)
    if(EPROSIMA_INSTALLER)
        install_msvc_libraries(${MSVC_ARCH})
    else()
        set(DIR_EXTENSION "")
        if(EPROSIMA_INSTALLER_MINION)
            set(DIR_EXTENSION "/${MSVC_ARCH}")
        endif()

        install(EXPORT ${PROJECT_NAME}Targets
            DESTINATION ${LIB_INSTALL_DIR}${DIR_EXTENSION}/${PROJECT_NAME}/cmake
            COMPONENT cmake
            )

        string(TOUPPER "${MSVC_ARCH}" MSVC_ARCH_UPPER)
        set(CPACK_COMPONENT_LIBRARIES_${MSVC_ARCH_UPPER}_DISPLAY_NAME "${MSVC_ARCH}" PARENT_SCOPE)
        set(CPACK_COMPONENT_LIBRARIES_${MSVC_ARCH_UPPER}_DESCRIPTION "eProsima ${PROJECT_NAME_LARGE} libraries for platform ${MSVC_ARCH}" PARENT_SCOPE)
        set(CPACK_COMPONENT_LIBRARIES_${MSVC_ARCH_UPPER}_GROUP "Libraries" PARENT_SCOPE)
    endif()

    set(CPACK_COMPONENT_GROUP_LIBRARIES_DESCRIPTION "eProsima ${PROJECT_NAME_LARGE} libraries" PARENT_SCOPE)
endif()

install(
  DIRECTORY cmake
  DESTINATION share/${PROJECT_NAME}
)
