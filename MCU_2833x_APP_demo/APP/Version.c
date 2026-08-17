#include "Version.h"
#include <string.h>
#include "drv_fpga.h"
#include "build_info.h"

ST_VERSION st_version;

void Init_Version(void)
{
    ST_VERSION *p = &st_version;
    memset(&st_version, 0, sizeof(ST_VERSION));

    p->DSPversion =      1;   //
    p->FPGAversion_std = 1;   //     //相匹配的FPGA版本
    p->device_num = DEVICE_HTFB_SOURCE;
}


void Run_Version(void)
{
    ST_VERSION *p = &st_version;

    p->dsp_bulid_time.u32 = BUILD_UNIX_TIME;
    p->FPGArunflag = (st_fpga_main_rd.fpga_info.bit.value123 == 123) ? 1 : 0;
    p->FPGAversion = st_fpga_main_rd.fpga_info.bit.version;
}

