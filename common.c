#include "def.h"

extern VIDEO_PCI_PROP video_pci_prop;


#define is_digit(c)	((c) >= '0' && (c) <= '9')

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

unsigned int StoH(char * s)
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
unsigned char hex_to_char(unsigned char hex_num)
{
    if ((hex_num>=0x30) && (hex_num<=0x39))
    {
        /* code */
        hex_num = '0'+ hex_num - 0x30;
    }
    else if ((hex_num>=0x41) && (hex_num<=0x5A))
    {
        /* code */
        hex_num = 'A' + (hex_num - 0x41);
    }
    else if ( (hex_num>=0x61) && (hex_num<=0x7A))
    {
        /* code */
        hex_num = 'a' + (hex_num - 0x61);
    }
    else
        hex_num = 0xff;
    return hex_num;

}
#ifdef __ubuntu__
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
#endif

#ifdef __dos__
void udelay(int x)
{
	unsigned int i;

	for (i = 0; i < x; ++i){
		_asm{
			mov	dx, 0x0ed 
			out	dx, al
		}
	}
}

void mdelay(int x)
{
	int i;
	for (i = 0; i < x; i ++)
			udelay(1000);
}
#endif

void write8(unsigned int index, unsigned char value)
{
	*(unsigned char*)(video_pci_prop.mapped_mmioBase + index) = value;
}

unsigned char read8(unsigned int index)
{
    return *((unsigned char*)(video_pci_prop.mapped_mmioBase+index));
}

/*
int atoi(const char *s)
{
	int i = 0;
	if(s==NULL)
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
*/

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

unsigned long htoi(const char *s)  
{  
    int i;  
    unsigned long n = 0;  
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

#ifdef __dos__
unsigned short ReadPciCfgWord(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg)
{
	unsigned short v;
	unsigned int flag = 0;

	_asm pushfd;
	_asm pop flag;
	_asm cli;
	outpd(0xCF8, (0x80000000|(bus<<16)|(dev<<11)|(func<<8)|(reg&0xFC)));
	v = inpw(0xCFC+(reg&3));
	_asm push flag;
	_asm popfd;

	return v;
}

unsigned int ReadPciCfgDword(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg)
{
	unsigned int v, flag = 0;

	_asm pushfd;
	_asm pop flag;
	_asm cli;
	outpd(0xCF8, (0x80000000|(bus<<16)|(dev<<11)|(func<<8)|(reg&0xFC)));
	v = inpd(0xCFC);
	_asm push flag;
	_asm popfd;

	return v;
}

void WritePciCfgByte(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg, unsigned char v)
{
	unsigned int flag = 0;

	_asm pushfd;
	_asm pop flag;
	_asm cli;
	outpd(0xCF8, (0x80000000|(bus<<16)|(dev<<11)|(func<<8)|(reg&0xFC)));
	outp(0xCFC+(reg&3), v);
	_asm push flag;
	_asm popfd;
}

void WritePciCfgWord(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg, unsigned short v)
{
	unsigned int flag = 0;

	_asm pushfd;
	_asm pop flag;
	_asm cli;
	outpd(0xCF8, (0x80000000|(bus<<16)|(dev<<11)|(func<<8)|(reg&0xFC)));
	outpw(0xCFC+(reg&3), v);
	_asm push flag;
	_asm popfd;
}

void WritePciCfgDword(unsigned char bus, unsigned char dev, unsigned char func, unsigned char reg, unsigned int v)
{
	unsigned int flag = 0;

	_asm pushfd;
	_asm pop flag;
	_asm cli;
	outpd(0xCF8, (0x80000000|(bus<<16)|(dev<<11)|(func<<8)|(reg&0xFC)));
	outpd(0xCFC, v);
	_asm push flag;
	_asm popfd;
}
#endif

int ReadMMIO(unsigned int addr,int size)
{
	int value = -1;

	switch(size)
	{
		case 0:
			value = *(unsigned char*)(video_pci_prop.mapped_mmioBase + addr);
			break;			
		case 1:
			value = *(unsigned short*)(video_pci_prop.mapped_mmioBase+addr);
			break;			
		case 2:
			value = *(unsigned int*)(video_pci_prop.mapped_mmioBase+addr);
	}

  return value;
}

void WriteMMIO(unsigned int addr,unsigned int value,int size)
{
	switch(size)
	{
		case S3X_BYTE:
			*(unsigned char*)(video_pci_prop.mapped_mmioBase + addr) = (unsigned char)(value&0xFF);
			break;
			
		case S3X_WORD:
			*(unsigned short*)(video_pci_prop.mapped_mmioBase+addr) = (unsigned short)(value&0xFFFF);
			break;
			
		case S3X_DWORD:
			*(unsigned int*)(video_pci_prop.mapped_mmioBase+addr) = value;
	}
}

void WriteMMIOMask(unsigned int address,unsigned int value, unsigned int mask)
{
    unsigned int temp;
    unsigned int reg_temp;
    reg_temp = ReadMMIO(address,S3X_DWORD);
    temp = ((reg_temp& (~mask)) | (value & mask));
    WriteMMIO(address, temp, S3X_DWORD);              
}
