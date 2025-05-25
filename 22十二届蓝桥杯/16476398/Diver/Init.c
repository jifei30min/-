#include <Init.H>

//关闭LED 继电器 蜂鸣器
void System_Init()
{
	P2 = (P2 & 0X1F) | 0X80; 
	P0 = 0XFF;
	//译码器导入数据之后变为原样
	P2 = P2 & 0X1F;
	
	P2 = (P2 & 0X1F) | 0XA0; 
	P0 = 0X00;
	P2 = P2 & 0X1F;
}

