//
// Created by ziyang on 11/17/16.
//
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <chrono>
#include <unistd.h>
#include <curl/curl.h>
#include "pugixml.hpp"

#include "Weather.h"


size_t AppendDataToStringCurlCallback(void *ptr, size_t size, size_t nmemb, void *vstring) {
	std::string *pstring = (std::string *) vstring;
	pstring->append((char *)ptr, size * nmemb);
	return size * nmemb;
}

bool Weather::GetWeatherFromNatWeatherService() {

	CURL *curl_handle;

	curl_global_init(CURL_GLOBAL_ALL);

	//init the curl session 
	curl_handle = curl_easy_init();

	//set URL to get here 
	curl_easy_setopt(curl_handle, CURLOPT_URL, nws_url_.c_str());

	//Switch on full protocol/debug output while testing 
	curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 0L);

	//disable progress meter, set to 0L to enable and disable debug output 
	curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 0L);

	//set user agent 
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "Mozilla/5.0");

	//write data to string 
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, AppendDataToStringCurlCallback);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &body_);

	//get it! 
	CURLcode code = curl_easy_perform(curl_handle);

	//check and return false if there are errors
	if (code != CURLE_OK)
	{
		std::cout << "Failed to run curl " << code << std::endl;

		return false;
	}

	//update time
	time(&last_retrieved_);

	//cleanup curl stuff 
	curl_easy_cleanup(curl_handle);

	return true;
}

bool Weather::ParseXml() {

	//load string into XML DOM
	pugi::xml_parse_result result = xmldoc_.load(body_.c_str());

	//check if there are errors
	if (result) {

		//set last_retrieved_
		time(&last_retrieved_);

		//print success
		std::cout << "XML parsed without errors" << std::endl;
	}
	else {

		//print error message if there are pasrsing errors
		std::cout << "XML parsed with errors" << std::endl;
		std::cout << "Error description: " << result.description() << "\n";

		return false;
	}

	return true;
}

bool Weather::GetCurrentWeather() {

	//lock mutex for updating
	mlock_.lock();

	//this is the top level node
	pugi::xml_node rootnode = xmldoc_.child("dwml");

	//search all child for current observations
	for (pugi::xml_node it = rootnode.first_child(); it; it = it.next_sibling()) {

		if (strcmp(it.attribute("type").as_string(), "current observations") == 0) {

			//read current temperature
			ReadXmlChildValue(weather_data_.temperature, it.child("parameters").child("temperature").child("value"));

			//read current weather conditions
			weather_data_.weather_conditions = it.child("parameters")
				.child("weather").child("weather-conditions")
				.attribute("weather-summary").as_string();

			//read sustained wind speed
			ReadXmlChildValue(weather_data_.wind_speed, it.child("parameters")
				.child("wind-speed").next_sibling().child("value"));

			//done reading
			break;
		}
	}

	//unlock mutex for updating
	mlock_.unlock();

	return true;
}

bool Weather::GetForecastWeather()
{
	//lock mutex for updating
	mlock_.lock();

	//this is the top level node
	pugi::xml_node rootnode = xmldoc_.child("dwml");

	//search all child for forecast
	for (pugi::xml_node it = rootnode.first_child(); it; it = it.next_sibling()) {

		if (strcmp(it.attribute("type").as_string(), "forecast") == 0) {

			//search for max and min temperature from identical <temperature> tag
			for (pugi::xml_node it2 = it.child("parameters").child("temperature"); it2; it2 = it2.next_sibling())
			{
				//read the first value for min temp for the day
				if (strcmp(it2.attribute("type").as_string(), "minimum") == 0)
					ReadXmlChildValue(weather_data_.min_temperature, it2.child("value"));

				//read the first value for max temp for the day
				if (strcmp(it2.attribute("type").as_string(), "maximum") == 0)
					ReadXmlChildValue(weather_data_.max_temperature, it2.child("value"));
			}
		}
	}

	//unlock mutex for updating
	mlock_.unlock();

	return true;
}

bool Weather::PrintCurrentWeather() {
	std::cout << "Last Retrieved: " << last_retrieved_ << std::endl
		<< "Current Temperature: " << weather_data_.temperature << std::endl
		<< "Weather Conditions: " << weather_data_.weather_conditions << std::endl
		<< "Wind Speed: " << weather_data_.wind_speed << std::endl
		<< "Min Temperature: " << weather_data_.min_temperature << std::endl
		<< "Max Temperature: " << weather_data_.max_temperature << std::endl << std::endl;
}

void Weather::RunThread() {

	while (!stop_thread_)
	{
		//sleep for defined interval
		std::this_thread::sleep_for(std::chrono::seconds(update_interval_seconds_));

		//retrieve weather
		GetWeatherFromNatWeatherService();
		ParseXml();
		GetForecastWeather();
		GetCurrentWeather();
		PrintCurrentWeather();
	}
}