#include "..\\code\\dataStruct.h"
#include "..\\code\\common.h"

/**
 * 电流差动继电器
 * 由line函数按相调用，对于相间故障phase=0，代表AB相间，以此类推
 */
void currentDiffRelay(Device* device, int phase) {
    writeLog(device, "电流差动保护测试");
}