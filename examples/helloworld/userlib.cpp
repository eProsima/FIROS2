#include <iostream>
#include "String_PubSubTypes.h"
#include "HelloWorldPubSubTypes.h"

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

extern "C" void USER_LIB_EXPORT transform(SerializedPayload_t *serialized_input, SerializedPayload_t *serialized_output){
	// User types
	std_msgs::msg::dds_::String_ string_data;
	std_msgs::msg::dds_::String_PubSubType string_pst;
	HelloWorld helloworld_data;
	HelloWorldPubSubType helloworld_pst;

	// Deserialization
	helloworld_pst.deserialize(serialized_input, &helloworld_data);

	// Custom transformation
	string_data.data_(helloworld_data.message() + ": " + std::to_string(helloworld_data.index()));
	std::cout << string_data.data_() << std::endl;

	// Serialization
	serialized_output->reserve(string_pst.m_typeSize);
	string_pst.serialize(&string_data, serialized_output);
}
