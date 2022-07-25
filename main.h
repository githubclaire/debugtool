#ifndef _MAIN_H_
#define _MAIN_H_

#define writel(val, addr)  (*(volatile unsigned int *)(addr) = (val))
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
    struct
    {
        unsigned int vendorid;
        unsigned int deviceid;
    }id;
    struct
    {
        unsigned long mmiobase;
        unsigned long fbbase;
    }addr;
    struct
    {
        unsigned int serialport0;
        unsigned int serialport1;
        unsigned int serialport2;
        unsigned int i2c_delay;
    }i2c;
    struct
    {
        unsigned int uart_enable;
        unsigned int ioport_no;
        unsigned int uart_baud_rate;
    }uart;
    struct 
    {
        unsigned int debug;
    }print;
    struct 
    {
        unsigned int mmio;
    }reg;
} configuration;

typedef enum
{
	MMIO = 0,
	FRAME_BUFFER,
	CPU_MEM	
}IOTYPE;

#endif
