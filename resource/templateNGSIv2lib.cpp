#include <iostream>
#include "idl/JsonNGSIv2PubSubTypes.h"

#if defined(_WIN32) && defined (BUILD_SHARED_LIBS)
#if defined (_MSC_VER)
#pragma warning(disable: 4251)
#endif
#if defined(userlib_EXPORTS)
#define  USER_LIB_EXPORT __declspec(dllexport)
#else
#define  USER_LIB_EXPORT __declspec(dllimport)
#endif
#else
#define USER_LIB_EXPORT
#endif

using namespace eprosima::fastrtps::rtps;

extern "C" void USER_LIB_EXPORT transformFromNGSIv2(SerializedPayload_t *serialized_input, SerializedPayload_t *serialized_output)
{
	// Custom tranformation of data received from NGSIv2
}

extern "C" void USER_LIB_EXPORT transformToNGSIv2(SerializedPayload_t *serialized_input, SerializedPayload_t *serialized_output)
{
	// Custom tranformation of data received from RTPS
}
