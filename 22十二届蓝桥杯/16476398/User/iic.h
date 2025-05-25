#include <STC15F2K60S2.H>

static void I2C_Delay(unsigned char n);
void I2CStart();
void I2CStop();
void I2CSendByte(unsigned char byt);
unsigned char I2CReceiveByte();
unsigned char I2CWaitAck();
void I2CSendAck(unsigned char ackbit);
unsigned char Ad_Read(unsigned char addr);
void Da_Write(unsigned char dat);
void EEPROM_Write(unsigned char* EEPROM_String, unsigned char addr, unsigned char num);
void EEPROM_Read(unsigned char* EEPROM_String, unsigned char addr, unsigned char num);