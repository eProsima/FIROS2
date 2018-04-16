/*
 * Tranformation library for CameraInfo provided as example for TIS.
 * This have been not tested (produced xml may be malformed).
 * Intended to provide a proof of concept.
 * To use in real demo experiment might be adapted to NGSIv2 format
 * and the IDL files must be reviewed to store all necessary data.
 * If inverse tranformation is not needed, use an unidirectional bridge
 * instead with the desired NGSIv2 bridge library provided (or create a
 * new one).
 */

#include <iostream>
#include "CameraInfoPubSubTypes.h"
#include "../../src/NGSIv2/idl/JsonNGSIv2PubSubTypes.h"

#include "json/json.h"

#if defined(_WIN32) && defined (BUILD_SHARED_LIBS)
#if defined (_MSC_VER)
#pragma warning(disable: 4251)
#endif
#if defined(EPROSIMA_USER_DLL_EXPORT)
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
	CameraInfo camera_data;
	CameraInfoPubSubType camera_pst;

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
        Json::Value camera = data[0];
        camera_data.camera_id(camera["id"].asString());
        std::cout << "Parsing camera with ID = " << camera_data.camera_id() << std::endl;
        camera_data.transmission_time(camera["transmission_time"]["value"].asString());
        camera_data.number_of_people(camera["number_of_people"]["value"].asLargestUInt());
        // We suppose guest_locations as an array of floor, x and y
        // and features of color and other
        for (int i = 0; i < camera_data.number_of_people(); i++)
        {
            GuestLocation loc;
            loc.floor(camera["guest_locations"][i]["floor"]["value"].asLargestUInt());
            loc.x(camera["guest_locations"][i]["x"]["value"].asLargestUInt());
            loc.y(camera["guest_locations"][i]["y"]["value"].asLargestUInt());
            camera_data.guest_locations().push_back(loc);
            GuestFeature fea;
            fea.color(camera["guest_features"][i]["color"]["value"].asString());
            fea.other(camera["guest_features"][i]["other"]["value"].asString());
            camera_data.guest_features().push_back(fea);
        }
        // Serialization
        serialized_output->reserve(camera_pst.m_typeSize);
        camera_pst.serialize(&camera_data, serialized_output);
    }
    else
    {
        std::cout  << "Failed to parse" << errs;
    }
}

extern "C" void USER_LIB_EXPORT transformToNGSIv2(SerializedPayload_t *serialized_input, SerializedPayload_t *serialized_output){
	// User types
	CameraInfo camera_data;
	CameraInfoPubSubType camera_pst;

    JsonNGSIv2PubSubType string_pst;
    JsonNGSIv2 string_data;

	// Deserialization
	camera_pst.deserialize(serialized_input, &camera_data);

	// Custom transformation
    std::stringstream ss;
    ss << "{\"transmission_time\": { \"value\": \"" << camera_data.transmission_time() << "\"}, ";
    ss << "\"number_of_people\": {\"value\": " << camera_data.number_of_people() << "},";
    ss << "\"guest_locations\" : [";
    // We suppose guest_locations as an array of floor, x and y
    for (int i = 0; i < camera_data.number_of_people();)
    {
        ss << "{";
        ss << "\"floor\": {\"value\": " << camera_data.guest_locations()[i].floor() << "}, ";
        ss << "\"x\": {\"value\": " << camera_data.guest_locations()[i].x() << "}, ";
        ss << "\"y\": {\"value\": " << camera_data.guest_locations()[i++].y() << "} "; // Increment here i!
        ss << "}";
        if (i < camera_data.number_of_people()) // To check here against "next" iteration
        {
            ss << ",";
        }
    }
    ss << "],";
    ss << "\"guest_features\" : [";
    // and features of color and other
    for (int i = 0; i < camera_data.number_of_people();)
    {
        ss << "{";
        ss << "\"color\": {\"value\": " << camera_data.guest_features()[i].color() << "},";
        ss << "\"other\": {\"value\": " << camera_data.guest_features()[i++].other() << "}"; // Increment here i!
        ss << "}";
        if (i < camera_data.number_of_people()) // To check here against "next" iteration
        {
            ss << ",";
        }
    }
    ss << "]}";
	string_data.entityId(camera_data.camera_id());
	string_data.data(ss.str());
    std::cout << string_data.data() << std::endl;

	// Serialization
	serialized_output->reserve(string_pst.m_typeSize);
	string_pst.serialize(&string_data, serialized_output);
}
