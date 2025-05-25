/*	#   I2C代码片段说明
	1. 	本文件夹中提供的驱动代码供参赛选手完成程序设计参考。
	2. 	参赛选手可以自行编写相关代码或以该代码为基础，根据所选单片机类型、运行速度和试题
		中对单片机时钟频率的要求，进行代码调试和修改。
*/
#include "iic.h"
#define DELAY_TIME	5

sbit sda = P2^1;
sbit scl = P2^0;
//
static void I2C_Delay(unsigned char n)
{
    do
    {
   	
    }
    while(n--);      	
}

//
void I2CStart()
{
    sda = 1;
    scl = 1;
	I2C_Delay(DELAY_TIME);
    sda = 0;
	I2C_Delay(DELAY_TIME);
    scl = 0;    
}

//
void I2CStop()
{
    sda = 0;
    scl = 1;
	I2C_Delay(DELAY_TIME);
    sda = 1;
	I2C_Delay(DELAY_TIME);
}

//
void I2CSendByte(unsigned char byt)
{
    unsigned char i;
	
    for(i=0; i<8; i++){
        scl = 0;
		I2C_Delay(DELAY_TIME);
        if(byt & 0x80){
            sda = 1;
        }
        else{
            sda = 0;
        }
		I2C_Delay(DELAY_TIME);
        scl = 1;
        byt <<= 1;
		I2C_Delay(DELAY_TIME);
    }
	
    scl = 0;  
}

//
unsigned char I2CReceiveByte()
{
	unsigned char da;
	unsigned char i;
	for(i=0;i<8;i++){   
		scl = 1;
		I2C_Delay(DELAY_TIME);
		da <<= 1;
		if(sda) 
			da |= 0x01;
		scl = 0;
		I2C_Delay(DELAY_TIME);
	}
	return da;    
}

//
unsigned char I2CWaitAck()
{
	unsigned char ackbit;
	
    scl = 1;
	I2C_Delay(DELAY_TIME);
    ackbit = sda; 
    scl = 0;
	I2C_Delay(DELAY_TIME);
	
	return ackbit;
}

//
void I2CSendAck(unsigned char ackbit)
{
    scl = 0;
    sda = ackbit; 
	I2C_Delay(DELAY_TIME);
    scl = 1;
	I2C_Delay(DELAY_TIME);
    scl = 0; 
	sda = 1;
	I2C_Delay(DELAY_TIME);
}

//AD转换  读取AD
unsigned char Ad_Read(unsigned char addr)
{
	unsigned char temp;//接收返回值的变量
	I2CStart();//发送I2C启动信号
	I2CSendByte(0X90);//发送0X90 告诉单片机我开始写数据了 发送设备地址0x90（表示写模式）
	I2CWaitAck();//等待应答
	I2CSendByte(addr);//发送地址 发送addr地址，指定要读取的通道或寄存器。
	I2CWaitAck();//等待应答
	
	I2CStart();//再次发送I2C启动信号
	I2CSendByte(0X91);//发送0X91 发送设备地址0x91（表示读模式）
	I2CWaitAck();//等待应答
	temp = I2CReceiveByte();//读取值 读取AD转换器的值
	
	I2CSendByte(1);//发送非应答信号 发送非应答信号并停止I2C通信
	I2CStop();//停止
	
	return temp; //返回读取的值
	
}

void Da_Write(unsigned char dat)
{
	I2CStart();//启动单总线 发送I2C启动信号
	I2CSendByte(0X90);//发送0X90 告诉单片机我开始写数据了 发送设备地址0x90（表示写模式）
	I2CWaitAck();//等待应答	
	
	I2CSendByte(0X41);//使能DAC转换 发送命令字0x41（用于使能DAC转换）
	I2CWaitAck();//等待应答	
	I2CSendByte(dat);//写 发送dat数据，指定DA转换器的输出值
	I2CWaitAck();//等待应答	
	I2CStop();//停止 停止I2C通信
	
}

//函数名：写EEPROM函数
//入口参数：需要写入的字符串，写入的地址(务必为8的倍数)，写入数量
//返回值：无
//函数功能：向EERPOM的某个地址写入字符串中特定数量的字符。
void EEPROM_Write(unsigned char* EEPROM_String, unsigned char addr, unsigned char num)
{
	I2CStart();//发送开启信号
	I2CSendByte(0XA0);//选择EEPROM芯片，确定写的模式
	I2CWaitAck();//等待EEPROM反馈
	
	I2CSendByte(addr);//写入要存储的数据地址
	I2CWaitAck();//等待EEPROM反馈		

	while(num--)
	{
		I2CSendByte(*EEPROM_String++);//将要写入的信息写入
		I2CWaitAck();//等待EEPROM反馈		
		I2C_Delay(200);	
	}
	I2CStop();//停止发送	
}


//函数名：读EEPROM函数
//入口参数：读到的数据需要存储的字符串，读取的地址(务必为8的倍数)，读取的数量
//返回值：无
//函数功能：读取EERPOM的某个地址中的数据，并存放在字符串数组中。
void EEPROM_Read(unsigned char* EEPROM_String, unsigned char addr, unsigned char num)
{
	I2CStart();//发送开启信号 
	I2CSendByte(0XA0);//选择EEPROM芯片，确定写的模式
	I2CWaitAck();//等待EEPROM反馈
	
	I2CSendByte(addr);//写入要读取的数据地址
	I2CWaitAck();//等待EEPROM反馈		

	I2CStart();//发送开启信号
	I2CSendByte(0XA1);//选择EEPROM芯片，确定读的模式
	I2CWaitAck();//等待EEPROM反馈	
	
	while(num--)
	{
		*EEPROM_String++ = I2CReceiveByte();//将要写入的信息写入
		if(num) I2CSendAck(0);//发送应答
			else I2CSendAck(1);//不应答
	}
	
	I2CStop();//停止发送	
}


