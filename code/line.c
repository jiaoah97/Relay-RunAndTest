#include "..\\code\\dataStruct.h"
#include "..\\code\\common.h"

extern void lineStarter(Device* device, int phase);
extern void deltaDistanceRelay(Device* device, int phase);
extern void distanceRelay(Device* device, int phase);
extern void overCurrentRelay(Device* device, int phase);
extern void currentDiffRelay(Device* device, int phase);
extern void zeroSeqCurrentRelay(Device* device, int phase);


void line(Device* device) {

    int phase = 0;

    // 将采样值存入瞬时值数组
    sample2inst(device);
    
    // 瞬时值滤波后存入并更新滤波后数组
    dataFilter(device);
    
    // 利用滤波后数据计算12通道相量,存入phasor数组
    toPhasor(device);

    /**
     * 启动判据
     * 只有保护没有启动才进入判别, 只要有一种启动判据动作置位,
     * 就不再进行启动判别.
     */

    for (phase = 0; phase < 3; phase++) {
        if (device->startFlag == 0) {
            lineStarter(device, phase);
        }
    }
    

    /**
     * 保护主判据, 使用计算得到的相量进行相关保护逻辑的实现
     */

    /**
     * 差动保护
     */
    if (device->currentDiffEnable == 1) {
        for (phase = 0; phase < 3; phase++) {
            if (device->startFlag == 1) {
                currentDiffRelay(device, phase);
            }
        }
    }

    /**
     * 工频变化量距离保护
     */
    if (device->deltaDistanceEnable == 1) {
        for (phase = 0; phase < 3; phase++) {
            if (device->startFlag == 1) {
                deltaDistanceRelay(device, phase);
            }
        }
    }


    /**
     * 距离保护
     */
    if (device->distanceEnable == 1) {
        for (phase = 0; phase < 3; phase++) {
            if (device->startFlag == 1) {
                distanceRelay(device, phase);
            }
        }
    }

    /**
     * 零序电流保护
     */
    if (device->zeroSequenceEnable == 1) {
        for (phase = 0; phase < 3; phase++) {
            if (device->startFlag == 1) {
                zeroSeqCurrentRelay(device, phase);
            }
        }
    }

    /**
     * 过电流保护
     */
    if (device->overCurrentEnable == 1) {
        for (phase = 0; phase < 3; phase++) {
            if (device->startFlag == 1) {
                overCurrentRelay(device, phase);
            }
        }
    }

    // 综合各保护动作情况,对tripFlag置位
    for (phase = 0; phase < 3; phase++) {
        if (device->overCurrentTripFlag[phase] == 1 || device->distanceTripFlag[phase] == 1) {
            device->tripFlag[phase] = 1;
        }
    }

    /**
     * 保护返回
     * 启动后4s后, 执行返回逻辑
     */
    if (device->startFlag == 1 && device->time - device->startTime > 4) {
        // 启动标志位置零
        device->startFlag = 0;
        // 各个保护标志位置零, 有必要设置各个保护跳闸标志位字段吗?

        // 跳闸标志位置零
        for (phase = 0; phase < 3; phase++) {
            device->tripFlag[phase] = 0;
        }
        writeLog(device, "保护返回");
    }



    /**
     * 录波模块 启动后7个周波后, 将instIma一次写入
     */
    if ((device->startFlag == 1) && ((device->time-device->startTime)/0.02 > 7) && notYet(device, "数据录波")) {
        writeLog(device, "装置数据录波");
        recordData(device);
    }

    // 测试
    /*
        device->tripFlag[1] = device->phasor[0].real;
        device->tripFlag[2] = memoryPhasorValue(device, device->memoryVma).real;
    */



    
    
}