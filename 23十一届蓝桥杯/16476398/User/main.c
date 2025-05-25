//ͷ�ļ�������
#include <STC15F2K60S2.H>//��Ƭ���洢��ͷ�ļ�
#include <Init.H>//��ʼ���ײ�����ר��ͷ�ļ�
#include <Led.H>//Led�ײ�ȡ��ר��ͷ�ļ�
#include <Seg.h>//�����ײ�����ר��ͷ�ļ�
#include <Key.h>//����ܵײ�����ר��ͷ�ļ�
#include <ds1302.h>//ʱ������ר��ͷ�ļ�
#include "onewire.h"//�¶�ר��ģ��
#include "iic.h" //I2cģ��
#include "intrins.h"

//��������
unsigned char Key_Val,Key_Down,Key_Up,Key_Old;//����ɨ��ר�ñ���
unsigned int Key_Slow_Down; //��������ר�ñ���10ms
unsigned char Seg_Buf[8] = {1,2,3,4,5,6,7,8}; //�������ʾ���ݴ������
unsigned char Seg_Point[8] = {0,0,0,0,0,0,0,0};//�����С�������ݴ������
unsigned char Seg_Pos;//�����ɨ�����
unsigned int Seg_Slow_Down; //����ܼ���ר�ñ���500ms
unsigned char ucLed[8] = {0,0,0,0,0,0,0,0}; //LED��ʾ���ݴ������

unsigned char Smg_Mode = 1;//�������ʾ���� 1:���ݽ��� 2:��������
float temprate;//DS18B20�ɼ������¶�
unsigned char temprate_disp[] = {30, 20};//�¶Ȳ�����ʾ �¶�����Ĭ��Ϊ30 ����Ĭ��Ϊ20
unsigned char temprate_set[] = {30, 20};//�¶Ȳ�������
bit System_temprate = 0;//�¶������л�����λ
unsigned int temprate_slow = 0;//�¶��л���ʱ����500ms
unsigned char temprate_select = 0;//�¶Ȳ���ѡ�� 0ѡ��MAX 1ѡ��MIN

//��ʱ����
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
		
			//Smg_Mode++��֮��=2 �����������
			if(Smg_Mode == 2) //�����ݽ����л�����������
			{
				temprate_select = 1;//ÿ�δ����ݽ����л����������� ��Ĭ��ѡ��min
				temprate_disp[0] = temprate_set[0];
				temprate_disp[1] = temprate_set[1];				
			}
			
			//�Ӳ��������л������ݽ����
			if(Smg_Mode == 1)
			{
				//���MAX >= MIN temprate_disp ��ֵ���� temprate_set Ȼ�����л������ݽ���ʱ 
				//������ temprate_set��ֵ �� temprate_disp ʹ��ÿһ�ζ�����ʾ�ϴβ������õ�ֵ
				//���MAX < MIN ˵���������ò��ɹ� �� temprate_set�ĳ�ʼֵ����temprate_disp
				//�򽫸ı��
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

//��Ϣ������
void Seg_Proc()
{
	if(Seg_Slow_Down) return;
	Seg_Slow_Down = 1;//����ܼ��ٳ���
	
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

//������ʾ����
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

//��ʱ��0��ʼ������
//12MHZ ��ʱ��0 1ms 16λ�Զ���װ�� 12T
void Timer0Init()		//1����@12.000MHz
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
	
	if(++temprate_slow == 500)
	{
		temprate_slow = 0;
		System_temprate = !System_temprate;
	}
}

//Main����
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