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

RSManager::RSManager(std::string xml_file_path) : active(false){

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError error;
	doc.LoadFile(xml_file_path.c_str());
	if(doc.Error()){
		cout << "error en xml";
	}

	tinyxml2::XMLElement *bridge_element = doc.FirstChildElement("firos");
	if(!bridge_element){
		//throw 1;
	}

	for (auto child = bridge_element->FirstChildElement(); child != NULL; child = child->NextSiblingElement()){
		try{
			tinyxml2::XMLElement *subscriber_element = child->FirstChildElement("subscriber");
			tinyxml2::XMLElement *publisher_element = child->FirstChildElement("publisher");
			if(!subscriber_element || !subscriber_element)
				throw 0;

			tinyxml2::XMLElement *current_element;
			current_element = assignCurrentElement(subscriber_element, "participant");
			const char* input_participant_name = current_element->GetText();
			current_element = assignCurrentElement(subscriber_element, "topic");
			const char* input_topic_name = current_element->GetText();
			current_element = assignCurrentElement(subscriber_element, "type");
			const char* input_type_name = current_element->GetText();
			current_element = assignCurrentElement(publisher_element, "participant");
			const char* output_participant_name = current_element->GetText();
			current_element = assignCurrentElement(publisher_element, "topic");
			const char* output_topic_name = current_element->GetText();
			current_element = assignCurrentElement(publisher_element, "type");
			const char* output_type_name = current_element->GetText();

			int input_domain = 0;
			current_element = assignCurrentElement(subscriber_element, "domain");
			if(current_element->QueryIntText(&input_domain))
				throw 0;

			int output_domain = 0;
			current_element = assignCurrentElement(publisher_element, "domain");
			if(current_element->QueryIntText(&output_domain))
				throw 0;

			const char* function_path;
			if (child->FirstChildElement("transformation")){
				function_path = child->FirstChildElement("transformation")->GetText();
			}
			else{
				function_path = nullptr;
			}

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

			bridge.emplace_back(participant_publisher_params,
											participant_subscriber_params,
											publisher_params,
											subscriber_params,
											function_path);
			active = true;

			std::cout << "Created bridge between [" << input_topic_name << "] and [" << output_topic_name << "]" << std::endl;
		}
		catch (int e_code){
				std::cout << "Invalid configuration, skipping bridge" << std::endl;
		}
	}
}

tinyxml2::XMLElement* RSManager::assignCurrentElement(tinyxml2::XMLElement *element, std::string name){
	if (!element->FirstChildElement(name.c_str())){
		throw 0;
	}
	return element->FirstChildElement(name.c_str());
}

bool RSManager::isActive(){
	return active;
}
