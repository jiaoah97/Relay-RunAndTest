#include "..\\code\\dataStruct.h"
#include "..\\code\\common.h"

/**
 * 零序电流继电器
 * 由line函数按相调用，对于相间故障phase=0，代表AB相间，以此类推
 */
void zeroSeqCurrentRelay(Device* device, int phase) {
    writeLog(device, "零序电流保护测试");
}