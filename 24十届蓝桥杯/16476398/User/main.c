//头文件声明区
#include <STC15F2K60S2.H>//单片机存储区头文件
#include <Init.H>//初始化底层驱动专用头文件
#include <Led.H>//Led底层取到专用头文件
#include <Seg.h>//按键底层驱动专用头文件
#include <Key.h>//数码管底层驱动专用头文件
#include "iic.h"


//我的代码中S6无法关闭LED的原因是因为我在点亮LED时 没有通过ucLed[]来赋值 我是直接调用LED函数赋值的 所以即便我在定时1的终端中改变了ucLed[]的值 也无济于事
//所以:所有LED操作都通过ucLed[]数组进行，确保状态一致性

//声明变量
unsigned char Key_Val,Key_Down,Key_Up,Key_Old;//按键扫描专用变量
unsigned int Key_Slow_Down; //按键减速专用变量10ms
unsigned char Seg_Buf[8] = {1,2,3,4,5,6,7,8}; //数码管显示数据存放数组
unsigned char Seg_Point[8] = {0,0,0,0,0,0,0,0};//数码管小数点数据存放数组
unsigned char Seg_Pos;//数码管扫描变量
unsigned int Seg_Slow_Down; //数码管减速专用变量500ms
unsigned char ucLed[8] = {0,0,0,0,0,0,0,0}; //LED显示数据存放数组

unsigned char Smg_Mode = 1;//数码管显示界面 1为频率显示界面 2为电压显示界面
unsigned int Dat;//频率
unsigned int Timer1000ms;//定时器1 1000ms
float UC;//RB2的输出电压
float DAC_Send;//DAC输出电压
bit DAC_System = 1;//DAC输出模式切换
bit LED_System = 1;//LED功能启动切换

bit SMG_System = 1;//数码管功能切换




//按键处理函数
void Key_Proc()
{
	//如果Key_Slow_Down非零，if条件成立，执行return;语句，Key_Proc()会停止执行，不会执行后面的Key_Slow_Down = 1;
	if(Key_Slow_Down) return;
	
	//Key_Slow_Down == 0时  Key_Slow_Down变为1
	Key_Slow_Down = 1;//按键减速程序
	
	//照搬代码
	Key_Val = Key_Read(); //读取按下的键码值
	
	Key_Down = Key_Val & (Key_Val ^ Key_Old);//捕捉下降沿 //按键按下那一瞬间的值
	
	Key_Up = ~Key_Val & (Key_Val ^ Key_Old);//捕捉上升沿 //按键抬起那一瞬间的值
	
	Key_Old = Key_Val;//辅助扫描 //按键一直按下的时候的值
	
	switch(Key_Down)
	{
		case 4:
			if(++Smg_Mode == 3) Smg_Mode = 1;
		break;
		case 5:
			DAC_System = !DAC_System;
		break;
		case 6:
			LED_System = !LED_System;
		break;
		case 7:
			SMG_System = !SMG_System;
		break;
	}
}

//信息处理函数
void Seg_Proc()
{
	if(Seg_Slow_Down) return;
	Seg_Slow_Down = 1;//数码管减速程序
	
	UC = (float)Ad_Read(0X43) / 51.0;
	if(DAC_System == 1) DAC_Send = 2.0;
	if(DAC_System == 0) DAC_Send = UC;
	
	Da_Send((unsigned char)(DAC_Send * 51.0));
	
	
	switch(Smg_Mode)
	{
		case 1:
			Seg_Buf[0] = 15;
			Seg_Buf[1] = 17;
			Seg_Buf[2] = Dat > 99999? (Dat / 100000) % 10 : 17;
			Seg_Buf[3] = Dat > 9999? (Dat / 10000) % 10 : 17;
			Seg_Buf[4] = Dat > 999? (Dat / 1000) % 10 : 17;
			Seg_Buf[5] = Dat > 99? (Dat / 100) % 10 : 17;
			Seg_Buf[6] = Dat > 9? (Dat / 10) % 10 : 17;
			Seg_Buf[7] = Dat > 0? Dat % 10 : 17;
			Seg_Point[5] = 0;
		break;
			
		case 2:
			Seg_Buf[0] = 19;
			Seg_Buf[1] = 17;
			Seg_Buf[2] = 17;
			Seg_Buf[3] = 17;
			Seg_Buf[4] = 17;
			Seg_Buf[5] = (unsigned int)UC;
			Seg_Buf[6] = (unsigned int)(UC * 10) % 10;
			Seg_Buf[7] = (unsigned int)(UC * 100) % 10;
			Seg_Point[5] = 1;
			
		break;
		
	}
	
	
}

//其他显示函数
void Led_Proc()
{
	if(Smg_Mode == 2)
	{
//		Led_Disp(0,1);
		ucLed[0] = 1;
//		Led_Disp(1,0);
		ucLed[1] = 0;
	}
	if(Smg_Mode == 1)
	{
//		Led_Disp(0,0);
		ucLed[0] = 0;
//		Led_Disp(1,1);
		ucLed[1] = 1;
	}
	
	//L3
	if(UC < 1.5) ucLed[2] = 0;
	if(UC >= 1.5 && UC < 2.5) ucLed[2] = 1;
	if(UC >= 2.5 && UC < 3.5) ucLed[2] = 0;
	if(UC >= 3.5) ucLed[2] = 1;
	
	//L4
	if(Dat < 1000) ucLed[3] = 0;
	if(Dat >= 1000 && Dat < 5000) ucLed[3] = 1;
	if(Dat >= 5000 && Dat < 10000) ucLed[3] = 0;
	if(Dat >= 10000) ucLed[3] = 1; 
	
	//L5
	if(DAC_Send == 2.0) ucLed[4] = 0; 
	if(DAC_Send == UC) ucLed[4] = 1; 	
	
}


//定时器0 计数模式 16位
void Timer0Init()		//0毫秒@12.000MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x05;		//设置定时器模式
	TL0 = 0x00;		//设置定时初值
	TH0 = 0x00;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
}

//定时器1 定时模式 16位自动重装载 1ms
void Timer1Init()		//1毫秒@12.000MHz
{
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x20;		//设置定时初值
	TH1 = 0xD1;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	
	ET1 = 1;
	EA = 1;
}


//定时器1中断服务函数
void Timer1Service() interrupt 3
{
	//自动重装载 不需要赋初值
	
	Key_Slow_Down++;
	if(Key_Slow_Down == 10) Key_Slow_Down=0; //if(++Key_Slow_Down == 10)Key_Slow_Down=0;
	
	Seg_Slow_Down++;
	if(Seg_Slow_Down == 500) Seg_Slow_Down=0;//if(++Seg_Slow_Down == 500) Seg_Slow_Down=0;
	
	if(++Seg_Pos == 8) Seg_Pos = 0;
	if(SMG_System == 1)
	{
		Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos],Seg_Point[Seg_Pos]);
	}else{
		Seg_Disp(Seg_Pos,17,0);
	}
	  
	if(LED_System == 1)
	{
		Led_Disp(Seg_Pos,ucLed[Seg_Pos]);
	}else
	{
		Led_Disp(Seg_Pos,0);
	}
	
	if(++Timer1000ms == 1000)
	{
		Timer1000ms = 0;
		Dat = (TH0 << 8) | TL0;
		TL0 = 0x00;		//设置计数初值
		TH0 = 0x00;		//设置计数初值
	}
}

//Main函数
void main()
{
	System_Init();
	Timer0Init();
	Timer1Init();
	while(1)
	{
		Key_Proc();
		Seg_Proc();
		Led_Proc();
	}
}