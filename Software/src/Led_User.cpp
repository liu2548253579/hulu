#include "Led_User.h"
#include "Main.h"

uint8_t led_brightness = 10;

void Led_Init(void)
{
    pinMode(LED, OUTPUT);//LED
    ledcSetup(PWM_LED, 1000, 10);
    ledcAttachPin(LED, PWM_LED);
}

void Led_On(void)
{
    ledcWrite(PWM_LED, ((uint16_t)(100-led_brightness)*10.24f));
}

void Led_Off(void)
{
    ledcWrite(PWM_LED, 1024);
}




