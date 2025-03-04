#include "main.h"

#include "Led_User.h"
#include "Timer_User.h"
#include "Key_User.h"
#include "Buzzer_User.h"
#include "Mic_User.h"
#include "Power_User.h"
#include "SD_User.h"
#include "USB_User.h"
#include "FTP_User.h"
#include "WIFI_User.h"
#include "Sensor_User.h"
#include "Motion_User.h"
#include "Display_User.h"
#include "Speaker_User.h"


void setup() 
{
  Serial0.begin(115200,SERIAL_8N1,RX0,TX0);//USB串口初始化

  Key_Init();
  Led_Init();
  Buzzer_Init();
  Power_Init();

  Led_On();Buzzer_On();delay(20);
  Led_Off();Buzzer_Off();delay(20);
  Led_On();Buzzer_On();delay(20);
  Led_Off();Buzzer_Off();delay(20);
  delay(1000);

  SD_Init();
  USB_Udisk_Init();
  Led_On();
  IIC_Pheripheral_Init();
  Sensor_Init();
  Mic_Init();
  Speaker_Init();
  Wifi_Init();

  // xTaskCreate(SD_Task,"SD_Task",1024*2,NULL,1,NULL);


  Serial0.println("Setup done.");
}



void loop() 
{
  // Mic_Task();
  amplifier();
  Key_Task();
  Sensor_Task();

  Time_Tick(&Task_100ms);
  if(Get_Time_State(&Task_100ms))//100ms
  {
    Time_Reset(&Task_100ms);//复位计时器
    Power_Task();
  }
  if(key1.out==1)//检测到按键多按
  {
    Serial0.print("Key_Times:");
    Serial0.println(key1.times);
    

    Key_Clean(&key1);
  }
  else if(key1.out==2)//检测到按键长按
  {
    Serial0.println("shutdown");
    Key_Clean(&key1);
    Power_Off();
  }

  delay(1);
}
