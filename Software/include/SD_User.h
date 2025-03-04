#ifndef __SD_USER_H__
#define __SD_USER_H__


#include "stdint.h"
#include "main.h"

#define MOUNT_POINT "/sdcard"
extern sdmmc_card_t *card;

void SD_Init(void);
void SD_Deinit(void);



#endif //__SD_USER_H__
