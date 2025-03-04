#include "Display_User.h"
#include "main.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x78 ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);



void Display_Init(void)
{
    uint8_t ret = display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    if(!ret) 
    {
        Serial0.println("OLED SSD1306 init failed!");
    }
    else
    {
        display.clearDisplay();
        display.display();
        Serial0.println("OLED SSD1306 init done.");
    }
    Display_Task();
}



void Display_Task(void)
{
    display.clearDisplay();

    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0,0);             // Start at top-left corner
    display.println("Hello, world!");
    display.display();
    delay(2000);

}



