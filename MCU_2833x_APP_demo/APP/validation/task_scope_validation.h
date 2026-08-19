#ifndef __TASK_SCOPE_VALIDATION_H_
#define __TASK_SCOPE_VALIDATION_H_

#include "TypeDefine.h"

extern float32 g_dso_validation_value;
extern Uint32 g_dso_validation_isr_count;

void DSO_ValidationInit(void);
void DSO_ValidationISR(void);

#endif
