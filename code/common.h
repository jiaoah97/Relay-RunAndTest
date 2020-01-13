// common

void globalInit();

void switchRelay(Device* device, double time, const double* port1, const double* port2);

void lineLinkSimulation(Device* device, char* deviceName, double time, int deviceEnable, double* port1, double* port2, double* tripSignal);
void busLinkSimulation(Device* device, char* deviceName, double time, int deviceEnable, double* tripSignal);


void deviceInit(Device* device, char* deviceName, int deviceEnable);
int readConfiguration(Device* device, char elementType);
void initSwitchQueueTime(Device* device);
double findSetValueIndex(char* target, char (*paramName)[STRING_LENGTH], double* paramValue, int n, Device* device);

int upTo10A(Device* device);
int upTo5(Device* device);

// 仿真采样
void sample(Device* device, double time, double* input, double* brk);

void sample2inst(Device*);

void dataFilter(Device*);

void toPhasor(Device*);

Phasor phasorAdd(Phasor pa, Phasor pb);
Phasor phasorSub(Phasor pa, Phasor pb);
Phasor phasorMulti(Phasor pa, Phasor pb);
Phasor phasorDiv(Phasor pa, Phasor pb);

double phasorAbs(Phasor p);
double phasorAngle(Phasor p);
Phasor phasorNumMulti(double a, Phasor p);
double phasorAngleDiff(Phasor pa, Phasor pb);

Phasor phasorContrarotate(Phasor p, double angle);
Phasor phasorSeq(Phasor pa, Phasor pb, Phasor pc, int seq);

void lowPassFilter(double* aft, double* bef);

void inst2phasor(double* inst, int start, Phasor* phasor);

int notYet(Device* device, char* str);
unsigned int SDBMHash(char *str, int arrLength); 

void writeLog(Device* device, char* content);
void writeLogWithPhase(Device* device, char* content, int phase);
void writeErrorLog(Device* device, char* content);

Phasor memoryPhasorValue(Device* device, Phasor* memoryPhasors);

void recordData(Device* device);





