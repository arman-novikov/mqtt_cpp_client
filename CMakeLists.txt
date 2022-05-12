cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME mqtt_client)
project(${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

add_library(mqtt_manager-lib
    mqtt_client/mqtt_client.h
    mqtt_client/mqtt_client.cpp
    mqtt_client/imessage_handler.h
    mqtt_client/test.h
    mqtt_client/test.cpp
)


include_directories(mqtt_client/include)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads mqtt_manager-lib)
