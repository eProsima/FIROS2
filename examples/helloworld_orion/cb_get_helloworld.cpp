#include <iostream>
#include <string>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

using namespace curlpp::options;

int main(int argc, char *argv[])
{
    try
    {
        std::string url;
        if (argc == 2)
        {
            url = argv[1];
        }
        else
        {
            url = "localhost:1026";
        }
        // That's all that is needed to do cleanup of used resources (RAII style).
        curlpp::Cleanup myCleanup;

        // Our request to be sent.
        curlpp::Easy myRequest;

        // Set the URL.
        // entities/{entityId}?type=&attrs=
        myRequest.setOpt<Url>(url + "/v2/entities/Helloworld/attrs/count/value?type=Helloworld");

        // Send request and get a result.
        // By default the result goes to standard output.
        std::cout << "Helloworld: ";
        myRequest.perform();
        std::cout << std::endl;
    }

    catch(curlpp::RuntimeError & e)
    {
        std::cout << e.what() << std::endl;
    }

    catch(curlpp::LogicError & e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
