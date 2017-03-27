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

//#include "TestSubscriber.h"

#include <fastrtps/Domain.h>

#include <fastrtps/fastrtps_fwd.h>
#include <fastrtps/subscriber/Subscriber.h>
#include <fastrtps/attributes/SubscriberAttributes.h>
#include <fastrtps/subscriber/SubscriberListener.h>
#include <fastrtps/subscriber/SampleInfo.h>

#include <fastrtps/utils/eClock.h>


#include "TestPubSubTypes.h"

using namespace eprosima;
using namespace eprosima::fastrtps;


class SubListener : public SubscriberListener{
public:
    SubListener() : correct_samples(0), fail(false){};
    ~SubListener(){};
    void onSubscriptionMatched(Subscriber* sub,MatchingInfo& info){};
    void onNewDataMessage(Subscriber* sub);
    SampleInfo_t m_info;
    int correct_samples;
    bool fail;
};

void SubListener::onNewDataMessage(Subscriber* sub){
	// Take data
	Test st;

	if(sub->takeNextData(&st, &m_info)){
		if(m_info.sampleKind == ALIVE){
            if(st.message() == "text_modified"){
                correct_samples++;
                std::cout << "Message succesfully received" << std::endl;
            }
            else{

            }
		}
	}
}


int main(int argc, char** argv)
{
    std::cout << "Fast RTPS subscriber node" << std::endl;

    SubListener m_listener;
    ParticipantAttributes PParam;
	PParam.rtps.builtin.domainId = 0; //MUST BE THE SAME AS IN THE PUBLISHER
	PParam.rtps.builtin.leaseDuration = c_TimeInfinite;
	PParam.rtps.setName("test_firos2_subscriber"); //You can put the name you want
	Participant *mp_participant = Domain::createParticipant(PParam);
	if(mp_participant == nullptr)
		return false;

	//Register the type
    TestPubSubType myType;
	Domain::registerType(mp_participant,(TopicDataType*) &myType);

	// Create Subscriber
	SubscriberAttributes Rparam;
	Rparam.topic.topicKind = NO_KEY;
	Rparam.topic.topicDataType = myType.getName(); //Must be registered before the creation of the subscriber
	Rparam.topic.topicName = "test_fastrtps_topic";
	Subscriber *mp_subscriber = Domain::createSubscriber(mp_participant,Rparam,(SubscriberListener*)&m_listener);
	if(mp_subscriber == nullptr)
		return false;

    bool test_running = true;
    int test_result = 0;
    while(test_running){
        if (m_listener.fail){
            test_result = 1;
            test_running = false;
        }
        else if (m_listener.correct_samples > 4){
            test_result = 0;
            test_running = false;
        }
        else{
            eClock::my_sleep(100);
        }
    }

    Domain::removeParticipant(mp_participant);

    std::cout << "Closing..." << std::endl;
	return test_result;
}
