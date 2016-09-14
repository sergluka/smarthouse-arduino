# Dummy CMakeList file for CLion will be able to index project files.
# You have to compile all *.ino files in a regular Arduino way

cmake_minimum_required(VERSION 3.4.0)

set(PROJECT_NAME Arduino)
set(CMAKE_CXX_FLAGS "-std=c++17")
project(${PROJECT_NAME})

file(GLOB_RECURSE INO *.ino)
file(GLOB_RECURSE SRCS *.cpp)
file(GLOB_RECURSE HDRS *.h)

ADD_EXECUTABLE(Arduino  ${INO} ${SRCS} ${HDRS})

include_directories(C:/dev/Tools/Arduino/hardware/arduino/avr/cores/arduino)
include_directories(C:/dev/Tools/Arduino/hardware/arduino/avr/libraries/EEPROM)
include_directories(C:/dev/Tools/Arduino/libraries/MySensors-2.0)
include_directories(C:/dev/Tools/Arduino/libraries/SimpleTimer)
include_directories(./sketches)
include_directories(./libraries/MySensors-2.0)
include_directories(./libraries/Button)
include_directories(./libraries/SimpleTimer)
include_directories(./libraries/LEDFader)
include_directories(./libraries/Logging)
include_directories(./libraries/TimerOne)
include_directories(./libraries/NewButton)
include_directories(./libraries/MsTimer2Obj)
include_directories(./tests/dependencies/gmock/include)
include_directories(./tests/dependencies/gtest/include)
include_directories(./tests/mocks)

