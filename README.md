## C++ Weather Application ##

This is a simple weather application written in c++. It connects to US National Weather Service (NWS) API and retrives the weather forecast in NDFD format. It parses the xml for use.

It is meant to run in a background thread. Therefore, it very useful for integrating into another codes.

I initially wrote this to run a weather update LED display on Rpi.

### Reference ###
US National Weather Service NDFD (https://graphical.weather.gov/xml/)
PugiXML parser (http://pugixml.org/)

### Requirements ###
* libcurl
* cmake

### Platforms ###
* Testing on Rpi running debian

### installation ###
    mkdir build
    cmake ..
    make

### User Customization ###
See Weather.h

### Example use ###
See main.cpp

### License ###
GPLv3
