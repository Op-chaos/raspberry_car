/**
* @par Copyright (C): 2010-2019, Shenzhen Yahboom Tech
* @file         advance.c
* @author       Danny
* @version      V1.0
* @date         2017.08.16
* @brief        与小车前进程序一起使用，读取管脚电位
* @details
* @par History  见如下说明
*
*/
#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>
#include <sys/time.h>

//定义引脚
int Left_motor_go = 28;       //左电机前进AIN2连接Raspberry的wiringPi编码28口
int Left_motor_back = 29;     //左电机后退AIN1连接Raspberry的wiringPi编码29口

int Right_motor_go = 24;      //右电机前进BIN2连接Raspberry的wiringPi编码24口
int Right_motor_back = 25;    //右电机后退BIN1连接Raspberry的wiringPi编码25口

int Left_motor_pwm = 27;      //左电机控速PWMA连接Raspberry的wiringPi编码27口
int Right_motor_pwm = 23;     //右电机控速PWMB连接Raspberry的wiringPi编码23口


/**
* Function       main
* @author        Danny
* @date          2017.08.16
* @brief         先延时2s，接着前进
* @param[in]     void
* @retval        void
* @par History   无
*/
void main()
{
    while(1)
    {
        printf("左：%d %d %d\n",digitalRead(Left_motor_go),digitalRead(Left_motor_back),digitalRead(Left_motor_pwm));
        printf("左：%d %d %d\n",digitalRead(Right_motor_go),digitalRead(Right_motor_back),digitalRead(Right_motor_pwm));
        delay(500);
    }
  return;
}