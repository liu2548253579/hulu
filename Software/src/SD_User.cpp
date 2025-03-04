#include "SD_User.h"
#include "main.h"
#include "ff.h"
#include "diskio.h"

sdmmc_card_t *card;

void SD_Init(void)
{
    esp_err_t ret;
    const char mount_point[] = MOUNT_POINT;
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {.format_if_mount_failed = false, .max_files = 5, .allocation_unit_size = 16 * 1024};

    sdmmc_host_t host = {
        .flags = SDMMC_HOST_FLAG_1BIT | SDMMC_HOST_FLAG_DDR,
        .slot = SDMMC_HOST_SLOT_1,
        .max_freq_khz = SDMMC_FREQ_HIGHSPEED,
        .io_voltage = 3.3f,
        .init = &sdmmc_host_init,
        .set_bus_width = &sdmmc_host_set_bus_width,
        .get_bus_width = &sdmmc_host_get_slot_width,
        .set_bus_ddr_mode = &sdmmc_host_set_bus_ddr_mode,
        .set_card_clk = &sdmmc_host_set_card_clk,
        .do_transaction = &sdmmc_host_do_transaction,
        .deinit = &sdmmc_host_deinit,
        .io_int_enable = sdmmc_host_io_int_enable,
        .io_int_wait = sdmmc_host_io_int_wait,
        .command_timeout_ms = 0,
    };
    sdmmc_slot_config_t slot_config = {
        .clk = (gpio_num_t)SD_SCLK,
        .cmd = (gpio_num_t)SD_CMD,
        .d0 = (gpio_num_t)SD_DATA0,
        .cd = SDMMC_SLOT_NO_CD,
        .wp = SDMMC_SLOT_NO_WP,
        .width = 1,
        .flags = SDMMC_SLOT_FLAG_INTERNAL_PULLUP,
    };

    ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) 
    {
        Serial0.println("SD_Card mount failed!");
    }
    else
    {
        Serial0.println("SD_Card mounted.");
    }

        // 获取SD卡容量信息
        FATFS *fs;
        DWORD fre_clust;
        uint32_t total_bytes, free_bytes ,used_bytes;
        // 获取文件系统信息
        f_getfree(mount_point, &fre_clust, &fs);
        // 计算总容量和可用空间
        total_bytes = ((uint64_t)(fs->n_fatent - 2) * fs->csize * 512);
        free_bytes = ((uint64_t)fre_clust * fs->csize * 512);
        used_bytes = total_bytes - free_bytes;
        // 打印信息
        Serial0.printf("Total space: %lu bytes\r\n", total_bytes);
        Serial0.printf("Used space: %lu bytes\r\n", used_bytes);

}

void SD_Deinit(void)
{
    esp_err_t ret;
    ret = esp_vfs_fat_sdmmc_unmount();
    if (ret != ESP_OK)
    {
        Serial0.println("SD_Card unmount failed!");
    }
    else
    {
        Serial0.println("SD_Card unmounted.");
    }
    sdmmc_host_deinit(); // Deinitialize the SDMMC host interface
}



