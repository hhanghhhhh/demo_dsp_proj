#ifndef __TASK_EEPROM_DOWNLOAD_H_
#define __TASK_EEPROM_DOWNLOAD_H_

#include "drv_Eeprom.h"


extern Uint16 glb_download_flag;

extern void InitEeromPara_Downloads(void);
extern void WriteEeromPara_Downloads(Uint16 download_flag);
extern void load_application(void);
extern void Boot_ResetToApplication(void);



#endif
/***************************************************************************
*           END, do not code behind this line!!                            *
****************************************************************************/
