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


#ifndef _RSBRIDGENGSIv2TOFASTRTPS_H_
#define _RSBRIDGENGSIv2TOFASTRTPS_H_

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
#include "../../thirdparty/routing-service/src/RSBridge.h"
#include "../../thirdparty/routing-service/src/GenericPubSubTypes.h"

#include "NGSIv2Params.h"

#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Infos.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include <boost/asio.hpp>
#include <boost/array.hpp>

using namespace std;
using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;
using namespace curlpp::options;
using namespace curlpp::infos;

using boost::asio::ip::tcp;

class RSBridgeNGSIv2ToFastRTPS : public RSBridge
{
public:
    RSBridgeNGSIv2ToFastRTPS(NGSIv2Params par_ngsiv2_params,
                ParticipantAttributes par_fastrtps_params,
                NGSIv2SubscriptionParams sub_params,
                PublisherAttributes pub_params,
                const char* file_path);
    //RSBridgeNGSIv2ToFastRTPS(RSBridgeNGSIv2ToFastRTPS &&b);
    virtual ~RSBridgeNGSIv2ToFastRTPS();
    void onTerminate() override;
private:
    Participant *mf_participant;
    Publisher *mf_publisher;
    GenericPubSubType *data_type;
    std::string ngsiv2_host;
    uint16_t ngsiv2_port;
    std::string ngsiv2_id;

    class NGSIv2Listener
    {
    private:
        boost::asio::io_service* io_service;
        void* handle;
    public:
        Publisher *fastrtps_pub;
        string url;
        string listener_host;
        uint16_t listener_port;
        string subscription_id;
        bool exit;
        userf_t user_transformation;
        NGSIv2Listener(const string host, const uint16_t port);
        //NGSIv2Listener(NGSIv2Listener &&l);
        ~NGSIv2Listener();
        string getListenerURL();
        string getAttrList(const std::vector< string > list);
        string addSubscription(const string server, const string idPattern,
                     const string type, const string attrs, const string expression,
                     const string listener, const string notifAttrs, const string expiration = "",
                     const int throttling = -1, const string description = "");
        void deleteSubscription(const string server, const string id);
        void listener();
        void setPublisher(Publisher *fastrtps_pub);
        void startListenerAndSubscribe(NGSIv2SubscriptionParams sub_params, const char* file_path);
    } ngsiv2_listener;
};

#endif // _Header__SUBSCRIBER_H_
