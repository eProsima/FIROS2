#include <iostream>
//#include "MyTypes.h"

#if defined(_WIN32) && defined (BUILD_SHARED_LIBS)
#if defined (_MSC_VER)
#pragma warning(disable: 4251)
#endif
#if defined(EPROSIMA_SHAPES_DLL_EXPORT)
#define  SHAPES_LIB_EXPORT __declspec(dllexport)
#else
#define  SHAPES_LIB_EXPORT __declspec(dllimport)
#endif
#else
#define SHAPES_LIB_EXPORT
#endif

extern "C" TopicDataType* SHAPES_LIB_EXPORT GetTopicType(const char *name)
{
    // if (strncmp(name, "MyType", 6) == 0)
    // {
    //     return new MyType();
    // }
    return nullptr;
}