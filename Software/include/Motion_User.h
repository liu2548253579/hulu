#ifndef __MOTION_USER_H__
#define __MOTION_USER_H__

#include "stdint.h"

// 定义结构体存储四元数
typedef struct {
    float q0;  // w
    float q1;  // x
    float q2;  // y 
    float q3;  // z
} Quaternion;



// 定义姿态角结构体
typedef struct {
    float aacx;   // 加速度计x轴
    float aacy;   // 加速度计y轴
    float aacz;   // 加速度计z轴
    float gyrx;   // 陀螺仪x轴
    float gyry;   // 陀螺仪y轴
    float gyrz;   // 陀螺仪z轴
    float heat;   // 加热温度

    float magx;   // 磁力计x轴
    float magy;   // 磁力计y轴
    float magz;   // 磁力计z轴

    float heading; // 航向角

    float roll;    // 横滚角
    float pitch;   // 俯仰角
    float yaw;     // 偏航角
} EulerAngle;

extern EulerAngle angle;
extern Quaternion quarter;

void IMU_UpdateQuaternion(void);
// void IMU_Mag_UpdateQuaternion(void);

void Set_Heater_Temp(float Temp_Desired, float Temp_Current);
// extern float xhat[4];    
// void EKF_Update(void);
// void Quaternion_ToEuler(float *q, EulerAngle *angle);

#endif // __MOTION_USER_H__
