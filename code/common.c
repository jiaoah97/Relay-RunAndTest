#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <direct.h>
#include "..\\code\\dataStruct.h"
#include "..\\code\\common.h"

// 声明各类保护函数
extern void line(Device*); // 线路保护
extern char logDirName[100];
extern int globalInitFlag;

/**
 * 全局初始化函数
 * 主要负责创建文件系统
 */
void globalInit() {
    if (globalInitFlag == 0) {
        // 根据系统时间确定日志文件名
        time_t t;
        struct tm * lt;
        time (&t);
        lt = localtime (&t);//转为时间结构

        sprintf(logDirName, "..\\\\log\\\\%04d-%02d-%02d_%02d-%02d",
                (lt->tm_year+1900), lt->tm_mon, lt->tm_mday,
                lt->tm_hour, lt->tm_min);

        // 根据时间, 在log下创建文件夹
        mkdir(logDirName);
        globalInitFlag = 1;
    }

}


/**
 * 仿真链接函数
 * 综合以下功能
 * 初始化/仿真步长设置/跳闸指令
 */
void linkSimulation(Device* device, char* deviceName, double time, int deviceEnable, double* port1, double* port2, int* tripSignal) {
    // 设置整定值
    if (notYet(device, "设置保护装置名及保护定值")) {
        deviceInit(device, deviceName, deviceEnable);
    }

    // 只有装置启用情况下才进行计算
    // 仿真程序跑10次, 进行一次采样和保护计算
    if (device->deviceEnable == 1 && upTo10(device) == 1) {
        sample(device, time, port1, port2);
        line(device);
    }

    // 结果输出
    tripSignal[0] = device->tripFlag[0]; 
    tripSignal[1] = device->tripFlag[1]; 
    tripSignal[2] = device->tripFlag[2];

}

void deviceInit(Device* device, char* deviceName, int deviceEnable){
    // 设置装置名
    if (deviceEnable == 0) {
        // 装置不启用
        device->deviceEnable = 0;
    } else {
        device->deviceEnable = 1;

        strcpy(device->deviceName, deviceName);

        // 设置globalFileName和deviceFileName
        sprintf(device->globalFileName, "%s/log.txt", logDirName); // 不同装置共用log.txt
        sprintf(device->deviceFileName, "%s/%s", logDirName, deviceName); // 不同装置录波文件分别存放, 按装置名分开

        // 读取配置文件, 设置整定值
        readConfiguration(device);

        // 初始化完毕,记录日志
        writeLog(device, "装置初始化");
    }

}

int Trim(char s[])
{
    int n;
    for (n = strlen(s) - 1; n >= 0; n--)
    {
        if (s[n] != ' ' && s[n] != '\t' && s[n] != '\n')
            break;
        s[n + 1] = '\0';
    }
    return n;
}

int readConfiguration(Device* device) {
    // 双数组 参数名-参数对应
    char paramName[PARAM_COUNT][50];
    double paramValue[PARAM_COUNT];
    char fileName[50];

    int index = 0;
    int i, j;

    // 根据device中的deviceName打开对应的配置文件并进行赋值
    sprintf(fileName, "%s%s%s", "..\\config\\", device->deviceName, ".conf");

    FILE *file = fopen(fileName, "r");
    if (file == NULL)
    {
        printf("[Error]open %s failed.\n", fileName);
        return -1;
    }

    char buf[MAX_BUF_LEN];
    int text_comment = 0;
    while (fgets(buf, MAX_BUF_LEN, file) != NULL)
    {
        Trim(buf);
        // to skip text comment with flags /* ... */
        if (buf[0] != '#' && (buf[0] != '/' || buf[1] != '/'))
        {
            if (strstr(buf, "/*") != NULL)
            {
                text_comment = 1;
                continue;
            }
            else if (strstr(buf, "*/") != NULL)
            {
                text_comment = 0;
                continue;
            }
        }
        if (text_comment == 1)
            continue;

        int buf_len = strlen(buf);
        // ignore and skip the line with first chracter '#', '=' or '/'
        if (buf_len <= 1 || buf[0] == '#' || buf[0] == '=' || buf[0] == '/')
            continue;
        buf[buf_len - 1] = '\0';

        char _paramk[MAX_KEY_LEN] = {0}, _paramv[MAX_VAL_LEN] = {0};
        int _kv = 0, _klen = 0, _vlen = 0;
        int i = 0;
        for (i = 0; i < buf_len; ++i)
        {
            if (buf[i] == ' ')
                continue;
            // scan param key name
            if (_kv == 0 && buf[i] != '=')
            {
                if (_klen >= MAX_KEY_LEN)
                    break;
                _paramk[_klen++] = buf[i];
                continue;
            }
            else if (buf[i] == '=')
            {
                _kv = 1;
                continue;
            }
            // scan param key value
            if (_vlen >= MAX_VAL_LEN || buf[i] == '#')
                break;
            _paramv[_vlen++] = buf[i];
        }
        if (strcmp(_paramk, "") == 0 || strcmp(_paramv, "") == 0)
            continue;

        sprintf(paramName[index], _paramk);
        paramValue[index] = atof(_paramv);
        index++;
        // printf("%s=%s\n", _paramk, _paramv);
    }


    device->lineStartSetValue[0] = findSetValueIndex("线路电流突变量启动", paramName, paramValue, index, device); // 电流突变量启动
    device->lineStartSetValue[1] = findSetValueIndex("线路零序电流启动", paramName, paramValue, index, device); // 零序电流启动

    device->currentDiffEnable = (int) findSetValueIndex("线路电流差动保护投入", paramName, paramValue, index, device);

    device->deltaDistanceEnable = (int) findSetValueIndex("线路工频变化量距离保护投入", paramName, paramValue, index, device);

    device->distanceEnable = (int) findSetValueIndex("线路距离保护投入", paramName, paramValue, index, device);
    device->distanceSetValue[0] = findSetValueIndex("线路距离I段", paramName, paramValue, index, device);
    device->distanceSetValue[1] = findSetValueIndex("线路距离II段", paramName, paramValue, index, device);
    device->distanceSetValue[2] = findSetValueIndex("线路距离III段", paramName, paramValue, index, device);
    device->distanceTimeSetValue[0] = findSetValueIndex("线路距离I段时间", paramName, paramValue, index, device);
    device->distanceTimeSetValue[1] = findSetValueIndex("线路距离II段时间", paramName, paramValue, index, device);
    device->distanceTimeSetValue[2] = findSetValueIndex("线路距离III段时间", paramName, paramValue, index, device);
    device->distanceTimeSetValue[3] = findSetValueIndex("线路距离保护返回时间", paramName, paramValue, index, device);

    device->zeroSequenceEnable = (int) findSetValueIndex("线路零序过电流保护投入", paramName, paramValue, index, device);

    device->overCurrentEnable = (int)findSetValueIndex("线路过电流保护投入", paramName, paramValue, index, device);
    device->overCurrentSetValue[0] = findSetValueIndex("线路过电流I段", paramName, paramValue, index, device);  // I段
    device->overCurrentSetValue[1] = findSetValueIndex("线路过电流II段", paramName, paramValue, index, device);  // II段
    device->overCurrentSetValue[2] = findSetValueIndex("线路过电流III段", paramName, paramValue, index, device);  // III段
    device->overCurrentTimeSetValue[0] = findSetValueIndex("线路过电流I段时间", paramName, paramValue, index, device); // I段延时20ms
    device->overCurrentTimeSetValue[1] = findSetValueIndex("线路过电流II段时间", paramName, paramValue, index, device); // II段
    device->overCurrentTimeSetValue[2] = findSetValueIndex("线路过电流III段时间", paramName, paramValue, index, device); // III段
    device->overCurrentTimeSetValue[3] = findSetValueIndex("线路过电流保护返回时间", paramName, paramValue, index, device); // 返回
    
    return 0;
}

/**
 * 根据target名, 返回对应的整定值的大小
 * @param target
 * @param paramName
 * @param paramValue
 * @param n
 * @return
 */
double findSetValueIndex(char* target, char (*paramName)[50], double* paramValue, int n, Device* device) {
    int i = 0;
    char errorLog[100];

    for (i = 0; i < n; i++) {
        if (strcmp(target, *(paramName+i)) == 0) {
            return paramValue[i];
        }
    }
    // 如果没有找到对应的整定值, 将错误信息记录到日志中
    sprintf(errorLog, "%s%s", target, "参数未定义!");
    writeErrorLog(device, errorLog);
    return 0;
}


int upTo10(Device* device) {
    device->sampleCount++;
    if (device->sampleCount == 10) {
        device->sampleCount = 0;
        return 1;
    } else {
        return 0;
    }
}


// 仿真采样
void sample(Device* device, double time, double* port1, double* port2) {
    int i = 0;

    // 更新装置时间
    device->time = time;

    for (i = 0; i < 6; i++) {
        // 本侧电压电流
        device->sample[i] = port1[i];
        // 对侧电压电流
        device->sample[i+6] = port2[i];
    }

    // 采样值合位为0, 开位为2 --转换为--> 合位状态为1,开位状态为0
   for (i = 0; i < 3; i++) {
       // 断路器状态不需要记忆, 随采随用即可
        device->brkStatus[i] = (int)((2-port1[i+6])/2);
        device->brkStatus[i+3] = (int)((2-port2[i+6])/2);
   }

    
}

void sample2inst(Device* device) {
    int i = 0;
    
    // 更新数据    
    for (i = RECORD_LENGTH-1; i >= 1; i--) {
        device->instTime[i] = device->instTime[i-1];
        device->instVma[i] = device->instVma[i-1];
        device->instVmb[i] = device->instVmb[i-1];
        device->instVmc[i] = device->instVmc[i-1];
        device->instIma[i] = device->instIma[i-1];
        device->instImb[i] = device->instImb[i-1];
        device->instImc[i] = device->instImc[i-1];

        device->instVna[i] = device->instVna[i-1];
        device->instVnb[i] = device->instVnb[i-1];
        device->instVnc[i] = device->instVnc[i-1];
        device->instIna[i] = device->instIna[i-1];
        device->instInb[i] = device->instInb[i-1];
        device->instInc[i] = device->instInc[i-1];
    }

    device->instTime[0] = device->time;
    device->instVma[0] = device->sample[0];
    device->instVmb[0] = device->sample[1];
    device->instVmc[0] = device->sample[2];
    device->instIma[0] = device->sample[3];
    device->instImb[0] = device->sample[4];
    device->instImc[0] = device->sample[5];

    device->instVna[0] = device->sample[6];
    device->instVnb[0] = device->sample[7];
    device->instVnc[0] = device->sample[8];
    device->instIna[0] = device->sample[9];
    device->instInb[0] = device->sample[10];
    device->instInc[0] = device->sample[11];
    
}

void dataFilter(Device* device) {
    
    int i = 0;
    // 滤波后数据后移
    for (i = WINDOW-1; i >= 1; i--) {
        device->filterVma[i] = device->filterVma[i-1];
        device->filterVmb[i] = device->filterVmb[i-1];
        device->filterVmc[i] = device->filterVmc[i-1];
        device->filterIma[i] = device->filterIma[i-1];
        device->filterImb[i] = device->filterImb[i-1];
        device->filterImc[i] = device->filterImc[i-1];

        device->filterVna[i] = device->filterVna[i-1];
        device->filterVnb[i] = device->filterVnb[i-1];
        device->filterVnc[i] = device->filterVnc[i-1];
        device->filterIna[i] = device->filterIna[i-1];
        device->filterInb[i] = device->filterInb[i-1];
        device->filterInc[i] = device->filterInc[i-1];
    }

    // 更新新的滤波后数据点
    lowPassFilter(device->filterVma, device->instVma);
    lowPassFilter(device->filterVmb, device->instVmb);
    lowPassFilter(device->filterVmc, device->instVmc);

    lowPassFilter(device->filterIma, device->instIma);
    lowPassFilter(device->filterImb, device->instImb);
    lowPassFilter(device->filterImc, device->instImc);

    lowPassFilter(device->filterVna, device->instVna);
    lowPassFilter(device->filterVnb, device->instVnb);
    lowPassFilter(device->filterVnc, device->instVnc);

    lowPassFilter(device->filterIna, device->instIna);
    lowPassFilter(device->filterInb, device->instInb);
    lowPassFilter(device->filterInc, device->instInc);
  
}

void toPhasor(Device* device) {
    inst2phasor(device->filterVma, 0, &device->phasor[0]);
    inst2phasor(device->filterVmb, 0, &device->phasor[1]);
    inst2phasor(device->filterVmc, 0, &device->phasor[2]);

    inst2phasor(device->filterIma, 0, &device->phasor[3]);
    inst2phasor(device->filterImb, 0, &device->phasor[4]);
    inst2phasor(device->filterImc, 0, &device->phasor[5]);

    inst2phasor(device->filterVna, 0, &device->phasor[6]);
    inst2phasor(device->filterVnb, 0, &device->phasor[7]);
    inst2phasor(device->filterVnc, 0, &device->phasor[8]);

    inst2phasor(device->filterIna, 0, &device->phasor[9]);
    inst2phasor(device->filterInb, 0, &device->phasor[10]);
    inst2phasor(device->filterInc, 0, &device->phasor[11]);

}


/**
 * 低通滤波函数
 * 参数分别为滤波后数组和滤波前数组
 */
void lowPassFilter(double* aft, double* bef) {
    // 滤波参数直接给出,使用bufferFilter.m计算得到 此处为48点采样,100hz截止频率
    double at = 0.014401;
    double bt = -1.632993;
    double ct = 0.690599;

    aft[0] = at*bef[0] + 2*at*bef[1] + at*bef[2] - bt*aft[1] - ct*aft[2];
}

/**
 * 全周傅式算法
 */
void inst2phasor(double* inst, int start, Phasor* phasor) {
    int i = start;
    // 因为定义的全局变量, 需要先把上次计算值清掉
    phasor->real = 0;
    phasor->img = 0;


    for (i = start; i < start + POINTS; i++) {
        phasor->real += inst[i]*sin(2*PI*(i-start)/(double)POINTS);
        phasor->img  += inst[i]*cos(2*PI*(i-start)/(double)POINTS);
    }

    // C语言语法规则: 2/400等于零!
    phasor->real = phasor->real * (2/(double)POINTS);
    phasor->img =  phasor->img * (2/(double)POINTS);
}


double phasorAbs(Phasor p) {
    return sqrt(p.real*p.real + p.img*p.img);
}

Phasor phasorAdd(Phasor pa, Phasor pb) {
    Phasor p;

    p.real = pa.real + pb.real;
    p.img = pa.img + pb.img;

    return p;
}

Phasor phasorSub(Phasor pa, Phasor pb) {
    Phasor p;

    p.real = pa.real - pb.real;
    p.img = pa.img - pb.img;

    return p;
}


/**
 * 计算常数与相量乘积
 */
Phasor phasorMulti(double a, Phasor p) {
    p.real *= a;
    p.img *= a;

    return p;
}

/**
 * 逆时针旋转相量
 * @param:角度数
 */
Phasor phasorContrarotate(Phasor p, double angle) {
    Phasor newp;
    double radian;

    radian = angle / 180 * PI;

    newp.real = p.real*cos(radian) - p.img*sin(radian);
    newp.img = p.img*cos(radian) + p.real*sin(radian);

    return newp;
}


/**
 * 序量计算
 * @param:seq指示正序/负序/零序
 */
Phasor phasorSeq(Phasor pa, Phasor pb, Phasor pc, int seq) {
    Phasor seqPhasor;
    Phasor rpa;
    Phasor rpb;
    Phasor rpc;
    Phasor sum;

    if (seq == 1) {
        // 正序
        rpa = pa;
        rpb = phasorContrarotate(pb, 120.0);
        rpc = phasorContrarotate(pc, 240.0);
    } else if (seq == 2) {
        // 负序
        rpa = pa;
        rpb = phasorContrarotate(pb, 240.0);
        rpc = phasorContrarotate(pc, 120.0);
    } else if (seq == 0) {
        // 零序
        rpa = pa;
        rpb = pb; 
        rpc = pc;
    }

    sum = phasorAdd(rpa, phasorAdd(rpb, rpc));
    return phasorMulti(1/3.0, sum);
}


/**
 * 返回相角差
 * 注意：atan(double y, double x) 参数顺序！
 */
double phasorAngleDiff(Phasor pa, Phasor pb) {
    double ang1;
    double ang2;
    double res;

    ang1 = atan2(pa.img, pa.real) * 180.0 / PI;
    ang2 = atan2(pb.img, pb.real) * 180.0 / PI;

    printf("%f\n", ang1);
    printf("%f\n", ang2);

    // 转成0~360
    ang1 = ang1 < -0.0001 ? ang1+360.0 : ang1;
    ang2 = ang2 < -0.0001 ? ang2+360.0 : ang2;

    res = ang1 - ang2;
    return res;

}


/**
 * 日志模块 
 * 参考 C语言实现写入日志文件 https://blog.csdn.net/sunlion81/article/details/8647028
 */

/**
* 写入日志文件
* @param filename [in]: 日志文件名
* @param buffer [in]: 日志内容
* @param buf_size [in]: 日志内容大小
* @return 空
*/
void writeLog(Device* device, char* content) {
    if (content != NULL && notYet(device, content)) {
        // 写日志
        {
            FILE *fp;
            fp = fopen(device->globalFileName, "at+");
            if (fp != NULL)
            { 
                fprintf(fp, "[LOG-INFO] Simulation Time: %fs [%s]: ", device->time, device->deviceName);
        
                fprintf(fp, content);
                fprintf(fp, "...OK\n");
                fclose(fp);
                fp = NULL;  
            }
        }
    }
}

/**
 * 写入错误日志信息
 * @param device
 * @param content
 */
void writeErrorLog(Device* device, char* content) {
    if (content != NULL && notYet(device, content)) {
        // 写日志
        {
            FILE *fp;
            fp = fopen(device->globalFileName, "at+");
            if (fp != NULL)
            {
                fprintf(fp, "[LOG-ERROR] Simulation Time: %fs [%s]: ", device->time, device->deviceName);

                fprintf(fp, content);
                fprintf(fp, "\n");
                fclose(fp);
                fp = NULL;
            }
        }
    }
}


/**
 * 上面日志函数的重载形式, 主要用于相别信息
*/
void writeLogWithPhase(Device* device, char* content, int phase) {
    char formatContent[128];
    char charPhase;

    // 将相别数字转换为字母
    charPhase = (char)('A'+phase);

    // 格式化字符串
    sprintf(formatContent, content, charPhase);
    
    if (formatContent != NULL && notYet(device, formatContent)) {
        // 写日志
        {
            FILE *fp;
            fp = fopen(device->globalFileName, "at+");
            if (fp != NULL)
            { 
                fprintf(fp, "[LOG-INFO] Simulation Time: %fs [%s]: ",device->time, device->deviceName);
        
                fprintf(fp, formatContent);
                fprintf(fp, "...OK\n");
                fclose(fp);
                fp = NULL;

            }
        }
    }
}


/**
 * hash算法
 * @param:需要求hash值的字符串
 * @param:数组长度
 */
unsigned int SDBMHash(char *str, int arrLength) {
    unsigned int hash = 0; 
    while (*str)
    {
        // equivalent to: hash = 65599*hash + (*str++);
        hash = (*str++) + (hash << 6) + (hash << 16) - hash;
    }
 
    return (hash & 0x7FFFFFFF)%arrLength;
}


/**
 * notYet函数
 * 功能:将该函数的返回值作为判别条件,可以保证条件语句内的
 * 语句仅执行一次, 使用方法如下:
 * if (notYet(device, "描述代码段的作用")) {
 *     // 执行语句...
 * }
 * 原理:利用"描述代码段的作用"作为该代码段的标识,通过hash计算基本确保唯一性
 */
int notYet(Device* device, char* str) {
    unsigned int hashCode;

    hashCode = SDBMHash(str, MAXSIZE);
    if (device->notYetFlag[hashCode] == 0) {
        device->notYetFlag[hashCode] = 1;
        return 1;
    } 
    return 0;    
}

/**
 * 根据当前时间寻找对应记忆量
 * 想得到谁的当前时间的记忆量, 就传入谁的记忆量数组
 */
Phasor memoryPhasorValue(Device* device, Phasor* memoryPhasors){
    double t , tFault;
    int tDelta, i;

    t = device->time;
    tFault = device->startTime;
    tDelta = (int) ((t-tFault)/0.02*POINTS);
    i = tDelta % POINTS;

    return memoryPhasors[i];
}

/**
 * 录波
 * 录波数据以文本格式输出到log/<本次仿真文件夹>/<装置名-record.txt>
 */
void recordData(Device* device) {
    int len = RECORD_LENGTH;
    char recordFileName[100];
    FILE *fp;
    int i = 0;

    sprintf(recordFileName, "%s-record.txt", device->deviceFileName);
    fp = fopen(recordFileName, "at+");

    if (fp != NULL) {
        // 标题
        fprintf(fp, "[%s]录波数据---kV/kA\n", device->deviceName);
        fprintf(fp, "======TIME===========Va===========Vb============Vc=============Ia============Ib=============Ic=====\n");

        for (i = len-1; i >= 0; i--) {
            fprintf(fp, "%12.6f, %12.6f, %12.6f, %12.6f, %12.6f, %12.6f, %12.6f\n",
                    device->instTime[i],
                    device->instVma[i], device->instVmb[i], device->instVmc[i],
                    device->instIma[i], device->instImb[i], device->instImc[i]);
        }
        fclose(fp);
        fp = NULL;
    }
}


