#include <iostream>
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

ISBridgeDummy* loadDummyBridge(void *config);

// TODO ISBridge must be now an interface (or abstract class) that all Bridges must implement.
extern "C" ISBridge* USER_LIB_EXPORT createBridge(const char* config)
{
    // Add Bridge constructor and...
    //return new ISBridgeDummy(config);

    // OR parse the config xml here and return a cleaner Bridge class!
    //return loadDummyBridge(config);
}

// TODO parse the xml and return a configured RSBRidgeDummy
ISBridgeDummy* loadDummyBridge(void *config)
{
    ISBridgeDummyConfig *dummyConfig = (ISBridgeDummyConfig*)config;
    try
    {
        Element *nodeA_element = dummyConfig->FirstChildElement("nodeA");
        if (!nodeA_element)
        {
            throw 0;
        }
        Element *nodeB_element = dummyConfig->FirstChildElement("nodeB");
        if(!nodeB_element)
        {
            throw 0;
        }

        // TODO Parse configuration for each node

        // TODO Load transformation library function_path
        const char* function_path = dummyConfig->FirstChildElement("transformation")->GetText();

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

