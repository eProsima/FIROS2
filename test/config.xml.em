@{
import sys, os
transformation_path = os.environ.get("TEST_TRANSFORMATION_PATH")
}@

<?xml version="1.0"?>
<rs>
	<bridge>
		<subscriber>
			<participant>ros2_subscriber</participant>
			<domain>0</domain>
			<topic>test_ros2_topic</topic>
			<type>std_msgs::msg::dds_::String_</type>
		</subscriber>
		<publisher>
			<participant>fastrtps_publisher</participant>
			<domain>0</domain>
			<topic>test_fastrtps_topic</topic>
			<type>Test</type>
		</publisher>
		<transformation>@(transformation_path)</transformation>
	</bridge>
</rs>
