#include "task_boot_eeprom.h"
#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"
#include <string.h>

#define I2C_SUCCESS 0x0000

APP_BOOT_EEPROM_PARAM g_boot_eeprom_param;
Uint16 g_boot_eeprom_status = EEROM_DATAOK;

static Uint16 s_boot_eeprom_byte_buf[APP_BOOT_EEPROM_WORDS * 2];
static Uint16 s_boot_eeprom_inited = 0;

extern Uint8 local_lip[4];

/* 将一个 Uint16 拆成两个 EEPROM byte，低字节在前。 */
static void BootEeprom_WordToBytes(Uint16 word, Uint16 *bytes)
{
    bytes[0] = word & 0x00FF;
    bytes[1] = (word >> 8) & 0x00FF;
}

/* 将两个 EEPROM byte 合成一个 Uint16，低字节在前。 */
static Uint16 BootEeprom_BytesToWord(Uint16 *bytes)
{
    return (bytes[0] & 0x00FF) | ((bytes[1] & 0x00FF) << 8);
}

/* 按 Uint16 word 读取 boot 共享参数块。 */
static Uint16 BootEeprom_ReadWords(Uint16 start, Uint16 *buf, Uint16 word_len)
{
    Uint16 i = 0;
    Uint16 rst = 0;

    rst = RDE2ROM(s_boot_eeprom_byte_buf, start, word_len * 2);
    if (rst != I2C_SUCCESS)
    {
        return rst;
    }

    for (i = 0; i < word_len; i++)
    {
        buf[i] = BootEeprom_BytesToWord(&s_boot_eeprom_byte_buf[i * 2]);
    }

    return I2C_SUCCESS;
}

/* 按 Uint16 word 写入 boot 共享参数块。 */
static Uint16 BootEeprom_WriteWords(Uint16 start, Uint16 *buf, Uint16 word_len)
{
    Uint16 i = 0;

    for (i = 0; i < word_len; i++)
    {
        BootEeprom_WordToBytes(buf[i], &s_boot_eeprom_byte_buf[i * 2]);
    }

    return WRE2ROM(s_boot_eeprom_byte_buf, start, word_len * 2);
}

/* 计算 boot 共享参数块校验和，checksum 和 tail 字段不参与。 */
static Uint16 BootEepromParam_CalcChecksum(APP_BOOT_EEPROM_PARAM *param)
{
    Uint16 i = 0;
    Uint16 checksum = 0;
    Uint16 *word = (Uint16 *)param;

    for (i = 0; i < APP_BOOT_EEPROM_CHECK_WORDS; i++)
    {
        checksum += word[i];
    }

    return checksum;
}

/* 写入前刷新固定头、版本、尾和校验和。 */
static void BootEepromParam_Prepare(APP_BOOT_EEPROM_PARAM *param)
{
    param->magic = APP_BOOT_EEPROM_MAGIC;
    param->version = APP_BOOT_EEPROM_VERSION;
    param->tail = APP_BOOT_EEPROM_TAIL;
    param->checksum = BootEepromParam_CalcChecksum(param);
}

/* EEPROM 无效时使用默认 IP，并把下载标志当作已清除。 */
void BootEepromParam_LoadDefault(void)
{
    APP_BOOT_EEPROM_PARAM *param = &g_boot_eeprom_param;

    memset(param, 0, sizeof(APP_BOOT_EEPROM_PARAM));

    param->ip[0] = local_lip[0];
    param->ip[1] = local_lip[1];
    param->ip[2] = local_lip[2];
    param->ip[3] = local_lip[3];
    param->download_flag = APP_BOOT_DOWNLOAD_CLEAR;

    BootEepromParam_Prepare(param);
}

/* 读取并校验 boot 共享参数块。 */
Uint16 BootEepromParam_Init(void)
{
    Uint16 rst = 0;
    Uint16 checksum = 0;

    if (s_boot_eeprom_inited != 0)
    {
        return g_boot_eeprom_status;
    }

    memset(&g_boot_eeprom_param, 0, sizeof(APP_BOOT_EEPROM_PARAM));

    rst = BootEeprom_ReadWords(APP_BOOT_EEPROM_START,
                               (Uint16 *)&g_boot_eeprom_param,
                               APP_BOOT_EEPROM_WORDS);
    if (rst != I2C_SUCCESS)
    {
        BootEepromParam_LoadDefault();
        g_boot_eeprom_status = rst;
        s_boot_eeprom_inited = 1;
        return rst;
    }

    checksum = BootEepromParam_CalcChecksum(&g_boot_eeprom_param);
    if ((g_boot_eeprom_param.magic != APP_BOOT_EEPROM_MAGIC) ||
        (g_boot_eeprom_param.version != APP_BOOT_EEPROM_VERSION) ||
        (g_boot_eeprom_param.tail != APP_BOOT_EEPROM_TAIL) ||
        (g_boot_eeprom_param.checksum != checksum))
    {
        BootEepromParam_LoadDefault();
        g_boot_eeprom_status = EEROM_ERR_CRC;
        s_boot_eeprom_inited = 1;
        return g_boot_eeprom_status;
    }

    g_boot_eeprom_status = EEROM_DATAOK;
    s_boot_eeprom_inited = 1;
    return EEROM_DATAOK;
}

/* 保存 boot 共享参数块，boot 侧主要用于清除下载标志。 */
Uint16 BootEepromParam_Save(void)
{
    Uint16 rst = 0;

    BootEepromParam_Prepare(&g_boot_eeprom_param);

    rst = BootEeprom_WriteWords(APP_BOOT_EEPROM_START,
                                (Uint16 *)&g_boot_eeprom_param,
                                APP_BOOT_EEPROM_WORDS);
    g_boot_eeprom_status = (rst == I2C_SUCCESS) ? EEROM_DATAOK : rst;
    s_boot_eeprom_inited = 1;
    return rst;
}

/* 修改下载标志并写回 EEPROM。 */
Uint16 BootEepromParam_SetDownloadFlag(Uint16 download_flag)
{
    BootEepromParam_Init();
    g_boot_eeprom_param.download_flag = download_flag;
    return BootEepromParam_Save();
}
