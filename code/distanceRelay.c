#include "..\\code\\dataStruct.h"
#include "..\\code\\common.h"

/**
 * 距离保护算法
 * 接地距离保护 + 相间距离保护
 * @param device
 * @param phase
 */

/**
 * 以下仅为Demo, 需要重写, 包括接地距离+相间距离
 * 说明:
 * 1.整定值如果在dataStruct.h未定义, 暂时直接写在代码中, 到后期再进行统一
 * 2.函数具体定义可通过ctrl+<单击>查看
 * 3.该保护函数按相调用, 对于单相phase=0代表A相, 对于相间phase=0代表AB相
 * 4.对于不需要按相调用的保护, 一次判别故障后三相Flag置位即可
 */


void distanceRelay(Device* device, int phase) {
    double startTime = device->startTime;
    double time = device->time;
    // 整定值
    double Z1set = device->distanceSetValue[0];
    double Z2set = device->distanceSetValue[1];
    double Z3set = device->distanceSetValue[2];

    double t1set = device->distanceTimeSetValue[0];
    double t2set = device->distanceTimeSetValue[1];
    double t3set = device->distanceTimeSetValue[2];

    Phasor U, I;
    Phasor x, y;

    // 参数本地化
    U = device->phasor[0+phase];
    I = device->phasor[3+phase];


    // I段
    x = phasorSub(U, phasorNumMulti(0.5*Z1set, I));
    y = phasorNumMulti(0.5*Z1set, I);

    if ((time-startTime) > t1set && (phasorAbs(x) - phasorAbs(y)) < 0 ) {
        device->distanceTripFlag[phase] = 1;
        writeLogWithPhase(device, "%c相距离保护I段动作", phase);
    }

 
    // II段
    x = phasorSub(U, phasorNumMulti(0.5*Z2set, I));
    y = phasorNumMulti(0.5*Z2set, I);

    if ((time-startTime) > t2set && (phasorAbs(x) - phasorAbs(y)) < 0 ) {
        device->distanceTripFlag[phase] = 1;
        writeLogWithPhase(device, "%c相距离保护II段动作", phase);
    }


    // III段
    x = phasorSub(U, phasorNumMulti(0.5*Z3set, I));
    y = phasorNumMulti(0.5*Z3set, I);

    if ((time-startTime) > t3set && (phasorAbs(x) - phasorAbs(y)) < 0 ) {
        device->distanceTripFlag[phase] = 1;
        writeLogWithPhase(device, "%c相距离保护III段动作", phase);
    }

}