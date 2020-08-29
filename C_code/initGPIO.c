/**
* @par Copyright (C): 2010-2019, Shenzhen Yahboom Tech
* @file         ColorLED.c
* @author       Danny
* @version      V1.0
* @date         2017.08.16
* @brief        七彩探照灯实验
* @details
* @par History  见如下说明
*/
#include <wiringPi.h>
#include <stdio.h>

#define ON 1     //使能LED
#define OFF 0    //禁止LED

//定义引脚
int LED_R = 3;  //LED_R接在Raspberry上的wiringPi编码3口
int LED_G = 2;  //LED_G接在Raspberry上的wiringPi编码2口
int LED_B = 5;  //LED_B接在Raspberry上的wiringPi编码5口



/**
* Function       main
* @author        Danny
* @date          2017.08.16
* @brief         循环显7色LED
* @param[in]     void
* @retval        void
* @par History   无
*/
int main()
{
	wiringPiSetup();
	
	//RGB引脚模式设置为输出模式
	pinMode(LED_R, OUTPUT);
	pinMode(LED_G, OUTPUT);
	pinMode(LED_B, OUTPUT);
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_B, LOW);

   return 0;	
}