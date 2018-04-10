#include <iostream>
#include "String_PubSubTypes.h"
#include "../../src/NGSIv2/idl/JsonNGSIv2PubSubTypes.h"

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

using eprosima::fastrtps::rtps::SerializedPayload_t;

extern "C" void USER_LIB_EXPORT transform(SerializedPayload_t *serialized_input, SerializedPayload_t *serialized_output){
    // User types
    std_msgs::msg::dds_::String_ data;
    std_msgs::msg::dds_::String_PubSubType data_pst;

    JsonNGSIv2PubSubType string_pst;
    JsonNGSIv2 string_data;

    // Deserialization
    data_pst.deserialize(serialized_input, &data);

    // "Hello World: X"
    // -->
    // std::string json = "{ \"id\": \"Helloworld\", \"type\": \"Helloworld\", \
            \"count\": { \"value\": x, \"type\": \"Number\" } }";

    std::string msg = data.data_();
    msg = msg.substr(msg.find(":") + 2); // ": "

    // Custom transformation
    std:: string json = "{\"count\": { \"value\": " + msg + "} }";
    string_data.entityId("Helloworld"); // Fixed for the example
    string_data.data(json);
    std::cout << string_data.data() << std::endl;

    // Serialization
    serialized_output->reserve(string_pst.m_typeSize);
    string_pst.serialize(&string_data, serialized_output);
}
