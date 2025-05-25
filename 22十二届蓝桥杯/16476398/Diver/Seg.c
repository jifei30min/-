#include <Seg.h>

                            // 0   1   2     3    4    5    6    7   8    9   10A  11B   12C 13D   14E  15F  16-  17Ãð 18P
unsigned char seg_dula[] = {0XC0,0XF9,0XA4,0XB0,0X99,0X92,0X82,0XF8,0X80,0X90,0X88,0X83,0XC6,0XA1,0X86,0X8E,0XBF,0XFF,0X8C};
unsigned char seg_wela[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

void Seg_Disp(unsigned char wela, unsigned char dula, unsigned char point)
{
	//ÏûÓ°
	P0 = 0xff;
	P2 = P2 & 0x1f | 0xe0;
	P2 &= 0x1f;

	P0 = seg_wela[wela];
	P2 = P2 & 0x1f | 0xc0;
	P2 &= 0x1f;
	
	P0 = seg_dula[dula];
	if(point)
		P0 &= 0x7f;
	P2 = P2 & 0x1f | 0xe0;
	P2 &= 0x1f;	
}