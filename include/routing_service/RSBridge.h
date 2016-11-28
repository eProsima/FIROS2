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


#ifndef _RSBRIDGE_H_
#define _RSBRIDGE_H_

#include <fastrtps/fastrtps_fwd.h>
#include "fastrtps/participant/Participant.h"
#include "fastrtps/attributes/ParticipantAttributes.h"
#include "fastrtps/publisher/Publisher.h"
#include "fastrtps/publisher/PublisherListener.h"
#include "fastrtps/attributes/PublisherAttributes.h"
#include "fastrtps/subscriber/Subscriber.h"
#include "fastrtps/subscriber/SubscriberListener.h"
#include "fastrtps/subscriber/SampleInfo.h"
#include "fastrtps/attributes/SubscriberAttributes.h"
#include "GenericPubSubTypes.h"
#include "RSBridge.h"

#include <dlfcn.h>

using namespace eprosima::fastrtps;

typedef void (*userf_t)(SerializedPayload_t *serialized_input, SerializedPayload_t *serialized_output);

class RSBridge
{
public:
	RSBridge(ParticipantAttributes par_pub_params,
				ParticipantAttributes par_sub_params,
				PublisherAttributes pub_params,
				SubscriberAttributes sub_params,
				const char* file_path);
	virtual ~RSBridge();
private:
	Participant *mp_participant;
	Participant *ms_participant;
	Subscriber *ms_subscriber;
	Publisher *mp_publisher;
	GenericPubSubType *input_type;
	GenericPubSubType *output_type;

	class SubListener : public SubscriberListener
	{
		public:
		SubListener(const char* file_path);
		~SubListener();
		void setPublisher(Publisher* publisher){
			listener_publisher = publisher;
		}
		void onSubscriptionMatched(Subscriber* sub,MatchingInfo& info);
		void onNewDataMessage(Subscriber* sub);
		Publisher *listener_publisher;
		userf_t user_transformation;
		void *handle;
		char *error;

		SampleInfo_t m_info;
		int n_matched;
		int n_msg;
	} m_listener;
};

#endif // _Header__SUBSCRIBER_H_
