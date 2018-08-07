# Robot data example

For this example we are going to setup a FIROS2 Bridge to be used to communicate some data of a theorical robot between ROS2 and FIWARE Orion contextBroker (NGSIv2 protocol) using Dynamic Types.

### Prerequisites:

- Installed and working ROS2 with Fast-RTPS and FIROS2.

### Bridge library:

In this example, we will use the builtin library *isdynbridgengsiv2lib* (Dynamic NGSIv2 library), included with the installation of FIROS2.
This library is the dynamic type version of *isbridgengsiv2lib*.

### IDL files:

This example only uses static data types for RobotExample executable. This is intended to show the intercommunication
between participants using different kind of compatible types, in this case the same structure, in one side defined as
static, and in the other side as dynamic.

### Dynamic Types:

FIROS2 includes a DynJsonPubSubType.hpp, which will be used by the builtin Dynamic NGSIv2 library. This header file is very simple, and provides a DynamicPubSubType with two strings, *entityId* and *data*.

- *entityId* is intended to be filled with the entityId that will use contextBroker to identify the entity (the robot), by the **transformation library** (explained later) which will convert ros2 data to ngsiv2 protocol.

- *data* will contain both raw JSON from contextBroker and attributes to be update to contextBroker.

To sending data to the robot we will use RobotSnd type from the Type Library included in the transformation library file.
To receive data from the robot we will use RobotRcv instead, again from the Type Library.
Both are very similar, and include RobotPosition.
The only difference in this typess is that RobotRcv has "state" and "destination" fields, and RobotSnd has "position" field.

- *robot_id*: Identifier of the robot (must be the same in ROS2 and  contextBroker.

- *transmission_time*: Timestamp of the message.

- *destination*: (Only in RobotRcv.idl) Where the robot is going. This is a RobotPosition type.

- *state*: (Only in RobotRcv.idl) State of the robot. Can be **ACTION** or **STAND_BY**.

- *position*: (Only in RobotSnd.idl) Current robot position. This is a RobotPosition type.

RobotPosition is composed by:

- *floor*: Current floor of the robot.
- *x*: X coordinate of the robot.
- *y*: Y coordinate of the robot.
- *zeta*: Orientation of the robot.

All this types are generated dynamically on demand by *userlib*.

### Transformation library:

Following the example in templatelib.cpp, we have already created a library to transform in both ways (only transformation functions are shown in order to clarity).

	extern "C" void USER_LIB_EXPORT transformFromNGSIv2(DynamicData *input, DynamicData *output)
	{
		// Custom transformation
		std::stringstream ss(input->GetStringValue(input->GetMemberIdByName("data")));
		Json::Value root;
		Json::CharReaderBuilder jsonReader;
		std::string errs;
		if (Json::parseFromStream(jsonReader, ss, &root, &errs))
		{
			Json::Value data = root["data"];
			Json::Value robot = data[0];
			output->SetStringValue(robot["id"].asString(), output->GetMemberIdByName("id"));
			LOG_INFO("Parsing robot with ID = " << output->GetStringValue(output->GetMemberIdByName("id")));
			output->SetStringValue(robot["transmission_time"]["value"].asString(),
										output->GetMemberIdByName("transmission_time"));
			DynamicData *position = output->LoanValue(output->GetMemberIdByName("destination"));
			position->SetUint32Value(robot["floor"]["value"].asLargestUInt(), position->GetMemberIdByName("floor"));
			position->SetUint32Value(robot["x"]["value"].asLargestUInt(), position->GetMemberIdByName("x"));
			position->SetUint32Value(robot["y"]["value"].asLargestUInt(), position->GetMemberIdByName("y"));
			position->SetUint32Value(robot["zeta"]["value"].asLargestUInt(), position->GetMemberIdByName("zeta"));
			output->ReturnLoanedValue(position);
		}
		else
		{
			LOG_ERROR("Failed to parse" << errs);
		}
	}

	extern "C" void USER_LIB_EXPORT transform(DynamicData *input, DynamicData *output)
	{
		// Custom transformation
		std::stringstream ss;
		ss << "{\"transmission_time\": { \"value\": \""
		<< input->GetStringValue(input->GetMemberIdByName("transmission_time")) << "\"}, ";

		DynamicData* destination = input->LoanValue(input->GetMemberIdByName("destination"));
		ss << "\"floor\": {\"value\": "
			<< destination->GetUint32Value(destination->GetMemberIdByName("floor")) << "}, ";
		ss << "\"x\": {\"value\": " << destination->GetUint32Value(destination->GetMemberIdByName("x")) << "}, ";
		ss << "\"y\": {\"value\": " << destination->GetUint32Value(destination->GetMemberIdByName("y")) << "}, ";
		ss << "\"zeta\": {\"value\": "
			<< destination->GetUint32Value(destination->GetMemberIdByName("zeta")) << "},";
		input->ReturnLoanedValue(destination);

		ss << "\"state\": {\"value\": \"" << input->GetEnumValue(input->GetMemberIdByName("state")).c_str() << "\"} }";

		output->SetStringValue(input->GetStringValue(input->GetMemberIdByName("id")),
							output->GetMemberIdByName("entityId"));
		output->SetStringValue(ss.str(), output->GetMemberIdByName("data"));
		LOG_INFO(output->GetStringValue(output->GetMemberIdByName("data")));
	}

The logic is very simple. The function **transformFromNGSIv2** receives the JSON data from NGSIv2.
Uses [JsonCpp library](http://jsoncpp.sourceforge.net/) to parse this data, and fills RobotSnd structure with the interesting data.

NOTE: We only parse the first robot as in this example only one robot will change at a time, but with minimal changes a collection of robots will be able to be parsed.

The other function **transform** receives the data in a RobotRcv structure and creates a JSON with the data of the robot being parsed. Then fills *entityId* with the received robot identifier.

These function names are not casual. As explained before, we are going to share the transformation library between both subscribers.

### Config.xml

The *config.xml* file used in this example is the following:

	<is>
		<topic_types>
			<type name="RobotSnd">
				<participants>
					<participant name="ros2"/>
				</participants>
				<library>/path/to/compiled/library/libuserlib.so</library>
			</type>
			<type name="RobotRcv">
				<participants>
					<participant name="ros2"/>
				</participants>
				<library>/path/to/compiled/library/libuserlib.so</library>
			</type>
		</topic_types>

		<profiles>

			<participant profile_name="ros2">
				<rtps>
					<builtin>
						<domainId>0</domainId>
					</builtin>
				</rtps>
			</participant>

			<subscriber profile_name="ros2_subscriber">
				<topic>
					<name>RobotTopic</name>
					<dataType>RobotSnd</dataType>
				</topic>
				<qos>
					<partition>
						<names>
							<name>rt</name>
						</names>
					</partition>
				</qos>
				<historyMemoryPolicy>DYNAMIC</historyMemoryPolicy>
			</subscriber>

			<publisher profile_name="ros2_publisher">
				<topic>
					<name>RobotTopic</name>
					<dataType>RobotSnd</dataType>
				</topic>
				<qos>
					<partition>
						<names>
							<name>rt</name>
						</names>
					</partition>
				</qos>
				<historyMemoryPolicy>DYNAMIC</historyMemoryPolicy>
			</publisher>
		</profiles>

        <bridge name="ngsiv2">
            <library>libisbridgengsiv2lib.so</library> <!-- Path to the NGSIv2 library -->

            <subscriber name="ngsiv2_subscriber">
                <property>
                    <name>host</name>
                    <value>localhost</value>
                </property>
                <property>
                    <name>port</name>
                    <value>1026</value>
                </property>
                <property>
                    <name>id</name>
                    <value>.*</value>
                </property>
                <property>
                    <name>type</name>
                    <value>Robot</value>
                </property>
                <property>
                    <name>notifs</name>
                    <value>id,transmission_time,floor,x,y,zeta</value>
                </property>
                <property>
                    <name>listener_host</name>
                    <value>localhost</value>
                </property>
                <property>
                    <name>listener_port</name>
                    <value>12345</value>
                </property>
            </subscriber>

            <publisher name="ngsiv2_publisher">
                <property>
                    <name>host</name>
                    <value>localhost</value>
                </property>
                <property>
                    <name>port</name>
                    <value>1026</value>
                </property>
            </publisher>
        </bridge>

        <connector name="ros2_ngsiv2">
            <subscriber participant_name="ros2" subscriber_name="ros2_subscriber"/>
            <publisher participant_name="ngsiv2" publisher_name="ngsiv2_publisher"/>
            <transformation file="/path/to/compiled/library/libuserlib.so" function="transform"/>
        </connector>

        <connector name="ngsiv2_ros2">
            <subscriber participant_name="ngsiv2" subscriber_name="ngsiv2_subscriber"/>
            <publisher participant_name="ros2" publisher_name="ros2_publisher"/>
            <transformation file="/path/to/compiled/library/libuserlib.so" function="transformFromNGSIv2"/>
        </connector>
    </is>

Our ROS2 Topic is RobotTopic in the partition "rt" and with domain 0.
As type we declare RobotSnd, as is more general than RobotRcv. The ideal case is to use exactly the same type, but compatible types like this case are ok.
NGSIv2 will be configured to connect with a contextBroker server at localhost in port 1026 and will subscribe to any "Robot" change.
We only want to received *id*, *transmission_time*, *floor*, *x*, *y* and *zeta* attributes.
Finally we will listen to these changes at localhost in port 12345 (internally *libdynisbridgengsiv2lib* library launches the server to listen at the address specified in the subscription field).

As you can see, we are reusing the same "/path/to/compiled/library/libuserlib.so" for transformation for both RTPS and NGSIv2 subscribers and data types.
This is possible because RTPS will look for a function named "transform", and NGSIv2 for a function named "transformFromNGSIv2" and both are included in that library.
In type cases, FIROS will look for "GetTopicType" function and will call it with the name of de desired topic type.

### Run the example

First, we need to compile the example transformation library (remember to have already installed and working ROS2, with Fast-RTPS and FIROS2).
Let's suppose our ROS2 path is **~/ros2**

	$ cd ~/ros2
	$ . ./install/local_setup.bash
	$ cd src/ros2/firos2/examples/TIS_NGSIv2
	$ mkdir build && cd build
	$ cmake ..
	$ make

In Windows:

	> cd C:\dev\ros2
	> call install\local_setup.bat
	> cd src\ros2\firos2\examples\TIS_NGSIv2
    > mkdir build && cd build
    > cmake .. -G "Visual Studio 14 2015 Win64"
    > cmake --build . --config Release
    > cd Release

Three auxiliary utilities are compiled:

**cb_create_robot**: Allows you to create a robot with the expected structure in the Orion contextBroker.
The following example will create a robot with ID *Robot_001*.

	./cb_create_robot localhost:1026 Robot_001

Create at least, Robot_001, as in this test ROS2 simulator will modify that robot.

**cb_update_attr**: Allows you to modify an attribute value of a robot.
The following example will modify *floor* attribute of robot *Robot_001* setting its value to 5.

	./cb_update_attr localhost:1026 Robot_001 floor n 5

**RobotExample**: Is a simple application similar to HelloWorld. It allows two possible parameters.
- *publisher*: Launches *RobotExample* as a publisher. It will update the floor of Robot_001 each 10 seconds, up to 5 in a loop.
- *subscriber*: Launches *RobotExample* as a subscriber, showing the floor received for any robot.

Finally, we are in condition to launch the test.
Let's suppose again that our ROS2 path is **~/ros2**

	$ cd ~/ros2
	$ . ./install/local_setup.bash
	$ cd src/ros2/firos2/examples/Dyn_TIS_NGSIv2
	$ firos2 config.xml

Now, you can create or modify any robot you has in your contextBroker with *cb_create_robot* and *cb_update_attr*, and will see how the firos2 received and transmits the data.
Using *RobotExample* program you can check and modify the ROS2 side and observe how the changes propagate to contextBroker.

