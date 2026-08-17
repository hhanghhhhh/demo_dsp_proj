#include "drv_GlobalVar.h"
#include <string.h>
#include "DSP2833x_Device.h"     // Header file Include File
#include "DSP2833x_Examples.h"   // Examples Include File
/***********************************************************************
* Function Name : Variable definition
***********************************************************************/
extern void mgmd_vInit(void);
extern void Init_W5500(void);
extern void InitEeromPara_Downloads(void);
extern void Init_Version(void);
/***********************************************************************
* Function Name : InitUserPara
* Arguments     :
* Returns       :
* Description   : Initial the control parameter
* Updated ID    :
***********************************************************************/
void InitUserPara(void)
{

    mgmd_vInit();
    Init_Version();
    //initeerpm 函数放在最后初始化，里边有赋值的操作
    InitEeromPara_Downloads();
    //init W5500必须放在InitEeromPara后边
    Init_W5500();
}


extern void ServiceDog(void);
//void DisableDog(void)   禁用看门狗
//void ServiceDog(void)   喂狗
//使能看门狗
void EnableWDog(void)
{
    //清零计数
//    ServiceDog();
//    //使能看门狗并且设置频率，使其间隔为280ms
//    EALLOW;
//    SysCtrlRegs.WDCR = 0x002F;  //   2F---280ms  0x0028--4ms;  最长280ms 最短4ms
//    EDIS;
}
/******************************************************************************
* == END OF MODULE/FILE (EOF) ==
******************************************************************************/
