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


#ifndef _NGSIv2_PUBLISHER_H_
#define _NGSIv2_PUBLISHER_H_

#include "ISBridge.h"
#include "NGSIv2Params.h"

class NGSIv2Publisher : public ISWriter
{
private:
    std::string url;
    NGSIv2Params part_params;
public:
    NGSIv2Publisher(const std::string &name);
    NGSIv2Publisher(const std::string &name, const NGSIv2Params &params);
    ~NGSIv2Publisher() override;

    bool write(SerializedPayload_t* payload) override;
    bool write(eprosima::fastrtps::types::DynamicData* payload) override { return false; }
};

#endif // _NGSIv2_PUBLISHER_H_
