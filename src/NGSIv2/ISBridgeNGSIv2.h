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


#ifndef _ISBRIDGENGSIv2_H_
#define _ISBRIDGENGSIv2_H_

#include <iostream>

#include "fastrtps/participant/Participant.h"
#include "fastrtps/attributes/ParticipantAttributes.h"
#include "fastrtps/publisher/Publisher.h"
#include "fastrtps/publisher/PublisherListener.h"
#include "fastrtps/attributes/PublisherAttributes.h"
#include "fastrtps/subscriber/Subscriber.h"
#include "fastrtps/subscriber/SubscriberListener.h"
#include "fastrtps/subscriber/SampleInfo.h"
#include "fastrtps/attributes/SubscriberAttributes.h"
#include "../../thirdparty/integration-services/src/ISBridge.h"
#include "../../thirdparty/integration-services/src/GenericPubSubTypes.h"
#include "../../thirdparty/integration-services/src/dynamicload/dynamicload.h"
#include <asio.hpp>

#include "NGSIv2Params.h"

#ifdef _WIN32
#define CURL_STATICLIB
#endif

#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Infos.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;
using namespace curlpp::options;
using namespace curlpp::infos;

typedef void (*userf_t)(SerializedPayload_t *serialized_input, SerializedPayload_t *serialized_output);

class NGSIv2Publisher : public ISPublisher
{
private:
    std::string url;
    std::string write(SerializedPayload_t *payload);
    Publisher *mf_publisher;
    Participant *mf_participant;
    std::string ngsiv2_host;
    uint16_t ngsiv2_port;
    //std::string ngsiv2_id;
public:
    NGSIv2Publisher();
    NGSIv2Publisher(const std::string &host, const uint16_t &port);
    static NGSIv2Publisher* configureNGSIv2Publisher(const NGSIv2Params &params);
    void setHostPort(const std::string &host, const uint16_t &port);
    ~NGSIv2Publisher() override;
    bool publish(void* payload) override;
};

class NGSIv2Listener : public ISSubscriber
{
private:
    asio::io_service* io_service;
    void* handle;
    std::string url;
    std::string subscription_id;
    bool exit;
    userf_t user_transformation;
    Participant *mf_participant;
    std::string ngsiv2_id;
    NGSIv2SubscriptionParams sub_params;
public:
    NGSIv2Listener(const std::string &host, const uint16_t &port);
    ~NGSIv2Listener() override;
    static NGSIv2Listener* configureNGSIv2Listener(const NGSIv2Params &params,
                                                   const NGSIv2SubscriptionParams &sub_params);
    std::string getListenerURL();
    void setTransformation(const char* file_path);
    std::string getAttrList(const std::vector< std::string > &list);
    std::string addSubscription(const std::string &server, const std::string &idPattern,
                    const std::string &type, const std::string &attrs, const std::string &expression,
                    const std::string &listener, const std::string &notifAttrs, const std::string &expiration = "",
                    const int &throttling = -1, const std::string &description = "");
    void deleteSubscription();
    void listener();
    void startListenerAndSubscribe();
    void setPublisher(ISPublisher* publisher) override;
    virtual bool onDataReceived(void * data) override;
};

class ISBridgeNGSIv2 : public ISBridge
{
public:
    ISBridgeNGSIv2(NGSIv2Publisher *pub,
        NGSIv2Listener *sub,
        const char* file_path);
    virtual ~ISBridgeNGSIv2();
    void onTerminate() override;
private:
    const char* file_path;
};

#endif // _Header__SUBSCRIBER_H_
