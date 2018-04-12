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

#include <fastrtps/Domain.h>

#include "RSBridgeFastRTPSToNGSIv2.h"
#include "idl/JsonNGSIv2PubSubTypes.h" // Type to send to NGSIv2

using asio::ip::tcp;

RSBridgeFastRTPSToNGSIv2::RSBridgeFastRTPSToNGSIv2(
                    ParticipantAttributes par_fastrtps_params,
                    NGSIv2Params par_ngsiv2_params,
                    SubscriberAttributes sub_params,
                    const char *file_path
                ) : mf_participant(nullptr),
                    mf_subscriber(nullptr),
                    data_type(nullptr),
                    ngsiv2_host(par_ngsiv2_params.host),
                    ngsiv2_port(par_ngsiv2_params.port),
                    ngsiv2_id(par_ngsiv2_params.idPattern),
                    ngsiv2_publisher(ngsiv2_host, ngsiv2_port),
                    m_listener(file_path)
{
    // Create RTPSParticipant
    mf_participant = Domain::createParticipant(par_fastrtps_params);
    if(mf_participant == nullptr) return;

    //Register types
    data_type = new GenericPubSubType();
    data_type->setName(sub_params.topic.topicDataType.c_str());
    Domain::registerType(mf_participant,(TopicDataType*) data_type);

    m_listener.setPublisher(&ngsiv2_publisher);

    // Create Subscriber
    mf_subscriber = Domain::createSubscriber(mf_participant,sub_params,(SubscriberListener*)&m_listener);
    if(mf_subscriber == nullptr) return;
}

RSBridgeFastRTPSToNGSIv2::~RSBridgeFastRTPSToNGSIv2(){
    if(mf_participant != nullptr) Domain::removeParticipant(mf_participant);
}

void RSBridgeFastRTPSToNGSIv2::onTerminate()
{
    // Don't need to do anything here.
}

void RSBridgeFastRTPSToNGSIv2::SubListener::onSubscriptionMatched(Subscriber* sub, MatchingInfo& info){
    if (info.status == MATCHED_MATCHING)
    {
        n_matched++;
        std::cout << "Subscriber matched" << std::endl;
        std::cout << "Sub: " << sub->getGuid() << std::endl;
        std::cout << "Topic: " << sub->getAttributes().topic.getTopicName() << std::endl;
    }
    else
    {
        n_matched--;
        std::cout << "Subscriber unmatched" << std::endl;
    }
}

RSBridgeFastRTPSToNGSIv2::SubListener::SubListener() : user_transformation(nullptr), handle(nullptr)
{
}

RSBridgeFastRTPSToNGSIv2::SubListener::SubListener(const char* file_path) : user_transformation(nullptr), handle(nullptr)
{
    loadLibrary(file_path);
}

void RSBridgeFastRTPSToNGSIv2::SubListener::loadLibrary(const char* file_path)
{
    if(file_path){
        handle = eProsimaLoadLibrary(file_path);
        user_transformation = (userf_t)eProsimaGetProcAddress(handle, "transformToNGSIv2");
        if (!user_transformation)
        {
            user_transformation = (userf_t)eProsimaGetProcAddress(handle, "transform");
        }
    }
}

RSBridgeFastRTPSToNGSIv2::SubListener::~SubListener(){
    if(handle) eProsimaCloseLibrary(handle);
}

void RSBridgeFastRTPSToNGSIv2::SubListener::onNewDataMessage(Subscriber* sub){
    SerializedPayload_t serialized_input;
    SerializedPayload_t serialized_output;
    if(sub->takeNextData(&serialized_input, &m_info)){
        if(m_info.sampleKind == ALIVE){
            if(user_transformation){
                user_transformation(&serialized_input, &serialized_output);
            }
            else{
                serialized_output.copy(&serialized_input, false);
            }
            listener_publisher->write(&serialized_output);
        }
    }
}

RSBridgeFastRTPSToNGSIv2::NGSIv2Publisher::NGSIv2Publisher()
{
}

RSBridgeFastRTPSToNGSIv2::NGSIv2Publisher::NGSIv2Publisher(const string host, const uint16_t port)
{
    setHostPort(host, port);
}

void RSBridgeFastRTPSToNGSIv2::NGSIv2Publisher::setHostPort(const string host, const uint16_t port)
{
    stringstream strstr;
    strstr << host << ":" << port;
    url = strstr.str();
}

RSBridgeFastRTPSToNGSIv2::NGSIv2Publisher::~NGSIv2Publisher() {}
/*
string getEntityId(const string json)
{
    return json.substr(0, json.find_first_of("@"));
}

string getPayload(const string json)
{
    return json.substr(json.find_first_of("@") + 1);
}
*/
string RSBridgeFastRTPSToNGSIv2::NGSIv2Publisher::write(SerializedPayload_t* payload)
{
    try {
        curlpp::Cleanup cleaner;
        curlpp::Easy request;

        JsonNGSIv2PubSubType json_pst;
        JsonNGSIv2 json;
        json_pst.deserialize(payload, &json);

        //string entityId = getEntityId(json);
        //string payload = getPayload(json);
        string entityId = json.entityId();
        string payload = json.data();
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

        cout << response.str() << endl;
    }
    catch ( curlpp::LogicError & e ) {
        std::cout << e.what() << std::endl;
    }
    catch ( curlpp::RuntimeError & e ) {
        std::cout << e.what() << std::endl;
    }
}
