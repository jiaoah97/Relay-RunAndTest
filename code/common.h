// common

void globalInit();

void switchRelay(Device* device, double time, double* port1, double* port2);

void linkSimulation(Device* device, char* deviceName, double time, int deviceEnable, double* port1, double* port2, double* tripSignal);

void deviceInit(Device* device, char* deviceName, int deviceEnable);
int readConfiguration(Device* device);
void initSwitchQueueTime(Device* device);
double findSetValueIndex(char* target, char (*paramName)[50], double* paramValue, int n, Device* device);

int upTo10A(Device* device);
int upTo10B(Device* device);

// 仿真采样
void sample(Device* device, double time, double* input, double* brk);

void sample2inst(Device*);

void dataFilter(Device*);

void toPhasor(Device*);


double phasorAbs(Phasor p);
Phasor phasorAdd(Phasor pa, Phasor pb);
Phasor phasorSub(Phasor, Phasor);
Phasor phasorMulti(double a, Phasor p);

Phasor phasorContrarotate(Phasor p, double angle);
Phasor phasorSeq(Phasor pa, Phasor pb, Phasor pc, int seq);
double phasorAngleDiff(Phasor pa, Phasor pb);

void lowPassFilter(double* aft, double* bef);

void inst2phasor(double* inst, int start, Phasor* phasor);

int notYet(Device* device, char* str);
unsigned int SDBMHash(char *str, int arrLength); 

void writeLog(Device* device, char* content);
void writeLogWithPhase(Device* device, char* content, int phase);
void writeErrorLog(Device* device, char* content);

Phasor memoryPhasorValue(Device* device, Phasor* memoryPhasors);

void recordData(Device* device);





