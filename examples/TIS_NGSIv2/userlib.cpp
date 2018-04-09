#include <iostream>
#include "RobotSndPubSubTypes.h"
#include "RobotRcvPubSubTypes.h"
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

using namespace eprosima::fastrtps::rtps;

extern "C" void USER_LIB_EXPORT transformFromNGSIv2(SerializedPayload_t *serialized_input, SerializedPayload_t *serialized_output){
	// User types
	RobotSnd robot_data;
	RobotSndPubSubType robot_pst;

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
        Json::Value data = root["data"];
        Json::Value robot = data[0];
        robot_data.robot_id(robot["id"].asString());
        std::cout << "Parsing robot with ID = " << robot_data.robot_id() << std::endl;
        robot_data.transmission_time(robot["transmission_time"]["value"].asString());
        robot_data.position().floor(robot["floor"]["value"].asLargestUInt());
        robot_data.position().x(robot["x"]["value"].asLargestUInt());
        robot_data.position().y(robot["y"]["value"].asLargestUInt());
        robot_data.position().zeta(robot["zeta"]["value"].asLargestUInt());
        // Serialization
        serialized_output->reserve(robot_pst.m_typeSize);
        robot_pst.serialize(&robot_data, serialized_output);
    }
    else
    {
        std::cout  << "Failed to parse" << errs;
    }
}

extern "C" void USER_LIB_EXPORT transformToNGSIv2(SerializedPayload_t *serialized_input, SerializedPayload_t *serialized_output){
	// User types
	RobotRcv robot_data;
	RobotRcvPubSubType robot_pst;

    JsonNGSIv2PubSubType string_pst;
    JsonNGSIv2 string_data;

	// Deserialization
	robot_pst.deserialize(serialized_input, &robot_data);

	// Custom transformation
    std::stringstream ss;
    //ss << robot_data.robot_id() << "@";
    ss << "{\"transmission_time\": { \"value\": \"" << robot_data.transmission_time() << "\"}, ";
    ss << "\"floor\": {\"value\": " << robot_data.destination().floor() << "}, ";
    ss << "\"x\": {\"value\": " << robot_data.destination().x() << "}, ";
    ss << "\"y\": {\"value\": " << robot_data.destination().y() << "}, ";
    ss << "\"zeta\": {\"value\": " << robot_data.destination().zeta() << "},";
    ss << "\"state\": {\"value\": " << ((robot_data.state() == State::ACTION) ? "\"ACTION\"" : "\"STAND_BY\"") << "} }";
	string_data.entityId(robot_data.robot_id());
	string_data.data(ss.str());
    std::cout << string_data.data() << std::endl;

	// Serialization
	serialized_output->reserve(string_pst.m_typeSize);
	string_pst.serialize(&string_data, serialized_output);
}
