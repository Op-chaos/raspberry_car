/**
* @par Copyright (C): 2010-2019, Shenzhen Yahboom Tech
* @file         gForce_control.c
* @author       Danny
* @version      V1.0
* @date         2017.08.16
* @brief        手环控制智能小车实验
* @details
* @par History  见如下说明
*
*/
#include <wiringPi.h>
#include <softPwm.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringSerial.h>
#include <errno.h>
#include "gForceAdapter.h"


#define run_car     '1'//按键前
#define back_car    '2'//按键后
#define left_car    '3'//按键左
#define right_car   '4'//按键右
#define stop_car    '0'//按键停


/*小车运行状态枚举*/
enum {
  enSTOP = 0,
  enRUN,
  enBACK,
  enLEFT,
  enRIGHT,
  enTLEFT,
  enTRIGHT
} enCarState;

//定义引脚
int Left_motor_go = 28;       //左电机前进AIN2连接Raspberry的wiringPi编码28口
int Left_motor_back = 29;     //左电机后退AIN1连接Raspberry的wiringPi编码29口

int Right_motor_go = 24;      //右电机前进BIN2连接Raspberry的wiringPi编码24口
int Right_motor_back = 25;    //右电机后退BIN1连接Raspberry的wiringPi编码25口

int Left_motor_pwm = 27;      //左电机控速PWMA连接Raspberry的wiringPi编码27口
int Right_motor_pwm = 23;     //右电机控速PWMB连接Raspberry的wiringPi编码23口

/*蜂鸣器引脚设置*/
int buzzer = 10;                //设置控制蜂鸣器引脚为wiringPi编码10口

/*小车初始速度控制*/
int CarSpeedControl = 150;

/*设置舵机驱动引脚*/
int ServoPin = 4;

/*RGBLED引脚设置*/
int LED_R = 3;           //LED_R接在Raspberry上的wiringPi编码3口
int LED_G = 2;           //LED_G接在Raspberry上的wiringPi编码2口
int LED_B = 5;           //LED_B接在Raspberry上的wiringPi编码5口

/*串口设备打开的文件描述符*/
int fd;

/*串口长度变量*/
int g_num=0;
int g_packnum=0;

/*串口数据设置*/
unsigned char InputString[50] = {0};   //用来储存接收到的内容
int NewLineReceived = 0;      //前一次数据结束标志
int StartBit  = 0;            //协议开始标志
int g_CarState = enSTOP;      //1前2后3左4右0停止

/**
* Function       servo_pulse
* @author        Danny
* @date          2017.08.16
* @brief         定义一个脉冲函数，用来模拟方式产生PWM值
*                时基脉冲为20ms,该脉冲高电平部分在0.5-2.5ms
*                控制0-180度
* @param[in1]    ServPin:舵机控制引脚
* @param[in2]    myangle:舵机转动指定的角度
* @param[out]    void
* @retval        void
* @par History   无
*/
void servo_pulse(int ServoPin, int myangle)
{
  int PulseWidth;                    //定义脉宽变量
  PulseWidth = (myangle * 11) + 500; //将角度转化为500-2480 的脉宽值
  digitalWrite(ServoPin, HIGH);      //将舵机接口电平置高
  delayMicroseconds(PulseWidth);     //延时脉宽值的微秒数
  digitalWrite(ServoPin, LOW);       //将舵机接口电平置低
  delay(20 - PulseWidth / 1000);     //延时周期内剩余时间
  return;
}

/**
* Function       run
* @author        Danny
* @date          2017.08.16
* @brief         小车前进
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void run()
{
  //左电机前进
  digitalWrite(Left_motor_go, HIGH);   //左电机前进使能
  digitalWrite(Left_motor_back, LOW);  //左电机后退禁止
  softPwmWrite(Left_motor_pwm, CarSpeedControl);

  //右电机前进
  digitalWrite(Right_motor_go, HIGH);  //右电机前进使能
  digitalWrite(Right_motor_back, LOW); //右电机后退禁止
  softPwmWrite(Right_motor_pwm, CarSpeedControl);
}

/**
* Function       brake
*  @author        Danny
* @date          2017.08.16
* @brief         小车刹车
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void brake()
{
  digitalWrite(Left_motor_go, LOW);
  digitalWrite(Left_motor_back, LOW);
  digitalWrite(Right_motor_go, LOW);
  digitalWrite(Right_motor_back, LOW);
}

/**
* Function       left
* @author        Danny
* @date          2017.08.16
* @brief         小车左转(左轮不动,右轮前进)
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void left()
{
  //左电机停止
  digitalWrite(Left_motor_go, LOW);     //左电机前进禁止
  digitalWrite(Left_motor_back, LOW);   //左电机后退禁止
  softPwmWrite(Left_motor_pwm, 0);

  //右电机前进
  digitalWrite(Right_motor_go, HIGH);  //右电机前进使能
  digitalWrite(Right_motor_back, LOW); //右电机后退禁止
  softPwmWrite(Right_motor_pwm, CarSpeedControl);
}

/**
* Function       spin_left
* @author        Danny
* @date          2017.08.16
* @brief         小车原地左转(左轮后退，右轮前进)
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void spin_left()
{
  //左电机后退
  digitalWrite(Left_motor_go, LOW);     //左电机前进禁止
  digitalWrite(Left_motor_back, HIGH);  //左电机后退使能
  softPwmWrite(Left_motor_pwm, CarSpeedControl);

  //右电机前进
  digitalWrite(Right_motor_go, HIGH);  //右电机前进使能
  digitalWrite(Right_motor_back, LOW); //右电机后退禁止
  softPwmWrite(Right_motor_pwm, CarSpeedControl);
}

/**
* Function       right
* @author        Danny
* @date          2017.08.16
* @brief         小车右转(左轮前进,右轮不动)
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void right()
{
  //左电机前进
  digitalWrite(Left_motor_go, HIGH);    //左电机前进使能
  digitalWrite(Left_motor_back, LOW);   //左电机后退禁止
  softPwmWrite(Left_motor_pwm, CarSpeedControl);

  //右电机停止
  digitalWrite(Right_motor_go, LOW);    //右电机前进禁止
  digitalWrite(Right_motor_back, LOW);  //右电机后退禁止
  softPwmWrite(Right_motor_pwm, 0);
}

/**
* Function       spin_right
* @author        Danny
* @date          2017.08.16
* @brief         小车原地右转(右轮后退，左轮前进)
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void spin_right()
{
  //左电机前进
  digitalWrite(Left_motor_go, HIGH);    //左电机前进使能
  digitalWrite(Left_motor_back, LOW);   //左电机后退禁止
  softPwmWrite(Left_motor_pwm, CarSpeedControl);

  //右电机后退
  digitalWrite(Right_motor_go, LOW);    //右电机前进禁止
  digitalWrite(Right_motor_back, HIGH); //右电机后退使能
  softPwmWrite(Right_motor_pwm, CarSpeedControl);
}

/**
* Function       back
* @author        Danny
* @date          2017.08.16
* @brief         小车后退
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void back()
{
  //左电机后退
  digitalWrite(Left_motor_go, LOW);     //左电机前进禁止
  digitalWrite(Left_motor_back, HIGH);  //左电机后退使能
  softPwmWrite(Left_motor_pwm, CarSpeedControl);

  //右电机后退
  digitalWrite(Right_motor_go, LOW);    //右电机前进禁止
  digitalWrite(Right_motor_back, HIGH); //右电机后退使能
  softPwmWrite(Right_motor_pwm, CarSpeedControl);
}

/**
* Function       whistle
* @author        Danny
* @date          2017.08.16
* @brief         小车鸣笛
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void whistle()
{
  digitalWrite(buzzer, LOW);   //发声音
  delay(100);                  //延时100ms
  digitalWrite(buzzer, HIGH);  //不发声音
  delay(1);                    //延时1ms

  digitalWrite(buzzer, LOW);   //发声音
  delay(200);                  //延时200ms
  digitalWrite(buzzer, HIGH);  //不发声音
  delay(2);                    //延时2ms
  return;
}

/**
* Function       servo_appointed_detection
* @author        Danny
* @date          2017.08.16
* @brief         舵机旋转到指定角度
* @param[in]     pos：指定的角度
* @param[out]    void
* @retval        void
* @par History   无
*/
void servo_appointed_detection(int pos)
{
  int i = 0;
  for (i = 0; i <= 15; i++)    //产生PWM个数，等效延时以保证能转到响应角度
  {
    servo_pulse(ServoPin, pos); //模拟产生PWM
  }
}

/**
* Function       color_led_pwm
* @author        Danny
* @date          2017.08.16
* @brief         七彩灯亮指定的颜色
* @param[in1]    v_iRed:指定的颜色值（0-255）
* @param[in2]    v_iGreen:指定的颜色值（0-255）
* @param[in3]    v_iBlue:指定的颜色值（0-255）
* @param[out]    void
* @retval        void
* @par History   无
*/
void color_led_pwm(int v_iRed, int v_iGreen, int v_iBlue)
{
  softPwmWrite(LED_R, v_iRed);
  softPwmWrite(LED_G, v_iGreen);
  softPwmWrite(LED_B, v_iBlue);
  return;
}

/**
* Function       serialEvent
* @author        Danny
* @date          2017.08.16
* @brief         串口解包
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void serialEvent()
{  
  unsigned char uartvalue = 0;
  static int num = 0;
  static int Len = 0;
  if(serialDataAvail(fd) != 0)
  {
	uartvalue = (unsigned char)serialGetchar(fd);
    //printf("----------uartvalue1 is %#x\n",uartvalue);
    if (num == 0 && uartvalue == MAGNUM_LOW)
    {
	  InputString[num] = uartvalue;
	  num++;
	  return ;
    }
	if (num == 1 && uartvalue == MAGNUM_HIGH)
    {
	  InputString[num] = uartvalue;
	  num++;
	  return;
    }
    //判断是否是手势数据(0X0F代表不带package ID 0X8F代表带package ID)
	//package ID主要用于的是所发送的数据包的校验
	if(num == 2 )
	{
	    if((uartvalue == 0x0f) || (uartvalue == 0x8f))
		{
    printf("uartvalue1 is %#x\n",uartvalue);
			InputString[num] = uartvalue; 
			num++;
			return;
		}
		else
		{
		    //表示收到的并不是手势的数据，直接取消接下来的接收
			StartBit = 0;
			num = 0;
			return;
		}
	}
	//协议中规定这里是数据是表示Byte[4:]总共包含的字节数
	if(num == 3)//获取数据Len
	{
    printf("uartvalue2 is %#x\n",uartvalue);
		Len = uartvalue;
		InputString[num] = uartvalue; 
		num++;
		StartBit = 1;
		return;
	}
	//开始接收后面的数据，也许是BYTE[4]也许是BYTE[4]和BYTE[5]
	if(StartBit == 1 && num > 3)
	{
    printf("uartvalue3 is %#x\n",uartvalue);
		InputString[num] = uartvalue; 
		num++;  	 
	} 
	//4其实是因为前面num已经自增1了
	if (num == Len + 4 ) 
	{
		StartBit = 0;
	    num = 0;
        NewLineReceived = 1;		
	    //协议数据接收完成调用数据解包函数	   
	    ForceAdapter(InputString);		  
    //printf("serialdataniceto:%s\n",InputString);
	}		
	if(num >= 50)
	{
		num = 0;
		StartBit = 0;
		Len = 0;
		NewLineReceived = 0;
	}	
  }
  
}
/**
* Function       main
* @author        Danny
* @date          2017.08.16
* @brief         对串口发送过来的数据解析，并执行相应的指令
* @param[in]     void
* @retval        void
* @par History   无
*/
int main()
{
  int i;
  //wiringPi初始化
  wiringPiSetup();
  //初始化电机驱动IO为输出方式

  pinMode(Left_motor_go, OUTPUT);
  pinMode(Left_motor_back, OUTPUT);
  pinMode(Right_motor_go, OUTPUT);
  pinMode(Right_motor_back, OUTPUT);
  
  //创建两个软件控制的PWM脚
  softPwmCreate(Left_motor_pwm,0,255); 
  softPwmCreate(Right_motor_pwm,0,255);
  //初始化蜂鸣器IO为输出方式
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, HIGH);

  //初始化RGB三色LED的IO口为输出方式，并初始化
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  softPwmCreate(LED_R,0,255); 
  softPwmCreate(LED_G,0,255); 
  softPwmCreate(LED_B,0,255); 

  //初始化舵机引脚为输出模式
  pinMode(ServoPin, OUTPUT);
  printf("hello world\n"); 
  //打开串口设备，如若失败则会打印错误信息
  if ((fd = serialOpen("/dev/ttyAMA0", 115200)) < 0)
  {
    fprintf(stderr, "Uable to open serial device: %s\n", strerror(errno));
	return -1;
  }

  for(;;)
  {
   //调用串口解包函数
   serialEvent();
   if (NewLineReceived == 1)
   {	
   // printf("serialdata:%s\n",InputString);
	printf("data is %d\n",g_enumGesture);
    switch(g_enumGesture)
	{
		case GF_RELEASE: brake();color_led_pwm(0,0,0); break;               //放松  停止
		case GF_FIST:back();color_led_pwm(255,0,0);  break;     //握拳  后退
		case GF_SPREAD: run();color_led_pwm(0,255,0); break;    //申掌  前进
		case GF_WAVEIN:	 left();color_led_pwm(0,0,255); break;  //左摆  左转
		case GF_WAVEOUT: right();color_led_pwm(255,255,255); break;   //右摆  右转
		case GF_PINCH:  whistle();CarSpeedControl+=50;if(CarSpeedControl>250)CarSpeedControl=250; break; //捏	   减速
		case GF_SHOOT:  whistle();CarSpeedControl-=50;if(CarSpeedControl<50)CarSpeedControl=50; break;   //射击  加速
		case GF_UNKNOWN:brake();color_led_pwm(0,0,0);break;  //未知  停止
	}
	  NewLineReceived = 0;
   }
     // delay(10);
  }
 serialClose(fd);  //关闭串口
 return 0;
}




