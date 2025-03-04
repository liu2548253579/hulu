#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdint.h>
#include <Arduino.h>

#include <Wire.h>
#include <SPI.h>
#include "driver/i2s.h"
#include "esp_a2dp_api.h"

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <WebServer.h>
#include "FS.h"
#include "SD_MMC.h"
#include "SD.h"
#include "USB.h"
#include "USBMSC.h"
#include "dirent.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "esp32-hal-tinyusb.h"
#include "class/hid/hid_device.h"

#include <QMC5883LCompass.h>//磁力计
#include "ICM42688.h"//加速度计

#include "BH1750.h"//光照传感器
#include "INA226.h"//电流传感器
#include "bme68xLibrary.h"//气压计 VOC传感器
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>



#include <ArduinoJson.h>
#include <SD_MMC.h>
#include <SPIFFS.h>




// GPIO引脚定义
#define TX0 43
#define RX0 44

#define TXD1  17
#define RXD1  18

#define SDA  6
#define SCL  7

#define SDA1 4
#define SCL1 5


#define HID_DM 19
#define HID_DP 20


#define MOSI 11
#define SCLK 12
#define MISO 13
//陀螺仪
#define CS_GYRO 9
#define INT1 15 
#define HEAT 16
//气压计
#define CS_BME 14
//拓展接口
#define CS_EXT 10 


#define KEY 21
#define I2S1_SCK 48
#define I2S1_DAT 45
#define I2S1_WS 47

#define SD_DATA0 38
#define SD_CMD 39
#define SD_SCLK 40

#define I2S_SCK 41
#define I2S_WS 42
#define I2S_DAT 2

#define BUZZER 1
#define LED  8
#define VBAT_AD 3
#define VBAT_EN 46


//PWM channel
#define PWM_BUZZER 1
#define PWM_LED 2
#define PWM_HEAT 3


extern TwoWire Wire,Wire1;
extern SPIClass SPI1;


void IIC_Pheripheral_Init(void);
void SPI_Pheripheral_Init(uint8_t cs);

#endif // __MAIN_H__
