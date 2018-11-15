#include <iostream>
#include "NGSIv2ISBridge.h"
#include "NGSIv2Publisher.h"
#include "NGSIv2Subscriber.h"
#include "log/ISLog.h"

#if defined(_WIN32) && defined (BUILD_SHARED_LIBS)
	#if defined (_MSC_VER)
		#pragma warning(disable: 4251)
	#endif
  #if defined(isbridgengsiv2lib_EXPORTS)
  	#define  USER_LIB_EXPORT __declspec(dllexport)
  #else
    #define  USER_LIB_EXPORT __declspec(dllimport)
  #endif
#else
  #define USER_LIB_EXPORT
#endif

NGSIv2ISBridge* loadNGSIv2Bridge(const char* name);
NGSIv2Subscriber* loadNGSIv2Subscriber(const char* name, const std::vector<std::pair<std::string, std::string>> *config);
NGSIv2Publisher* loadNGSIv2Publisher(const char* name, const std::vector<std::pair<std::string, std::string>> *config);

extern "C" USER_LIB_EXPORT ISBridge* create_bridge(const char* name,
    const std::vector<std::pair<std::string, std::string>> *config)
{
    return loadNGSIv2Bridge(name);
}

extern "C" USER_LIB_EXPORT ISReader* create_reader(ISBridge *bridge, const char* name,
    const std::vector<std::pair<std::string, std::string>> *config)
{
    return loadNGSIv2Subscriber(name, config);
}

extern "C" USER_LIB_EXPORT ISWriter* create_writer(ISBridge *bridge, const char* name,
    const std::vector<std::pair<std::string, std::string>> *config)
{
    return loadNGSIv2Publisher(name, config);
}

NGSIv2ISBridge* loadNGSIv2Bridge(const char* name)
{
    try
    {
        return new NGSIv2ISBridge(name);
    }
    catch (int e_code)
    {
        LOG("Invalid configuration, skipping bridge " << e_code);
        return nullptr;
    }
}

NGSIv2Subscriber* loadNGSIv2Subscriber(const char* name, const std::vector<std::pair<std::string, std::string>> *config)
{
    if (!config)
    {
        return nullptr;
    }

    NGSIv2Params participant_ngsiv2_params;
    participant_ngsiv2_params.name = name;
    if (!participant_ngsiv2_params.LoadConfig(config))
    {
        std::cout << "Failed to parse subscriber '" << name << "' properties." << std::endl;
    }

    NGSIv2SubscriptionParams participant_ngsiv2_subscription_param;
    if (!participant_ngsiv2_subscription_param.LoadConfig(config))
    {
        std::cout << "Failed to parse subscriber '" << name << "' properties." << std::endl;
    }


    NGSIv2Subscriber* listener = new NGSIv2Subscriber(name, participant_ngsiv2_params);
    listener->setSubscriptionParams(participant_ngsiv2_subscription_param);
    listener->startListenerAndSubscribe();

    return listener;
}

NGSIv2Publisher* loadNGSIv2Publisher(const char* name, const std::vector<std::pair<std::string, std::string>> *config)
{
    if (!config)
    {
        return nullptr;
    }

    NGSIv2Params participant_ngsiv2_params;
    participant_ngsiv2_params.name = name;
    if (!participant_ngsiv2_params.LoadConfig(config))
    {
        std::cout << "Failed to parse publisher '" << name << "' properties." << std::endl;
    }

    NGSIv2Publisher* publisher = new NGSIv2Publisher(name, participant_ngsiv2_params);
    return publisher;
}
