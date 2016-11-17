//
// Created by ziyang on 11/17/16.
//
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <curl.h>
#include "pugixml.hpp"


#include "Weather.h"

size_t AppendDataToStringCurlCallback(void *ptr, size_t size, size_t nmemb, void *vstring) {
    std::string *pstring = (std::string *) vstring;
    pstring->append((char *) ptr, size * nmemb);
    return size * nmemb;
}

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

bool Weather::GetWeatherFromNatWeatherService() {

    //curl variables
    CURL *curl_handle;
    CURLcode res;

    //debug
    static const char *pagefilename = "page.out";
    FILE* pagefile = fopen(pagefilename, "wb");

    //NatWeatherService URL for Ann Arbor
    const std::string url_ann_arbor = "http://forecast.weather.gov/MapClick.php?textField1=42.28&textField2=-83.74&FcstType=dwml";

    std::cout << url_ann_arbor << std::endl;

    curl_handle = curl_easy_init();
    if (curl_handle) {

        //set URL and follow redirection
        curl_easy_setopt(curl_handle, CURLOPT_URL, url_ann_arbor.c_str());
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);

        //write to body
        //curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, AppendDataToStringCurlCallback);
        //curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &body_);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);

        // Perform the request, res will get the return code
        res = curl_easy_perform(curl_handle);

        // Check for errors
        if (res != CURLE_OK) {
            //print error
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            //get time which url is accessed
            time(&last_retrieved_);
        }

        //clean up
        curl_easy_cleanup(curl_handle);
    }

    return true;
}

bool Weather::ParseXml() {

    std::cout << body_ << std::endl;

    pugi::xml_parse_result result = xmldoc_.load(body_.c_str());

    if (result) {
        std::cout << "XML parsed without errors" << std::endl;
    } else {
        std::cout << "XML parsed with errors" << std::endl;
        std::cout << "Error description: " << result.description() << "\n";
    }

    return true;
}

bool Weather::GetCurrentWeather() {

    //this is the top level node
    pugi::xml_node rootnode = xmldoc_.child("dwml");

    //search all child for current observations
    for (pugi::xml_node it = rootnode.first_child(); it; it = it.next_sibling()) {

        if (it.attribute("type").as_string() == "current observations") {

            //read current temperature
            ReadXmlChildValue(weather_data_.temperature, it.child("parameters").child("temperature"));

            //read current  weather conditions
            weather_data_.weather_conditions = it.child("parameters")
                    .child("weather").child("weather-conditions")
                    .attribute("weather-summary").as_string();

            //read wind speed
            ReadXmlChildValue(weather_data_.wind_speed, it.child("parameters")
                    .child("wind-speed").child("value"));
        }
    }

    return true;
}

bool Weather::PrintCurrentWeather() {
    std::cout << weather_data_.temperature << std::endl
              << weather_data_.weather_conditions << std::endl
              << weather_data_.wind_speed << std::endl;
}