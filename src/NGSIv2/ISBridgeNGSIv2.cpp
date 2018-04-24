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

using asio::ip::tcp;

ISBridgeNGSIv2::~ISBridgeNGSIv2()
{
}

NGSIv2Listener::~NGSIv2Listener()
{
    exit = true;
}

NGSIv2Publisher::~NGSIv2Publisher()
{
}

std::string NGSIv2Listener::getListenerURL()
{
    std::stringstream strstr;
    strstr << "http://" << sub_params.host << ":" << sub_params.port;

    return strstr.str();
}

NGSIv2Listener::NGSIv2Listener(const std::string &name, const std::string &host, const uint16_t &port) :
    ISSubscriber(name)
{
    std::stringstream strstr;
    strstr << host << ":" << port;
    url = strstr.str();

    exit = false;
}

NGSIv2Publisher::NGSIv2Publisher(const std::string &name, const std::string &host, const uint16_t &port) :
    ISPublisher(name)
{
    setHostPort(host, port);
}

void NGSIv2Publisher::setHostPort(const std::string &host, const uint16_t &port)
{
    std::stringstream strstr;
    strstr << host << ":" << port;
    url = strstr.str();
}

/*
NGSIv2Listener* NGSIv2Listener::configureNGSIv2Listener(const NGSIv2Params &params,
                                                        const NGSIv2SubscriptionParams &sub_params)
{
    NGSIv2Listener* listener = new NGSIv2Listener(params.host, params.port);
    listener->sub_params = sub_params;

    listener->handle = nullptr;
    listener->user_transformation = nullptr;

    return listener;
}

NGSIv2Publisher * NGSIv2Publisher::configureNGSIv2Publisher(const NGSIv2Params &params)
{
    NGSIv2Publisher* publisher = new NGSIv2Publisher(params.host, params.port);

    publisher->ngsiv2_host = params.host;
    publisher->ngsiv2_port = params.port;
    //publisher->ngsiv2_id = params.idPattern;

    return publisher;
}
*/

void NGSIv2Listener::startListenerAndSubscribe()
{
    std::thread thread(&NGSIv2Listener::listener, this);

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

std::string NGSIv2Listener::addSubscription(const std::string &server, const std::string &idPattern,
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
        if (!expiration.empty()) { ssjson << ",\"expires\": \"" << expiration << "\""; }
        if (throttling > 0) { ssjson << ",\"throttling\": " << throttling; }
        ssjson << "}";

        std::string json = ssjson.str();

        subRequest.setOpt(new curlpp::options::PostFields(json));
        subRequest.setOpt(new curlpp::options::PostFieldSize(json.length()));

        std::stringstream ss;
        ss << server << "/v2/subscriptions/";
        subRequest.setOpt<Url>(ss.str());

        std::ostringstream response;

        subRequest.setOpt(new curlpp::options::WriteStream(&response));
        subRequest.setOpt(new curlpp::options::Header(1));

        // Send request and get a result.
        subRequest.perform();

        //cout << response.str() << endl;

        std::string subsc_id;
        std::istringstream responseHeader(response.str());

        for (std::string line; std::getline(responseHeader, line); )
        {
            if (line.find("Location: /v2/subscriptions/") != std::string::npos)
            {
                subsc_id = line.substr(line.find_last_of("/") + 1);
                subsc_id.pop_back(); // Remove \r tail
                std::cout << "Added subscription with ID: " << subsc_id << std::endl;
                std::cout << subsc_id << std::endl;
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

void NGSIv2Listener::onTerminate()
{
    deleteSubscription();
}

void NGSIv2Listener::deleteSubscription()
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

        std::ostringstream delResponse;
        delRequest.setOpt(new curlpp::options::WriteStream(&delResponse));

        // Send request and get a result.
        delRequest.perform();

        std::cout << delResponse.str() << std::endl;
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

void NGSIv2Listener::onDataReceived(void* data)
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
    /*
    //std::cout << "Received: " << *str << std::endl;
    SerializedPayload_t serialized_output;
    if(user_transformation){
        user_transformation(&serialized_input, &serialized_output);
    }

    if (listener_publisher)
    {
        bool result = listener_publisher->publish(&serialized_output);
        //std::cout << "Payload wrote" << std::endl;
        return result;
    }
    return false;
    */
}


void NGSIv2Listener::listener()
{
    try
    {
        std::array<char, 2048> buf;
        asio::io_service io_service;
        this->io_service = &io_service;
        asio::error_code error;
        tcp::endpoint myendpoint(tcp::v4(), sub_params.port);
        tcp::acceptor acceptor(io_service, myendpoint);

        while (!exit)
        {
            tcp::socket socket(io_service);
            acceptor.accept(socket);

            size_t len = socket.read_some(asio::buffer(buf), error);

            if (error == asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw asio::system_error(error); // Some other error.

            //std::cout.write(buf.data(), len);
            std::stringstream ss;
            ss << buf.data();

            std::string data = ss.str();
            data = data.substr(data.find_first_of("{\""));

            //std::cout << "Recv " << len << " bytes" << std::endl;

            if (len < 2048)
            {
                onDataReceived(&data);
            }
            else
            {
                std::cout << "Received message too big (>= 2048 B). It will be ignored." << std::endl;
            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

bool NGSIv2Publisher::publish(void* payload)
{
    write((SerializedPayload_t*) payload);
    return true;
}

std::string NGSIv2Publisher::write(SerializedPayload_t* payload)
{
    try {
        curlpp::Cleanup cleaner;
        curlpp::Easy request;

        JsonNGSIv2PubSubType json_pst;
        JsonNGSIv2 json;
        json_pst.deserialize(payload, &json);

        //std::string entityId = getEntityId(json);
        //std::string payload = getPayload(json);
        std::string entityId = json.entityId();
        std::string payload = json.data();
        request.setOpt(new curlpp::options::Url(url + "/v2/entities/" + entityId + "/attrs"));
        request.setOpt(new curlpp::options::Verbose(true));
        std::list<std::string> header;
        header.push_back("Content-Type: application/json");
        //header.push_back("Content-Length: application/json");

        request.setOpt(new curlpp::options::HttpHeader(header));
        request.setOpt(new curlpp::options::PostFields(payload));
        request.setOpt(new curlpp::options::PostFieldSize(payload.length()));

        std::ostringstream response;
        request.setOpt(new curlpp::options::WriteStream(&response));

        request.perform();

        std::cout << response.str() << std::endl;
        return response.str();
    }
    catch ( curlpp::LogicError & e ) {
        std::cout << e.what() << std::endl;
    }
    catch ( curlpp::RuntimeError & e ) {
        std::cout << e.what() << std::endl;
    }
    return "";
}

