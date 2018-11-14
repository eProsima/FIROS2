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

#include <fastrtps/Domain.h>
#include <fastrtps/types/DynamicData.h>
#include "json/json.h"
#include "DynNGSIv2Subscriber.h"
#include "NGSIv2Utils.h"
#include "DynJsonPubSubType.hpp" // Received type from NGSIv2
#include "log/ISLog.h"
#include <thread>

#ifndef LISTENER_BUFFER_SIZE
#define LISTENER_BUFFER_SIZE 2048
#endif

using asio::ip::tcp;

DynNGSIv2Subscriber::~DynNGSIv2Subscriber()
{
    exit = true;
}

std::string DynNGSIv2Subscriber::getListenerURL()
{
    std::stringstream strstr;
    strstr << "http://" << sub_params.host << ":" << sub_params.port;

    return strstr.str();
}

DynNGSIv2Subscriber::DynNGSIv2Subscriber(const std::string &name, const NGSIv2Params &params)
: ISSubscriber(name)
, part_params(params)
{
    std::stringstream strstr;
    strstr << params.host << ":" << params.port;
    url = strstr.str();

    exit = false;

    input_type = getDynJsonPubSubType();
}

void DynNGSIv2Subscriber::startListenerAndSubscribe()
{
    std::thread thread(&DynNGSIv2Subscriber::listener, this);

    subscription_id = addSubscription(url, sub_params.idPattern, sub_params.type, sub_params.attrs, sub_params.expression,
        getListenerURL(), sub_params.notif, sub_params.expiration, sub_params.throttling, sub_params.description);

    thread.detach();
}

std::string DynNGSIv2Subscriber::addSubscription(const std::string &server, const std::string &idPattern,
                     const std::string &type, const std::string &attrs, const std::string &expression,
                     const std::string &listener, const std::string &notifAttrs, const std::string &expiration,
                     const int &throttling, const std::string &description)
{
    try
    {
        curlpp::Cleanup myCleanup;// Now add a subscription
        curlpp::Easy subRequest;
        std::list<std::string> sheader;
        sheader.push_back("Content-Type: application/json");
        subRequest.setOpt(new curlpp::options::HttpHeader(sheader));
        //subRequest.setOpt(new curlpp::options::Verbose(true));
        std::string delimiter = ",";

        Json::Value output;
        if (!description.empty())
        {
            output["description"] = description;
        }

        Json::Value entity;
        entity["idPattern"] = idPattern;
        entity["type"] = type;
        output["subject"]["entities"] = Json::arrayValue;
        output["subject"]["entities"].append(entity);

        if (!attrs.empty() || !expression.empty())
        {
            if (!attrs.empty())
            {
                output["subject"]["condition"]["attrs"] = Json::arrayValue;
                size_t start = 0;
                size_t end = attrs.find(delimiter);
                while (end != std::string::npos)
                {
                    output["subject"]["condition"]["attrs"].append(attrs.substr(start, end - start));
                    start = end + delimiter.length();
                    end = attrs.find(delimiter, start);
                }

                output["subject"]["condition"]["attrs"].append(attrs.substr(start, end));
            }
            if (!expression.empty())
            {
                output["subject"]["condition"]["expression"]["q"] = expression;
            }
        }

        output["notification"]["http"]["url"] = listener;
        output["notification"]["attrs"] = Json::arrayValue;
        if (!notifAttrs.empty())
        {
            size_t start = 0;
            size_t end = notifAttrs.find(delimiter);
            while (end != std::string::npos)
            {
                output["notification"]["attrs"].append(notifAttrs.substr(start, end - start));
                start = end + delimiter.length();
                end = notifAttrs.find(delimiter, start);
            }
            output["notification"]["attrs"].append(notifAttrs.substr(start, end));
        }

        if (!expiration.empty())
        {
            output["expires"] = expiration;
        }

        if (throttling > 0)
        {
            output["throttling"] = throttling;
        }

        Json::StreamWriterBuilder builder;
        builder["indentation"] = "";
        std::string json = Json::writeString(builder, output);

        subRequest.setOpt(new curlpp::options::PostFields(json));
        subRequest.setOpt(new curlpp::options::PostFieldSize((long)json.length()));

        std::stringstream ss;
        ss << server << "/v2/subscriptions/";
        subRequest.setOpt<Url>(ss.str());

        std::ostringstream response;

        subRequest.setOpt(new curlpp::options::WriteStream(&response));
        subRequest.setOpt(new curlpp::options::Header(1));
        subRequest.setOpt(new curlpp::options::Timeout(part_params.httpTimeout));

        // Send request and get a result.
        subRequest.perform();

        std::string subsc_id;
        std::istringstream responseHeader(response.str());

        for (std::string line; std::getline(responseHeader, line); )
        {
            if (line.find("Location: /v2/subscriptions/") != std::string::npos)
            {
                subsc_id = line.substr(line.find_last_of("/") + 1);
                subsc_id.pop_back(); // Remove \r tail
                LOG_INFO("Added subscription with ID: " << subsc_id);
                //std::cout << subsc_id << std::endl;
                return subsc_id;
            }
        }
    }
    catch(curlpp::RuntimeError & e)
    {
        LOG_ERROR(e.what());
    }
    catch(curlpp::LogicError & e)
    {
        LOG_ERROR(e.what());
    }
    return "";
}

void DynNGSIv2Subscriber::onTerminate()
{
    deleteSubscription();
}

void DynNGSIv2Subscriber::deleteSubscription()
{
    try
    {
        curlpp::Cleanup myCleanup;

        curlpp::Easy delRequest;
        std::list<std::string> delHeader;

        std::stringstream ss;
        ss << url << "/v2/subscriptions/" << subscription_id;
        delRequest.setOpt<Url>(ss.str());

        delRequest.setOpt(new curlpp::options::CustomRequest("DELETE"));

        // Send request and get a result.
        performAndRetry(part_params.retries, delRequest);
    }
    catch(curlpp::RuntimeError & e)
    {
        LOG_ERROR(e.what());
    }
    catch(curlpp::LogicError & e)
    {
        LOG_ERROR(e.what());
    }
}

void DynNGSIv2Subscriber::onDataReceived(void* data)
{
    if (!data)
    {
        return;
    }

    std::string* str = (std::string*)data;

    DynamicData *json = static_cast<DynamicData*>(input_type->createData());
    // No need to parse here (or empty means complete json in data ;) )
    json->SetStringValue("", json->GetMemberIdByName("entityId"));
    json->SetStringValue(*str, json->GetMemberIdByName("data"));

    on_received_data(json);
    input_type->deleteData(json);
}


void DynNGSIv2Subscriber::listener()
{
    try
    {
        const int buf_size = (sub_params.buffer_size > 0) ? sub_params.buffer_size : LISTENER_BUFFER_SIZE;
        std::array<char, LISTENER_BUFFER_SIZE> buf;
        asio::io_service io_service;
        this->io_service = &io_service;
        asio::error_code error;
        tcp::endpoint myendpoint(tcp::v4(), sub_params.port);
        tcp::acceptor acceptor(io_service, myendpoint);

        while (!exit)
        {
            std::stringstream ss;
            std::string data;
            size_t len = 0;
            size_t totalLen = 0;

            tcp::socket socket(io_service);
            acceptor.accept(socket);

            do
            {
                len = socket.read_some(asio::buffer(buf), error);
                totalLen += len;

                if (error == asio::error::eof)
                {
                    LOG_ERROR("Connection closed by server");
                    break; // Connection closed cleanly by peer.
                }
                else if (error)
                {
                    throw asio::system_error(error); // Some other error.
                }

                ss.write(buf.data(), LISTENER_BUFFER_SIZE);

                buf.fill(0);
            } while (len == LISTENER_BUFFER_SIZE && totalLen < buf_size);

            data = ss.str();
            LOG_DEBUG(data);
            data = data.substr(data.find_first_of("{\""));
            LOG_DEBUG("Recv " << len << " bytes");

            if (len < buf_size)
            {
                onDataReceived(&data);
            }
            else
            {
                LOG_WARN("Received message too big (>= " << buf_size << " B). It will be ignored.");
            }
        }
    }
    catch (std::exception& e)
    {
        LOG_ERROR(e.what());
    }
}

void DynNGSIv2Subscriber::setSubscriptionParams(const NGSIv2SubscriptionParams &params)
{
    sub_params = params;
}