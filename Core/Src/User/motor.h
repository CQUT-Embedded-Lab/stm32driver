//
// Created by Cause_Hhc on 2020/8/24.
//

#ifndef MAZE_STM32_CUBEMX_CLION_MOTOR_H
#define MAZE_STM32_CUBEMX_CLION_MOTOR_H
#include "tim.h"
typedef struct {
    char x; //坐标x
    char y; //坐标y
    char dir; //绝对方向
}carInfoType;
typedef struct {
    int ENC;  //当前编码器示�?
    int ADD;  //编码器累计示�?
    int TGT;  //电机转�?�期望�??
    int PWM;  //电机实际PWM赋�??
}motorInfoType;
void check_ENC(motorInfoType *leftInfo, motorInfoType *rightInfo);
void plus_ADD(motorInfoType *leftInfo, motorInfoType *rightInfo);
void incremental_PI_A(motorInfoType *leftInfo);
void incremental_PI_B(motorInfoType *rightInfo);
void range_PWM(motorInfoType *leftInfo, motorInfoType *rightInfo, int amplitude);
void set_PWM(motorInfoType *leftInfo, motorInfoType *rightInfo);

#endif //MAZE_STM32_CUBEMX_CLION_MOTOR_H
