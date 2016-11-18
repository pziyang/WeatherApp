//
// Created by ziyang on 11/17/16.
//

#ifndef WEATHER_WEATHER_H
#define WEATHER_WEATHER_H

#include <string>
#include <sstream>
#include <time.h>
#include "pugixml.hpp"

template< typename T > inline bool ReadXmlChildValue(T& vars, pugi::xml_node node)
{
    std::stringstream stream;
    T data;
    int counter = 0;

    //check if node exist
    if (node == NULL) return false;					//fail exit point

    //if node exist, get value and store in stringstream
    stream << node.child_value();

    //check if stream is empty
    if (stream.str().empty()) return false;			//fail exit point

    //use stringstream to read data into correct datatype
    while (stream >> data)
    {
        vars = data;
    }

    return true;									//successful exit point
}

struct WeatherDataStruct{
	//current
    float temperature;
    std::string weather_conditions;
    float wind_speed;

	//daily variation
	float max_temperature;
	float min_temperature;
};

class Weather {
private:
    std::string body_;
    pugi::xml_document xmldoc_;
    time_t last_retrieved_;
    WeatherDataStruct weather_data_;
public:
    bool GetWeatherFromNatWeatherService();
    bool ParseXml();
    bool GetCurrentWeather();
	bool GetForecastWeather();

    bool PrintCurrentWeather();

};


#endif //WEATHER_WEATHER_H
