#ifndef __VERSION_H_
#define __VERSION_H_


#include "TypeDefine.h"

#define     DEVICE_HIGHCURRENT2_0    1
#define     DEVICE_HIGHVOL1_3        2
#define     DEVICE_LOWVOL            3
#define     DEVICE_HIGHVOL2_0        4
#define     DEVICE_METER             6
#define     DEVICE_LV_BOOT           20
#define     DEVICE_HC_BOOT           21

typedef struct
{
    Uint16 DSPversion;
    Uint16 FPGAversion;
    Uint16 FPGAversion_std;
    Uint16 FPGArunflag;
    Uint16 device_num;
    Uint16 initok;
}ST_VERSION;

extern ST_VERSION st_version;






extern void Init_Version(void);



  


#endif

/**************************************************************************************************
 END OF FILE (EOF)
**************************************************************************************************/
