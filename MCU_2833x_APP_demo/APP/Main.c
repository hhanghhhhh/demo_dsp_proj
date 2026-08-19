#include "drv_GlobalVar.h"
#include "Main.h"
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



void Init_ADC_DMA(void);


static void Main_InitInterruptController(void)
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
}

static void Main_EnableInterruptSources(void)
{
    /* 配置外设期间可能产生标志，开放中断前统一清除。 */
    PieCtrlRegs.PIEIFR1.all = 0U;
    PieCtrlRegs.PIEIFR9.all = 0U;
    PieCtrlRegs.PIEACK.all = 0xFFFFU;
    IFR = 0x0000U;

    /* 所有ISR和外设完成配置后，再统一开放PIE与CPU中断组。 */
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1U;
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1U;
    PieCtrlRegs.PIEIER9.bit.INTx7 = 1U; // CANbINT0
    IER |= M_INT1;
    IER |= M_INT9; // CANbINT0
}

static void Main_ConfigCpuTimer0(void)
{
    ConfigCpuTimer(&CpuTimer0, 150, SYS_PERIOD);
    CpuTimer0Regs.TCR.bit.TIF = 1U;
    CpuTimer0Regs.TCR.bit.TRB = 1U;
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
    Main_InitInterruptController();
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

    Main_ConfigCpuTimer0();
    Main_EnableInterruptSources();

    /* 中断接收端先就绪，AD7982 ePWM与CPU Timer作为触发源最后启动。 */
    ERTM;
    EINT;
    StartCpuTimer0();
    EnableWDog();

    while (1)
    {
        FpgaMainReadUpdate();

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
            /* 升级标志保存成功后，通过看门狗复位进入Boot。 */
            g_app_boot_eeprom_param.download_flag = APP_BOOT_DOWNLOAD_FLAG;
            DisableDog();
            if (AppBootEeprom_Save() == 0U)
            {
                AppBoot_ResetToBoot();
            }
            else
            {
                g_app_boot_eeprom_param.download_flag = APP_BOOT_DOWNLOAD_CLEAR;
                EnableWDog();
            }
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
