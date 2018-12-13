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


#ifndef _NGSIv2_SUBSCRIBER_H_
#define _NGSIv2_SUBSCRIBER_H_

#include <iostream>

#include "ISBridge.h"
#include "GenericPubSubTypes.h"
#include <asio.hpp>

#include "NGSIv2Params.h"

#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Infos.hpp>
#include <curlpp/Easy.hpp>

using namespace curlpp::options;
using namespace curlpp::infos;

class NGSIv2Subscriber : public ISReader
{
private:
    asio::io_service* io_service;
    std::string url;
    std::string subscription_id;
    bool exit;
    std::string ngsiv2_id;
    NGSIv2SubscriptionParams sub_params;
    NGSIv2Params part_params;
public:
    NGSIv2Subscriber(const std::string &name, const NGSIv2Params &params);
    ~NGSIv2Subscriber() override;
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
    void onDataReceived(void* data);
    void onTerminate() override;
    void setSubscriptionParams(const NGSIv2SubscriptionParams &params);
};

#endif // _NGSIv2_SUBSCRIBER_H_
