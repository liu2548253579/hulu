#ifndef __POWER_USER_H__
#define __POWER_USER_H__

#include "stdint.h"

#define ADC_VREF 3.300f //ADC参考电压
#define ADC_RESOLUTION 4095.000f //ADC采样精度

#define VBAT_MAX_VOL 4.2f //电池百分比最大电压
#define VBAT_MIN_VOL 3.2f //电池百分比最小电压

typedef struct
{
    uint8_t Percent;//电池电量百分比
    uint32_t Val;//ADC采样值RAW值
    float VOL;//实际电压值
}ADC_Bat_Element;

extern ADC_Bat_Element Power_Bat;

void Power_Init(void);
void Power_Task(void);
void Power_Off(void);

#endif //__POWER_USER_H__
