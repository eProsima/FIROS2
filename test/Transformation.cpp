#include <iostream>
#include "String_PubSubTypes.h"
#include "TestPubSubTypes.h"

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
	std_msgs::msg::dds_::String_ string_data; //input
	std_msgs::msg::dds_::String_PubSubType string_pst;
	Test test_data; //output
	TestPubSubType test_pst;

	// Deserialization
	string_pst.deserialize(serialized_input, &string_data);

	// Custom transformation
	test_data.message(string_data.data_() + "_modified");

	// Serialization
	serialized_output->reserve(test_pst.m_typeSize);
    test_pst.serialize(&test_data, serialized_output);
}
