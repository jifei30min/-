//头文件声明区
#include <STC15F2K60S2.H>//单片机存储区头文件
#include <Init.H>//初始化底层驱动专用头文件
#include <Led.H>//Led底层取到专用头文件
#include <Seg.h>//按键底层驱动专用头文件
#include <Key.h>//数码管底层驱动专用头文件
#include <ds1302.h>//时钟驱动专用头文件
#include "onewire.h"//温度专用模块
#include "iic.h" //I2c模块

//声明变量
unsigned char Key_Val,Key_Down,Key_Up,Key_Old;//按键扫描专用变量
unsigned int Key_Slow_Down; //按键减速专用变量10ms
unsigned char Seg_Buf[8] = {1,2,3,4,5,6,7,8}; //数码管显示数据存放数组
unsigned char Seg_Point[8] = {0,0,0,0,0,0,0,0};//数码管小数点数据存放数组
unsigned char Seg_Pos;//数码管扫描变量
unsigned int Seg_Slow_Down; //数码管减速专用变量500ms
unsigned char ucLed[8] = {0,0,0,0,0,0,0,0}; //LED显示数据存放数组
unsigned char Smg_Mode = 1;//界面切换 1为温度显示界面 2为参数设置界面 3为DAC输出界面
float Tempartae_One = 0;//1温度显示界面温度数据
unsigned char Tempartae_Disp = 25;//2温度显示界面温度参数
unsigned char Tempartae_Set = 25;//温度显示界面设置参数
float UC;//3输出界面电压参数
unsigned char SmgS5 = 1;//1为模式1 2为模式2



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
			if(++Smg_Mode == 4) Smg_Mode = 1;
			if(Smg_Mode == 1) Tempartae_Disp = Tempartae_Set;
			if(Smg_Mode == 2) Tempartae_Set = Tempartae_Disp;
		break;
		
		//减
		case 8:
			if(Smg_Mode == 2)
			{
				if(--Tempartae_Disp < 10 ) Tempartae_Disp = 85;
			}
		break;
			
			//加
		case 9:
			if(Smg_Mode == 2)
			{
				if(++Tempartae_Disp > 85 ) Tempartae_Disp = 10;
			}			
		break;
		case 5:
			if(++SmgS5 == 3) SmgS5 = 1;
		break;
		
	}
}

//信息处理函数
void Seg_Proc()
{
	if(Seg_Slow_Down) return;
	Seg_Slow_Down = 1;//数码管减速程序
	
	//读取温度
	Tempartae_One = read_t();
	
	if(SmgS5 == 1)
	{
		if(Tempartae_One < Tempartae_Disp) UC = 0.0;
		else 
		{
			UC = 5.0;
		}
	}
	if(SmgS5 == 2)
	{
		if(Tempartae_One <= 20) UC = 1.0;
		if(Tempartae_One >= 40) UC = 4.0;
		if(Tempartae_One > 20 && Tempartae_One < 40) UC = (0.15 * (Tempartae_One - 20)) + 1.0;
	}
	
	//DAC输出电压
	Da_Write((unsigned char)(UC * 51.0));
	
	switch(Smg_Mode)
	{
		case 1:
			Seg_Buf[0] = 12;
			Seg_Buf[1] = 17;
			Seg_Buf[2] = 17;
			Seg_Buf[3] = 17;
			Seg_Buf[4] = (unsigned int)Tempartae_One / 10;
			Seg_Buf[5] = (unsigned int)Tempartae_One % 10;
			Seg_Buf[6] = (unsigned int)Tempartae_One * 10 /10 % 10;
			Seg_Buf[7] = (unsigned int)Tempartae_One / 100 /10 % 10;
			Seg_Point[5] = 1;
		break;
		case 2:
			Seg_Buf[0] = 18;
			Seg_Buf[1] = 17;
			Seg_Buf[2] = 17;
			Seg_Buf[3] = 17;
			Seg_Buf[4] = 17;
			Seg_Buf[5] = 17;
			Seg_Buf[6] = Tempartae_Disp / 10;
			Seg_Buf[7] = Tempartae_Disp % 10;
			Seg_Point[5] = 0;
		break;		
		case 3:
			Seg_Buf[0] = 10;
			Seg_Buf[1] = 17;
			Seg_Buf[2] = 17;
			Seg_Buf[3] = 17;
			Seg_Buf[4] = 17;
			Seg_Buf[5] = (unsigned int)UC % 10;
			Seg_Buf[6] = (unsigned int)(UC * 10) % 10; //避免了UC先被强制转换后小数点消失 然后显示不准确的问题
			Seg_Buf[7] = (unsigned int)(UC * 100) % 10; //避免了UC先被强制转换后小数点消失 然后显示不准确的问题
			Seg_Point[5] = 1;
		break;			
	}
	
}

//其他显示函数
void Led_Proc()
{
	if(SmgS5 == 1) Led_Disp(0,1);
	if(Smg_Mode == 1) Led_Disp(1,1);
	if(Smg_Mode == 2) Led_Disp(2,1);
	if(Smg_Mode == 3) Led_Disp(3,1);
}

//定时器0初始化函数
//12MHZ 定时器0 1ms 16位自动重装载 12T
void Timer0Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x18;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	
	ET0 = 1;
	EA = 1;
}


//定时器0中断服务函数
void Timer0Service() interrupt 1
{
	//自动重装载 不需要赋初值
	
	Key_Slow_Down++;
	if(Key_Slow_Down == 10) Key_Slow_Down=0; //if(++Key_Slow_Down == 10)Key_Slow_Down=0;
	
	Seg_Slow_Down++;
	if(Seg_Slow_Down == 500) Seg_Slow_Down=0;//if(++Seg_Slow_Down == 500) Seg_Slow_Down=0;
	
	if(++Seg_Pos == 8) Seg_Pos = 0;
	Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos],Seg_Point[Seg_Pos]);
	Led_Disp(Seg_Pos,ucLed[Seg_Pos]);
}

//Main函数
void main()
{
	System_Init();
	Timer0Init();
	
	while(1)
	{
		Key_Proc();
		Seg_Proc();
		Led_Proc();
	}
}