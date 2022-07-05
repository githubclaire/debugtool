#include "def.h"

#define is_digit(c)	((c) >= '0' && (c) <= '9')

unsigned int StoH(unsigned char * s)
{
	unsigned int hdata;
	int i = 0;
	hdata = 0;
	while(s[i] != '\0')
	{
		if(s[i] >= '0' && s[i] <= '9')
			hdata = hdata*16 + (s[i] - 0x30);
		else if(s[i] >= 'a' && s[i] <= 'f')
			hdata = hdata *16 + (s[i]- 0x61 + 0x0a);
		else if(s[i] >= 'A' && s[i] <= 'F')
			hdata = hdata *16 + (s[i]- 0x41 + 0x0a);
			
		i++;
	}
	return hdata;
}
//for dos delay
void udelay(int t)
{
    volatile unsigned int i,j;
    for(i=0;i<t;i++)
    {
        for(j=0;j<367;j++);
    }
}

void mdelay(int x)
{
	int i;
	for (i = 0; i < x; i ++)
			udelay(1000);
}
/*
void udelay(int cont)
{
    usleep(cont);
}
 
void mdelay(int cont)
{
    int i;
    for(i=cont;i>0;i--)
    {
        udelay(1000);
    }
}
*/

int atoi(const char *s)
{
	int i = 0;
	if(s==0)
		return 0;
	while (is_digit(*s))
    {
        i = i * 10 + *((s)++) - '0';
    }
		

	return i;
}

unsigned int StoD(unsigned char * s)
{
	unsigned int hdata;
	int i = 0;
	hdata = 0;
	while(s[i] != '\0')
	{
		if(s[i] >= '0' && s[i] <= '9')
			hdata = hdata*10 + (s[i] - 0x30);			
		i++;
	}
	return hdata;
}


int tolower(int c)  
{  
    if (c >= 'A' && c <= 'Z')  
        {  
            return c + 'a' - 'A';  
        }  
    else  
        {  
            return c;  
        }  
} 

int htoi(const char *s)  
{  
    int i;  
    int n = 0;  
    if (s[0] == '0' && (s[1]=='x' || s[1]=='X'))  
        {  
            i = 2;  
        }  
    else  
        {  
            i = 0;  
        }  
    for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >='A' && s[i] <= 'Z');++i)  
        {  
            if (tolower(s[i]) > '9')  
                {  
                    n = 16 * n + (10 + tolower(s[i]) - 'a');  
                }  
            else  
                {  
                    n = 16 * n + (tolower(s[i]) - '0');  
                }  
        }  
    return n;  
}  
int ReadMMIO(unsigned int addr,int size)
{
  int value = -1;

  switch(size)
  {
    case 0:
      value = *(unsigned char*)(g_mmiobase + addr);
      break;
      
    case 1:
      value = *(unsigned short*)(g_mmiobase+addr);
      break;
      
    case 2:
      value = *(unsigned int*)(g_mmiobase+addr);
  }

  return value;

}

void WriteMMIO(unsigned int addr,unsigned int value,int size)
{

  switch(size)
  {
    case S3X_BYTE:
      *(unsigned char*)(g_mmiobase + addr) = (unsigned char)(value&0xFF);
      break;
      
    case S3X_WORD:
      *(unsigned short*)(g_mmiobase+addr) = (unsigned short)(value&0xFFFF);
      break;
      
    case S3X_DWORD:
      *(unsigned int*)(g_mmiobase+addr) = value;
  }
}

void WriteMMIOMask(unsigned int address,unsigned int value, unsigned int mask){
          unsigned int temp;
                  unsigned int reg_temp;
          reg_temp = ReadMMIO(address,S3X_DWORD);
                 // printf("reg_temp 0x%8.8x ;",reg_temp);
                                  //printf("value is 0x%8.8x;",value);
                                 // printf("mask is 0x%8.8x;",mask);
          temp = ((reg_temp& (~mask)) | (value & mask));
                 // printf("temp_MMIO is 0x%8.8x;",temp);
          WriteMMIO(address, temp, S3X_DWORD);
                 
}
