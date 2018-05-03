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

#include "NGSIv2Publisher.h"
#include "NGSIv2Utils.h"
#include "idl/JsonNGSIv2PubSubTypes.h" // Received type from NGSIv2
#include "../../thirdparty/integration-services/src/log/ISLog.h"

#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Infos.hpp>
#include <curlpp/Easy.hpp>

#include <iostream>

using namespace curlpp::options;
using namespace curlpp::infos;

NGSIv2Publisher::~NGSIv2Publisher()
{
}

NGSIv2Publisher::NGSIv2Publisher(const std::string &name)
    : ISPublisher(name)
{
}

NGSIv2Publisher::NGSIv2Publisher(const std::string &name, const NGSIv2Params &params)
: ISPublisher(name)
, part_params(params)
{
    std::stringstream strstr;
    strstr << params.host << ":" << params.port;
    url = strstr.str();
}

bool NGSIv2Publisher::publish(SerializedPayload_t* payload)
{
    write(payload);
    return true;
}

std::string NGSIv2Publisher::write(SerializedPayload_t* payload)
{
    try
    {
        curlpp::Cleanup cleaner;
        curlpp::Easy request;

        JsonNGSIv2PubSubType json_pst;
        JsonNGSIv2 json;
        json_pst.deserialize(payload, &json);

        std::string entityId = json.entityId();
        std::string payload = json.data();
        request.setOpt(new curlpp::options::Url(url + "/v2/entities/" + entityId + "/attrs"));
        //request.setOpt(new curlpp::options::Verbose(true));
        std::list<std::string> header;
        header.push_back("Content-Type: application/json");

        request.setOpt(new curlpp::options::HttpHeader(header));
        request.setOpt(new curlpp::options::PostFields(payload));
        request.setOpt(new curlpp::options::PostFieldSize((long)payload.length()));
        request.setOpt(new curlpp::options::Timeout(part_params.httpTimeout));

        performAndRetry(part_params.retries, request);

        return "";
    }
    catch (curlpp::LogicError & e)
    {
        LOG_ERROR(e.what());
    }
    catch (curlpp::RuntimeError & e)
    {
        LOG_ERROR(e.what());
    }
    return "";
}
