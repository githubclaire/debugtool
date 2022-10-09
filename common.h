#ifndef _COMMON_H_
#define _COMMON_H_

typedef enum
{ 
  S3X_BYTE =0,
  S3X_WORD =1,
  S3X_DWORD=2,
}SIZE;


void udelay(int cont);
void mdelay(int cont);
//int atoi(const char *s);
//unsigned int StoD(unsigned char * s);
unsigned int StoH(char * s);
unsigned char hex_to_char(unsigned char hex_num);
int tolower(int c);
unsigned long htoi(const char *s);
void write8(unsigned int index, unsigned char value);
unsigned char read8(unsigned int index);
int ReadMMIO(unsigned int addr,int size);
void WriteMMIO(unsigned int addr,unsigned int value,int size);
void Delay1us(unsigned long x);
void WriteMMIOMask(unsigned int address,unsigned int value, unsigned int mask);
unsigned char ReadPciCfgByte(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg);
unsigned short ReadPciCfgWord(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg);
unsigned int ReadPciCfgDword(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg);
void WritePciCfgByte(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg, unsigned char v);
void WritePciCfgWord(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg, unsigned short v);
void WritePciCfgDword(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg, unsigned int v);

#endif

