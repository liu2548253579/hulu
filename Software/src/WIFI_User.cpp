#include "WIFI_User.h"
#include "main.h"

void SmartConfig()
{
    WiFi.mode(WIFI_STA);
    Serial0.printf("\r\nWait for Smartconfig...\r\n");
    WiFi.beginSmartConfig();
    while (1)
    {
    Serial0.print(".");
    delay(500);                   // wait for a second
    if (WiFi.smartConfigDone())
    {
        Serial0.println("\r\nSmartConfig Success\r\n");
        Serial0.printf("SSID:%s\r\n", WiFi.SSID().c_str());
        Serial0.printf("PSW:%s\r\n", WiFi.psk().c_str());
        break;
    }
    }
}

bool AutoConfig()
{
    WiFi.begin();
    Serial0.printf("\r\nWIFI AutoConfig Trying.");
    for (int i = 0; i < 10; i++)
    {
        int wstatus = WiFi.status();
        if (wstatus == WL_CONNECTED)
        {
            Serial0.printf("\r\nWIFI AutoConfig Success\r\n");
            Serial0.printf("SSID:%s\r\n", WiFi.SSID().c_str());
            Serial0.printf("PSW:%s\r\n", WiFi.psk().c_str());
            Serial0.printf("LocalIP:");
            Serial0.println(WiFi.localIP());
            Serial0.printf("GateIP:");
            Serial0.println(WiFi.gatewayIP());
            return true;
        }
        else
        {
            Serial0.printf(".");
            delay(1000);
        }
    }
    Serial0.printf("\r\nWIFI AutoConfig Faild!\r\n");
    return false;
}

void Wifi_Init(void)
{
    if (!AutoConfig())
    {
        SmartConfig();
    }
}






