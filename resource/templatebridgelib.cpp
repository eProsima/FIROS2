#include <iostream>
#include <tinyxml2.h>
#include "ISBridgeDummy.h"

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

ISBridgeDummy* loadDummyBridge(tinyxml2::XMLElement *bridge_element);

// TODO ISBridge must be now an interface (or abstract class) that all Bridges must implement.
extern "C" ISBridge* USER_LIB_EXPORT createBridge(tinyxml2::XMLElement *bridge_element)
{
    // Add Bridge constructor and...
    //return new ISBridgeDummy(bridge_element);

    // OR parse the config xml here and return a cleaner Bridge class!
    //return loadDummyBridge(bridge_element);
}

// TODO parse the xml and return a configured RSBRidgeDummy
ISBridgeDummy* loadDummyBridge(tinyxml2::XMLElement *bridge_element)
{
    try
    {
        tinyxml2::XMLElement *nodeA_element = bridge_element->FirstChildElement("nodeA");
        if (!nodeA_element)
        {
            throw 0;
        }
        tinyxml2::XMLElement *nodeB_element = bridge_element->FirstChildElement("nodeB");
        if(!nodeB_element)
        {
            throw 0;
        }

        // TODO Parse configuration for each node

        // TODO Load transformation library function_path
        const char* function_path = bridge_element->FirstChildElement("transformation")->GetText();

        // TODO NodeA configuration
        DummyNodeAAttributes participant_nodeA_params;
        participant_nodeA_params.dummyProperty = parsed_value_a;
        // [...]

        // TODO NodeB configuration
        DummyNodeBAttributes participant_nodeB_params;
        participant_nodeB_params.dummyProperty = parsed_value_b;
        // [...]

        ISBridgeDummy *bridge = new ISBridgeDummy(
                                    participant_nodeA_params,
                                    participant_nodeB_params,
                                    function_path);

        return bridge;
    }
    catch (int e_code){
        std::cout << "Invalid configuration, skipping bridge " << e_code << std::endl;
    }
}

