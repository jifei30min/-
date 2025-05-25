//ͷ�ļ�������
#include <STC15F2K60S2.H>//��Ƭ���洢��ͷ�ļ�
#include <Init.H>//��ʼ���ײ�����ר��ͷ�ļ�
#include <Led.H>//Led�ײ�ȡ��ר��ͷ�ļ�
#include <Seg.h>//�����ײ�����ר��ͷ�ļ�
#include <Key.h>//����ܵײ�����ר��ͷ�ļ�
#include <ds1302.h>//ʱ������ר��ͷ�ļ�
#include "onewire.h"//�¶�ר��ģ��
#include "iic.h" //I2cģ��

//��������
unsigned char Key_Val,Key_Down,Key_Up,Key_Old;//����ɨ��ר�ñ���
unsigned int Key_Slow_Down; //��������ר�ñ���10ms
unsigned char Seg_Buf[8] = {1,2,3,4,5,6,7,8}; //�������ʾ���ݴ������
unsigned char Seg_Point[8] = {0,0,0,0,0,0,0,0};//�����С�������ݴ������
unsigned char Seg_Pos;//�����ɨ�����
unsigned int Seg_Slow_Down; //����ܼ���ר�ñ���500ms
unsigned char ucLed[8] = {0,0,0,0,0,0,0,0}; //LED��ʾ���ݴ������
unsigned char Smg_Mode = 1;//�����л� 1Ϊ�¶���ʾ���� 2Ϊ�������ý��� 3ΪDAC�������
float Tempartae_One = 0;//1�¶���ʾ�����¶�����
unsigned char Tempartae_Disp = 25;//2�¶���ʾ�����¶Ȳ���
unsigned char Tempartae_Set = 25;//�¶���ʾ�������ò���
float UC;//3��������ѹ����
unsigned char SmgS5 = 1;//1Ϊģʽ1 2Ϊģʽ2



//����������
void Key_Proc()
{
	//���Key_Slow_Down���㣬if����������ִ��return;��䣬Key_Proc()��ִֹͣ�У�����ִ�к����Key_Slow_Down = 1;
	if(Key_Slow_Down) return;
	
	//Key_Slow_Down == 0ʱ  Key_Slow_Down��Ϊ1
	Key_Slow_Down = 1;//�������ٳ���
	
	//�հ����
	Key_Val = Key_Read(); //��ȡ���µļ���ֵ
	
	Key_Down = Key_Val & (Key_Val ^ Key_Old);//��׽�½��� //����������һ˲���ֵ
	
	Key_Up = ~Key_Val & (Key_Val ^ Key_Old);//��׽������ //����̧����һ˲���ֵ
	
	Key_Old = Key_Val;//����ɨ�� //����һֱ���µ�ʱ���ֵ
	
	switch(Key_Down)
	{
		case 4:
			if(++Smg_Mode == 4) Smg_Mode = 1;
			if(Smg_Mode == 1) Tempartae_Disp = Tempartae_Set;
			if(Smg_Mode == 2) Tempartae_Set = Tempartae_Disp;
		break;
		
		//��
		case 8:
			if(Smg_Mode == 2)
			{
				if(--Tempartae_Disp < 10 ) Tempartae_Disp = 85;
			}
		break;
			
			//��
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

//��Ϣ������
void Seg_Proc()
{
	if(Seg_Slow_Down) return;
	Seg_Slow_Down = 1;//����ܼ��ٳ���
	
	//��ȡ�¶�
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
	
	//DAC�����ѹ
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
			Seg_Buf[6] = (unsigned int)(UC * 10) % 10; //������UC�ȱ�ǿ��ת����С������ʧ Ȼ����ʾ��׼ȷ������
			Seg_Buf[7] = (unsigned int)(UC * 100) % 10; //������UC�ȱ�ǿ��ת����С������ʧ Ȼ����ʾ��׼ȷ������
			Seg_Point[5] = 1;
		break;			
	}
	
}

//������ʾ����
void Led_Proc()
{
	if(SmgS5 == 1) Led_Disp(0,1);
	if(Smg_Mode == 1) Led_Disp(1,1);
	if(Smg_Mode == 2) Led_Disp(2,1);
	if(Smg_Mode == 3) Led_Disp(3,1);
}

//��ʱ��0��ʼ������
//12MHZ ��ʱ��0 1ms 16λ�Զ���װ�� 12T
void Timer0Init(void)		//1����@12.000MHz
{
	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0x18;		//���ö�ʱ��ֵ
	TH0 = 0xFC;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	
	ET0 = 1;
	EA = 1;
}


//��ʱ��0�жϷ�����
void Timer0Service() interrupt 1
{
	//�Զ���װ�� ����Ҫ����ֵ
	
	Key_Slow_Down++;
	if(Key_Slow_Down == 10) Key_Slow_Down=0; //if(++Key_Slow_Down == 10)Key_Slow_Down=0;
	
	Seg_Slow_Down++;
	if(Seg_Slow_Down == 500) Seg_Slow_Down=0;//if(++Seg_Slow_Down == 500) Seg_Slow_Down=0;
	
	if(++Seg_Pos == 8) Seg_Pos = 0;
	Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos],Seg_Point[Seg_Pos]);
	Led_Disp(Seg_Pos,ucLed[Seg_Pos]);
}

//Main����
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