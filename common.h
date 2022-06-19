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
