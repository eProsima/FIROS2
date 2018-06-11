Making a hello world example
============================

For this example we are going to use three elements:

- *Firos2* with an example configuration and transformation.
- *ROS2* `Talker/Listener example <https://github.com/ros2/ros2/wiki/Linux-Development-Setup#try-some-examples>`_.
- *Fast RTPS* `HelloWorld example <https://github.com/eProsima/Fast-RTPS/tree/master/examples/C%2B%2B/HelloWorldExample>`_.

The goal will be to make a bridge from the *Fast RTPS* HelloWorld publisher and the *Listener* node of *ROS2*.

Types generation
----------------

First of all we are going to check which IDL is using each node. In the case of *Fast RTPS* HelloWorld the *idl* of the example is located on the main directory. But in the case of *ROS2*, we have to take the *idl* file from the generated files that *ROS2* creates at compilation time (if you need more information about *idl* files and its generation from *ROS2* *msg* files, you can find it `here <https://github.com/JavierIH/armageddon/tree/firos/doc#types-and-interfaces>`_. For this particular example, we can find a compatible *idl* named *String\_.idl* in the install directory. These two IDLs has been copied to the `idl <idl>`_ directory.

Now using these two *idl* files and the tool `fastrtpsgen <http://eprosima-fast-rtps.readthedocs.io/en/latest/geninfo.html>`_ we are going to generate the needed source files that contains the definition of the described types in the IDL:

.. code-block:: shell

    $ fastrtpsgen idl/HelloWorld.idl
    $ fastrtpsgen idl/String_.idl


From the `resource <../../resource>`_ directory, we are going to use the provided template for the *CMakeLists.txt* and *config.xml* files. In this point, our directory looks like it is showed on the following graph.

.. code-block:: shell

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


Transformation and mapping definition
-------------------------------------

Once it is done, we are going to fill the conversion functions with our application specific code. In this example, the HelloWorld publisher has an IDL which contains a string attribute: *message*, and an unsigned long attribute: *index*. In each cycle, the *Fast RTPS* application publishes the text "HelloWorld" in *message* and an ascending value on *index*.

On the *ROS2* side, *Listener* is working with an IDL which only has an string attribute: *data\_*. In this example we are going to define a conversion for putting the text of *message* and the number of *index* in an only one string stored on *data\_*. This transformation replicates the original behaviour of the Talker/Listener example, in which Talker publishes a string "Hello World: X", where X takes its value from an ascending counter defined on the *ROS2* node.

Our transformation function code, which is located on *userlib.cpp*, will be defined like this:

.. code-block:: c

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


Now, we have to compile the library using these commands on Linux:

.. code-block:: shell

    $ mkdir build && cd build
    $ cmake ..
    $ make


Or these other on Windows:

.. code-block:: shell

    > mkdir build && cd build
    > cmake .. -G "Visual Studio 14 2015 Win64"
    > cmake --build . --config Release



Publisher/subscriber parameters
-------------------------------

The next step is to set the *config.xml* file with the specific parameters of our bridge:

.. code-block:: xml

    <is>
        <topic_types>
            <type name="HelloWorld">
                <participants>
                    <participant name="rtps"/>
                </participants>
            </type>
            <type name="std_msgs::msg::dds_::String_">
                <participants>
                    <participant name="ros2"/>
                </participants>
            </type>
        </topic_types>

        <profiles>
            <participant profile_name="rtps">
                <rtps>
                    <builtin>
                        <domainId>0</domainId>
                    </builtin>
                </rtps>
            </participant>

            <participant profile_name="ros2">
                <rtps>
                    <builtin>
                        <domainId>0</domainId>
                    </builtin>
                </rtps>
            </participant>

            <subscriber profile_name="fastrtps_subscriber">
                <topic>
                    <name>HelloWorldTopic</name>
                    <dataType>HelloWorld</dataType>
                </topic>
                <historyMemoryPolicy>DYNAMIC</historyMemoryPolicy>
            </subscriber>

            <publisher profile_name="ros2_publisher">
                <topic>
                    <name>chatter</name>
                    <dataType>std_msgs::msg::dds_::String_</dataType>
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

        <connector name="domain_change"> 
            <subscriber participant_name="rtps" subscriber_name="fastrtps_subscriber"/>
            <publisher participant_name="ros2" publisher_name="ros2_publisher"/>
            <transformation file="/path/to/compiled/library/libuserlib.so" function="transform"/>
        </connector>
    </is>



Publisher and subscriber labels are referenced to *Firos2*, which means that the subscriber is going to receive data from the *Fast RTPS* publisher and the publisher is going to send data to the *ROS2* subscriber.

Now, we only have to launch the two examples and the *Firos2* node from *ROS2*. If this instructions has been succesfully followed we will see this output on the Listener side.

.. code-block:: shell

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
