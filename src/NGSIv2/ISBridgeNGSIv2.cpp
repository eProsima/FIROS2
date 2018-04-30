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

#include <fastrtps/participant/Participant.h>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/subscriber/Subscriber.h>
#include <fastrtps/attributes/SubscriberAttributes.h>
#include "fastrtps/publisher/Publisher.h"
#include "fastrtps/attributes/PublisherAttributes.h"
#include <fastcdr/Cdr.h>

#include <fastrtps/Domain.h>

#include "ISBridgeNGSIv2.h"
#include "idl/JsonNGSIv2PubSubTypes.h" // Received type from NGSIv2

#include <algorithm>
#include <thread>

#ifndef LISTENER_BUFFER_SIZE
#define LISTENER_BUFFER_SIZE 2048
#endif

using asio::ip::tcp;

bool performAndRetry(int n_retries, curlpp::Easy &request);

ISBridgeNGSIv2::~ISBridgeNGSIv2()
{
}

NGSIv2Subscriber::~NGSIv2Subscriber()
{
    exit = true;
}

NGSIv2Publisher::~NGSIv2Publisher()
{
}

std::string NGSIv2Subscriber::getListenerURL()
{
    std::stringstream strstr;
    strstr << "http://" << sub_params.host << ":" << sub_params.port;

    return strstr.str();
}

NGSIv2Subscriber::NGSIv2Subscriber(const std::string &name, const NGSIv2Params &params)
: ISSubscriber(name)
, part_params(params)
{
    std::stringstream strstr;
    strstr << params.host << ":" << params.port;
    url = strstr.str();

    exit = false;
}

NGSIv2Publisher::NGSIv2Publisher(const std::string &name, const NGSIv2Params &params)
: ISPublisher(name)
, part_params(params)
{
    std::stringstream strstr;
    strstr << params.host << ":" << params.port;
    url = strstr.str();
}

void NGSIv2Subscriber::startListenerAndSubscribe()
{
    std::thread thread(&NGSIv2Subscriber::listener, this);

    subscription_id = addSubscription(url, sub_params.idPattern, sub_params.type, sub_params.attrs, sub_params.expression,
        getListenerURL(), sub_params.notif, sub_params.expiration, sub_params.throttling, sub_params.description);

    thread.detach();
}

void find_and_replace(std::string& source, const std::string &find, const std::string &replace)
{
    for(std::string::size_type i = 0; (i = source.find(find, i)) != std::string::npos;)
    {
        source.replace(i, find.length(), replace);
        i += replace.length();
    }
}

std::string NGSIv2Subscriber::addSubscription(const std::string &server, const std::string &idPattern,
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

        std::string myAttrs = attrs;
        std::string myNotifAttrs = notifAttrs;
        std::string comma(",");
        std::string quotecommaquote("\",\"");
        //std::replace(myAttrs.begin(), myAttrs.end(), comma, quotecommaquote);
        //std::replace(myNotifAttrs.begin(), myNotifAttrs.end(), comma, quotecommaquote);
        find_and_replace(myAttrs, comma, quotecommaquote);
        find_and_replace(myNotifAttrs, comma, quotecommaquote);
        myAttrs.push_back('"');
        myNotifAttrs.push_back('"');
        myAttrs.insert(0, "\"");
        myNotifAttrs.insert(0, "\"");

        std::stringstream ssjson;

        ssjson << "{";
        if (!description.empty()) { ssjson << "\"description\": \"" << description << "\", "; }
        ssjson << "\"subject\": { ";
        ssjson << "\"entities\": [ ";
        ssjson << "{";
        ssjson << "\"idPattern\": \"" << idPattern << "\",";
        ssjson << "\"type\": \"" << type << "\"";
        ssjson << "}";
        ssjson << "]";
        if (!attrs.empty() || !expression.empty())
        {
            ssjson << "\", condition\": {";
            if (!attrs.empty())
            {
                ssjson << "\"attrs\": [ " << myAttrs << " ]";
            }
            if (!expression.empty())
            {
                if (!attrs.empty())
                {
                    ssjson << ",";
                }
                ssjson << "\"expression\": {";
                ssjson << "\"q\": \"" << expression << "\"";
                ssjson << "}";
            }
            ssjson << "}";
        }
        ssjson << "},";
        ssjson << "\"notification\": {";
        ssjson << "\"http\": {";
        ssjson << "\"url\": \"" << listener << "\"";
        ssjson << "},";
        ssjson << "\"attrs\": [ " << myNotifAttrs << " ]";
        ssjson << "}";
        if (!expiration.empty())
        {
            ssjson << ",\"expires\": \"" << expiration << "\"";
        }
        if (throttling > 0)
        {
            ssjson << ",\"throttling\": " << throttling;
        }
        ssjson << "}";

        std::string json = ssjson.str();

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
                std::cout << "Added subscription with ID: " << subsc_id << std::endl;
                //std::cout << subsc_id << std::endl;
                return subsc_id;
            }
        }

    }
    catch(curlpp::RuntimeError & e)
    {
        std::cout << e.what() << std::endl;
    }
    catch(curlpp::LogicError & e)
    {
        std::cout << e.what() << std::endl;
    }
    return "";
}

void NGSIv2Subscriber::onTerminate()
{
    deleteSubscription();
}

void NGSIv2Subscriber::deleteSubscription()
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
        std::cout << e.what() << std::endl;
    }
    catch(curlpp::LogicError & e)
    {
        std::cout << e.what() << std::endl;
    }
}

void NGSIv2Subscriber::onDataReceived(void* data)
{
    if (!data)
    {
        return;
    }

    std::string* str = (std::string*)data;
    SerializedPayload_t serialized_input(2048);
    JsonNGSIv2PubSubType json_pst;
    JsonNGSIv2 json;
    json.data(*str);
    json.entityId(""); // No need to parse here (or empty means complete json in data ;) )
    json_pst.serialize(&json, &serialized_input);

    on_received_data(&serialized_input);
}


void NGSIv2Subscriber::listener()
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
            //std::cout << data << std::endl;
            data = data.substr(data.find_first_of("{\""));
            //std::cout << "Recv " << len << " bytes" << std::endl;

            if (len < buf_size)
            {
                onDataReceived(&data);
            }
            else
            {
                std::cout << "Received message too big (>= " << buf_size << " B). It will be ignored." << std::endl;
            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
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
        std::cout << e.what() << std::endl;
    }
    catch (curlpp::RuntimeError & e)
    {
        std::cout << e.what() << std::endl;
    }
    return "";
}

bool performAndRetry(int n_retries, curlpp::Easy &request)
{
    int tries = 0;
    bool success = false;
    std::ostringstream response;
    do
    {
        request.setOpt(new curlpp::options::WriteStream(&response));
        request.perform();
        long responseCode = curlpp::infos::ResponseCode::get(request);

        switch (responseCode / 100)
        {
            case 1: // 1XX codes
                success = true;
                tries = n_retries; // Success
                break;
            case 2: // 2XX codes
                success = true;
                tries = n_retries; // Success
                break;
            case 3: // 3XX codes
                tries = n_retries; // Abort
                std::cout << "Connection failed: " << responseCode << std::endl;
                std::cout << response.str() << std::endl;
                break;
            case 4: // 4XX codes
                std::cout << "Connection failed: " << responseCode << std::endl;
                tries = n_retries; //Don't retry
                break;
            case 5: // 5XX codes
                std::cout << "Connection failed: " << responseCode << std::endl;
                break;
            default:
                std::cout << response.str() << std::endl;
                break;
        }
    } while (!success && ++tries < n_retries);
    return success;
}
