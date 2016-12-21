#Generation of Fast RTPS IDLs from ROS2 messages.

For making easier the creation of types interfaces used by the communication bridges, Firos2 includes an IDL generator based on *rosidl_generator_dds_idl* package. ROS2 messages definitions *msg* are slightly different from Fast RTPS *idl* types, but since ROS2 is running over Fast RTPS, for each *msg* file exists an equivalent *idl* file.

<p align="center">
<img src="https://github.com/JavierIH/armageddon/blob/firos/doc/images/firos2_idl.png">
</p>


To get a deeper comprehension about the relation between ROS2 and Fast RTPS IDL definitions, you can see [this article](http://design.ros2.org/articles/mapping_dds_types.html). The Firos2 IDL generation feature allows two different options:

- Generate a Fast RTPS compatible *idl* file for an specific package. This can be achieved just adding Firos2 in the package CMakeLists as a dependency.

<p align="center">
<img src="https://github.com/JavierIH/armageddon/blob/firos/doc/images/idl_specific.png"> 
</p>

- Generate Fast RTPS compatible *idl* files by default for all the packages defined in the ROS2 workspace. To make this possible Firos2 must be added to the rosidl_default_generators list.

<p align="center">
<img src="https://github.com/JavierIH/armageddon/blob/firos/doc/images/idl_default.png"> 
</p>

The *idl* files will be created inside the workspace build directory at compilation time. 
