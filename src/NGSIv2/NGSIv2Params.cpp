#include "NGSIv2Params.h"

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


NGSIv2Params::NGSIv2Params()
    : name("")
    , host("")
    , port(0)
    , retries(0)
    , httpTimeout(0)
{
}

bool NGSIv2Params::LoadConfig(const std::vector<std::pair<std::string, std::string>> *config)
{
    for (auto pair : *config)
    {
        try
        {
            if (pair.first.compare(s_sHost) == 0)
            {
                host = pair.second;
            }
            else if (pair.first.compare(s_sPort) == 0)
            {
                port = std::stoi(pair.second);
            }
            else if (pair.first.compare(s_sHttpTimeout) == 0)
            {
                httpTimeout = std::stoi(pair.second);
            }
            else if (pair.first.compare(s_sRetries) == 0)
            {
                retries = std::stoi(pair.second);
            }
        }
        catch (...)
        {
            return false;
        }
    }
    return true;
}

NGSIv2SubscriptionParams::NGSIv2SubscriptionParams()
: idPattern("")
, type("")
, attrs("")
, expression("")
, notif("")
, expiration("")
, throttling(0)
, description("")
, host("")
, port(0)
, buffer_size(0)
{
}

bool NGSIv2SubscriptionParams::LoadConfig(const std::vector<std::pair<std::string, std::string>> *config)
{
    for (auto pair : *config)
    {
        try
        {
            if (pair.first.compare(s_sId) == 0)
            {
                idPattern = pair.second;
            }
            else if (pair.first.compare(s_sType) == 0)
            {
                type = pair.second;
            }
            else if (pair.first.compare(s_sAttrs) == 0)
            {
                attrs = pair.second;
            }
            else if (pair.first.compare(s_sExpression) == 0)
            {
                expression = pair.second;
            }
            else if (pair.first.compare(s_sNotifs) == 0)
            {
                notif = pair.second;
            }
            else if (pair.first.compare(s_sListenerHost) == 0)
            {
                host = pair.second;
            }
            else if (pair.first.compare(s_sListenerPort) == 0)
            {
                port = std::stoi(pair.second);
            }
            else if (pair.first.compare(s_sExpiration) == 0)
            {
                expiration = pair.second;
            }
            else if (pair.first.compare(s_sThrottling) == 0)
            {
                throttling = std::stoi(pair.second);
            }
            else if (pair.first.compare(s_sDescription) == 0)
            {
                description = pair.second;
            }
            else if (pair.first.compare(s_sListenerBufferSize) == 0)
            {
                buffer_size = std::stoi(pair.second);
            }
        }
        catch (...)
        {
            return false;
        }
    }
    return true;
}

