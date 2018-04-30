#include <iostream>
#include "ISBridgeNGSIv2.h"

static const std::string s_sHost("host");
static const std::string s_sPort("port");
static const std::string s_sId("id");
static const std::string s_sType("type");
static const std::string s_sAttrs("attrs");
static const std::string s_sExpression("expression");
static const std::string s_sNotifs("notifs");
static const std::string s_sListenerHost("listener_host");
static const std::string s_sListenerPort("listener_port");
static const std::string s_sListenerBufferSize("listener_buffer_size");
static const std::string s_sExpiration("expiration");
static const std::string s_sThrottling("throttling");
static const std::string s_sDescription("description");
static const std::string s_sRetries("http_retries");
static const std::string s_sHttpTimeout("http_timeout");

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
NGSIv2Subscriber* loadNGSIv2Subscriber(const char* name, const std::vector<std::pair<std::string, std::string>> *config);
NGSIv2Publisher* loadNGSIv2Publisher(const char* name, const std::vector<std::pair<std::string, std::string>> *config);

extern "C" USER_LIB_EXPORT ISBridge* create_bridge(const char* name,
    const std::vector<std::pair<std::string, std::string>> *config)
{
    return loadNGSIv2Bridge(name);
}

extern "C" USER_LIB_EXPORT ISSubscriber* create_subscriber(ISBridge *bridge, const char* name,
    const std::vector<std::pair<std::string, std::string>> *config)
{
    return loadNGSIv2Subscriber(name, config);
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
    catch (int e_code)
    {
        std::cout << "Invalid configuration, skipping bridge " << e_code << std::endl;
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
    NGSIv2SubscriptionParams participant_ngsiv2_subscription_param;
    participant_ngsiv2_params.name = name;

    for (auto pair : *config)
    {
        try
        {
            if (pair.first.compare(s_sHost) == 0)
            {
                participant_ngsiv2_params.host = pair.second;
            }
            else if (pair.first.compare(s_sPort) == 0)
            {
                participant_ngsiv2_params.port = std::stoi(pair.second);
            }
            else if (pair.first.compare(s_sHttpTimeout) == 0)
            {
                participant_ngsiv2_params.httpTimeout = std::stoi(pair.second);
            }
            else if (pair.first.compare(s_sRetries) == 0)
            {
                participant_ngsiv2_params.retries = std::stoi(pair.second);
            }
            else if (pair.first.compare(s_sId) == 0)
            {
                participant_ngsiv2_subscription_param.idPattern = pair.second;
            }
            else if (pair.first.compare(s_sType) == 0)
            {
                participant_ngsiv2_subscription_param.type = pair.second;
            }
            else if (pair.first.compare(s_sAttrs) == 0)
            {
                participant_ngsiv2_subscription_param.attrs = pair.second;
            }
            else if (pair.first.compare(s_sExpression) == 0)
            {
                participant_ngsiv2_subscription_param.expression = pair.second;
            }
            else if (pair.first.compare(s_sNotifs) == 0)
            {
                participant_ngsiv2_subscription_param.notif = pair.second;
            }
            else if (pair.first.compare(s_sListenerHost) == 0)
            {
                participant_ngsiv2_subscription_param.host = pair.second;
            }
            else if (pair.first.compare(s_sListenerPort) == 0)
            {
                participant_ngsiv2_subscription_param.port = std::stoi(pair.second);
            }
            else if (pair.first.compare(s_sExpiration) == 0)
            {
                participant_ngsiv2_subscription_param.expiration = pair.second;
            }
            else if (pair.first.compare(s_sThrottling) == 0)
            {
                participant_ngsiv2_subscription_param.throttling = std::stoi(pair.second);
            }
            else if (pair.first.compare(s_sDescription) == 0)
            {
                participant_ngsiv2_subscription_param.description = pair.second;
            }
            else if (pair.first.compare(s_sListenerBufferSize) == 0)
            {
                participant_ngsiv2_subscription_param.buffer_size = std::stoi(pair.second);
            }
        }
        catch(...)
        {
            std::cout << "Failed to parse subscriber '" << name << "' properties." << std::endl;
        }
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

    for (auto pair : *config)
    {
        try
        {
            if (pair.first.compare(s_sHost) == 0)
            {
                participant_ngsiv2_params.host = pair.second;
            }
            else if (pair.first.compare(s_sPort) == 0)
            {
                participant_ngsiv2_params.port = std::stoi(pair.second);
            }
            else if (pair.first.compare(s_sRetries) == 0)
            {
                participant_ngsiv2_params.retries = std::stoi(pair.second);
            }
            else if (pair.first.compare(s_sHttpTimeout) == 0)
            {
                participant_ngsiv2_params.httpTimeout = std::stoi(pair.second);
            }
        }
        catch(...)
        {
            std::cout << "Failed to parse publisher '" << name << "' properties." << std::endl;
        }
    }

    NGSIv2Publisher* publisher = new NGSIv2Publisher(name, participant_ngsiv2_params);
    return publisher;
}
