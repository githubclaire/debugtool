#ifndef _MAIN_H_
#define _MAIN_H_

#define writel(val, addr)  (*(unsigned long *)(addr) = (val))
#define readl(addr)  (*(volatile unsigned int*)(addr))
#define writeb(val, addr)  (*(volatile unsigned char*)(addr) = (val))
#define readb(addr)  (*(volatile unsigned char*)(addr))


#ifndef  NULL
#define  NULL (void*)0
#endif

#define TRUE 	1
#define FALSE 	0

typedef struct
{
    int vendorid;
    int deviceid;
} configuration;

typedef enum
{
	MMIO = 0,
	FRAME_BUFFER,
	CPU_MEM	
}IOTYPE;

#endif