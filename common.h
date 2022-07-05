#ifndef _COMMON_H_
#define _COMMON_H_

unsigned int StoH(unsigned char * s);
void udelay(int cont);
void mdelay(int cont);
int atoi(const char *s);
unsigned int StoD(unsigned char * s);
int tolower(int c);
unsigned long htoi(const char *s);
int ReadMMIO(unsigned int addr,int size);
void WriteMMIO(unsigned int addr,unsigned int value,int size);
//unsigned int StoH(unsigned char * s);
void Delay1us(unsigned long x);
void WriteMMIOMask(unsigned int address,unsigned int value, unsigned int mask);
typedef enum
{ 
  S3X_BYTE =0,
  S3X_WORD =1,
  S3X_DWORD=2,
}SIZE;

#endif

#ifndef _COMMON_H_
#define _COMMON_H_

unsigned int StoH(char * s);
void udelay(int cont);
void mdelay(int cont);
//int atoi(const char *s);
//unsigned int StoD(unsigned char * s);
int tolower(int c);
unsigned long htoi(const char *s);
void write8(unsigned int index, unsigned char value);
unsigned char read8(unsigned int index);
unsigned char ReadPciCfgByte(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg);
unsigned short ReadPciCfgWord(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg);
unsigned int ReadPciCfgDword(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg);
void WritePciCfgByte(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg, unsigned char v);
void WritePciCfgWord(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg, unsigned short v);
void WritePciCfgDword(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg, unsigned int v);

#endif

