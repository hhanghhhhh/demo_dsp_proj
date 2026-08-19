#include "task_scope_validation.h"
#include "task_scope.h"

/* 验证专用斜坡：每次定时 ISR 增加 1，便于上位机检查丢点和顺序。 */
float32 g_dso_validation_value = 0.0F;
Uint32 g_dso_validation_isr_count = 0UL;

void DSO_ValidationInit(void)
{
    g_dso_validation_value = 0.0F;
    g_dso_validation_isr_count = 0UL;

    DSO_Init();
    DSO_ConfigChannel(CH0_XXDATA,
                      &g_dso_validation_value,
                      1UL,
                      DSO_BUF_LEN);
    DSO_Start(CH0_XXDATA);
}

void DSO_ValidationISR(void)
{
    g_dso_validation_value = (float32)g_dso_validation_isr_count;
    g_dso_validation_isr_count++;
    DSO_Sample();
}
