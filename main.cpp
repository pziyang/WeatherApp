#include <iostream>

#include "Weather.h"

int main() {
    Weather weather;

    weather.GetWeatherFromNatWeatherService();
    weather.ParseXml();
    weather.GetCurrentWeather();
    weather.PrintCurrentWeather();

    return 0;
}