#ifndef __SPI_FLASH_H__
#define __SPI_FLASH_H__

//#define writel(val, addr)  (*(unsigned long *)(addr) = (val))
#define readl(addr)  (*(volatile unsigned int*)(addr))
#define writeb(val, addr)  (*(volatile unsigned char*)(addr) = (val))
#define readb(addr)  (*(volatile unsigned char*)(addr))

//#define SF_BASE_ADDR 	0x0000
#define SF_CONTROL 		0x8a33 // one byte
#define SF_CMD_ADDR		0x8a34 // byte0: cmd, byte1`byte3: addr
#define SF_WDATA		0x8a38 
#define SF_RDATA		0x8a3c

#define GRAY_VBIOS_START_ADDR 0x10000 

#define TIME_OUT_COUNTER 100000

typedef union 
{
	unsigned char uint8;
	struct 
	{
		unsigned char nop0		:2;
		unsigned char data_len	:3;	// data length:0-4 bytes
		unsigned char rw		:1; // 0:read, 1:write
		unsigned char addr_en	:1; // 0: not send 3 bytes address, 1: send 3 bytes address.
		unsigned char en_flag	:1; // write: write one to start once transform; read: 0: free, 1: busy
	}reg;	
}Reg_SF_Control;

typedef union 
{
	unsigned int uint32;
	struct 
	{
		unsigned int cmd		:8; // nor flash command
		unsigned int addr		:24;	// nor flash 24 bits address
	}reg;	
}Reg_SF_Cmd_Addr;

typedef union 
{
	unsigned int uint32;
	struct 
	{
		unsigned int byte0		:8; 
		unsigned int byte1		:8;
		unsigned int byte2		:8;
		unsigned int byte3		:8;
	}reg;	
}Reg_SF_Data;

void sf_init(void);
unsigned int sf_read_data(unsigned int addr,unsigned char len);
unsigned short sf_read_status(void);
unsigned short sf_read_chip_id(void);
void sf_write_enable(void);
void sf_write_data(unsigned int addr, unsigned int data, unsigned char len);
void sf_write_status(unsigned short data);
void sf_sector_erase(unsigned int addr);
void sf_read_status_test();
void sf_flash_test(int argc, char *argv[]);
//void writel(unsigned int data,unsigned long MmioBaseAddr);
// void sf_32k_erase(unsigned int addr);
// void sf_64k_erase(unsigned int addr);
// void sf_chip_erase(unsigned int addr);


#endif
