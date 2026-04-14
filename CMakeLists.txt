cmake_minimum_required(VERSION 3.10.0)
project(LogicGateSimulator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(includes)

set(SOURCES
    src/gates/And.cpp

    src/Controllers.cpp
    src/Errors.cpp
    src/Gate.cpp
    src/Grid.cpp
    src/Managers.cpp
    src/main.cpp
    src/Pin.cpp
    src/Renderers.cpp
    src/Wire.cpp
)

set(HEADERS
    includes/Controllers.hpp
    includes/Errors.hpp
    includes/Gate.hpp
    includes/Grid.hpp
    includes/Managers.hpp
    includes/Pin.hpp
    includes/Renderers.hpp
    includes/Simulation.hpp
    includes/Wire.hpp
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

find_package(SFML COMPONENTS Graphics Window System CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics SFML::Window SFML::System)
