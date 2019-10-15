#ifndef _DYN_JSON_PBTYPE_
#define _DYN_JSON_PBTYPE_

#include <fastrtps/types/DynamicType.h>
#include <fastrtps/types/DynamicTypePtr.h>
#include <fastrtps/types/DynamicTypeBuilder.h>
#include <fastrtps/types/DynamicTypeBuilderPtr.h>
#include <fastrtps/types/DynamicTypeBuilderFactory.h>
#include <fastrtps/types/DynamicPubSubType.h>

/*
struct JsonNGSIv2
{
    string entityId;
    string data;
};
*/

using namespace eprosima::fastrtps::types;

static DynamicPubSubType* getDynJsonPubSubType()
{
    DynamicTypeBuilderFactory *factory = DynamicTypeBuilderFactory::GetInstance();
    DynamicTypeBuilder_ptr struct_builder = factory->CreateStructBuilder();
    int idx = 0;
    DynamicTypeBuilder_ptr string_builder = factory->CreateStringBuilder();
    DynamicTypeBuilder_ptr long_string_builder = factory->CreateStringBuilder(2048);
    struct_builder->AddMember(idx++, "entityId", string_builder.get());
    struct_builder->AddMember(idx++, "data", long_string_builder.get());
    struct_builder->SetName("JsonNGSIv2");

    DynamicType_ptr dyn_type = struct_builder->Build();
    return new DynamicPubSubType(dyn_type);
}

#endif // _DYN_JSON_PBTYPE_