#include <iostream>
#include "String_PubSubTypes.h"
#include "../../src/NGSIv2/idl/JsonNGSIv2PubSubTypes.h"

#include "json/json.h"

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
    string_pst.deserialize(serialized_input, &string_data);

    // Custom transformation
    std::stringstream ss(string_data.data());
    Json::Value root;
    Json::CharReaderBuilder jsonReader;
    std::string errs;
    if (Json::parseFromStream(jsonReader, ss, &root, &errs))
    {
        Json::Value data_node = root["data"];
        Json::Value helloworld = data_node[0];
        data.data_("Helloworld: " + helloworld["count"]["value"].asString());
        // Serialization
        serialized_output->reserve(data_pst.m_typeSize);
        data_pst.serialize(&data, serialized_output);
    }
    else
    {
        std::cout  << "Failed to parse" << errs;
    }
}
