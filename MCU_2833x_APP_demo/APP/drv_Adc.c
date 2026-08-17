#include "DSP2833x_Adc.h"
#include "drv_Adc.h"
#include "DSP2833x_DMA.h"
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
/*=============== ADC sample results () ============================*/
ST_ADC_BUFFER_VAL      adc_stBuf;
ST_ADC_SAMPPLE_RESULT  adc_stResult;

volatile Uint16 g_AdcRawBuf[16];

void Init_ADC_DMA(void)
{
    EALLOW; // DMA 寄存器受保护

    // 1. DMA 全局复位
    DmaRegs.DMACTRL.bit.HARDRESET = 1;
    asm(" NOP"); // 延时等待复位

    // -------------------------------------------------------------
    // 配置 DMA 通道 1 (CH1)
    // -------------------------------------------------------------

    // 2. 触发源选择
    // PERINTSEL = 1 对应 SEQ1INT (ADC Sequence 1 Interrupt)
    DmaRegs.CH1.MODE.bit.PERINTSEL = 1;
    DmaRegs.CH1.MODE.bit.PERINTE = 1;    // 使能外设中断触发
    DmaRegs.CH1.MODE.bit.CONTINUOUS = 1; // 传输结束后重新初始化，准备下一次

    // 3. 突发传输 (Burst) 设置
    // 每次触发搬运 16 个字 (N-1 = 15)
    DmaRegs.CH1.BURST_SIZE.bit.BURSTSIZE = 15;
    DmaRegs.CH1.SRC_BURST_STEP = 1;      // 源地址递增：RESULT0 -> RESULT1...
    DmaRegs.CH1.DST_BURST_STEP = 1;      // 目的地址递增：Buf[0] -> Buf[1]...

    // 4. 传输 (Transfer) 设置
    // 这里我们希望 1 次触发就完成所有搬运，所以 Transfer Size 设为 0 (1 burst/transfer)
    DmaRegs.CH1.TRANSFER_SIZE = 0;

    // 当 TRANSFER_SIZE = 0 时，TRANSFER_STEP 不起作用，
    // 因为每次 Transfer 结束，Shadow 寄存器会自动重载 Active 寄存器，实现地址回绕。
    DmaRegs.CH1.SRC_TRANSFER_STEP = 0;
    DmaRegs.CH1.DST_TRANSFER_STEP = 0;

    // 5. 地址设置 (关键！)
    // 源地址：指向 AdcMirror 区域的 RESULT0
    DmaRegs.CH1.SRC_ADDR_SHADOW = (Uint32)&AdcMirror.ADCRESULT0;
    // 目的地址：指向我们的 RAM 数组
    DmaRegs.CH1.SRC_BEG_ADDR_SHADOW = (Uint32)&AdcMirror.ADCRESULT0; // 如果需要 Reset 指针
    DmaRegs.CH1.DST_ADDR_SHADOW = (Uint32)&g_AdcRawBuf[0];
    DmaRegs.CH1.DST_BEG_ADDR_SHADOW = (Uint32)&g_AdcRawBuf[0];

    // 6. 启动 DMA 通道
    // 注意：DMA 准备好接收触发信号，而不是立即搬运
    DmaRegs.CH1.CONTROL.bit.RUN = 1;

    EDIS;
}



/***********************************************************************
* Function Name : GetAdc
* Arguments     :
* Returns       :
* Description   : Read ADC value, include the calibrated offset part
* Updated ID    :
***********************************************************************/
// 根据实际情况配置通道数、采样率，采样率越低值越稳、时间越长
// 确保所有通道采样完，时间小于定时器中断
// 注意采样通道之间的延时是否能接受
// 10 路总共 4.4us
void InitAdc(void);


void GetAdc(void)
{
    // DMA 已将 adc 采样数据搬运到 g_AdcRawBuf
    // 直接将其转化为真实值即可







    //触发下一次采样
    AdcRegs.ADCTRL2.bit.SOC_SEQ1 = 1;
}



/******************************************************************************
* == END OF MODULE/FILE (EOF) ==
******************************************************************************/
