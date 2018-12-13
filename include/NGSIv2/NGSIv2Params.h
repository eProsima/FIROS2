#ifndef _NGSIv2PARAMS_H_
#define _NGSIv2PARAMS_H_

#include <iostream>

class NGSIv2Params
{
public:
    std::string name;
    std::string idPattern;
    std::string host;
    uint16_t port;
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
};

#endif
