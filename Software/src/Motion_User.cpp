#include "Motion_User.h"
#include "Filter_User.h"
#include "Sensor_User.h"
#include "main.h"

// 全局变量
Quaternion quarter = {1.0f, 0.0f, 0.0f, 0.0f};  // 四元数初始化
EulerAngle angle;    // 欧拉角初始化

// Mahony算法参数
#define Kp 10.0f    // 比例增益
#define Ki 0.0001f    // 积分增益
#define halfT 0.005f  // 采样周期的一半,根据实际采样率调整

// 最大积分误差限制
#define MAX_INT 0.1f



void IMU_UpdateQuaternion(void)
{
    static float exInt = 0.0f, eyInt = 0.0f, ezInt = 0.0f;  // 积分误差初始化
    float norm;
    float ax = angle.aacx, ay = angle.aacy, az = angle.aacz;
    float gx = angle.gyrx * PI / 180.0f; // 转换为弧度
    float gy = angle.gyry * PI / 180.0f;
    float gz = angle.gyrz * PI / 180.0f; // 修正：应该使用gyrz

    // 加速度计归一化
    norm = sqrt(ax * ax + ay * ay + az * az);
    ax = ax / norm;
    ay = ay / norm;
    az = az / norm;

    // 估计重力方向和方向误差
    float vx = 2.0f * (quarter.q1 * quarter.q3 - quarter.q0 * quarter.q2);
    float vy = 2.0f * (quarter.q0 * quarter.q1 + quarter.q2 * quarter.q3);
    float vz = quarter.q0 * quarter.q0 - quarter.q1 * quarter.q1 - quarter.q2 * quarter.q2 + quarter.q3 * quarter.q3;

    // 错误的方向是估计的重力方向与测量的重力方向的叉积
    float ex = (ay * vz - az * vy);
    float ey = (az * vx - ax * vz);
    float ez = (ax * vy - ay * vx);

    // 积分误差补偿
    exInt = exInt + ex * Ki;
    eyInt = eyInt + ey * Ki;
    ezInt = ezInt + ez * Ki;

    // 补偿后的陀螺仪测量
    gx = gx + Kp * ex + exInt;
    gy = gy + Kp * ey + eyInt;
    gz = gz + Kp * ez + ezInt;

    // 四元数微分方程
    float q0_dot = -quarter.q1 * gx - quarter.q2 * gy - quarter.q3 * gz;
    float q1_dot = quarter.q0 * gx - quarter.q3 * gy + quarter.q2 * gz;
    float q2_dot = quarter.q3 * gx + quarter.q0 * gy - quarter.q1 * gz;
    float q3_dot = -quarter.q2 * gx + quarter.q1 * gy + quarter.q0 * gz;

    // 更新四元数
    quarter.q0 = quarter.q0 + (q0_dot * halfT);
    quarter.q1 = quarter.q1 + (q1_dot * halfT);
    quarter.q2 = quarter.q2 + (q2_dot * halfT);
    quarter.q3 = quarter.q3 + (q3_dot * halfT);

    // 四元数归一化
    norm = sqrt(quarter.q0 * quarter.q0 + quarter.q1 * quarter.q1 + quarter.q2 * quarter.q2 + quarter.q3 * quarter.q3);
    quarter.q0 = quarter.q0 / norm;
    quarter.q1 = quarter.q1 / norm;
    quarter.q2 = quarter.q2 / norm;
    quarter.q3 = quarter.q3 / norm;

    // 计算欧拉角
    angle.roll = atan2(2.0f * (quarter.q0 * quarter.q1 + quarter.q2 * quarter.q3), 1.0f - 2.0f * (quarter.q1 * quarter.q1 + quarter.q2 * quarter.q2)) * 180.0f / PI;
    angle.pitch = asin(2.0f * (quarter.q0 * quarter.q2 - quarter.q3 * quarter.q1)) * 180.0f / PI;
    angle.yaw = atan2(2.0f * (quarter.q0 * quarter.q3 + quarter.q1 * quarter.q2), 1.0f - 2.0f * (quarter.q2 * quarter.q2 + quarter.q3 * quarter.q3)) * 180.0f / PI;
}



void IMU_Mag_UpdateQuaternion(void)
{
    static float exInt = 0.0f, eyInt = 0.0f, ezInt = 0.0f;
    float norm;
    float ax = angle.aacx, ay = angle.aacy, az = angle.aacz;
    float mx = angle.magx, my = angle.magy, mz = angle.magz;
    float gx = angle.gyrx * PI / 180.0f;
    float gy = angle.gyry * PI / 180.0f;
    float gz = angle.gyrz * PI / 180.0f;

    // 加速度计归一化
    norm = sqrt(ax * ax + ay * ay + az * az);
    if (norm < 1e-6) return;
    ax /= norm;
    ay /= norm;
    az /= norm;

    // 磁力计归一化
    norm = sqrt(mx * mx + my * my + mz * mz);
    if (norm < 1e-6) return;
    mx /= norm;
    my /= norm;
    mz /= norm;

    // 计算参考磁场方向
    float hx = mx * (1.0f - 2.0f * (quarter.q2 * quarter.q2 + quarter.q3 * quarter.q3)) +
               my * (2.0f * (quarter.q1 * quarter.q2 - quarter.q0 * quarter.q3)) +
               mz * (2.0f * (quarter.q1 * quarter.q3 + quarter.q0 * quarter.q2));
    float hy = mx * (2.0f * (quarter.q1 * quarter.q2 + quarter.q0 * quarter.q3)) +
               my * (1.0f - 2.0f * (quarter.q1 * quarter.q1 + quarter.q3 * quarter.q3)) +
               mz * (2.0f * (quarter.q2 * quarter.q3 - quarter.q0 * quarter.q1));
    float bx = sqrt(hx * hx + hy * hy);
    float bz = mx * (2.0f * (quarter.q1 * quarter.q3 - quarter.q0 * quarter.q2)) +
               my * (2.0f * (quarter.q2 * quarter.q3 + quarter.q0 * quarter.q1)) +
               mz * (1.0f - 2.0f * (quarter.q1 * quarter.q1 + quarter.q2 * quarter.q2));

    // 估计重力和磁场方向
    float vx = 2.0f * (quarter.q1 * quarter.q3 - quarter.q0 * quarter.q2);
    float vy = 2.0f * (quarter.q0 * quarter.q1 + quarter.q2 * quarter.q3);
    float vz = quarter.q0 * quarter.q0 - quarter.q1 * quarter.q1 - quarter.q2 * quarter.q2 + quarter.q3 * quarter.q3;

    // 计算磁场预测方向
    float wx = bx * (1.0f - 2.0f * (quarter.q2 * quarter.q2 + quarter.q3 * quarter.q3)) +
               bz * (2.0f * (quarter.q1 * quarter.q3 - quarter.q0 * quarter.q2));
    float wy = bx * (2.0f * (quarter.q1 * quarter.q2 - quarter.q0 * quarter.q3)) +
               bz * (2.0f * (quarter.q2 * quarter.q3 + quarter.q0 * quarter.q1));
    float wz = bx * (2.0f * (quarter.q0 * quarter.q2 + quarter.q1 * quarter.q3)) +
               bz * (1.0f - 2.0f * (quarter.q1 * quarter.q1 + quarter.q2 * quarter.q2));

    // 错误计算（叉积）
    float ex = (ay * vz - az * vy) + (my * wz - mz * wy);
    float ey = (az * vx - ax * vz) + (mz * wx - mx * wz);
    float ez = (ax * vy - ay * vx) + (mx * wy - my * wx);

    // 积分误差
    exInt = fmin(fmax(exInt + ex * Ki, -MAX_INT), MAX_INT);
    eyInt = fmin(fmax(eyInt + ey * Ki, -MAX_INT), MAX_INT);
    ezInt = fmin(fmax(ezInt + ez * Ki, -MAX_INT), MAX_INT);

    // 应用反馈
    gx = gx + Kp * ex + exInt;
    gy = gy + Kp * ey + eyInt;
    gz = gz + Kp * ez + ezInt;

    // 四元数积分
    float q0_dot = -quarter.q1 * gx - quarter.q2 * gy - quarter.q3 * gz;
    float q1_dot = quarter.q0 * gx - quarter.q3 * gy + quarter.q2 * gz;
    float q2_dot = quarter.q3 * gx + quarter.q0 * gy - quarter.q1 * gz;
    float q3_dot = -quarter.q2 * gx + quarter.q1 * gy + quarter.q0 * gz;

    quarter.q0 += q0_dot * halfT;
    quarter.q1 += q1_dot * halfT;
    quarter.q2 += q2_dot * halfT;
    quarter.q3 += q3_dot * halfT;

    // 四元数归一化
    norm = sqrt(quarter.q0 * quarter.q0 + quarter.q1 * quarter.q1 + 
               quarter.q2 * quarter.q2 + quarter.q3 * quarter.q3);
    quarter.q0 /= norm;
    quarter.q1 /= norm;
    quarter.q2 /= norm;
    quarter.q3 /= norm;

    // 计算欧拉角
    angle.roll = atan2(2.0f * (quarter.q0 * quarter.q1 + quarter.q2 * quarter.q3),
                       1.0f - 2.0f * (quarter.q1 * quarter.q1 + quarter.q2 * quarter.q2)) * 180.0f / PI;
    angle.pitch = asin(fmin(fmax(2.0f * (quarter.q0 * quarter.q2 - quarter.q3 * quarter.q1),
                       -1.0f), 1.0f)) * 180.0f / PI;
    angle.yaw = atan2(2.0f * (quarter.q0 * quarter.q3 + quarter.q1 * quarter.q2),
                      1.0f - 2.0f * (quarter.q2 * quarter.q2 + quarter.q3 * quarter.q3)) * 180.0f / PI;
}



void Set_Heater_Temp(float Temp_Desired, float Temp_Current)
{
    float PWM_Apply;
    float Temp_Diff=fabs(Temp_Desired - Temp_Current);
    if(Temp_Diff <= 0.5f){PWM_Apply=100;}
    else if(Temp_Diff <= 1.0f){PWM_Apply=100;}
    else if(Temp_Diff <= 3.0f){PWM_Apply=200;}
    else if(Temp_Diff <= 5.0f){PWM_Apply=300;}
    else if(Temp_Diff <= 7.0f){PWM_Apply=350;}
    else if(Temp_Diff <= 10.0){PWM_Apply=700;}
    else{PWM_Apply=800;}

    if(Temp_Current<Temp_Desired){ledcWrite(PWM_HEAT, PWM_Apply);}
    else{ledcWrite(PWM_HEAT, 0);}
}





