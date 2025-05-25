//头文件声明区
#include <STC15F2K60S2.H>//单片机存储区头文件
#include <Init.H>//初始化底层驱动专用头文件
#include <Led.H>//Led底层取到专用头文件
#include <Seg.h>//按键底层驱动专用头文件
#include <Key.h>//数码管底层驱动专用头文件
#include <ds1302.h>//时钟驱动专用头文件
#include "onewire.h"//温度专用模块
#include "iic.h" //I2c模块
#include "intrins.h"

//声明变量
unsigned char Key_Val,Key_Down,Key_Up,Key_Old;//按键扫描专用变量
unsigned int Key_Slow_Down; //按键减速专用变量10ms
unsigned char Seg_Buf[8] = {1,2,3,4,5,6,7,8}; //数码管显示数据存放数组
unsigned char Seg_Point[8] = {0,0,0,0,0,0,0,0};//数码管小数点数据存放数组
unsigned char Seg_Pos;//数码管扫描变量
unsigned int Seg_Slow_Down; //数码管减速专用变量500ms
unsigned char ucLed[8] = {0,0,0,0,0,0,0,0}; //LED显示数据存放数组

unsigned char Smg_Mode = 1;//数码管显示功能 1:数据界面 2:参数界面
float temprate;//DS18B20采集环境温度
unsigned char temprate_disp[] = {30, 20};//温度参数显示 温度上限默认为30 下限默认为20
unsigned char temprate_set[] = {30, 20};//温度参数设置
bit System_temprate = 0;//温度亮灭切换比特位
unsigned int temprate_slow = 0;//温度切换延时函数500ms
unsigned char temprate_select = 0;//温度参数选择 0选择MAX 1选择MIN

//延时函数
void Delay750ms()		//@12.000MHz
{
	unsigned char i, j, k;

	_nop_();
	i = 6;
	j = 180;
	k = 26;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

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
		
			//Smg_Mode++完之后=2 进入参数界面
			if(Smg_Mode == 2) //从数据界面切换到参数界面
			{
				temprate_select = 1;//每次从数据界面切换到参数界面 都默认选中min
				temprate_disp[0] = temprate_set[0];
				temprate_disp[1] = temprate_set[1];				
			}
			
			//从参数界面切换到数据界面后
			if(Smg_Mode == 1)
			{
				//如果MAX >= MIN temprate_disp 的值赋给 temprate_set 然后再切换到数据界面时 
				//将赋给 temprate_set的值 给 temprate_disp 使其每一次都能显示上次参数设置的值
				//如果MAX < MIN 说明参数设置不成功 则 temprate_set的初始值传给temprate_disp
				//则将改变的
				if(temprate_disp[0] >= temprate_disp[1])
				{
					temprate_set[0] = temprate_disp[0];
					temprate_set[1] = temprate_disp[1];
				}
			}
		break;
		
		case 5:
			if(Smg_Mode == 2)
			{
				if(++temprate_select == 2) temprate_select = 0;
			}
		
		break;
		
		case 6:
			if(Smg_Mode == 2)
			{
//				if(temprate_select == 0)
//				{
//					if(++temprate_disp[0] == 100) temprate_disp[0] = 0;
//				}				
//				if(temprate_select == 1) 
//				{	
//					if(++temprate_disp[1] == 100) temprate_disp[1] = 0;
//				}						
				if(++temprate_disp[temprate_select] == 100) temprate_disp[temprate_select] = 0;
			}		
		break;
		
		case 7:
			if(Smg_Mode == 2)
			{
//				if(temprate_select == 0)
//				{
//					if(--temprate_disp[0] == 255) temprate_disp[0] = 99;	
//				}				
//				if(temprate_select == 1)
//				{
//					if(--temprate_disp[1] == 255) temprate_disp[1] = 99;
//				}			
				if(--temprate_disp[temprate_select] == 255) temprate_disp[temprate_select] = 99;
			}	
		break;
	}
}

//信息处理函数
void Seg_Proc()
{
	if(Seg_Slow_Down) return;
	Seg_Slow_Down = 1;//数码管减速程序
	
	temprate = read_t();
	switch(Smg_Mode)
	{
		case 1:
			Seg_Buf[0] = 12;
			Seg_Buf[1] = 17;
			Seg_Buf[2] = 17;
			Seg_Buf[3] = 17;
			Seg_Buf[4] = 17;
			Seg_Buf[5] = 17;
			Seg_Buf[6] = (unsigned char)temprate / 10 % 10;
			Seg_Buf[7] = (unsigned char)temprate % 10;
		
		
		break;
		
		case 2:
			Seg_Buf[0] = 18;
			Seg_Buf[1] = 17;
			Seg_Buf[2] = 17;
			Seg_Buf[3] = temprate_disp[0] / 10 % 10;
			Seg_Buf[4] = temprate_disp[0] % 10;
			Seg_Buf[5] = 17;
			Seg_Buf[6] = temprate_disp[1] / 10 % 10;
			Seg_Buf[7] = temprate_disp[1] % 10;			
			if(temprate_select == 0)
			{
				Seg_Buf[3] = System_temprate? temprate_disp[0] / 10 % 10 : 17;
				Seg_Buf[4] = System_temprate? temprate_disp[0] % 10 : 17;
			}
		
			if(temprate_select == 1)
			{
				Seg_Buf[6] = System_temprate? temprate_disp[1] / 10 % 10 : 17;
				Seg_Buf[7] = System_temprate? temprate_disp[1] % 10 : 17;
			}	
		break;
	}
	
}

//其他显示函数
void Led_Proc() 
{
	if(temprate > temprate_disp[0]) Da_Write(4.0 * 51.0);
	if(temprate >= temprate_disp[1] && temprate <= temprate_disp[0]) Da_Write(3.0 * 51.0); 
	if(temprate < temprate_disp[1]) Da_Write(2.0 * 51.0);
	
	if(temprate > temprate_disp[0]) Led_Disp(0,1);
	if(temprate >= temprate_disp[1] && temprate <= temprate_disp[0]) Led_Disp(1,1); 
	if(temprate < temprate_disp[1]) Led_Disp(2,1);
	if(temprate_disp[0] < temprate_disp[1]) Led_Disp(3,1); 
}

//定时器0初始化函数
//12MHZ 定时器0 1ms 16位自动重装载 12T
void Timer0Init()		//1毫秒@12.000MHz
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
	
	if(++temprate_slow == 500)
	{
		temprate_slow = 0;
		System_temprate = !System_temprate;
	}
}

//Main函数
void main()
{
	read_t();
	Delay750ms();
	System_Init();
	Timer0Init();
	
	while(1)
	{
		Key_Proc();
		Seg_Proc();
		Led_Proc();
	}
}