#include <iostream>
#include "ISBridgeDummy.h"

#if defined(_WIN32) && defined (BUILD_SHARED_LIBS)
	#if defined (_MSC_VER)
		#pragma warning(disable: 4251)
	#endif
  #if defined(testlib_EXPORTS)
  	#define  USER_LIB_EXPORT __declspec(dllexport)
  #else
    #define  USER_LIB_EXPORT __declspec(dllimport)
  #endif
#else
  #define USER_LIB_EXPORT
#endif

extern "C" USER_LIB_EXPORT ISBridge* create_bridge(const char* name,
    const std::vector<std::pair<std::string, std::string>> *config)
{
    return loadDummyBridge(name);
}

extern "C" USER_LIB_EXPORT ISSubscriber* create_subscriber(ISBridge *bridge, const char* name,
    const std::vector<std::pair<std::string, std::string>> *config)
{
    return loadDummySubscriber(name, config);
}

extern "C" USER_LIB_EXPORT ISPublisher* create_publisher(ISBridge *bridge, const char* name,
    const std::vector<std::pair<std::string, std::string>> *config)
{
    return loadDummyPublisher(name, config);
}
