#include "..\\code\\dataStruct.h"
#include "..\\code\common.h"

/**
 * RelayMain.c
 * 所有保护装置的入口函数
 */


Device s1_line1;
Device s1_line2;

Device s1_bus1;

int globalInitFlag = 0;// 全局初始化标志
char logDirName[STRING_LENGTH];

// 声明各类保护函数
extern double line(Device*); // 线路保护


// 注意采用GFORTRAN,C函数名需要多一个下划线
void s1_line1_(double* time, int* deviceEnable, double* port1, double* port2, double* tripSignal) {
    globalInit();
    lineLinkSimulation(&s1_line1, "s1-line1", *time, *deviceEnable, port1, port2, tripSignal);
}

void s1_line2_(double* time, int* deviceEnable, double* port1, double* port2, double* tripSignal) {
    lineLinkSimulation(&s1_line2, "s1-line2", *time, *deviceEnable, port1, port2, tripSignal);
}

void s1_bus1_(double* time, int* deviceEnable, double* tripSignal) {
    busLinkSimulation(&s1_bus1, "s1-bus1", *time, *deviceEnable, tripSignal);
}












