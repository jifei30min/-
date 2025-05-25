#include <Led.H>

//addr:��Ҫ���ĸ���(0-7) enable:ʹ�ܱ�־λ,0��ʹ��(Ϩ��) 1Ϊʹ��(����)
void Led_Disp(unsigned char addr, unsigned char enable)
	{//static unsigned char��̬�ֲ�����:�ظ�����ʱ����ˢ�³�ֵ
	static unsigned char temp = 0x00;
	static unsigned char temp_old = 0xff;//��¼��һ�ε�״̬�������Ż�IO����
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