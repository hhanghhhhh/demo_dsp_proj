#ifndef XINTF_VALIDATION_TEST_H
#define XINTF_VALIDATION_TEST_H

#include "TypeDefine.h"

extern volatile Uint16 g_xintf_validation_state;
extern volatile Uint16 g_xintf_validation_pass;
extern volatile Uint16 g_xintf_validation_error;
extern volatile Uint16 g_xintf_validation_index;
extern volatile Uint32 g_xintf_validation_expected;
extern volatile Uint32 g_xintf_validation_actual;

void XintfValidationInit(void);
void XintfValidationProcess(void);

#endif
