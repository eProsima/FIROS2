// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#ifndef _DYN_NGSIv2_PUBLISHER_H_
#define _DYN_NGSIv2_PUBLISHER_H_

#include "ISBridge.h"
#include "NGSIv2Params.h"

class DynNGSIv2Publisher : public ISPublisher
{
private:
    std::string url;
    long write(eprosima::fastrtps::types::DynamicData* data);
    NGSIv2Params part_params;
public:
    DynNGSIv2Publisher(const std::string &name);
    DynNGSIv2Publisher(const std::string &name, const NGSIv2Params &params);
    ~DynNGSIv2Publisher() override;

    bool publish(SerializedPayload_t* payload) override { return false; }
    bool publish(eprosima::fastrtps::types::DynamicData* payload) override;
};

#endif // _DYN_NGSIv2_PUBLISHER_H_
