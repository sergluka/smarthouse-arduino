# Dummy CMakeList file for CLion will be able to index project files.
# You have to compile all *.ino files in a regular Arduino way

cmake_minimum_required(VERSION 3.4.0)

set(PROJECT_NAME Arduino)
project(${PROJECT_NAME})

file(GLOB_RECURSE INO *.ino)
file(GLOB_RECURSE SRCS *.cpp)
file(GLOB_RECURSE HDRS *.h)

ADD_EXECUTABLE(Arduino  ${INO} ${SRCS} ${HDRS})

include_directories(C:/dev/Tools/Arduino/hardware/arduino/avr/cores/arduino)
include_directories(C:/dev/Tools/Arduino/hardware/arduino/avr/libraries/EEPROM)
include_directories(C:/dev/Tools/Arduino/libraries/MySensors-2.0)
include_directories(C:/dev/Tools/Arduino/libraries/SimpleTimer)
include_directories(./libraries/MySensors-2.0)
include_directories(./libraries/Button)
include_directories(./libraries/SimpleTimer)
include_directories(./libraries/LEDFader)
include_directories(./libraries/Logging)
include_directories(./libraries/TimerOne)
include_directories(./libraries/NewButton)

