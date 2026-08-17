#ifndef _DEVICE_H_
#define _DEVICE_H_



#define FW_VER_HIGH   	1
#define FW_VER_LOW    	0



/* Private function prototypes -----------------------------------------------*/


extern void set_network(void);
extern void write_config_to_eeprom(void);
extern void set_default(void);
extern void get_config(void);





#endif

