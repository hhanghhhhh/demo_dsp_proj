#ifndef __APP_BOOT_H_
#define __APP_BOOT_H_

#include "TypeDefine.h"

/* 处理上位机升级请求，保存标志后复位进入Boot。 */
void AppBoot_Process(void);

void AppBoot_ResetToBoot(void);

#endif
