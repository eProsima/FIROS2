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
 * @file RobotExamplePublisher.cpp
 *
 */

#include "RobotExamplePublisher.h"
#include <fastrtps/participant/Participant.h>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/attributes/PublisherAttributes.h>
#include <fastrtps/publisher/Publisher.h>
#include <fastrtps/Domain.h>
#include <fastrtps/utils/eClock.h>
#include <string>

using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;

RobotExamplePublisher::RobotExamplePublisher():mp_participant(nullptr),
mp_publisher(nullptr)
{


}

bool RobotExamplePublisher::init()
{
    std::string robot_id = "Robot_001";
    std::string trans_time = "2018-04-01T00:00:00.00Z";
    m_Hello.robot_id(std::string(robot_id));
    m_Hello.transmission_time(trans_time);
    m_Hello.position().floor(0);
    m_Hello.position().x(0);
    m_Hello.position().y(0);
    m_Hello.position().zeta(0);

    ParticipantAttributes PParam;
    PParam.rtps.defaultSendPort = 11511;
    PParam.rtps.use_IP6_to_send = true;
    PParam.rtps.builtin.use_SIMPLE_RTPSParticipantDiscoveryProtocol = true;
    PParam.rtps.builtin.use_SIMPLE_EndpointDiscoveryProtocol = true;
    PParam.rtps.builtin.m_simpleEDP.use_PublicationReaderANDSubscriptionWriter = true;
    PParam.rtps.builtin.m_simpleEDP.use_PublicationWriterANDSubscriptionReader = true;
    PParam.rtps.builtin.domainId = 0;
    PParam.rtps.builtin.leaseDuration = c_TimeInfinite;
    PParam.rtps.setName("Participant_pub");
    mp_participant = Domain::createParticipant(PParam);

    if(mp_participant==nullptr)
        return false;
    //REGISTER THE TYPE

    Domain::registerType(mp_participant,&m_type);

    //CREATE THE PUBLISHER
    PublisherAttributes Wparam;
    Wparam.topic.topicKind = NO_KEY;
    Wparam.topic.topicDataType = "RobotSnd";
    Wparam.topic.topicName = "RobotTopic";
    Wparam.topic.historyQos.kind = KEEP_LAST_HISTORY_QOS;
    Wparam.topic.historyQos.depth = 30;
    Wparam.topic.resourceLimitsQos.max_samples = 50;
    Wparam.topic.resourceLimitsQos.allocated_samples = 20;
    Wparam.times.heartbeatPeriod.seconds = 2;
    Wparam.times.heartbeatPeriod.fraction = 200*1000*1000;
    Wparam.qos.m_reliability.kind = RELIABLE_RELIABILITY_QOS;
    Wparam.qos.m_partition.push_back("rt");
    mp_publisher = Domain::createPublisher(mp_participant,Wparam,(PublisherListener*)&m_listener);
    if(mp_publisher == nullptr)
        return false;

    return true;

}

RobotExamplePublisher::~RobotExamplePublisher()
{
    // TODO Auto-generated destructor stub
    Domain::removeParticipant(mp_participant);
}

void RobotExamplePublisher::PubListener::onPublicationMatched(Publisher* /*pub*/,MatchingInfo& info)
{
    if(info.status == MATCHED_MATCHING)
    {
        n_matched++;
        std::cout << "Publisher matched"<<std::endl;
    }
    else
    {
        n_matched--;
        std::cout << "Publisher unmatched"<<std::endl;
    }
}

void RobotExamplePublisher::run(uint32_t samples)
{
    for(uint32_t i = 0;i<samples;++i)
    {
        if(!publish())
            --i;
        else
        {
            std::cout << "Robot: "<<m_Hello.robot_id() << " teleported to floor: " << m_Hello.position().floor() << std::endl;
        }
        eClock::my_sleep(10000);
    }
}

bool RobotExamplePublisher::publish()
{
    if(m_listener.n_matched>0)
    {
        int floor = m_Hello.position().floor();
        if (++floor > 5)
        {
            floor = 0;
        }

        m_Hello.position().floor(floor);
        mp_publisher->write((void*)&m_Hello);
        return true;
    }
    return false;
}
