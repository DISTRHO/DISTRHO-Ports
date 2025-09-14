/*
 *  File:       UpdateChecker.h
 *
 *  Created:    05/05/2017
 *
 *	This file is part of WECore.
 *
 *  WECore is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  WECore is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with WECore.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UpdateChecker_h
#define UpdateChecker_h

#include <string>
#include <curl/curl.h>

class UpdateChecker {
public:
    UpdateChecker() = default;

    bool checkIsLatestVersion(const char* productName,
                              const char* productVersion) {

        // cURL setup
        CURL *curl;
        CURLcode result;
        curl = curl_easy_init();

        // build the URL we'll be sending
        std::string requestURL {TARGET_URL};
        requestURL.append("?product=");
        requestURL.append(productName);

        std::string response;

        // setup the request
        curl_easy_setopt(curl, CURLOPT_URL, requestURL.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _writeToString);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // send the request and clean up
        result = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        return (productVersion == response);
    }

private:
    const std::string TARGET_URL {"https://whiteelephantaudio.com/versionChecker.php"};

    static size_t _writeToString(char* ptr, size_t size, size_t nmemb, std::string* stream) {
        *stream = std::string(ptr);

        return size * nmemb;
    }
};

#endif /* UpdateChecker_h */
