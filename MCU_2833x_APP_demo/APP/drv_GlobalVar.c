#include "drv_GlobalVar.h"
#include "drv_Adc.h"
#include <string.h>
#include "task_eeprom_param.h"
#include "DSP2833x_Device.h"     // Header file Include File
#include "DSP2833x_Examples.h"   // Examples Include File
/***********************************************************************
* Function Name : Variable definition
***********************************************************************/
void mgmd_vInit(void);
void Init_Version(void);
void Init_W5500(void);
void FpgaDrvInit(void);
/***********************************************************************
* Function Name : InitUserPara
* Arguments     :
* Returns       :
* Description   : Initial the control parameter
* Updated ID    :
***********************************************************************/
void InitUserPara(void)
{
    memset(&adc_stBuf,0,sizeof(ST_ADC_BUFFER_VAL));
    memset(&adc_stResult,0,sizeof(ST_ADC_SAMPPLE_RESULT));

    mgmd_vInit();
    Init_Version();
    FpgaDrvInit();

    //initeerpm 函数放在最后初始化，里边有赋值的操作
    EepromParam_Init();
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
    ServiceDog();
    //使能看门狗并且设置频率，使其间隔为280ms
    EALLOW;
    SysCtrlRegs.WDCR = 0x002F;  //   2F---280ms  0x0028--4ms;  最长280ms 最短4ms
    EDIS;
}
/******************************************************************************
* == END OF MODULE/FILE (EOF) ==
******************************************************************************/
