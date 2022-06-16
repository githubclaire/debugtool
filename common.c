#include "def.h"

#define is_digit(c)	((c) >= '0' && (c) <= '9')

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
