#include "drv_GlobalVar.h"
#include "Interrupt.h"
#include "W5500/drv_Spi.h"
#include "W5500/udp.h"
#include "DSP2833x_Device.h"     // Header file Include File
#include "DSP2833x_Examples.h"   // Examples Include File
#include "Flash.h"
#include "drv_ModbusData.h"
#include "task_eeprom_download.h"

void TimerandIntCfg(void)
{
    DINT;
    DRTM;

    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    InitPieVectTable();

    EALLOW;
    PieVectTable.TINT0 = &INT6;
    EDIS;

    /* 先清除挂起标志，再开放中断；Timer0 由 main 最后启动。 */
    ConfigCpuTimer(&CpuTimer0, 150, SYS_PERIOD);
    CpuTimer0Regs.TCR.bit.TIF = 1;
    PieCtrlRegs.PIEIFR1.all = 0x0000;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
    IER |= M_INT1;
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;
}


/***********************************************************************
* Function Name : main
* Arguments     : 
* Returns       :
* Description   : main function
* Updated ID    : 
***********************************************************************/
Uint32 hh_main_cnt = 0;

Uint16 write_cmd = 0;
Uint16 write_num_u16 = 0;
void main(void)
{
    DINT;
    DRTM;
    Uint16 index = 0;
    Uint16 rst = 1;
    Uint16 i = 0;
    /*======================= MEM COPY FLASH TO RAM =========================*/
    MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
    MemCopy(&Flash28_API_LoadStart, &Flash28_API_LoadEnd,&Flash28_API_RunStart);
    InitFlash();
    FlashAPI_Init();
    /*=======================================================================*/
    InitSysCtrl();
    InitGpio();
    InitI2CGpio();
    InitI2C();
    InitSpiaGpio();
    InitSpi();
    InitCpuTimers();
    //某些特殊 IO 动作


    //USER Init
    InitUserPara();
    TimerandIntCfg();
    EINT;
    ERTM;
    StartCpuTimer0();
    while (1)
    {
        for(index = 0; index < SOCKET_NUM_USE; index++)
        {
            //第一个参数为socket号，直接用0 1 2...完后排列  最多8个
            do_tcp_server(index, index);   //网口模块
        }

        //socket7用来做udp广播
        do_udp(7);

        CheckW5500Status();

        if(mgmd_stSCIRx.flash_cmd == FLASH_ERASE)
        {
            mgmd_stSCIRx.flash_cmd = 0;
            mgmd_stSCIRx.flash_ok = 0;
            mgmd_stSCIRx.crc = 0;
            DINT;
            DRTM;
            rst = Flash_Erase_App();
            EINT;
            ERTM;
            if(rst == 0)    mgmd_stSCIRx.flash_ok = 1;
        }

        if(write_cmd == 1)
        {
            write_cmd = 0;
            mgmd_stSCIRx.flash_ok = 0;
            for(i = 0; i < write_num_u16; i++)
            {
                mgmd_stSCIRx.crc += mgmd_stSCIRx.data[i];
            }
            DINT;
            DRTM;
            rst = Flash_WR(mgmd_stSCIRx.addr.u32, mgmd_stSCIRx.data, write_num_u16);
            EINT;
            ERTM;
            if(rst == 0)    mgmd_stSCIRx.flash_ok = 1;
        }

        if(mgmd_stSCIRx.jump_cmd == JUMP_TO_APP)
        {
            mgmd_stSCIRx.jump_cmd = 0;
            // 清除下载标志，后续上电可直接进入 APP
            WriteEeromPara_Downloads(0);
            Close_All_Socket();
            Boot_ResetToApplication();
        }

        hh_main_cnt++;
    }
}

/********************************************************************
*			END, do not code behind this line!!                            *
****************************************************************************/


