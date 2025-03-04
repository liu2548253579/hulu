#ifndef __MIC_USER_H__
#define __MIC_USER_H__

#include "stdint.h"
#include "driver/i2s.h"
#define RECORD_PATH "/sdcard/Recordings"

extern i2s_port_t MIC_I2S_NUM;

void Mic_Init(void);
void Mic_Task(void);

#endif // __MIC_USER_H__
