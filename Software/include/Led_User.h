#ifndef __LED_USER_H__
#define __LED_USER_H__

#include "stdint.h"

extern uint8_t led_brightness;

void Led_Init(void);
void Led_On(void);
void Led_Off(void);


#endif //__LED_USER_H__

