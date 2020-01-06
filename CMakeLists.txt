cmake_minimum_required(VERSION 3.14)
project(RelayTest C)

set(CMAKE_C_STANDARD 99)

add_executable(RelayTest code/TestUnit.c code/common.c code/common.h code/dataStruct.h code/deltaDistanceRelay.c code/distanceRelay.c code/line.c code/lineStarter.c code/overCurrentRelay.c code/relayMain.c)