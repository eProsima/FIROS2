// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "NGSIv2Utils.h"
#include "log/ISLog.h"

using asio::ip::tcp;

bool performAndRetry(int n_retries, curlpp::Easy &request)
{
    int tries = 0;
    bool success = false;
    std::ostringstream response;
    do
    {
        request.setOpt(new curlpp::options::WriteStream(&response));
        request.perform();
        long responseCode = curlpp::infos::ResponseCode::get(request);

        switch (responseCode / 100)
        {
            case 1: // 1XX codes
                success = true;
                tries = n_retries; // Success
                break;
            case 2: // 2XX codes
                success = true;
                tries = n_retries; // Success
                break;
            case 3: // 3XX codes
                tries = n_retries; // Abort
                LOG_ERROR("Connection failed: " << responseCode);
                LOG_INFO(response.str());
                break;
            case 4: // 4XX codes
                LOG_ERROR("Connection failed: " << responseCode);
                tries = n_retries; //Don't retry
                break;
            case 5: // 5XX codes
                LOG_ERROR("Connection failed: " << responseCode);
                break;
            default:
                LOG(response.str());
                break;
        }
    } while (!success && ++tries < n_retries);
    return success;
}
