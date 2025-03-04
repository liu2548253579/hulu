#ifndef __SENSOR_USER_H__
#define __SENSOR_USER_H__

typedef struct
{
    float light;

    float voltage;
    float current;
    float power;

    float temperature;
    float humidity;
    float pressure;
    float gas_res;
    float altitude;

} Sensor_Element;


void IIC_Pheripheral_Init(void);

void Sensor_Init(void);
void Sensor_Task(void);
void BME680_Task(void);


#endif //__SENSOR_USER_H__
