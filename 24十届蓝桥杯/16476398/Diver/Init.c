#include <Init.H>

//�ر�LED �̵��� ������
void System_Init()
{
	P2 = (P2 & 0X1F) | 0X80; 
	P0 = 0XFF;
	//��������������֮���Ϊԭ��
	P2 = P2 & 0X1F;
	
	P2 = (P2 & 0X1F) | 0XA0; 
	P0 = 0X00;
	P2 = P2 & 0X1F;
}

