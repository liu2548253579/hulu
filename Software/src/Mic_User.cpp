#include "Mic_User.h"
#include "SD_User.h"
#include "main.h"

#define SAMPLE_RATE 96000
#define BUFFER_SIZE 1024

const int headerSize = 44;  // WAV 文件头的大小

// WAV 文件头
byte header[headerSize];
FILE* wavFile;
bool isRecording = false;
unsigned long recordingStartTime = 0;

i2s_port_t MIC_I2S_NUM = I2S_NUM_1;

uint8_t audio_buffer[BUFFER_SIZE];
size_t totalDataSize = 0;  // 用于跟踪实际写入的音频数据大小

void Mic_Init(void)
{
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = BUFFER_SIZE,
        .use_apll = true,  // 启用内部时钟源
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0
    };
    i2s_pin_config_t pin_config = {
        .mck_io_num = -1,
        .bck_io_num = I2S_SCK,  // 确认与硬件连接正确
        .ws_io_num = I2S_WS,  // 确认与硬件连接正确
        .data_out_num = I2S_DAT,  // 确认与硬件连接正确
        .data_in_num = I2S_DAT  // 数据输入和输出共享数据线
    };

    esp_err_t ret = i2s_driver_install(MIC_I2S_NUM, &i2s_config, 0, NULL);
    if (ret != ESP_OK) {
        Serial0.printf("I2S driver install failed: %s\n", esp_err_to_name(ret));
    }
    ret = i2s_set_pin(MIC_I2S_NUM, &pin_config);
    if (ret != ESP_OK) {
        Serial0.printf("I2S set pin failed: %s\n", esp_err_to_name(ret));
    }
    i2s_zero_dma_buffer(MIC_I2S_NUM);

    Serial0.println("Mic MSM261S4030 init done.\r\n");
}

void writeString(byte* header, int offset, const char* str)
{
    while (*str) {
        header[offset++] = *str++;
    }
}

void writeInt32(byte* header, int offset, unsigned int value)
{
    header[offset] = value & 0xFF;
    header[offset + 1] = (value >> 8) & 0xFF;
    header[offset + 2] = (value >> 16) & 0xFF;
    header[offset + 3] = (value >> 24) & 0xFF;
}

void writeInt16(byte* header, int offset, unsigned short value)
{
    header[offset] = value & 0xFF;
    header[offset + 1] = (value >> 8) & 0xFF;
}

void createWavHeader(byte* header, size_t waveDataSize)
{
    writeString(header, 0, "RIFF");
    writeInt32(header, 4, waveDataSize + headerSize - 8);  // 计算文件总大小
    writeString(header, 8, "WAVE");
    writeString(header, 12, "fmt ");

    writeInt32(header, 16, 16);                // Subchunk1Size
    writeInt16(header, 20, 3);                 // Format (IEEE Float)
    writeInt16(header, 22, 1);                 // Channels (Mono)
    writeInt32(header, 24, SAMPLE_RATE);       // Sample Rate
    writeInt32(header, 28, SAMPLE_RATE * 4);   // Byte Rate
    writeInt16(header, 32, 4);                 // Block Align
    writeInt16(header, 34, 32);                // Bits Per Sample

    writeString(header, 36, "data");
    writeInt32(header, 40, waveDataSize);  // 音频数据大小
}

// 获取下一个可用的文件序号
int getNextFileIndex(const char* directory) {
    int fileIndex = 1;  // 从 1 开始

    DIR* dir = opendir(directory);
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            // 只检查文件名以 "Rec_" 开头的文件
            if (strncmp(entry->d_name, "Rec_", 4) == 0) {
                int index = 0;
                // 尝试从文件名中解析数字部分
                if (sscanf(entry->d_name, "Rec_%d.wav", &index) == 1) {
                    // 更新最大文件编号
                    if (index >= fileIndex) {
                        fileIndex = index + 1;
                    }
                }
            }
        }
        closedir(dir);
    }

    return fileIndex;
}

void startRecording()
{
    if (!isRecording)
    {
        struct stat st;
        if (stat(RECORD_PATH, &st) != 0) {
            mkdir(RECORD_PATH, 0777);
        }

        // 获取下一个可用的文件编号
        int fileIndex = getNextFileIndex(RECORD_PATH);
        String filename = String(RECORD_PATH"/Rec_") + String(fileIndex) + ".wav";
        
        // 初始化文件头
        createWavHeader(header, totalDataSize);  // 初始时数据大小为 0
        wavFile = fopen(filename.c_str(), "w");
        fwrite(header, 1, headerSize, wavFile);

        isRecording = true;
        totalDataSize = 0;  // 重置已写入的字节数
        recordingStartTime = millis();
        Serial0.println("Recording started: " + filename);
    }
}

void stopRecording()
{
    if (isRecording) {
        isRecording = false;

        // 更新文件头，写入实际数据大小
        createWavHeader(header, totalDataSize);
        fseek(wavFile, 0, SEEK_SET);  // 回到文件开头写入更新后的头部
        fwrite(header, 1, headerSize, wavFile);
        fclose(wavFile);
        Serial0.println("Recording stopped");
    }
}

void processRecording() {
    if (isRecording) {
        static int32_t samples[BUFFER_SIZE];
        size_t bytesRead = 0;
        

        // 读取I2S数据
        esp_err_t result = i2s_read(MIC_I2S_NUM, samples, sizeof(samples), &bytesRead, portMAX_DELAY);
        
        if (result == ESP_OK && bytesRead > 0) {

        // 增益处理
        float gain = 40.0f; // 增益因子，可以根据需要调整
        for (size_t i = 0; i < bytesRead / sizeof(int32_t); i++) {
            // 应用增益并防止溢出
            int64_t tempSample = (int64_t)samples[i] * gain;

            // 检查是否溢出，并进行限制
            if (tempSample > INT32_MAX) {
                samples[i] = INT32_MAX;
            } else if (tempSample < INT32_MIN) {
                samples[i] = INT32_MIN;
            } else {
                samples[i] = (int32_t)tempSample;
            }
        }

        float* floatSamples = (float*)malloc(bytesRead); 
        for (size_t i = 0; i < bytesRead / sizeof(int32_t); i++) 
        {
            floatSamples[i] = (float)samples[i] / INT32_MAX;
        }
        fwrite(floatSamples, 1, bytesRead, wavFile);
        free(floatSamples);
        totalDataSize += bytesRead;
        }
    }
}


void Mic_Task(void)
{
    if (Serial0.available()) 
    {
        char cmd = Serial0.read();
        if(cmd=='r'){startRecording();}// 开始录音
        else if(cmd=='s'){stopRecording();}// 停止录音
    }

    if (isRecording) {processRecording();}// 录音处理
}
