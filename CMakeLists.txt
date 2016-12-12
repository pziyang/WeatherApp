cmake_minimum_required(VERSION 2.8)

project(Weather)

add_compile_options(-std=c++11)

#-------------------------------------------------------------------------------
# Add CURL
#-------------------------------------------------------------------------------
#Include libcurl
set(CURL_INCLUDE_DIR "/usr/include/curl")
include_directories(${CURL_INCLUDE_DIR})

#-------------------------------------------------------------------------------
# Add Threads
#-------------------------------------------------------------------------------
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

#-------------------------------------------------------------------------------
# Add PugiXML
#-------------------------------------------------------------------------------
# set PugiXML directory
set (PUGIXML_INCLUDE_DIR "pugixml")

# include Pugixml
include_directories(${PUGIXML_INCLUDE_DIR})
add_subdirectory(${PUGIXML_INCLUDE_DIR})

#-------------------------------------------------------------------------------
# Add src
#-------------------------------------------------------------------------------
#set sources
set(SOURCE_FILES main.cpp Weather.cpp Weather.h)


#-------------------------------------------------------------------------------
# Compile and Link
#-------------------------------------------------------------------------------
add_executable(Weather ${SOURCE_FILES})

#linking
target_link_libraries(Weather curl)
target_link_libraries(Weather -pthread)
target_link_libraries(Weather pugixml)