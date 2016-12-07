##Making a HelloWorld example
For this example we are going to use two elements:
	The ROS2 Talker/Listener example
	The Fast RTPS HelloWorld example

The target will be to make a bridge from Fast RTPS HelloWorld publisher and the node "Listener" of ROS2.

First of all we are going to check which IDL is using each node. In the case of Fast RTPS HelloWorld the IDL of the example is located on the main directory. But in the case of ROS2, we have to take the IDL file from the generated files that ROS2 creates at compilation time. For this particular example, we can find a compatible IDL named String_.idl in the install directory.

Now, with this two IDLs and the tool fastrtpsgen, we are going to generate the needed source files that contains the definition of the described types in the IDL file:

$ fastrtpsgen HelloWorld.idl
$ fastrtpsgen String_.idl

Inside the same directory, we are going to copy the provided template located on resource/user_functions. In this point, our directory looks like it is showed on the following graph.

```bash
   examples
   └── helloworld
	   ├── CMakeLists.txt
	   ├── customlib.cpp
	   ├── HelloWorld.cxx
	   ├── HelloWorld.h
	   ├── HelloWorld.idl
	   ├── HelloWorldPubSubTypes.cxx
	   ├── HelloWorldPubSubTypes.h
	   ├── String_.cxx
	   ├── String_.h
	   ├── String_.idl
	   ├── String_PubSubTypes.cxx
	   └── String_PubSubTypes.h
```

In the next step, we are going to fill the conversion functions with our application specific code. In this example, the HelloWorld publisher has an IDL which contains a string attribute: message, and an unsigned long attribute: index. In each cycle, the Fast RTPS application publishes the text "HelloWorld" in message and an ascending value on index.

On the ROS2 side, Listener is working with an IDL which only has an string attribute: data_. In this example we are going to define a conversion for putting the text of message and the number of index in an only one string stored on data_. This transformation replicates the original behaviour of the Talker/Listener example, where Talker publishes an string "Hello World: X", where X takes its value from an ascending counter defined on the ROS2 node.

Our tranformation code, which is located on userlib.cpp will be defined like this:

```
extern "C" void USER_LIB_EXPORT transform(SerializedPayload_t *serialized_input, SerializedPayload_t *serialized_output){
	// User types
	sensor_msgs::msg::dds_::String_ string_data;
	sensor_msgs::msg::dds_::String_PubSubType string_pst;
	HelloWorld helloworld_data;
	HelloWorldPubSubType helloworld_pst;

	// Deserialization
	helloworld_pst.deserialize(serialized_input, &helloworld_data);

	// Custom transformation
	string_data.data(helloworld_data.message() + ": " + std::to_string(helloworld_data.index()));

	// Serialization
	serialized_output->reserve(string_pst.m_typeSize);
	string_pst.serialize(&string_data, serialized_output);
}
```
