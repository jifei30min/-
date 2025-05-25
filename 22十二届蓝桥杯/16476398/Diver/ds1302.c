/*	# 	DS1302代码片段说明
	1. 	本文件夹中提供的驱动代码供参赛选手完成程序设计参考。
	2. 	参赛选手可以自行编写相关代码或以该代码为基础，根据所选单片机类型、运行速度和试题
		中对单片机时钟频率的要求，进行代码调试和修改。
*/								
#include <STC15F2K60S2.H>
#include <intrins.h> //使_nop_();不报错
#include "ds1302.h"

sbit SCK = P1^7;
sbit SDA = P2^3;
sbit RST = P1^3;
//写进地址
void Write_Ds1302(unsigned  char temp) 
{
	unsigned char i;
	for (i=0;i<8;i++)     	
	{ 
		SCK = 0;
		SDA = temp&0x01;
		temp>>=1; 
		SCK=1;
	}
}   

//写进地址address 和数据dat 
void Write_Ds1302_Byte( unsigned char address,unsigned char dat )     
{
 	RST=0;	_nop_();//_nop_();等待一会儿
 	SCK=0;	_nop_();
 	RST=1; 	_nop_();  
 	Write_Ds1302(address);	//写入地址
 	Write_Ds1302(dat);		//写入数据
 	RST=0; 
}

//读地址(入口参数:address) 然后返回数据temp
unsigned char Read_Ds1302_Byte ( unsigned char address )
{
 	unsigned char i,temp=0x00;
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1;	_nop_();
 	Write_Ds1302(address);
 	for (i=0;i<8;i++) 	
 	{		
		//下降沿为读数据
		SCK=0;
		temp>>=1;	
 		if(SDA)
 		temp|=0x80;	
 		SCK=1;
	} 
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
	SCK=1;	_nop_();
	SDA=0;	_nop_();
	SDA=1;	_nop_();
	return (temp);			
}

//将时间写入
void Set_Rtc(unsigned char* ucRtc) //*是一个指针 指向的是名字为ucRtc的数组
{
	//WP = 0寄存器可以输入数据
	Write_Ds1302_Byte(0X8E,0X00); //0X8E:是WP的地址 0X00:将WP置0
	
//	unsigned char i;
//	for(i=0;i<3;i++)
//	{
//		Write_Ds1302_Byte(0X84 - i*2,ucRtc[i]);
//	}
	
	//写小时
	Write_Ds1302_Byte(0X84,ucRtc[0]); //0X84:是写入小时的地址 ucRtc[0]:数组第0位 代表小时的值
	
	//写分钟
	Write_Ds1302_Byte(0X82,ucRtc[1]); //0X82:是写入分钟的地址 ucRtc[1]:数组第1位 代表分钟的值
	
	//写秒
	Write_Ds1302_Byte(0X80,ucRtc[2]); //0X80:是写入秒的地址 ucRtc[2]:数组第2位 代表秒的值
	
	//WP = 1寄存器禁止输入数据
	Write_Ds1302_Byte(0X8E,0X80); //0X8E:是WP的地址 0X80:将WP置1
}

//写一个读函数
void Read_Rtc(unsigned char * ucRtc)
{
//	unsigned char i;
//	for(i=0;i<3;i++)
//	{
//		ucRtc[i] = Read_Ds1302_Byte (0X85 - i * 2);
//	}
	
	//将读出的小时赋给数组第零位
	ucRtc[0] = Read_Ds1302_Byte (0X85); //0X85:是读出小时的地址
	
	//将读出的分钟赋给数组第一位
	ucRtc[1] = Read_Ds1302_Byte (0X83); //0X83:是读出分钟的地址
	
	//将读出的秒赋给数组第二位
	ucRtc[2] = Read_Ds1302_Byte (0X81); //0X81:是读出秒的地址
}
