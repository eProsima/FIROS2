#include <iostream>
#include "ISBridgeNGSIv2.h"

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

ISBridgeNGSIv2* loadNGSIv2Bridge(const char* name);
NGSIv2Listener* loadNGSIv2Listener(const char* name, const std::vector<std::pair<std::string, std::string>> *config);
NGSIv2Publisher* loadNGSIv2Publisher(const char* name, const std::vector<std::pair<std::string, std::string>> *config);

extern "C" USER_LIB_EXPORT ISBridge* create_bridge(const char* name,
    const std::vector<std::pair<std::string, std::string>> *config)
{
    return loadNGSIv2Bridge(name);
}

extern "C" USER_LIB_EXPORT ISSubscriber* create_subscriber(ISBridge *bridge, const char* name,
    const std::vector<std::pair<std::string, std::string>> *config)
{
    return loadNGSIv2Listener(name, config);
}

extern "C" USER_LIB_EXPORT ISPublisher* create_publisher(ISBridge *bridge, const char* name,
    const std::vector<std::pair<std::string, std::string>> *config)
{
    return loadNGSIv2Publisher(name, config);
}

ISBridgeNGSIv2* loadNGSIv2Bridge(const char* name)
{
    try
    {
        return new ISBridgeNGSIv2(name);
    }
    catch (int e_code){
        std::cout << "Invalid configuration, skipping bridge " << e_code << std::endl;
        return nullptr;
    }
}

NGSIv2Listener* loadNGSIv2Listener(const char* name, const std::vector<std::pair<std::string, std::string>> *config)
{
    if (!config)
    {
        return nullptr;
    }

    NGSIv2Params participant_ngsiv2_params;
    NGSIv2SubscriptionParams participant_ngsiv2_subscription_param;
    participant_ngsiv2_params.name = name;

    for (auto pair : *config)
    {
        try
        {
            if (pair.first.compare("host") == 0)
            {
                participant_ngsiv2_params.host = pair.second;
            }
            else if (pair.first.compare("port") == 0)
            {
                participant_ngsiv2_params.port = std::stoi(pair.second);
            }
            else if (pair.first.compare("id") == 0)
            {
                participant_ngsiv2_subscription_param.idPattern = pair.second;
            }
            else if (pair.first.compare("type") == 0)
            {
                participant_ngsiv2_subscription_param.type = pair.second;
            }
            else if (pair.first.compare("attrs") == 0)
            {
                participant_ngsiv2_subscription_param.attrs = pair.second;
            }
            else if (pair.first.compare("expression") == 0)
            {
                participant_ngsiv2_subscription_param.expression = pair.second;
            }
            else if (pair.first.compare("notifs") == 0)
            {
                participant_ngsiv2_subscription_param.notif = pair.second;
            }
            else if (pair.first.compare("listener_host") == 0)
            {
                participant_ngsiv2_subscription_param.host = pair.second;
            }
            else if (pair.first.compare("listener_port") == 0)
            {
                participant_ngsiv2_subscription_param.port = std::stoi(pair.second);
            }
            else if (pair.first.compare("expiration") == 0)
            {
                participant_ngsiv2_subscription_param.expiration = pair.second;
            }
            else if (pair.first.compare("throttling") == 0)
            {
                participant_ngsiv2_subscription_param.throttling = std::stoi(pair.second);
            }
            else if (pair.first.compare("description") == 0)
            {
                participant_ngsiv2_subscription_param.description = pair.second;
            }
            /*
            else if (pair.first.compare("listener_buffer_size") == 0)
            {
                participant_ngsiv2_subscription_param.buffer_size = std::stoi(pair.second);
            }
            */
        }
        catch(...)
        {
            std::cout << "Failed to parse subscriber '" << name << "' properties." << std::endl;
        }
    }

    //participant_ngsiv2_params.idPattern = ngsiv2_id;

    /*
    NGSIv2Listener* listener = NGSIv2Listener::configureNGSIv2Listener(participant_ngsiv2_params,
                                        participant_ngsiv2_subscription_param);
                                        */
    NGSIv2Listener* listener = new NGSIv2Listener(name, participant_ngsiv2_params.host,
                                                     participant_ngsiv2_params.port);
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

    for (auto pair : *config)
    {
        try
        {
            if (pair.first.compare("host") == 0)
            {
                participant_ngsiv2_params.host = pair.second;
            }
            else if (pair.first.compare("port") == 0)
            {
                participant_ngsiv2_params.port = std::stoi(pair.second);
            }
        }
        catch(...)
        {
            std::cout << "Failed to parse publisher '" << name << "' properties." << std::endl;
        }
    }

    //participant_ngsiv2_params.idPattern = ngsiv2_id;

    //NGSIv2Publisher* publisher = NGSIv2Publisher::configureNGSIv2Publisher(participant_ngsiv2_params);
    NGSIv2Publisher* publisher = new NGSIv2Publisher(name, participant_ngsiv2_params.host,
                                                     participant_ngsiv2_params.port);

    return publisher;
}
