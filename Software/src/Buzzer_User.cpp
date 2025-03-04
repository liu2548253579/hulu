#include "Buzzer_User.h"
#include "Timer_User.h"
#include "main.h"


// uint8_t Buzzer_Short_Trig=1;
// uint8_t Buzzer_Long_Trig=1;

// Timer_Element Buzzer_Short={
//     .Dir=0,
//     .Set=100
// };
// Timer_Element Buzzer_Long={
//     .Dir=0,
//     .Set=500
// };


void Buzzer_Init(void)
{
    pinMode(BUZZER, OUTPUT);//BUZZER
    ledcSetup(PWM_BUZZER, 3000, 10);
    ledcAttachPin(BUZZER, PWM_BUZZER);
}

void Buzzer_On(void)
{
    ledcWrite(PWM_BUZZER, 512);
}


void Buzzer_Off(void)
{
    ledcWrite(PWM_BUZZER, 0);
}

// void Buzzer_Task (void)
// {
//     if(Buzzer_Short_Trig!=0)
//     {
//         if(Buzzer_Short_Trig%2!=0){Buzzer_Off();}
//         else{Buzzer_On();}
// 		Time_Tick(&Buzzer_Short);
//         if(Get_Time_State(&Buzzer_Short))
//         {
//             Buzzer_Short_Trig--;
//             Time_Reset(&Buzzer_Short);
//         }
//     }
//     else if(Buzzer_Long_Trig)
//     {
//         if(Buzzer_Long_Trig%2!=0){Buzzer_Off();}
//         else{Buzzer_On();}
//         Time_Tick(&Buzzer_Long);
//         if(Get_Time_State(&Buzzer_Long))
//         {
//             Buzzer_Long_Trig--;
//             Time_Reset(&Buzzer_Long);
//         }
//     }
//     else
//     {
//         Buzzer_Off();
//     }
    
// }



