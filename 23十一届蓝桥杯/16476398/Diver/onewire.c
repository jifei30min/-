/*	# 	单总线代码片段说明
	1. 	本文件夹中提供的驱动代码供参赛选手完成程序设计参考。
	2. 	参赛选手可以自行编写相关代码或以该代码为基础，根据所选单片机类型、运行速度和试题
		中对单片机时钟频率的要求，进行代码调试和修改。
*/
#include <STC15F2K60S2.H>
#include "onewire.h"
sbit DQ = P1^4;

//
void Delay_OneWire(unsigned int t)  
{
	unsigned char i;
	while(t--){
		for(i=0;i<12;i++);
	}
}

//
void Write_DS18B20(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		DQ = 0;
		DQ = dat&0x01;
		Delay_OneWire(5);
		DQ = 1;
		dat >>= 1;
	}
	Delay_OneWire(5);
}

//
unsigned char Read_DS18B20(void)
{
	unsigned char i;
	unsigned char dat;
  
	for(i=0;i<8;i++)
	{
		DQ = 0;
		dat >>= 1;
		DQ = 1;
		if(DQ)
		{
			dat |= 0x80;
		}	    
		Delay_OneWire(5);
	}
	return dat;
}

//
bit init_ds18b20()
{
  	bit initflag = 0;
  	
  	DQ = 1;
  	Delay_OneWire(12);
  	DQ = 0;
  	Delay_OneWire(80);
  	DQ = 1;
  	Delay_OneWire(10); 
    initflag = DQ;     
  	Delay_OneWire(5);
  
  	return initflag;
}

//自己写的
float read_t()
{
	//定义高低字节
	unsigned char high;
	unsigned char low;
	
	//初始化芯片
	init_ds18b20();
	Write_DS18B20(0XCC);//跳过ROM
	Write_DS18B20(0X44);//温度转换
	
	//初始化芯片
	init_ds18b20();
	Write_DS18B20(0XCC);//跳过ROM
	Write_DS18B20(0XBE);//读取暂存寄存器的值
	
	//第一次读是读低位 第二次是读高位
	low = Read_DS18B20();//读低位
	high = Read_DS18B20();//读高位
	
	return ((high << 8) | low ) / 16.0;
}