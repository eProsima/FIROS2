#ifndef _NGSIv2PARAMS_H_
#define _NGSIv2PARAMS_H_

#include <iostream>
#include <vector>
#include <string>
#include <map>

class NGSIv2Params
{
public:
    std::string name;
    std::string host;
    uint16_t port;
    uint16_t retries;
    uint16_t httpTimeout;

    NGSIv2Params();
    bool LoadConfig(const std::vector<std::pair<std::string, std::string>> *config);
};

class NGSIv2SubscriptionParams
{
public:
    std::string idPattern;
    std::string type;
    std::string attrs;
    std::string expression;
    std::string notif;
    std::string expiration;
    int throttling;
    std::string description;
    std::string host;
    uint16_t port;
    unsigned int buffer_size;

    NGSIv2SubscriptionParams();
    bool LoadConfig(const std::vector<std::pair<std::string, std::string>> *config);
};

#endif
