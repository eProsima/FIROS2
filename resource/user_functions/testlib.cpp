#include <iostream>
#include "Imu_PubSubTypes.h"
#include "OutputPubSubTypes.h"

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

extern "C" void __declspec(dllexport) transform(SerializedPayload_t *serialized_input, SerializedPayload_t *serialized_output){
	sensor_msgs::msg::dds_::Imu_ imu_data;
	sensor_msgs::msg::dds_::Imu_PubSubType imu_pst;
	Output output_data;
	OutputPubSubType output_pst;
	// Deserialization
	imu_pst.deserialize(serialized_input, &imu_data);
	// Custom routing
	output_data.time(
		imu_data.header_().sec() +
		imu_data.header_().nanosec()/1000.0
	);
	output_data.rotation(-75.64);
	// Serialization
	serialized_output->reserve(output_pst.m_typeSize);
	output_pst.serialize(&output_data, serialized_output);
}
