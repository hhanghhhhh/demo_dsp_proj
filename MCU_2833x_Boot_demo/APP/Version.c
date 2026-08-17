#include "Version.h"
#include <string.h>

ST_VERSION st_version;

void Init_Version(void)
{
    ST_VERSION *p = &st_version;
    memset(&st_version, 0, sizeof(ST_VERSION));

    p->DSPversion =      1;   //
    p->FPGAversion_std = 1;   //     //相匹配的FPGA版本
    p->device_num = DEVICE_LV_BOOT;
}

