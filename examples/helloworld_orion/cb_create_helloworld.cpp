#include <cstdlib>
#include <cerrno>
#include <iostream>
#include <string>
#include <sstream>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

using namespace std;

int main(int argc, char *argv[])
{
    try {
        std::string url;
        if (argc == 2)
        {
            url = argv[1];
        }
        else
        {
            url = "localhost:1026";
        }
        curlpp::Cleanup cleaner;
        curlpp::Easy request;

        request.setOpt(new curlpp::options::Url(url + "/v2/entities"));
        request.setOpt(new curlpp::options::Verbose(true));

        std::list<std::string> header;
        header.push_back("Content-Type: application/json");

        request.setOpt(new curlpp::options::HttpHeader(header));

        std::string json = "{ \"id\": \"Helloworld\", \"type\": \"Helloworld\", \
            \"count\": { \"value\": 0, \"type\": \"Number\" } }";

        // Para actualizar un atributo individual, se puede hacer:
        // curl localhost:1026/v2/entities/Madrid-001/attrs/temperature/value -X PUT -s -S --header 'Content-Type: text/plain' --data-binary "-6.0"

        request.setOpt(new curlpp::options::PostFields(json));
        request.setOpt(new curlpp::options::PostFieldSize(json.length()));

        std::ostringstream response;
        request.setOpt(new curlpp::options::WriteStream(&response));

        request.perform();

        cout << response.str() << endl;
    }
    catch ( curlpp::LogicError & e ) {
        std::cout << e.what() << std::endl;
    }
    catch ( curlpp::RuntimeError & e ) {
        std::cout << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
