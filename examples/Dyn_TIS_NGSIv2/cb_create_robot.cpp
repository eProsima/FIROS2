/*
 *    Copyright (c) <2002-2005> <Jean-Philippe Barrette-LaPierre>
 *
 *    Permission is hereby granted, free of charge, to any person obtaining
 *    a copy of this software and associated documentation files
 *    (curlpp), to deal in the Software without restriction,
 *    including without limitation the rights to use, copy, modify, merge,
 *    publish, distribute, sublicense, and/or sell copies of the Software,
 *    and to permit persons to whom the Software is furnished to do so,
 *    subject to the following conditions:
 *
 *    The above copyright notice and this permission notice shall be included
 *    in all copies or substantial portions of the Software.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 *    OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * \file
 * Simple POST demo.
 *
 */


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

void usage(std::string name)
{
    std::cout << "Usage: " << name << " <url> <RobotId>" << std::endl;
    std::cout << "\turl: Address of the Orion contextBroker server in format <host>:<port>" << std::endl;
    std::cout << "\tRobotId: Identifier of the robot to be created." << std::endl;
}


int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        usage(argv[0]);
    }
    else
    {
        try {
            string url = argv[1];
            string robotId = argv[2];
            curlpp::Cleanup cleaner;
            curlpp::Easy request;

            request.setOpt(new curlpp::options::Url(url + "/v2/entities"));
            request.setOpt(new curlpp::options::Verbose(true));

            std::list<std::string> header;
            header.push_back("Content-Type: application/json");

            request.setOpt(new curlpp::options::HttpHeader(header));

            std::string json = "{ \"id\": \"" + robotId + "\", \"type\": \"Robot\", \"transmission_time\": { \"value\": \"2018-04-01T00:00:00.00Z\", \"type\": \"DateTime\"}, \
                \"floor\": { \"value\": 0, \"type\": \"Number\" }, \"x\": { \"value\": 0, \"type\": \"Number\" }, \"y\": { \"value\": 0, \"type\": \"Number\" }, \
                \"zeta\": { \"value\": 0, \"type\": \"Number\" }, \"state\": { \"value\": \"STAND_BY\" } }";

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
    }
    return EXIT_SUCCESS;
}
