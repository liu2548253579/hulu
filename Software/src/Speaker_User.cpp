#include "Speaker_User.h"
#include "Mic_User.h"
#include "main.h"

#define SAMPLE_RATE 96000
#define BUFFER_SIZE 1024

i2s_port_t SPEAKER_I2S_NUM = I2S_NUM_0;


void Speaker_Init(void)
{
    i2s_config_t i2s_speaker_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),  // 改为TX模式用于播放
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = BUFFER_SIZE,
        .use_apll = true,  // 启用内部时钟源
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };

    i2s_pin_config_t speaker_pin_config = {
        .mck_io_num = -1,
        .bck_io_num = I2S1_SCK,  // 扬声器的SCK引脚
        .ws_io_num = I2S1_WS,    // 扬声器的WS引脚
        .data_out_num = I2S1_DAT, // 扬声器的数据输出引脚
        .data_in_num = -1         // 播放不需要数据输入，设为-1
    };

    esp_err_t ret = i2s_driver_install(SPEAKER_I2S_NUM, &i2s_speaker_config, 0, NULL);
    if (ret != ESP_OK) {
        Serial0.printf("I2S driver install failed: %s\n", esp_err_to_name(ret));
    }
    ret = i2s_set_pin(SPEAKER_I2S_NUM, &speaker_pin_config);
    if (ret != ESP_OK) {
        Serial0.printf("I2S set pin failed: %s\n", esp_err_to_name(ret));
    }
    i2s_zero_dma_buffer(SPEAKER_I2S_NUM);

    Serial0.println("Speaker I2S init done.\r\n");
}





void amplifier(void) 
{
    static int32_t samples[BUFFER_SIZE];
    size_t bytesRead = 0;
    size_t bytesWritten = 0;
    
    // 读取I2S数据
    esp_err_t result = i2s_read(MIC_I2S_NUM, samples, sizeof(samples), &bytesRead, portMAX_DELAY);
    
    if (result == ESP_OK && bytesRead > 0) {
        // 音量增益处理
        float gain = 2.0f; // 增益系数，可根据需要调整
        
        for (size_t i = 0; i < bytesRead / sizeof(int32_t); i++) {
            // 应用增益并防止溢出
            int64_t tempSample = (int64_t)samples[i] * gain;
            
            // 限幅处理
            if (tempSample > INT32_MAX) {
                samples[i] = INT32_MAX;
            } else if (tempSample < INT32_MIN) {
                samples[i] = INT32_MIN;
            } else {
                samples[i] = (int32_t)tempSample;
            }
        }
        
        // 直接输出到扬声器
        i2s_write(SPEAKER_I2S_NUM, samples, bytesRead, &bytesWritten, portMAX_DELAY);
    }
}







