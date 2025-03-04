#include "USB_User.h"
#include "SD_User.h"
#include "main.h"

USBMSC MSC_User;
USBCDC CDC_User;




static void usbEventCallback(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == ARDUINO_USB_EVENTS) {
    arduino_usb_event_data_t *data = (arduino_usb_event_data_t *)event_data;
    switch (event_id) {
    case ARDUINO_USB_STARTED_EVENT:
        Serial0.println("USB PLUGGED");
        break;
    case ARDUINO_USB_STOPPED_EVENT:
        Serial0.println("USB UNPLUGGED");
        break;
    case ARDUINO_USB_SUSPEND_EVENT:
        Serial0.printf("USB SUSPENDED: remote_wakeup_en: %u\n", data->suspend.remote_wakeup_en);
        break;
    case ARDUINO_USB_RESUME_EVENT:
        Serial0.println("USB RESUMED");
        break;
    default:
        break;
    }
    }
}


static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t *buffer, uint32_t bufsize) {
    uint32_t count = (bufsize / card->csd.sector_size);
    sdmmc_write_sectors(card, buffer + offset, lba, count);
    return bufsize;
}

static int32_t onRead(uint32_t lba, uint32_t offset, void *buffer, uint32_t bufsize) {
    uint32_t count = (bufsize / card->csd.sector_size);
    uintptr_t buffer_addr = ((uintptr_t)buffer)+ offset;  // 计算出 buffer 地址
    void* new_buffer = (void*)buffer_addr;  // 转换回 void* 类型
    sdmmc_read_sectors(card, new_buffer, lba, count);
    return bufsize;
}


static bool onStartStop(uint8_t power_condition, bool start, bool load_eject) {
    return true;
}

void USB_Udisk_Init(void)
{
    USB.onEvent(usbEventCallback);
    MSC_User.vendorID("LAN");      
    MSC_User.productID("Hulu"); 
    MSC_User.productRevision("1.0");  
    MSC_User.onStartStop(onStartStop);
    MSC_User.onRead(onRead);
    MSC_User.onWrite(onWrite);
    MSC_User.mediaPresent(true);
    MSC_User.begin(card->csd.capacity, card->csd.sector_size); // 使用计算得到的扇区数和固定的扇区大小
    USB.begin();
}

void USB_Audio_Task(void)
{
}