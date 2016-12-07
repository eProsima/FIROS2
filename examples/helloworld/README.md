#Making a hello world example

For this example we are going to use three elements:
* Firos2 with an example configuration and transformation.
* ROS2 [Talker/Listener example](www.google.com).
* Fast [RTPS HelloWorld example](www.google.com).

The goal will be to make a bridge from the Fast RTPS HelloWorld publisher and the "Listener" node of ROS2.

##Types generation

First of all we are going to check which IDL is using each node. In the case of Fast RTPS HelloWorld the IDL of the example is located on the main directory. But in the case of ROS2, we have to take the IDL file from the generated files that ROS2 creates at compilation time. For this particular example, we can find a compatible IDL named String_.idl in the install directory. These two idl has been copied to the [idl](www.google.com) directory.

Now, on the [examples/helloworld](www.google.com) directory of Firos2 and using these two IDLs and the tool fastrtpsgen we are going to generate the needed source files that contains the definition of the described types in the IDL file:

``` bash
$ fastrtpsgen idl/HelloWorld.idl
$ fastrtpsgen idl/String_.idl
```

Inside the same directory, we are going to use the provided template for the *CMakeLists.txt* and *config.xml* files located on [resource/user_functions](www.google.com). In this point, our directory looks like it is showed on the following graph.

``` bash
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

##Transformation and mapping definition

Once it is done, we are going to fill the conversion functions with our application specific code. In this example, the HelloWorld publisher has an IDL which contains a string attribute: *message*, and an unsigned long attribute: *index*. In each cycle, the Fast RTPS application publishes the text "HelloWorld" in *message* and an ascending value on *index*.

On the ROS2 side, *Listener* is working with an IDL which only has an string attribute: *data_*. In this example we are going to define a conversion for putting the text of *message* and the number of *index* in an only one string stored on *data_*. This transformation replicates the original behaviour of the Talker/Listener example, where Talker publishes a string "Hello World: X", where X takes its value from an ascending counter defined on the ROS2 node.

Our transformation function code, which is located on [userlib.cpp](www.google.com), will be defined like this:

``` cpp
extern "C" void USER_LIB_EXPORT transform(SerializedPayload_t *serialized_input, SerializedPayload_t *serialized_output){
	// User types
	std_msgs::msg::dds_::String_ string_data;
	std_msgs::msg::dds_::String_PubSubType string_pst;
	HelloWorld helloworld_data;
	HelloWorldPubSubType helloworld_pst;

	// Deserialization
	helloworld_pst.deserialize(serialized_input, &helloworld_data);

	// Custom transformation
	string_data.data_(helloworld_data.message() + ": " + std::to_string(helloworld_data.index()));
	std::cout << string_data.data_() << std::endl;

	// Serialization
	serialized_output->reserve(string_pst.m_typeSize);
	string_pst.serialize(&string_data, serialized_output);
}
```

Now, we have to compile the library using these commands on Linux:

``` bash
$ mkdir build && cd build
$ cmake ..
$ make
```

Or these other on Windows:

``` bash
> mkdir build && cd build
> cmake .. -G "Visual Studio 14 2015 Win64"
> cmake --build . --config Release
```


##Publisher/subscriber parameters
The next step is to set the [config.xml](www.google.com) file with the specific parameters of our bridge:

``` xml
<bridge>
	<subscriber>
		<participant>fastrtps_subscriber</participant>
		<domain>80</domain>
		<topic>HelloWorldTopic</topic>
		<type>HelloWorld</type>
	</subscriber>
	<publisher>
		<participant>ros2_publisher</participant>
		<domain>0</domain>
		<topic>chatter</topic>
		<type>std_msgs::msg::dds_::String_</type>
	</publisher>
	<transformation>/path/to/compiled/library</transformation>
</bridge>
```

Publisher and subscriber labels are referenced to Firos2, which means that the subscriber is going to receive data from the Fast RTPS publisher and the publisher is going to send data to the ROS2 subscriber.

Now, we only have to launch the two examples and the Firos2 node from ROS2. If this instructions has been succesfully followed we will see this output on the Listener side.

``` bash
I heard: [HelloWorld: 1]
I heard: [HelloWorld: 2]
I heard: [HelloWorld: 3]
I heard: [HelloWorld: 4]
I heard: [HelloWorld: 5]
I heard: [HelloWorld: 6]
I heard: [HelloWorld: 7]
I heard: [HelloWorld: 8]
I heard: [HelloWorld: 9]
I heard: [HelloWorld: 10]
```
