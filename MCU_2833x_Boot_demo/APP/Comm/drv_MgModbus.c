#include "drv_MgModbus.h"
#include "drv_ModbusData.h"

/**************************************************************************************************
CONSTANT DESCRIPTION                                                               
**************************************************************************************************/
Uint16 mgmd_u16SlaveID = 1;
Uint16 mgmd_u16RxReady = 0;
Uint16 mgmd_u16Urt1BytesReceived = 0;


ST_SCI_RX_PAR    mgmd_stSCIRx;
extern void InitModbusData();
/**************************************************************************************************
EXPORTED FUNCTIONS
**************************************************************************************************/
/**************************************************************************************************
FUNCTION: mgmd_vInit
DESCRIPTION: mgcc modbus init
**************************************************************************************************/

void mgmd_vInit(void)
{
   //prepare data 
   md_vPrepareData();

   mgmd_u16SlaveID = 0x01;
   mgmd_u16RxReady = 0;
   mgmd_u16Urt1BytesReceived = 0;

   InitModbusData();
}


// 读取数据并填充到响应缓冲区
static Uint16 ReadRegisters(Uint16* pTxdata, Uint16 addr, Uint16 regnum, Uint16* buffer[], Uint16 startaddr, Uint16 endaddr, Uint16* sendnum)
{
    Uint16 i = 0;
    Uint16 addr_offset = 0;
    Uint16 value = 0;
    Uint16 rst = 0;

    if ((addr >= startaddr) && (addr <= endaddr) && ((addr + regnum - 1) <= endaddr))
    {
        for (i = 0; i < regnum; i++)
        {
            addr_offset = addr - startaddr + i;
            value = *buffer[addr_offset];
            pTxdata[(*sendnum)++] = (value >> 8) & 0x00FF;
            pTxdata[(*sendnum)++] = value & 0x00FF;
        }
        rst = 1;
    }

    return rst;
}

// 写单个寄存器到对应的缓冲区
static Uint16 WriteSingleRegister(Uint16 addr, Uint16 value, Uint16* buffer[], Uint16 startaddr, Uint16 endaddr)
{
    Uint16 addr_offset;
    Uint16 rst = 0;
    if ((addr >= startaddr) && (addr <= endaddr))
    {
        addr_offset = addr - startaddr;
        *buffer[addr_offset] = value;
        rst = 1;
    }

    return rst;
}

// 写多个寄存器到对应的缓冲区
static Uint16 WriteMultipleRegisters(Uint16 addr, Uint16 regnum, Uint16* pRxdata, Uint16* buffer[], Uint16 startaddr, Uint16 endaddr)
{
    Uint16 i;
    Uint16 addr_offset;
    Uint16 value;
    Uint16 value_H;
    Uint16 value_L;
    Uint16 rst = 0;

    if ((addr >= startaddr) && (addr <= endaddr) && ((addr + regnum - 1) <= endaddr))
    {
        addr_offset = addr - startaddr;
        for (i = 0; i < regnum; i++)
        {
            value_H = pRxdata[MD_DATA_OFFSET + i * 2];
            value_L = pRxdata[MD_DATA_OFFSET + i * 2 + 1];
            value = (value_H << 8) | value_L;
            *buffer[addr_offset + i] = value;
        }
        rst = 1;
    }

    return rst;
}

//错误回复
static void AddrErrHandle(Uint16* pTxdata, Uint16 funcode, Uint16* sendnum)
{
    *sendnum = 6;
    pTxdata[(*sendnum)++] = mgmd_u16SlaveID;
    pTxdata[(*sendnum)++] = funcode + 0x80;
    pTxdata[(*sendnum)++] = 0x02;
}

static Uint16 MsgHandle(Uint16* pRxdata, Uint16* pTxdata)
{
   Uint16 u16RegValue = (pRxdata[10] << 8) | pRxdata[11];
   Uint16 funcode = pRxdata[7];
   Uint16 startaddr = (pRxdata[8] << 8) | pRxdata[9];
   Uint16 regnum = (pRxdata[10] << 8) | pRxdata[11];
   Uint16 sendnum = 6;
   Uint16 rmulti_rst = 0;
   Uint16 wsigle_rst = 0;
   Uint16 wmulti_rst = 0;
   Uint16 i = 0;

   switch(funcode)
   {
        case READ_MULT:
            pTxdata[sendnum++] = mgmd_u16SlaveID;
            pTxdata[sendnum++] = funcode;
            pTxdata[sendnum++] = regnum*2;

            if(rmulti_rst == 0)     rmulti_rst = ReadRegisters(pTxdata, startaddr, regnum, md_r1_buf, MD_R1_START_ADDR, MD_R1_END_ADDR, &sendnum);
            if(rmulti_rst == 0)     rmulti_rst = ReadRegisters(pTxdata, startaddr, regnum, md_r2_buf, MD_R2_START_ADDR, MD_R2_END_ADDR, &sendnum);
            if(rmulti_rst == 0)     rmulti_rst = ReadRegisters(pTxdata, startaddr, regnum, md_rw1_buf, MD_RW1_START_ADDR, MD_RW1_END_ADDR, &sendnum);
            if(rmulti_rst == 0)     rmulti_rst = ReadRegisters(pTxdata, startaddr, regnum, md_rw2_buf, MD_RW2_START_ADDR, MD_RW2_END_ADDR, &sendnum);
            if(rmulti_rst == 0)     rmulti_rst = ReadRegisters(pTxdata, startaddr, regnum, md_w1_buf, MD_W1_START_ADDR, MD_W1_END_ADDR, &sendnum);
            break;

        case WRITE_SINGLE:
            //organize tx data
            for(i = 6; i <= 11; i++)
            {
                pTxdata[sendnum++] = pRxdata[i];
            }

            // 写单个寄存器
            if(wsigle_rst == 0)     wsigle_rst = WriteSingleRegister(startaddr, u16RegValue, md_rw1_buf, MD_RW1_START_ADDR, MD_RW1_END_ADDR);
            if(wsigle_rst == 0)     wsigle_rst = WriteSingleRegister(startaddr, u16RegValue, md_rw2_buf, MD_RW2_START_ADDR, MD_RW2_END_ADDR);
            if(wsigle_rst == 0)     wsigle_rst = WriteSingleRegister(startaddr, u16RegValue, md_w1_buf, MD_W1_START_ADDR, MD_W1_END_ADDR);
            break;

        case WRITE_MULT:
            for(i = 6; i <= 11; i++)
            {
                pTxdata[sendnum++] = pRxdata[i];
            }

            // 写多个寄存器
            if(wmulti_rst == 0)     wmulti_rst = WriteMultipleRegisters(startaddr, regnum, pRxdata, md_rw1_buf, MD_RW1_START_ADDR, MD_RW1_END_ADDR);
            if(wmulti_rst == 0)     wmulti_rst = WriteMultipleRegisters(startaddr, regnum, pRxdata, md_rw2_buf, MD_RW2_START_ADDR, MD_RW2_END_ADDR);
            if(wmulti_rst == 0)     wmulti_rst = WriteMultipleRegisters(startaddr, regnum, pRxdata, md_w1_buf, MD_W1_START_ADDR, MD_W1_END_ADDR);
           break;
        default:
           break;
   }

   //错误处理，地址无效
   if(rmulti_rst == 0 && wsigle_rst == 0 && wmulti_rst == 0)
       AddrErrHandle(pTxdata, funcode, &sendnum);

   return sendnum;
}

/**************************************************************************************************
FUNCTION: mgmd_vPollTb1
DESCRIPTION: mgcc modbus poll
**************************************************************************************************/
extern Uint16 write_cmd;
extern Uint16 write_num_u16;
void ModbusTCPFunc(Uint16* prxdata, Uint16* ptxdata, Uint16 socket, Uint16 (*fsendp)(Uint8, const Uint8*, Uint16))
{
   int i;
   Uint16 sendnum = 0;

   //reply to received frame header(the first four byte)
   for(i = 0; i < 4; i++)
   {
       ptxdata[i] = prxdata[i];
   }

   //for compatibility
   mgmd_stSCIRx.reserve = 0;

   sendnum = MsgHandle(prxdata, ptxdata);

   // 判断上位机是否发送写flash的数据
   Uint16 funcode = prxdata[7];
   Uint16 startaddr = (prxdata[8] << 8) | prxdata[9];
   Uint16 regnum = (prxdata[10] << 8) | prxdata[11];
   if(funcode == 0x10 && startaddr == 0x7010 && regnum > 2)
   {
       write_cmd = 1;
       write_num_u16 = regnum - 2;
   }

   //lenth
   ptxdata[4] = ((sendnum-6) >> 8) & 0x00FF;
   ptxdata[5] = ((sendnum-6) & 0x00FF);

   //send
   fsendp(socket, (Uint8 *)ptxdata, sendnum);
}


/**************************************************************************************************
 END OF FILE (EOF)
**************************************************************************************************/
