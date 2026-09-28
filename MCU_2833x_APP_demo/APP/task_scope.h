#ifndef TASK_SCOPE_H
#define TASK_SCOPE_H

#include "TypeDefine.h"

#define DSO_BUF_LEN    (512UL)

/* 初始化线性波形缓存。 */
void DSO_Init(void);

/* 清空本次记录索引，并按分频和目标点数开始存储。 */
void DSO_CaptureStart(Uint32 div_factor, Uint32 capture_count);

/* 在AD7982采样中断中输入一个实时采样值。 */
void DSO_CaptureSample(float32 value);

/* 处理上位机的波形数据及采样信息查询。 */
void DSO_CmdParse(Uint8 *rxbuf,
                  Uint8 socket,
                  Uint8 *txbuf,
                  Uint32 max_tx_byte,
                  Uint16 (*fsendp)(Uint8, const Uint8 *, Uint16));

#endif
