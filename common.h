#ifndef _COMMON_H_
#define _COMMON_H_

unsigned int StoH(unsigned char * s);
void udelay(int cont);
void mdelay(int cont);
int atoi(const char *s);
unsigned int StoD(unsigned char * s);
int tolower(int c);
int htoi(const char *s);
int ReadMMIO(unsigned int addr,int size);
void WriteMMIO(unsigned int addr,unsigned int value,int size);
//unsigned int StoH(unsigned char * s);
void Delay1us(unsigned long x);
void WriteMMIOMask(unsigned int address,unsigned int value, unsigned int mask);

#endif
