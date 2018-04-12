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

#include "RSBridgeNGSIv2ToFastRTPS.h"
#include "idl/JsonNGSIv2PubSubTypes.h" // Received type from NGSIv2

#include <algorithm>
#include <thread>

using asio::ip::tcp;

RSBridgeNGSIv2ToFastRTPS::RSBridgeNGSIv2ToFastRTPS(NGSIv2Params par_ngsiv2_params,
                    ParticipantAttributes par_fastrtps_params,
                    NGSIv2SubscriptionParams sub_params,
                    PublisherAttributes pub_params,
                    const char *file_path
                ) : mf_participant(nullptr),
                    mf_publisher(nullptr),
                    data_type(nullptr),
                    ngsiv2_host(par_ngsiv2_params.host),
                    ngsiv2_port(par_ngsiv2_params.port),
                    ngsiv2_id(par_ngsiv2_params.idPattern),
                    ngsiv2_listener(ngsiv2_host, ngsiv2_port)
{
    // Create RTPSParticipant
    mf_participant = Domain::createParticipant(par_fastrtps_params);
    if(mf_participant == nullptr) return;

    //Register types
    data_type = new GenericPubSubType();
    data_type->setName(pub_params.topic.topicDataType.c_str());
    Domain::registerType(mf_participant,(TopicDataType*) data_type);

    //Create publisher
    mf_publisher = Domain::createPublisher(mf_participant,pub_params,nullptr);
    if(mf_publisher == nullptr) return;

    // Configure and start listener
    ngsiv2_listener.setPublisher(mf_publisher);
    ngsiv2_listener.startListenerAndSubscribe(sub_params, file_path);
}

RSBridgeNGSIv2ToFastRTPS::~RSBridgeNGSIv2ToFastRTPS(){
    if(mf_participant != nullptr) Domain::removeParticipant(mf_participant);
}

string RSBridgeNGSIv2ToFastRTPS::NGSIv2Listener::getListenerURL()
{
    stringstream strstr;
    strstr << "http://" << listener_host << ":" << listener_port;

    return strstr.str();
}

RSBridgeNGSIv2ToFastRTPS::NGSIv2Listener::NGSIv2Listener(const string host, const uint16_t port)
{
    stringstream strstr;
    strstr << host << ":" << port;
    url = strstr.str();

    user_transformation = nullptr;
}

RSBridgeNGSIv2ToFastRTPS::NGSIv2Listener::~NGSIv2Listener()
{
    if(handle) eProsimaCloseLibrary(handle);
    exit = true;
}

void RSBridgeNGSIv2ToFastRTPS::NGSIv2Listener::startListenerAndSubscribe(NGSIv2SubscriptionParams sub_params, const char* file_path)
{
    listener_host = sub_params.host;
    listener_port = sub_params.port;
    exit = false;
    std::thread thread(&RSBridgeNGSIv2ToFastRTPS::NGSIv2Listener::listener, this);

    if(file_path){
        handle = eProsimaLoadLibrary(file_path);
        user_transformation = (userf_t)eProsimaGetProcAddress(handle, "transformFromNGSIv2");
        if (!user_transformation)
        {
            user_transformation = (userf_t)eProsimaGetProcAddress(handle, "transform");
        }
    }

    subscription_id = addSubscription(url, sub_params.idPattern, sub_params.type, sub_params.attrs, sub_params.expression,
        getListenerURL(), sub_params.notif, sub_params.expiration, sub_params.throttling, sub_params.description);

    thread.detach();
}

void RSBridgeNGSIv2ToFastRTPS::NGSIv2Listener::setPublisher(Publisher *fastrtps_pub)
{
    this->fastrtps_pub = fastrtps_pub;
}

void find_and_replace(string& source, string const& find, string const& replace)
{
    for(string::size_type i = 0; (i = source.find(find, i)) != string::npos;)
    {
        source.replace(i, find.length(), replace);
        i += replace.length();
    }
}

string RSBridgeNGSIv2ToFastRTPS::NGSIv2Listener::addSubscription(const string server, const string idPattern,
                     const string type, const string attrs, const string expression,
                     const string listener, const string notifAttrs, const string expiration,
                     const int throttling, const string description)
{
    try
    {
        curlpp::Cleanup myCleanup;// Now add a subscription
        curlpp::Easy subRequest;
        std::list<std::string> sheader;
        sheader.push_back("Content-Type: application/json");
        subRequest.setOpt(new curlpp::options::HttpHeader(sheader));
        //subRequest.setOpt(new curlpp::options::Verbose(true));

        string myAttrs = attrs;
        string myNotifAttrs = notifAttrs;
        string comma(",");
        string quotecommaquote("\",\"");
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

        string subsc_id;
        std::istringstream responseHeader(response.str());

        for (std::string line; std::getline(responseHeader, line); )
        {
            if (line.find("Location: /v2/subscriptions/") != std::string::npos)
            {
                subsc_id = line.substr(line.find_last_of("/") + 1);
                subsc_id.pop_back(); // Remove \r tail
                cout << "Added subscription with ID: " << subsc_id << endl;
                cout << subsc_id << endl;
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

void RSBridgeNGSIv2ToFastRTPS::onTerminate()
{
    ngsiv2_listener.deleteSubscription(ngsiv2_listener.url, ngsiv2_listener.subscription_id); // Delete our subcription from origin
}

void RSBridgeNGSIv2ToFastRTPS::NGSIv2Listener::deleteSubscription(const string server, const string id)
{
    try
    {
        curlpp::Cleanup myCleanup;

        curlpp::Easy delRequest;
        std::list<std::string> delHeader;

        std::stringstream ss;
        ss << server << "/v2/subscriptions/" << id;
        delRequest.setOpt<Url>(ss.str());

        delRequest.setOpt(new curlpp::options::CustomRequest("DELETE"));

        std::ostringstream delResponse;
        delRequest.setOpt(new curlpp::options::WriteStream(&delResponse));

        // Send request and get a result.
        delRequest.perform();

        cout << delResponse.str() << endl;
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

void RSBridgeNGSIv2ToFastRTPS::NGSIv2Listener::listener()
{
    try
    {
        array<char, 2048> buf;
        asio::io_service io_service;
        this->io_service = &io_service;
        asio::error_code error;
        tcp::endpoint myendpoint(tcp::v4(), listener_port);
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
            stringstream ss;
            ss << buf.data();

            string data = ss.str();
            data = data.substr(data.find_first_of("{\""));

            std::cout << "Recv " << len << " bytes" << std::endl;
            //std::cout << data << std::endl;

            //SerializedPayload_t serialized_input(data.length());
            SerializedPayload_t serialized_input(2048);
            JsonNGSIv2PubSubType json_pst;
            JsonNGSIv2 json;
            json.data(data);
            json.entityId(""); // No need to parse here (or empty means complete json in data ;) )
            json_pst.serialize(&json, &serialized_input);

            SerializedPayload_t serialized_output;
            if(user_transformation){
                user_transformation(&serialized_input, &serialized_output);
            }
            fastrtps_pub->write(&serialized_output);
            std::cout << "Payload wrote" << std::endl;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
