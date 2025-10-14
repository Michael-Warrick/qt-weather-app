cmake_minimum_required(VERSION 3.20)
project(qt_weather LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

add_executable(${PROJECT_NAME}
        src/Cache.cpp
        src/WeatherClient.cpp
        src/Window.cpp
        src/main.cpp

        include/Cache.hpp
        include/WeatherClient.hpp
        include/Window.hpp
)

target_include_directories(${PROJECT_NAME} PUBLIC include)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        Qt6::Widgets
        Qt6::Network
)

if(APPLE)
    set_target_properties(${PROJECT_NAME}
            PROPERTIES
            MACOSX_BUNDLE TRUE
    )
endif()