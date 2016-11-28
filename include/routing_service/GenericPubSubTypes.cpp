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

#include <fastcdr/FastBuffer.h>
#include <fastcdr/Cdr.h>

#include "GenericPubSubTypes.h"

GenericPubSubType::GenericPubSubType() {
	m_typeSize = 1; //(uint32_t)Dummy::getMaxCdrSerializedSize(); // <----- SerializedPayload_t max_size
	m_keyBuffer = (unsigned char*)malloc(16); //(Dummy::getKeyMaxCdrSerializedSize()>16 ? Dummy::getKeyMaxCdrSerializedSize() : 16);
}

GenericPubSubType::~GenericPubSubType() {
	if(m_keyBuffer!=nullptr)
		free(m_keyBuffer);
}

bool GenericPubSubType::serialize(void *data, SerializedPayload_t *payload) {
	SerializedPayload_t* p_type = (SerializedPayload_t*) data;
	payload->copy(p_type, false);
	return true;
}

bool GenericPubSubType::deserialize(SerializedPayload_t* payload, void* data) {
	SerializedPayload_t* p_type = (SerializedPayload_t*) data;
	p_type->copy(payload, false);
	return true;
}

std::function<uint32_t()> GenericPubSubType::getSerializedSizeProvider(void* data) {
	return [data]() -> uint32_t {
		return 1;
	};
}
