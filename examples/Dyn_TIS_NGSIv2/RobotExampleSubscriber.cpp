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

/**
 * @file RobotExampleSubscriber.cpp
 *
 */

#include "RobotExampleSubscriber.h"
#include <fastrtps/participant/Participant.h>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/attributes/SubscriberAttributes.h>
#include <fastrtps/subscriber/Subscriber.h>
#include <fastrtps/Domain.h>
#include <fastrtps/utils/eClock.h>

using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;

RobotExampleSubscriber::RobotExampleSubscriber():mp_participant(nullptr),
mp_subscriber(nullptr)
{
}

bool RobotExampleSubscriber::init()
{
    ParticipantAttributes PParam;
    //PParam.rtps.defaultSendPort = 10043;
    PParam.rtps.builtin.use_SIMPLE_RTPSParticipantDiscoveryProtocol = true;
    PParam.rtps.builtin.use_SIMPLE_EndpointDiscoveryProtocol = true;
    PParam.rtps.builtin.m_simpleEDP.use_PublicationReaderANDSubscriptionWriter = true;
    PParam.rtps.builtin.m_simpleEDP.use_PublicationWriterANDSubscriptionReader = true;
    PParam.rtps.builtin.domainId = 0;
    PParam.rtps.builtin.leaseDuration = c_TimeInfinite;
    PParam.rtps.setName("Participant_sub");
    mp_participant = Domain::createParticipant(PParam);
    if(mp_participant==nullptr)
        return false;

    //REGISTER THE TYPE

    Domain::registerType(mp_participant,&m_type);
    //CREATE THE SUBSCRIBER
    SubscriberAttributes Rparam;
    Rparam.topic.topicKind = NO_KEY;
    Rparam.topic.topicDataType = "rt/RobotSnd";
    Rparam.topic.topicName = "RobotTopic";
    Rparam.topic.historyQos.kind = KEEP_LAST_HISTORY_QOS;
    Rparam.topic.historyQos.depth = 30;
    Rparam.topic.resourceLimitsQos.max_samples = 50;
    Rparam.topic.resourceLimitsQos.allocated_samples = 20;
    Rparam.qos.m_reliability.kind = RELIABLE_RELIABILITY_QOS;
    Rparam.qos.m_durability.kind = TRANSIENT_LOCAL_DURABILITY_QOS;
    //Rparam.qos.m_partition.push_back("rt");
    mp_subscriber = Domain::createSubscriber(mp_participant,Rparam,(SubscriberListener*)&m_listener);

    if(mp_subscriber == nullptr)
        return false;


    return true;
}

RobotExampleSubscriber::~RobotExampleSubscriber() {
    // TODO Auto-generated destructor stub
    Domain::removeParticipant(mp_participant);
}

void RobotExampleSubscriber::SubListener::onSubscriptionMatched(Subscriber* /*sub*/,MatchingInfo& info)
{
    if(info.status == MATCHED_MATCHING)
    {
        n_matched++;
        std::cout << "Subscriber matched"<<std::endl;
    }
    else
    {
        n_matched--;
        std::cout << "Subscriber unmatched"<<std::endl;
    }
}

void RobotExampleSubscriber::SubListener::onNewDataMessage(Subscriber* sub)
{
    if(sub->takeNextData((void*)&m_Hello, &m_info))
    {
        if(m_info.sampleKind == ALIVE)
        {
            this->n_samples++;
            // Print your structure data here.
            std::cout << "Robot "<<m_Hello.robot_id()<< " says its floor is "<< m_Hello.position().floor()<< " RECEIVED"<<std::endl;
        }
    }

}


void RobotExampleSubscriber::run()
{
    std::cout << "Subscriber running. Please press enter to stop the Subscriber" << std::endl;
    std::cin.ignore();
}

void RobotExampleSubscriber::run(uint32_t number)
{
    std::cout << "Subscriber running until "<< number << "samples have been received"<<std::endl;
    while(number < this->m_listener.n_samples)
        eClock::my_sleep(500);
}
