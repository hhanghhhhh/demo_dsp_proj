#include "task_eeprom_param.h"
#include "app_eeprom_config.h"
#include "app_boot_eeprom.h"
#include "drv_ModbusData.h"
#include "drv_GlobalVar.h"
#include "DSP2833x_Examples.h"

#define I2C_SUCCESS              0x0000
#define EEPROM_CMD_COEF_1        1
#define EEPROM_CMD_COEF_2        2
#define EEPROM_CMD_COMM_IP_1     100
#define EEPROM_CMD_COMM_IP_2     101

static void EepromConfig_SyncToRuntime(void)
{
    Uint16 i = 0;
    APP_EEPROM_USER_DATA *user = &g_app_eeprom_config.payload.user;

    mgmd_stSCIRx.ip.addr1 = g_app_boot_eeprom_param.ip[0];
    mgmd_stSCIRx.ip.addr2 = g_app_boot_eeprom_param.ip[1];
    mgmd_stSCIRx.ip.addr3 = g_app_boot_eeprom_param.ip[2];
    mgmd_stSCIRx.ip.addr4 = g_app_boot_eeprom_param.ip[3];

    mgmd_stSCIRx.cfg.vbr_th = user->config.vbr_th;
    mgmd_stSCIRx.cfg.disable = user->config.prot_disable;
    mgmd_stSCIRx.cfg.ovp_th = user->config.prot_ovp_th;
    mgmd_stSCIRx.coef.enable.u16 = user->coef_enable;
    for (i = 0; i < APP_EEPROM_COEF_NUM; i++)
    {
//        mgmd_stSCIRx.coef.f32v_adc_p_k[i].f32 = user->coef_k[i];
//        mgmd_stSCIRx.coef.f32v_adc_p_b[i].f32 = user->coef_b[i];
    }
}

static void EepromConfig_SyncFromRuntime(void)
{
    Uint16 i = 0;
    APP_EEPROM_USER_DATA *user = &g_app_eeprom_config.payload.user;

    user->config.vbr_th = mgmd_stSCIRx.cfg.vbr_th;
    user->config.prot_disable = mgmd_stSCIRx.cfg.disable;
    user->config.prot_ovp_th = mgmd_stSCIRx.cfg.ovp_th;
    user->coef_enable = mgmd_stSCIRx.coef.enable.u16;
    for (i = 0; i < APP_EEPROM_COEF_NUM; i++)
    {
//        user->coef_k[i] = mgmd_stSCIRx.coef.f32v_adc_p_k[i].f32;
//        user->coef_b[i] = mgmd_stSCIRx.coef.f32v_adc_p_b[i].f32;
    }
}

static Uint16 EepromConfig_CheckCoef(void)
{
    Uint16 i = 0;
    APP_EEPROM_USER_DATA *user = &g_app_eeprom_config.payload.user;

    for (i = 0; i < APP_EEPROM_COEF_NUM; i++)
    {
        if ((user->coef_k[i] < 0.9) || (user->coef_k[i] > 1.1))
        {
            return EEROM_DATAERR_OVERRANG;
        }
    }
    return 0;
}

/* 运行期写 boot 共享块可能耗时较长，写入期间暂停看门狗。 */
static Uint16 EepromParam_SaveBoot(void)
{
    Uint16 rst = 0;

    DisableDog();
    rst = AppBootEeprom_Save();
    EnableWDog();
    return rst;
}

/* 统一配置块包含多页数据，整个写入过程都需要暂停看门狗。 */
static Uint16 EepromParam_SaveConfig(void)
{
    Uint16 rst = 0;

    DisableDog();
    rst = AppEepromConfig_Save();
    EnableWDog();
    return rst;
}

void EepromParam_Init(void)
{
    AppBootEeprom_Init();
    AppEepromConfig_Init();
    EepromConfig_SyncToRuntime();
}

void EepromParam_Process(void)
{
    Uint16 rst = 0;

    if ((mgmd_stSCIRx.coef_mode1 == EEPROM_CMD_COMM_IP_1) &&
        (mgmd_stSCIRx.coef_mode2 == EEPROM_CMD_COMM_IP_2))
    {
        mgmd_stSCIRx.coef_mode1 = 0;
        mgmd_stSCIRx.coef_mode2 = 0;
        g_app_boot_eeprom_param.ip[0] = mgmd_stSCIRx.ip.addr1;
        g_app_boot_eeprom_param.ip[1] = mgmd_stSCIRx.ip.addr2;
        g_app_boot_eeprom_param.ip[2] = mgmd_stSCIRx.ip.addr3;
        g_app_boot_eeprom_param.ip[3] = mgmd_stSCIRx.ip.addr4;
        rst = EepromParam_SaveBoot();
        mgmd_stSCIRx.ip.writeOk = (rst == I2C_SUCCESS) ? 1 : 0;
        return;
    }

    if ((mgmd_stSCIRx.coef_mode1 == EEPROM_CMD_COEF_1) &&
         (mgmd_stSCIRx.coef_mode2 == EEPROM_CMD_COEF_2))
    {
        mgmd_stSCIRx.coef_mode1 = 0;
        mgmd_stSCIRx.coef_mode2 = 0;
        EepromConfig_SyncFromRuntime();
        if (EepromConfig_CheckCoef() != 0)
        {
            g_app_eeprom_status = EEROM_DATAERR_OVERRANG;
            return;
        }
        EepromParam_SaveConfig();
        EepromConfig_SyncToRuntime();
    }
}
