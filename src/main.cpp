#include <iostream>
#include <memory>

#include "fastrtps/Domain.h"
#include "fastrtps/participant/Participant.h"
#include "fastrtps/attributes/ParticipantAttributes.h"
#include "fastrtps/publisher/Publisher.h"
#include "fastrtps/publisher/PublisherListener.h"
#include "fastrtps/attributes/PublisherAttributes.h"
#include "fastrtps/subscriber/Subscriber.h"
#include "fastrtps/subscriber/SubscriberListener.h"
#include "fastrtps/subscriber/SampleInfo.h"
#include "fastrtps/attributes/SubscriberAttributes.h"

#include "../include/routing_service/DummyPubSubTypes.h"
#include "../include/routing_service/DummyPubSub.h"
#include "../include/tinyxml2/tinyxml2.h"


int main(int argc, char * argv[]){

	if (!(argc > 1)){
		std::cout << "nope" << std::endl;
		return 0;
	}
	std::string path_to_config = argv[1];

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError error;
    if(doc.LoadFile(path_to_config.c_str())){
        std::cout << "Error: " << error << std::endl;
        return 0;
    }

	std::vector<DummyPubSub*> firos;

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

		firos.push_back(new DummyPubSub(participant_publisher_params,
										participant_subscriber_params,
										publisher_params,
										subscriber_params,
										function_path));

		std::cout << "New bridge between [" << input_topic_name << "] and [" << output_topic_name << "]" << std::endl;
	}

	std::cout << "\n### Firos2 is running, press any key for quit ###" << std::endl;
	fflush(stdout);
	std::cin.ignore();
	std::cout << "closing..." << std::endl;
	return 0;
}
