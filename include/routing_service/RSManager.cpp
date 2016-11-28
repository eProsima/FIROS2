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

#include "RSManager.h"
#include "../tinyxml2/tinyxml2.h"

RSManager::RSManager(std::string xml_file_path){

	std::cout << "Routing Service Manager" << std::endl;

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError error;
	if(doc.LoadFile(xml_file_path.c_str())){
		std::cout << "Error: " << error << std::endl;
	}

	tinyxml2::XMLElement *bridge_element = doc.FirstChildElement("firos");
	for (auto child = bridge_element->FirstChildElement(); child != NULL; child = child->NextSiblingElement()){
		const char* input_participant_name = child->FirstChildElement("subscriber")->
													FirstChildElement("participant")->GetText();
		int input_domain;
		child->FirstChildElement("subscriber")->QueryIntAttribute("domain", &input_domain);
		const char* input_topic_name = child->FirstChildElement("subscriber")->
											  FirstChildElement("topic")->GetText();
		const char* input_type_name = child->FirstChildElement("subscriber")->
											 FirstChildElement("type")->GetText();


		const char* output_participant_name = child->FirstChildElement("publisher")->
													 FirstChildElement("participant")->GetText();
		int output_domain;
		child->FirstChildElement("publisher")->QueryIntAttribute("domain", &output_domain);
		const char* output_topic_name = child->FirstChildElement("publisher")->
											   FirstChildElement("topic")->GetText();
		const char* output_type_name = child->FirstChildElement("publisher")->
											  FirstChildElement("type")->GetText();
		const char* function_path = child->FirstChildElement("transformation")->GetText();

		// Participant (publisher) configuration
		ParticipantAttributes participant_publisher_params;
		participant_publisher_params.rtps.builtin.domainId = output_domain;
		participant_publisher_params.rtps.builtin.leaseDuration = c_TimeInfinite;
		participant_publisher_params.rtps.setName(output_participant_name);

		// Participant (subscriber) configuration
		ParticipantAttributes participant_subscriber_params;
		participant_subscriber_params.rtps.builtin.domainId = input_domain;
		participant_subscriber_params.rtps.builtin.leaseDuration = c_TimeInfinite;
		participant_subscriber_params.rtps.setName(input_participant_name);

		// Publisher configuration
		PublisherAttributes publisher_params;
		publisher_params.historyMemoryPolicy = DYNAMIC_RESERVE_MEMORY_MODE;
		publisher_params.topic.topicDataType = output_type_name;
		publisher_params.topic.topicName = output_topic_name;

		// Subscriber configuration
		SubscriberAttributes subscriber_params;
		subscriber_params.historyMemoryPolicy = DYNAMIC_RESERVE_MEMORY_MODE;
		subscriber_params.topic.topicKind = NO_KEY;
		subscriber_params.topic.topicDataType = input_type_name;
		subscriber_params.topic.topicName = input_topic_name;

		bridge.push_back(new DummyPubSub(participant_publisher_params,
										participant_subscriber_params,
										publisher_params,
										subscriber_params,
										function_path));

		std::cout << "New bridge between [" << input_topic_name << "] and [" << output_topic_name << "]" << std::endl;
	}
}
