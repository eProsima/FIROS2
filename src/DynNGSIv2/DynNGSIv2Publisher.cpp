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

#include "DynNGSIv2Publisher.h"
#include "NGSIv2Utils.h"
#include "DynJsonPubSubType.hpp" // Received type from NGSIv2
#include "log/ISLog.h"

#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Infos.hpp>
#include <curlpp/Easy.hpp>

#include <iostream>

using namespace curlpp::options;
using namespace curlpp::infos;

DynNGSIv2Publisher::~DynNGSIv2Publisher()
{
}

DynNGSIv2Publisher::DynNGSIv2Publisher(const std::string &name)
    : ISWriter(name)
{
    output_type = getDynJsonPubSubType();
}

DynNGSIv2Publisher::DynNGSIv2Publisher(const std::string &name, const NGSIv2Params &params)
: ISWriter(name)
, part_params(params)
{
    std::stringstream strstr;
    strstr << params.host << ":" << params.port;
    url = strstr.str();
    output_type = getDynJsonPubSubType();
}

bool DynNGSIv2Publisher::write(DynamicData* data)
{
    long code = 600;
    try
    {
        curlpp::Cleanup cleaner;
        curlpp::Easy request;

        //std::string entityId = json.entityId();
        std::string entityId = data->GetStringValue(data->GetMemberIdByName("entityId"));
        std::string payload = data->GetStringValue(data->GetMemberIdByName("data"));
        request.setOpt(new curlpp::options::Url(url + "/v2/entities/" + entityId + "/attrs"));
        std::list<std::string> header;
        header.push_back("Content-Type: application/json");

        request.setOpt(new curlpp::options::HttpHeader(header));
        request.setOpt(new curlpp::options::PostFields(payload));
        request.setOpt(new curlpp::options::PostFieldSize((long)payload.length()));
        request.setOpt(new curlpp::options::Timeout(part_params.httpTimeout));

        performAndRetry(part_params.retries, request);

        code = curlpp::infos::ResponseCode::get(request);
    }
    catch (curlpp::LogicError & e)
    {
        LOG_ERROR(e.what());
        code = 601;
    }
    catch (curlpp::RuntimeError & e)
    {
        LOG_ERROR(e.what());
        code = 602;
    }
    return (code / 100) == 2;
}
