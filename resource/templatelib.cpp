#include <iostream>
//#include "TypeAPubSubTypes.h"
//#include "TypeBPubSubTypes.h"

#if defined(_WIN32) && defined (BUILD_SHARED_LIBS)
	#if defined (_MSC_VER)
		#pragma warning(disable: 4251)
	#endif
  #if defined(testlib_EXPORTS)
  	#define  USER_LIB_EXPORT __declspec(dllexport)
  #else
    #define  USER_LIB_EXPORT __declspec(dllimport)
  #endif
#else
  #define USER_LIB_EXPORT
#endif

extern "C" void USER_LIB_EXPORT transform(SerializedPayload_t *serialized_input, SerializedPayload_t *serialized_output){
	// Types definition
	/*
	TypeA input_data;
	TypeAPubSubType input_pst;
	TypeB output_data;
	TypeBPubSubType output_pst;
	*/

	// Deserialization
	/*
	imu_pst.deserialize(serialized_input, &input_data);
	*/

	// Custom routing
	/*
	output_data = 0;
	*/

	// Serialization
	/*
	serialized_output->reserve(output_pst.m_typeSize);
	output_pst.serialize(&output_data, serialized_output);
	*/
}
