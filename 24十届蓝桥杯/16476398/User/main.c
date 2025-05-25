//ͷ�ļ�������
#include <STC15F2K60S2.H>//��Ƭ���洢��ͷ�ļ�
#include <Init.H>//��ʼ���ײ�����ר��ͷ�ļ�
#include <Led.H>//Led�ײ�ȡ��ר��ͷ�ļ�
#include <Seg.h>//�����ײ�����ר��ͷ�ļ�
#include <Key.h>//����ܵײ�����ר��ͷ�ļ�
#include "iic.h"


//�ҵĴ�����S6�޷��ر�LED��ԭ������Ϊ���ڵ���LEDʱ û��ͨ��ucLed[]����ֵ ����ֱ�ӵ���LED������ֵ�� ���Լ������ڶ�ʱ1���ն��иı���ucLed[]��ֵ Ҳ�޼�����
//����:����LED������ͨ��ucLed[]������У�ȷ��״̬һ����

//��������
unsigned char Key_Val,Key_Down,Key_Up,Key_Old;//����ɨ��ר�ñ���
unsigned int Key_Slow_Down; //��������ר�ñ���10ms
unsigned char Seg_Buf[8] = {1,2,3,4,5,6,7,8}; //�������ʾ���ݴ������
unsigned char Seg_Point[8] = {0,0,0,0,0,0,0,0};//�����С�������ݴ������
unsigned char Seg_Pos;//�����ɨ�����
unsigned int Seg_Slow_Down; //����ܼ���ר�ñ���500ms
unsigned char ucLed[8] = {0,0,0,0,0,0,0,0}; //LED��ʾ���ݴ������

unsigned char Smg_Mode = 1;//�������ʾ���� 1ΪƵ����ʾ���� 2Ϊ��ѹ��ʾ����
unsigned int Dat;//Ƶ��
unsigned int Timer1000ms;//��ʱ��1 1000ms
float UC;//RB2�������ѹ
float DAC_Send;//DAC�����ѹ
bit DAC_System = 1;//DAC���ģʽ�л�
bit LED_System = 1;//LED���������л�

bit SMG_System = 1;//����ܹ����л�




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

//��Ϣ������
void Seg_Proc()
{
	if(Seg_Slow_Down) return;
	Seg_Slow_Down = 1;//����ܼ��ٳ���
	
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

//������ʾ����
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


//��ʱ��0 ����ģʽ 16λ
void Timer0Init()		//0����@12.000MHz
{
	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TMOD |= 0x05;		//���ö�ʱ��ģʽ
	TL0 = 0x00;		//���ö�ʱ��ֵ
	TH0 = 0x00;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
}

//��ʱ��1 ��ʱģʽ 16λ�Զ���װ�� 1ms
void Timer1Init()		//1����@12.000MHz
{
	AUXR |= 0x40;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
	TL1 = 0x20;		//���ö�ʱ��ֵ
	TH1 = 0xD1;		//���ö�ʱ��ֵ
	TF1 = 0;		//���TF1��־
	TR1 = 1;		//��ʱ��1��ʼ��ʱ
	
	ET1 = 1;
	EA = 1;
}


//��ʱ��1�жϷ�����
void Timer1Service() interrupt 3
{
	//�Զ���װ�� ����Ҫ����ֵ
	
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
		TL0 = 0x00;		//���ü�����ֵ
		TH0 = 0x00;		//���ü�����ֵ
	}
}

//Main����
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