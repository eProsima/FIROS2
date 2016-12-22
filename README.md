#Firos2

Firos2 is a ROS2 integrable tool focused in the intercommunication between ROS2 and Fast RTPS enviroments.

With Firos2 the user can create different parametric communication bridges between ROS2 and Fast RTPS performing transformation over the messages such as customized routing, mapping between input and output attributes or data modification.

<p align="center">
<img src="doc/images/firos2_schema.png"> 
</p>

##Installation on ROS2 workspace

Before using Firos2, it has to be installed along with the rest of ROS2 packages of your system. If you have followed the ROS2 installation manual provided [here](https://github.com/ros2/ros2/wiki/Installation) you only need to clone this repository on your ROS2 workspace. In Linux, these are the steps:

```
$ cd ~/ros2_ws/src/ros2/
$ git clone https://github.com/eProsima/firos2
```

In the case of Windows:

```
> cd C:\\dev\ros2\src\ros2
> git clone https://github.com/eProsima/firos2
```

Once this is done, it is needed to compile the package like any other ROS2 package. For example:

```
$ ament build --only-package firos2
```

##User parametrization

For setting-up a bridge, the user has to define a basic configuration with the information about input and output topics. A generic template can be found on [config.xml](resource/config.xml)

In this file is it possible to set different bridges between different topics. If the communication goes from two topics which are using the same type, it is possible to perform an automatic mapping. For using it, the [transformation] field has to be empty. But, in case the bridge goes from topics with different types, the user has to provide the needed functions to map or transform the message.

##Transformation and mapping.
As said before, when a bridge is connecting two topics with diferent types, the user has to provide a function to transform and/or map the attributes from one type to another. To make this step easier, there is an empty code template in [templatelib.cpp](resource/templatelib.cpp).

For a better understanding about how to set-up Firos2, in the [examples](examples/helloworld) directory is presented a step by step working example of a Firos2 bridge communicating the hello world programs provided by ROS2 and Fast RTPS.
