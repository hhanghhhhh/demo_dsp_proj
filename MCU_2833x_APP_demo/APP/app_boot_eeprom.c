#include "app_boot_eeprom.h"
#include "app_eeprom_config.h"
#include <string.h>

#define I2C_SUCCESS 0x0000

APP_BOOT_EEPROM_PARAM g_app_boot_eeprom_param;
Uint16 g_app_boot_eeprom_status = APP_EEPROM_DATAOK;

extern Uint8 local_lip[4];

/* 计算 boot 共享参数块校验和，checksum 和 tail 字段不参与。 */
static Uint16 AppBootEeprom_CalcChecksum(APP_BOOT_EEPROM_PARAM *param)
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
static void AppBootEeprom_Prepare(APP_BOOT_EEPROM_PARAM *param)
{
    param->magic = APP_BOOT_EEPROM_MAGIC;
    param->version = APP_BOOT_EEPROM_VERSION;
    param->tail = APP_BOOT_EEPROM_TAIL;
    param->checksum = AppBootEeprom_CalcChecksum(param);
}

/* 加载 boot 共享参数默认值，主要用于 EEPROM 为空或校验失败时。 */
void AppBootEeprom_LoadDefault(void)
{
    APP_BOOT_EEPROM_PARAM *param = &g_app_boot_eeprom_param;

    memset(param, 0, sizeof(APP_BOOT_EEPROM_PARAM));
    param->ip[0] = local_lip[0];
    param->ip[1] = local_lip[1];
    param->ip[2] = local_lip[2];
    param->ip[3] = local_lip[3];
    param->download_flag = APP_BOOT_DOWNLOAD_CLEAR;
    AppBootEeprom_Prepare(param);
}

/* 校验 boot 共享参数块是否有效。 */
Uint16 AppBootEeprom_Check(APP_BOOT_EEPROM_PARAM *param)
{
    if (param->magic != APP_BOOT_EEPROM_MAGIC)
    {
        return APP_EEPROM_ERR_HEADER;
    }
    if (param->version != APP_BOOT_EEPROM_VERSION)
    {
        return APP_EEPROM_ERR_VERSION;
    }
    if (param->tail != APP_BOOT_EEPROM_TAIL)
    {
        return APP_EEPROM_ERR_FOOTER;
    }
    if (AppBootEeprom_CalcChecksum(param) != param->checksum)
    {
        return APP_EEPROM_ERR_CHECKSUM;
    }
    return APP_EEPROM_DATAOK;
}

/* 初始化 boot 共享参数块，失败时恢复默认值并写回 EEPROM。 */
Uint16 AppBootEeprom_Init(void)
{
    Uint16 rst = 0;

    memset(&g_app_boot_eeprom_param, 0, sizeof(APP_BOOT_EEPROM_PARAM));
    rst = AppEeprom_ReadWords(APP_BOOT_EEPROM_START,
                              (Uint16 *)&g_app_boot_eeprom_param,
                              APP_BOOT_EEPROM_WORDS);
    if (rst != I2C_SUCCESS)
    {
        AppBootEeprom_LoadDefault();
        g_app_boot_eeprom_status = rst;
        return rst;
    }

    rst = AppBootEeprom_Check(&g_app_boot_eeprom_param);
    if (rst != APP_EEPROM_DATAOK)
    {
        AppBootEeprom_LoadDefault();
        AppBootEeprom_Save();
        g_app_boot_eeprom_status = rst;
        return rst;
    }

    g_app_boot_eeprom_status = APP_EEPROM_DATAOK;
    return APP_EEPROM_DATAOK;
}

/* 保存 boot 共享参数块，供 APP 修改 IP 或下载标志时调用。 */
Uint16 AppBootEeprom_Save(void)
{
    Uint16 rst = 0;

    AppBootEeprom_Prepare(&g_app_boot_eeprom_param);
    rst = AppEeprom_WriteWords(APP_BOOT_EEPROM_START,
                               (Uint16 *)&g_app_boot_eeprom_param,
                               APP_BOOT_EEPROM_WORDS);
    g_app_boot_eeprom_status = (rst == I2C_SUCCESS) ? APP_EEPROM_DATAOK : rst;
    return rst;
}
