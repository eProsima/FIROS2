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

void usage()
{
    cout << "USAGE" << endl;
    cout << "cb_update_attr <host:port> <entityId> <attributeId> [snb] <value>" << endl;
    cout << "\tMust specify s, n or b for value type." << endl;
    cout << "\ts for string" << endl;
    cout << "\tn for number" << endl;
    cout << "\tb for boolean" << endl;
}

int main(int argc, char *argv[])
{
    try {
        string url = argv[1];
        string entityId = argv[2];
        string attrId = argv[3];
        string type = argv[4];
        string value = argv[5];

        curlpp::Cleanup cleaner;
        curlpp::Easy request;

        // 'http://orion.lab.fiware.org/v2/entities/{entityId}/attrs/{attrName}/value?type='
        stringstream ss;
        ss << url << "/v2/entities/" << entityId << "/attrs/" << attrId << "/value";
        request.setOpt(new curlpp::options::Url(ss.str()));
        request.setOpt(new curlpp::options::Verbose(true));

        std::list<std::string> header;
        header.push_back("Content-Type: text/plain");

        request.setOpt(new curlpp::options::HttpHeader(header));
        request.setOpt(new curlpp::options::CustomRequest("PUT"));

        std::string payload = (type.compare("s") == 0) ? "\"" + value + "\"" : value;

        // Para actualizar un atributo individual, se puede hacer:
        // curl localhost:1026/v2/entities/Madrid-001/attrs/temperature/value -X PUT -s -S --header 'Content-Type: text/plain' --data-binary "-6.0"

        request.setOpt(new curlpp::options::PostFields(payload));
        request.setOpt(new curlpp::options::PostFieldSize(payload.length()));

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
    catch (...)
    {
        usage();
    }

    return EXIT_SUCCESS;
}
