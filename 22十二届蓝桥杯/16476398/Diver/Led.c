#include <Led.H>

//addr:是要亮哪个灯(0-7) enable:使能标志位,0不使能(熄灭) 1为使能(点亮)
void Led_Disp(unsigned char addr, unsigned char enable)
	{//static unsigned char静态局部变量:重复调用时不会刷新初值
	static unsigned char temp = 0x00;
	static unsigned char temp_old = 0xff;//记录上一次的状态，用于优化IO操作
		if(enable) 
			temp |= 0x01 << addr;
		else  
			temp &= ~(0x01 << addr);
		if(temp != temp_old)
		{
			P0 = ~temp;
			P2 = P2 & 0X1F | 0X80;
			P2 &= 0X1F;
			temp_old = temp;
			
		}
}