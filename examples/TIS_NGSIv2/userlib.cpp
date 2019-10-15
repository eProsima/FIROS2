#include <iostream>
//#include "RobotSndPubSubTypes.h"
#include "RobotRcvPubSubTypes.h"
#include "../../src/NGSIv2/idl/JsonNGSIv2PubSubTypes.h"
#include "../../thirdparty/integration-service/src/log/ISLog.h"
#include <fastrtps/types/DynamicTypePtr.h>
#include <fastrtps/types/DynamicTypeBuilderPtr.h>
#include <fastrtps/types/DynamicDataPtr.h>
#include <fastrtps/types/DynamicDataFactory.h>
#include <fastrtps/types/DynamicTypeBuilderFactory.h>
#include <fastrtps/types/DynamicPubSubType.h>

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
using namespace eprosima::fastrtps::types;

extern "C" USER_LIB_EXPORT void transformFromNGSIv2(SerializedPayload_t *serialized_input,
        SerializedPayload_t *serialized_output)
{
    // User types
    //RobotSnd robot_data;
    //RobotSndPubSubType robot_pst;
    // Dynamic user type
    DynamicTypeBuilderFactory *factory = DynamicTypeBuilderFactory::GetInstance();
    DynamicTypeBuilder_ptr position_builder = factory->CreateStructBuilder();
    int idx = 0;
    DynamicTypeBuilder_ptr uint32_builder = factory->CreateUint32Builder();
    position_builder->AddMember(idx++, "floor", uint32_builder.get());
    position_builder->AddMember(idx++, "x", uint32_builder.get());
    position_builder->AddMember(idx++, "y", uint32_builder.get());
    position_builder->AddMember(idx++, "zeta", uint32_builder.get());
    position_builder->SetName("RobotPosition");
    DynamicTypeBuilder_ptr robotsnd_builder = factory->CreateStructBuilder();
    idx = 0;
    DynamicTypeBuilder_ptr string_builder = factory->CreateStringBuilder();
    robotsnd_builder->AddMember(idx++, "id", string_builder.get());
    robotsnd_builder->AddMember(idx++, "transmission_time", string_builder.get());
    robotsnd_builder->AddMember(idx++, "position", position_builder.get());
    robotsnd_builder->SetName("RobotSnd");

    DynamicType_ptr dyn_type = robotsnd_builder->Build();
    DynamicData_ptr robot_data = DynamicDataFactory::GetInstance()->CreateData(dyn_type);
    DynamicPubSubType robot_pst(dyn_type);

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
        //robot_data.robot_id(robot["id"].asString());
        robot_data->SetStringValue(robot["id"].asString(), robot_data->GetMemberIdByName("id"));
        LOG_INFO("Parsing robot with ID = " << robot_data->GetStringValue(robot_data->GetMemberIdByName("id")));
        /*
        robot_data.transmission_time(robot["transmission_time"]["value"].asString());
        robot_data.position().floor(robot["floor"]["value"].asLargestUInt());
        robot_data.position().x(robot["x"]["value"].asLargestUInt());
        robot_data.position().y(robot["y"]["value"].asLargestUInt());
        robot_data.position().zeta(robot["zeta"]["value"].asLargestUInt());
        */
        robot_data->SetStringValue(robot["transmission_time"]["value"].asString(),
                                    robot_data->GetMemberIdByName("transmission_time"));
        DynamicData *position = robot_data->LoanValue(robot_data->GetMemberIdByName("position"));
        position->SetUint32Value(robot["floor"]["value"].asLargestUInt(), position->GetMemberIdByName("floor"));
        position->SetUint32Value(robot["x"]["value"].asLargestUInt(), position->GetMemberIdByName("x"));
        position->SetUint32Value(robot["y"]["value"].asLargestUInt(), position->GetMemberIdByName("y"));
        position->SetUint32Value(robot["zeta"]["value"].asLargestUInt(), position->GetMemberIdByName("zeta"));
        robot_data->ReturnLoanedValue(position);
        // Serialization
        //serialized_output->reserve(robot_pst.m_typeSize);
        serialized_output->reserve(robot_pst.getSerializedSizeProvider(robot_data.get())());
        robot_pst.serialize(robot_data.get(), serialized_output);
    }
    else
    {
        LOG_ERROR("Failed to parse" << errs);
    }
}

extern "C" USER_LIB_EXPORT void transform(SerializedPayload_t *serialized_input, SerializedPayload_t *serialized_output){
    // User types
    RobotRcv robot_data;
    RobotRcvPubSubType robot_pst;

    JsonNGSIv2PubSubType string_pst;
    JsonNGSIv2 string_data;

    // Deserialization
    robot_pst.deserialize(serialized_input, &robot_data);

    // Custom transformation
    std::stringstream ss;
    ss << "{\"transmission_time\": { \"value\": \"" << robot_data.transmission_time() << "\"}, ";
    ss << "\"floor\": {\"value\": " << robot_data.destination().floor() << "}, ";
    ss << "\"x\": {\"value\": " << robot_data.destination().x() << "}, ";
    ss << "\"y\": {\"value\": " << robot_data.destination().y() << "}, ";
    ss << "\"zeta\": {\"value\": " << robot_data.destination().zeta() << "},";
    ss << "\"state\": {\"value\": " << ((robot_data.state() == State::ACTION) ? "\"ACTION\"" : "\"STAND_BY\"") << "} }";
    string_data.entityId(robot_data.robot_id());
    string_data.data(ss.str());
    LOG_INFO(string_data.data());

    // Serialization
    serialized_output->reserve(string_pst.m_typeSize);
    string_pst.serialize(&string_data, serialized_output);
}
