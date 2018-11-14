#include <iostream>
#include "../../src/DynNGSIv2/DynJsonPubSubType.hpp"
#include "../../thirdparty/integration-service/src/log/ISLog.h"
#include <fastrtps/types/DynamicData.h>
#include <fastrtps/types/DynamicType.h>
#include <fastrtps/types/DynamicTypePtr.h>
#include <fastrtps/types/DynamicDataFactory.h>
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

using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;
using namespace eprosima::fastrtps::types;

TopicDataType* GetRobotSnd()
{
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
    return new DynamicPubSubType(dyn_type);
}

TopicDataType* GetRobotRcv()
{
    DynamicTypeBuilderFactory *factory = DynamicTypeBuilderFactory::GetInstance();

    // Enum state
    DynamicTypeBuilder_ptr myEnum_builder = factory->CreateEnumBuilder();
    myEnum_builder->SetName("State");
    myEnum_builder->AddEmptyMember(0, "ACTION");
    myEnum_builder->AddEmptyMember(1, "STAND_BY");
    DynamicType_ptr m_MyEnumType = myEnum_builder->Build();

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
    robotsnd_builder->AddMember(idx++, "destination", position_builder.get());
    robotsnd_builder->AddMember(idx++, "state", m_MyEnumType);
    robotsnd_builder->SetName("RobotRcv");

    DynamicType_ptr dyn_type = robotsnd_builder->Build();
    DynamicData_ptr robot_data = DynamicDataFactory::GetInstance()->CreateData(dyn_type);
    return new DynamicPubSubType(dyn_type);
}

extern "C" USER_LIB_EXPORT TopicDataType* GetTopicType(const char *name)
{
    if (strncmp(name, "RobotSnd", 8))
    {
        return GetRobotSnd();
    }
    else if (strncmp(name, "RobotRcv", 8))
    {
        return GetRobotRcv();
    }
    else return nullptr;
}

extern "C" USER_LIB_EXPORT void transformFromNGSIv2(DynamicData *input, DynamicData *output)
{
    // Custom transformation
    std::stringstream ss(input->GetStringValue(input->GetMemberIdByName("data")));
    Json::Value root;
    Json::CharReaderBuilder jsonReader;
    std::string errs;
    if (Json::parseFromStream(jsonReader, ss, &root, &errs))
    {
        Json::Value data = root["data"];
        Json::Value robot = data[0];
        output->SetStringValue(robot["id"].asString(), output->GetMemberIdByName("id"));
        LOG_INFO("Parsing robot with ID = " << output->GetStringValue(output->GetMemberIdByName("id")));
        output->SetStringValue(robot["transmission_time"]["value"].asString(),
                                    output->GetMemberIdByName("transmission_time"));
        DynamicData *position = output->LoanValue(output->GetMemberIdByName("destination"));
        position->SetUint32Value(robot["floor"]["value"].asLargestUInt(), position->GetMemberIdByName("floor"));
        position->SetUint32Value(robot["x"]["value"].asLargestUInt(), position->GetMemberIdByName("x"));
        position->SetUint32Value(robot["y"]["value"].asLargestUInt(), position->GetMemberIdByName("y"));
        position->SetUint32Value(robot["zeta"]["value"].asLargestUInt(), position->GetMemberIdByName("zeta"));
        output->ReturnLoanedValue(position);
    }
    else
    {
        LOG_ERROR("Failed to parse" << errs);
    }
}

extern "C" USER_LIB_EXPORT void transform(DynamicData *input, DynamicData *output)
{
    // Custom transformation
    std::stringstream ss;
    ss << "{\"transmission_time\": { \"value\": \""
       << input->GetStringValue(input->GetMemberIdByName("transmission_time")) << "\"}, ";

    DynamicData* destination = input->LoanValue(input->GetMemberIdByName("destination"));
    ss << "\"floor\": {\"value\": "
        << destination->GetUint32Value(destination->GetMemberIdByName("floor")) << "}, ";
    ss << "\"x\": {\"value\": " << destination->GetUint32Value(destination->GetMemberIdByName("x")) << "}, ";
    ss << "\"y\": {\"value\": " << destination->GetUint32Value(destination->GetMemberIdByName("y")) << "}, ";
    ss << "\"zeta\": {\"value\": "
        << destination->GetUint32Value(destination->GetMemberIdByName("zeta")) << "},";
    input->ReturnLoanedValue(destination);

    ss << "\"state\": {\"value\": \"" << input->GetEnumValue(input->GetMemberIdByName("state")).c_str() << "\"} }";

    output->SetStringValue(input->GetStringValue(input->GetMemberIdByName("id")),
                           output->GetMemberIdByName("entityId"));
    output->SetStringValue(ss.str(), output->GetMemberIdByName("data"));
    LOG_INFO(output->GetStringValue(output->GetMemberIdByName("data")));
}
