#include "Sensor_User.h"
#include "Motion_User.h"
#include "Filter_User.h"
#include "Timer_User.h"
#include "main.h"


SPIClass SPI1(HSPI);

BH1750 lightMeter;
INA226 INA(0x40, &Wire1);
QMC5883LCompass compass;
ICM42688 IMU(SPI1, CS_GYRO);
Bme68x bme;


Sensor_Element Sensor_Data;


void IIC_Pheripheral_Init(void)
{
    Wire.setPins(SDA, SCL);
    Wire1.setPins(SDA1, SCL1);
    Wire.begin();
    Wire1.begin();
}

void SPI_Pheripheral_Init(uint8_t cs)
{
    pinMode(CS_GYRO, OUTPUT);pinMode(CS_BME, OUTPUT);pinMode(CS_EXT, OUTPUT);
    digitalWrite(CS_GYRO, 1);digitalWrite(CS_BME, 1);digitalWrite(CS_EXT, 1);
    digitalWrite(cs, 0);
    SPI1.begin(SCLK, MISO, MOSI, cs);
}



void QMC5883L_Init(void) 
{
    compass.init();//初始化磁力计
    Serial0.println("Compass QMC5883L init done.");
}

volatile uint8_t dataReady = 0;
void setImuFlag(void){dataReady = 1;}//IMU 中断回调函数
void ICM42688_Init(void) 
{
    pinMode(HEAT, OUTPUT);//BUZZER
    ledcSetup(PWM_HEAT, 1000, 10);
    ledcAttachPin(HEAT, PWM_HEAT);

    SPI_Pheripheral_Init(CS_GYRO);
    int status = IMU.begin();

    if (status < 0) 
    {
        Serial0.println("IMU ICM-42688P init failed!");
        Serial0.print("IMU error status: ");
        Serial0.println(status);
    }
    else
    {
        pinMode(INT1, INPUT);
        attachInterrupt(INT1, setImuFlag, RISING);
        IMU.setAccelODR(ICM42688::odr200);
        IMU.setGyroODR(ICM42688::odr200);
        IMU.enableDataReadyInterrupt(); 
        Serial0.println("IMU ICM-42688P init done.");
    }
}


void BH1750_Init(void)
{
    uint8_t ret = lightMeter.begin(BH1750::Mode::CONTINUOUS_HIGH_RES_MODE, 0x23,&Wire);
    if(ret)
    {
        Serial0.println("Light_sensor BH1750 init done.");
    }
    else
    {
        Serial0.println("Light_sensor BH1750 init failed!");
    }
}

void INA226_Init(void)
{
    //电流传感器初始化
    if (!INA.begin())
    {
        Serial0.println("Cur_sensor INA226 init failed!");
    }
    else
    {
        INA.setMaxCurrentShunt(16, 0.005);//设置最大电流与采样电阻(16A 0.005Ω)
        Serial0.println("Cur_sensor INA226 init done.");
    }
}

void BME680_Init(void)
{
    SPI_Pheripheral_Init(CS_BME);
    bme.begin(CS_BME, SPI1);
	if(bme.checkStatus())
	{
		if (bme.checkStatus() == BME68X_ERROR)
		{
			Serial0.println("Sensor error:" + bme.statusString());
			Serial0.println("BME680 init failed!");
		}
		else if (bme.checkStatus() == BME68X_WARNING)
		{
			Serial0.println("Sensor Warning:" + bme.statusString());
		}
	}
    else
    {
        bme.setTPH();
        uint16_t tempProf[10] = { 100, 200, 320 };
        uint16_t durProf[10] = { 150, 150, 150 };
        bme.setSeqSleep(BME68X_ODR_0_59_MS);
        bme.setHeaterProf(tempProf, durProf, 3);
        bme.setOpMode(BME68X_SEQUENTIAL_MODE);
        Serial0.println("BME680 init done.");
    }
}




void QMC5883L_Task(void)
{
    compass.read();
    angle.magx = compass.getX();
    angle.magy = compass.getY();
    angle.magz = compass.getZ();

    sl_mag.value = atan2(angle.magy, angle.magx) * 180.0f / PI;  // 弧度转换为角度
    Slip_Filter(&sl_mag);
    lp_mag.value = sl_mag.out;
    LowPass_Filter(&lp_mag);
    angle.heading = lp_mag.out;
    if (angle.heading < 0){angle.heading += 360.0;}// 确保角度在0-360度范围内
}




void ICM42688_Task(void)
{
    if(dataReady)
    {
        dataReady = 0;
        IMU.getAGT();
        angle.aacx = IMU.accX();
        angle.aacy = IMU.accY();
        angle.aacz = IMU.accZ();

        angle.gyrx = IMU.gyrX();
        angle.gyry = IMU.gyrY();
        angle.gyrz = IMU.gyrZ();
        
        lp_heat.value = IMU.temp();
        LowPass_Filter(&lp_heat);
        angle.heat = lp_heat.out;
        IMU_UpdateQuaternion();
        // IMU_Mag_UpdateQuaternion();
        // Set_Heater_Temp(38.0f,angle.heat);
    }




}

void BH1750_Task(void)
{
    Sensor_Data.light = lightMeter.readLightLevel();
}

void INA226_Task(void)
{
    Sensor_Data.voltage = INA.getBusVoltage();
    Sensor_Data.current = INA.getCurrent_mA();
    Sensor_Data.power = INA.getPower_mW();
}

void BME680_Task(void)
{
    bme68xData bme_data;        
	if (bme.fetchData())
	{
        bme.getData(bme_data);
        Sensor_Data.temperature = bme_data.temperature;
        Sensor_Data.pressure = bme_data.pressure;
        Sensor_Data.humidity = bme_data.humidity;
        Sensor_Data.gas_res = bme_data.gas_resistance;
	}
}

void Sensor_Init(void)
{
    BH1750_Init();
    INA226_Init();
    QMC5883L_Init();
    ICM42688_Init();
    BME680_Init();
}


void Sensor_Task(void)
{
    BH1750_Task();
    INA226_Task();
    QMC5883L_Task();
    ICM42688_Task();
    BME680_Task();

    // Serial0.printf("Light:%.3f\r\n", Sensor_Data.light);

    // Serial0.printf("INA226: %.3fV, %.3fA, %.3fW, %.3fV*A, %.3fW\r\n", 
    // Sensor_Data.voltage, Sensor_Data.current, Sensor_Data.power, 
    // Sensor_Data.voltage * Sensor_Data.current, Sensor_Data.power - Sensor_Data.voltage * Sensor_Data.current);

    // Serial0.printf("XYZ: %d,%d,%d,%d,%d\r\n",(int32_t)angle.roll,(int32_t)angle.pitch,(int32_t)angle.yaw,(int32_t)angle.heat,(int32_t)angle.heading);

    // Serial0.printf("BME: %.2f,%.2f,%.2f,%.2f\r\n", 
    // Sensor_Data.temperature, Sensor_Data.pressure, 
    // Sensor_Data.humidity, Sensor_Data.gas_res);

}










