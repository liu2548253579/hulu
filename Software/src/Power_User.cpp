#include "Power_User.h"
#include "Buzzer_User.h"
#include "Led_User.h"
#include "SD_User.h"
#include "Filter_User.h"
#include "main.h"

ADC_Bat_Element Power_Bat;

Slip_Filter_Element Vbat_slip={
    .Sample_num=20,
    .count=0,
    .value=0,
    .out=0,
};


LowPass_Filter_Element Vbat_lowpass={
    .value=0,
    .pre=0,
    .out=0
};

/**
 * @brief 根据ADC采样值计算电压值
 */
float Get_ADC_Vol(uint32_t Adc_Val,const float Vol_Div)
{
    float temp;
    temp=((float)Adc_Val * 3.3f / 4095.0f)/Vol_Div;
    return  temp;
}

/*
 * @brief 根据最小电压、最大电压、当前电压计算电池电量百分比
 */
uint8_t Get_BAT_Percent(float Vol_Bat,const float Min_Vol,const float Max_Vol) 
{
    uint8_t temp; //电池电量百分比
    // 确保输入电压在有效范围内
    if (Vol_Bat <= Min_Vol){return 0;}
    if (Vol_Bat >= Max_Vol){return 100;}
    temp = (float)((Vol_Bat - Min_Vol) / (Max_Vol - Min_Vol)) * 100.0f;    // 计算电量百分比
    return temp;
}


void Power_Init(void)
{
    pinMode(LED, OUTPUT);digitalWrite(LED, HIGH);
    
    pinMode(VBAT_AD, ANALOG);
    for(int i=0;i<500;i++)
    {
        Power_Bat.Val = analogRead(VBAT_AD);
        Vbat_lowpass.value = Get_ADC_Vol(Power_Bat.Val,0.5f);
        LowPass_Filter(&Vbat_lowpass);
        Vbat_slip.value = Vbat_lowpass.out;
        Slip_Filter(&Vbat_slip);
        Power_Bat.VOL=Vbat_slip.out;
    }
    Serial0.printf("VBAT:%.2f\r\n",Power_Bat.VOL);

    if(Power_Bat.VOL < VBAT_MIN_VOL)
    {
        Serial0.println("Low Power!");
    }
    else
    {
        Serial0.println("Power Up.");
    }

    pinMode(VBAT_EN, OUTPUT);//3.3EN
    digitalWrite(VBAT_EN, HIGH);
}

void Power_Off(void)
{
    Led_On();Buzzer_On();
    delay(500);Led_Off();Buzzer_Off();
    
    SD_Deinit();
    digitalWrite(VBAT_EN, LOW);
    while(1){}
}

void Power_Task(void)
{
    Power_Bat.Val = analogRead(VBAT_AD);
    Vbat_lowpass.value = Get_ADC_Vol(Power_Bat.Val,0.5f);
    LowPass_Filter(&Vbat_lowpass);
    Vbat_slip.value = Vbat_lowpass.out;
    Slip_Filter(&Vbat_slip);
    Power_Bat.VOL=Vbat_slip.out;
    Power_Bat.Percent = Get_BAT_Percent(Power_Bat.VOL,VBAT_MIN_VOL,VBAT_MAX_VOL);
    if(Power_Bat.Percent<=5)//电量低于5%，关闭电源
    {
        Serial0.printf("%.2fV Low Power Off! \r\n",Power_Bat.VOL);
        Power_Off();
    }

}








