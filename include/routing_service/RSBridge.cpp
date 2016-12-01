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

#include "RSBridge.h"
#include "GenericPubSubTypes.h"


RSBridge::RSBridge(	ParticipantAttributes par_pub_params,
							ParticipantAttributes par_sub_params,
							PublisherAttributes pub_params,
							SubscriberAttributes sub_params,
							const char *file_path
						) : mp_participant(nullptr), ms_participant(nullptr),
						 	ms_subscriber(nullptr),	mp_publisher(nullptr),
							input_type(nullptr), output_type(nullptr),
							m_listener(file_path)
{
	// Create RTPSParticipant
	mp_participant = Domain::createParticipant(par_pub_params);
	if(mp_participant == nullptr) std::cout << "participant creation failed";

	// Create RTPSParticipant
	ms_participant = Domain::createParticipant(par_sub_params);
	if(ms_participant == nullptr) std::cout << "participant creation failed";

	//Register types
	input_type = new GenericPubSubType();
	input_type->setName(sub_params.topic.topicDataType.c_str());
	Domain::registerType(ms_participant,(TopicDataType*) input_type);
	output_type = new GenericPubSubType();
	output_type->setName(pub_params.topic.topicDataType.c_str());
	Domain::registerType(mp_participant,(TopicDataType*) output_type);

	//Create publisher
	mp_publisher = Domain::createPublisher(mp_participant,pub_params,nullptr);
	if(mp_publisher == nullptr)  std::cout << "publisher creation failed";
	m_listener.setPublisher(mp_publisher);

	// Create Subscriber
	ms_subscriber = Domain::createSubscriber(ms_participant,sub_params,(SubscriberListener*)&m_listener);
	if(ms_subscriber == nullptr)  std::cout << "subscriber creation failed";
}

RSBridge::~RSBridge(){
	Domain::removeParticipant(mp_participant);
	Domain::removeParticipant(ms_participant);
}

void RSBridge::SubListener::onSubscriptionMatched(Subscriber* sub,MatchingInfo& info){
	if (info.status == MATCHED_MATCHING)
	{
		n_matched++;
		cout << "Subscriber matched" << endl;
	}
	else
	{
		n_matched--;
		cout << "Subscriber unmatched" << endl;
	}
}

RSBridge::SubListener::SubListener(const char* file_path){
	//handle = dlopen (file_path, RTLD_LAZY);
	handle = eProsimaLoadLibrary(file_path);
    /*if (!handle) {
        fputs (dlerror(), stderr);
        exit(1);
    }*/
    user_transformation = (userf_t)eProsimaGetProcAddress(handle, "transform");
    /*if ((error = dlerror()) != NULL){
        fputs(error, stderr);
        exit(1);
    }*/
}

RSBridge::SubListener::~SubListener(){
	eProsimaCloseLibrary(handle);
}

void RSBridge::SubListener::onNewDataMessage(Subscriber* sub){
	SerializedPayload_t serialized_input;
	SerializedPayload_t serialized_output;
    if(sub->takeNextData(&serialized_input, &m_info)){
        if(m_info.sampleKind == ALIVE){
			user_transformation(&serialized_input, &serialized_output);
			listener_publisher->write(&serialized_output);
		}
	}
}
