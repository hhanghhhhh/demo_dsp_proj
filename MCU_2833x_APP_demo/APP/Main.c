#include "drv_GlobalVar.h"
#include "Interrupt.h"
#include "W5500/drv_Spi.h"
#include "W5500/udp.h"
#include "task_eeprom_param.h"
#include "app_boot_eeprom.h"
#include "app_boot.h"
#include "DSP2833x_Device.h"   // Header file Include File
#include "DSP2833x_Examples.h" // Examples Include File
#include "drv_ModbusData.h"
#include "drv_Fpga.h"
#include "Version.h"
#include "task_scope.h"
#include "validation/task_scope_validation.h"

// XINTF 临时验证代码；验证完成后删除本 include 和两个调用点。
#include "validation/xintf_validation_test.h"

extern void HH_test_main();
extern void LED_Ctrl();
void Init_ADC_DMA(void);

void PieInit(void)
{
    DINT;
    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    InitPieVectTable();
    EALLOW;
    PieVectTable.TINT0 = &INT6;
    PieVectTable.ECAN0INTB = &ISR_CanbInt0;
    EDIS;
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
    PieCtrlRegs.PIEIER9.bit.INTx7 = 1; // CANbINT0
    IER |= M_INT1;
    IER |= M_INT9; // CANbINT0
}

void TimerandIntCfg(void)
{
    ConfigCpuTimer(&CpuTimer0, 150, SYS_PERIOD);
    EINT;
    ERTM;
    StartCpuTimer0();
}

/***********************************************************************
 * Function Name : main
 * Arguments     :
 * Returns       :
 * Description   : main function
 * Updated ID    :
 ***********************************************************************/
Uint32 hh_main_cnt = 0;
void main(void)
{
    Uint16 index = 0;
    /*======================= MEM COPY FLASH TO RAM =========================*/
    //  MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
    //  InitFlash();
    /*=======================================================================*/
    InitSysCtrl();
    PieInit();
    InitGpio();
    InitSpiaGpio();
    InitSpi();
    //    InitECanGpio();
    //    InitECan();
    InitI2CGpio();
    InitI2C();
    InitAdc();
    Init_ADC_DMA();
    InitCpuTimers();
    InitXintf();
    DELAY_US(0.4);

    // USER Init
    InitUserPara();
    XintfValidationInit();

    /* 数字示波器验证：采集 ISR 中产生的递增斜坡。 */
    DSO_ValidationInit();

    TimerandIntCfg();
    EnableWDog();

    while (1)
    {
        FpgaMainReadUpdate();
        XintfValidationProcess();

        for (index = 0; index < SOCKET_NUM_USE; index++)
        {
            // 第一个参数为socket号，直接用0 1 2...完后排列  最多8个
            do_tcp_server(index, index); // 网口模块
        }

        // socket7用来做udp广播
        do_udp(7);

        CheckW5500Status();

        if (mgmd_stSCIRx.jump_cmd == JUMP_TO_BOOT)
        {
            mgmd_stSCIRx.jump_cmd = 0;
            // 将下载标志写入eeprom
            g_app_boot_eeprom_param.download_flag = APP_BOOT_DOWNLOAD_FLAG;
            DisableDog();
            AppBootEeprom_Save();
            EnableWDog();
            load_boot();
        }

        EepromParam_Process();

        Run_Version();
        FpgaMainWriteUpdate();
        ServiceDog();
        hh_main_cnt++;
    }
}

/********************************************************************
 *			END, do not code behind this line!!                            *
 ****************************************************************************/
