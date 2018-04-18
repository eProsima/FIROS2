#include <iostream>
#include <tinyxml2.h>
#include "ISBridgeNGSIv2.h"
#include "../../thirdparty/integration-services/src/xmlUtils.h"

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

ISBridgeNGSIv2* loadNGSIv2Bridge(tinyxml2::XMLElement *bridge_element);

extern "C" ISBridge* USER_LIB_EXPORT createBridge(const char *config)
{
    tinyxml2::XMLDocument doc;
    doc.Parse(config);
    return loadNGSIv2Bridge(doc.FirstChildElement("bridge_configuration"));
}

ISBridgeNGSIv2* loadNGSIv2Bridge(tinyxml2::XMLElement *bridge_element)
{
    // TODO adapt! Only ngsiv2 branch
    try
    {
        ISBridgeNGSIv2 *ngsiv2_rtps;

        tinyxml2::XMLElement *ngsiv2_element = bridge_element->FirstChildElement("ngsiv2");
        if(!ngsiv2_element)
        {
            throw 0;
        }

        tinyxml2::XMLElement *current_element;

        current_element = _assignNextElement(ngsiv2_element, "participant");
        const char* ngsiv2_participant_name = current_element->GetText();
        current_element = _assignNextElement(ngsiv2_element, "id");
        const char* ngsiv2_id = current_element->GetText();

        current_element = _assignOptionalElement(ngsiv2_element, "host");
        const char* ngsiv2_host = (current_element == nullptr) ? "localhost" : current_element->GetText();

        int ngsiv2_port;
        current_element = _assignNextElement(ngsiv2_element, "port");
        if(current_element->QueryIntText(&ngsiv2_port))
        {
            ngsiv2_port = 1026;
        }

        const char* function_path;
        if (ngsiv2_element->FirstChildElement("transformation"))
        {
            function_path = ngsiv2_element->FirstChildElement("transformation")->GetText();
        }
        else
        {
            if (ngsiv2_element->FirstChildElement("transformFromNGSIv2"))
            {
                function_path = ngsiv2_element->FirstChildElement("transformFromNGSIv2")->GetText();
            }
            else
            {
                function_path = nullptr;
                // No function defined for NGSIv2 -> RTPS
                //std::cout << "ERROR: No transformation function defined." << std::endl;
                //throw 0;
            }
        }

        // NGSIv2 configuration
        NGSIv2Params participant_ngsiv2_params;
        participant_ngsiv2_params.name = ngsiv2_participant_name;
        participant_ngsiv2_params.host = ngsiv2_host;
        participant_ngsiv2_params.idPattern = ngsiv2_id;
        participant_ngsiv2_params.port = ngsiv2_port;

        NGSIv2Publisher* publisher = NGSIv2Publisher::configureNGSIv2Publisher(participant_ngsiv2_params);

        // Subscription
        tinyxml2::XMLElement *ngsiv2_sub_element = _assignOptionalElement(ngsiv2_element, "subscription");

        if (ngsiv2_sub_element)
        {
            current_element = _assignOptionalElement(ngsiv2_sub_element, "type");
            const char* subscription_type = (current_element == nullptr) ? "" : current_element->GetText();

            current_element = _assignOptionalElement(ngsiv2_sub_element, "attrs");
            const char* subscription_attrs = (current_element == nullptr) ? "" : current_element->GetText();

            current_element = _assignOptionalElement(ngsiv2_sub_element, "expression");
            const char* subscription_expression = (current_element == nullptr) ? "" : current_element->GetText();

            const char* subscription_notifs = _assignNextElement(ngsiv2_sub_element, "notifs")->GetText();
            const char* subscription_host = _assignNextElement(ngsiv2_sub_element, "listener_host")->GetText();
            int subscription_port;
            current_element = _assignNextElement(ngsiv2_sub_element, "listener_port");
            if (current_element != nullptr)
            {
                current_element->QueryIntText(&subscription_port);
            }

            current_element = _assignOptionalElement(ngsiv2_sub_element, "expiration");
            const char* subscription_expiration = (current_element == nullptr) ? "" : current_element->GetText();

            current_element = _assignOptionalElement(ngsiv2_sub_element, "throttling");
            int subscription_throttling = -1;
            if (current_element != nullptr)
            {
                current_element->QueryIntText(&subscription_throttling);
            }

            current_element = _assignOptionalElement(ngsiv2_sub_element, "description");
            const char* subscription_description = (current_element == nullptr) ? "" : current_element->GetText();

            // NGSIv2 subscription configuration
            NGSIv2SubscriptionParams participant_ngsiv2_subscription_param;
            participant_ngsiv2_subscription_param.idPattern = ngsiv2_id;
            participant_ngsiv2_subscription_param.type = subscription_type;
            participant_ngsiv2_subscription_param.attrs = subscription_attrs;
            participant_ngsiv2_subscription_param.expression = subscription_expression;
            participant_ngsiv2_subscription_param.notif = subscription_notifs;
            participant_ngsiv2_subscription_param.expiration = subscription_expiration;
            participant_ngsiv2_subscription_param.throttling = subscription_throttling;
            participant_ngsiv2_subscription_param.description = subscription_description;
            participant_ngsiv2_subscription_param.host = subscription_host;
            participant_ngsiv2_subscription_param.port = subscription_port;

            NGSIv2Listener* listener = NGSIv2Listener::configureNGSIv2Listener(participant_ngsiv2_params,
                                            participant_ngsiv2_subscription_param);

            ngsiv2_rtps = new ISBridgeNGSIv2(publisher, listener, function_path);
        }
        else
        {
            ngsiv2_rtps = new ISBridgeNGSIv2(publisher, nullptr, function_path);
        }

        return ngsiv2_rtps;
    }
    catch (int e_code){
        std::cout << "Invalid configuration, skipping bridge " << e_code << std::endl;
        return nullptr;
    }
}
