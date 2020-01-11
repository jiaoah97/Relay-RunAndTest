cmake_minimum_required(VERSION 3.14)
project(RelayTest C)

set(CMAKE_C_STANDARD 99)

add_executable(RelayTest
        code/TestUnit.c
        code/common.c
        code/common.h
        code/dataStruct.h
        code/relayMain.c
        code/line.c
        code/lineStarter.c
        code/currentDiffRelay.c
        code/deltaDistanceRelay.c
        code/distanceRelay.c
        code/zeroSeqCurrentRelay.c
        code/overCurrentRelay.c
        )