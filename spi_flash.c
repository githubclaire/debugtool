#include "def.h"
#include <stdio.h>

extern VIDEO_PCI_PROP video_pci_prop;
static unsigned long SF_BASE_ADDR;

#define SFPRT printf
//for debug 
volatile unsigned char op_index=0; //0:erase, 1: write, 2:read
volatile unsigned long sectorNo, addrW,addrR, status_index;

#define TRANSFER_DELAY 0 //us

//for linux
void delay_us(int cont)
{
    udelay(cont);
}

void sf_exit(unsigned char extcode)
{
	printf("SPI Controller Error! Flash failed\n");
	exit(extcode);
}
 
// get sf base address
void sf_init(void)
{
	SF_BASE_ADDR = video_pci_prop.mapped_mmioBase;
	//enable GPIO_ST(Pad input) for sclk pad
	writel(0x80,SF_BASE_ADDR+0xa0024);
}

unsigned int sf_read_data(unsigned int addr, unsigned char len)
{
	Reg_SF_Control sf_ctrl;
	Reg_SF_Cmd_Addr sf_cmd_addr;
	volatile long i=0;
	unsigned int sf_data;	
	sf_cmd_addr.reg.cmd=0x03;
	sf_cmd_addr.reg.addr = addr;
	writel(sf_cmd_addr.uint32,SF_BASE_ADDR + SF_CMD_ADDR);

	sf_ctrl.reg.data_len = len;
	sf_ctrl.reg.addr_en =1;
	sf_ctrl.reg.rw = 0;	
	sf_ctrl.reg.en_flag = 1;
	if(sf_ctrl.reg.data_len > 4){
		sf_ctrl.reg.data_len=4;
	}
	delay_us(TRANSFER_DELAY);
	writeb(sf_ctrl.uint8,SF_BASE_ADDR + SF_CONTROL);

	//waitting for transfer have finished
	do{
		sf_ctrl.uint8 = readb(SF_BASE_ADDR+SF_CONTROL);
		i++;
		if(i>TIME_OUT_COUNTER)
		{
			SFPRT("%s: waitting for transfer timeout\n", __FUNCTION__);
			sf_exit(0);
			break;
		}
		delay_us(TRANSFER_DELAY);
	}	
	while(sf_ctrl.reg.en_flag);

	sf_data=readl(SF_BASE_ADDR + SF_RDATA);
	return sf_data;
}
unsigned short sf_read_status(void)
{
	volatile Reg_SF_Control sf_ctrl;
	volatile Reg_SF_Cmd_Addr sf_cmd_addr;
	volatile unsigned int sf_data;	
	volatile long i=0;

	sf_cmd_addr.reg.cmd=0x05;
	//sf_cmd_addr.reg.addr = addr;
	writel(sf_cmd_addr.uint32,SF_BASE_ADDR + SF_CMD_ADDR);

	sf_ctrl.reg.data_len = 2;
	sf_ctrl.reg.addr_en =0;
	sf_ctrl.reg.rw = 0;	
	sf_ctrl.reg.en_flag = 1;
	delay_us(TRANSFER_DELAY);
	writeb(sf_ctrl.uint8,SF_BASE_ADDR + SF_CONTROL);
	//waitting for transfer have finished
	do{
		sf_ctrl.uint8 = readb(SF_BASE_ADDR+SF_CONTROL);
		i++;
		if(i>TIME_OUT_COUNTER)
		{
			SFPRT("%s: waitting for transfer timeout\n", __FUNCTION__);
			sf_exit(0);
			break;
			
		}
	}	
	while(sf_ctrl.reg.en_flag);

	sf_data=readl(SF_BASE_ADDR + SF_RDATA);

	return sf_data & 0xffff;
}

unsigned short sf_read_chip_id(void)
{
	Reg_SF_Control sf_ctrl;
	Reg_SF_Cmd_Addr sf_cmd_addr;
	unsigned int sf_data;	

	sf_cmd_addr.reg.cmd=0x9f;
	//sf_cmd_addr.reg.addr = addr;
	writel(sf_cmd_addr.uint32,SF_BASE_ADDR + SF_CMD_ADDR);

	sf_ctrl.reg.data_len = 4;
	sf_ctrl.reg.addr_en =0;
	sf_ctrl.reg.rw = 0;	
	sf_ctrl.reg.en_flag = 1;
	delay_us(TRANSFER_DELAY);
	writeb(sf_ctrl.uint8,SF_BASE_ADDR + SF_CONTROL);

	//waitting for transfer have finished
	do{
		sf_ctrl.uint8 = readb(SF_BASE_ADDR+SF_CONTROL);
	}	
	while(sf_ctrl.reg.en_flag);

	sf_data=readl(SF_BASE_ADDR + SF_RDATA);

	return sf_data & 0xffff;
}
void sf_write_enable(void)
{
	Reg_SF_Control sf_ctrl;
	Reg_SF_Cmd_Addr sf_cmd_addr;
	volatile long i=0;	

	sf_cmd_addr.reg.cmd=0x06;
	writel(sf_cmd_addr.uint32,SF_BASE_ADDR + SF_CMD_ADDR);

	sf_ctrl.reg.data_len = 0;
	sf_ctrl.reg.addr_en =0;
	sf_ctrl.reg.rw = 1;	
	sf_ctrl.reg.en_flag = 1;
	delay_us(TRANSFER_DELAY);
	writeb(sf_ctrl.uint8,SF_BASE_ADDR + SF_CONTROL);

	//waitting for transfer have finished
	do{
		sf_ctrl.uint8 = readb(SF_BASE_ADDR+SF_CONTROL);
		i++;
		if(i>TIME_OUT_COUNTER)
		{
			SFPRT("%s: waitting for transfer timeout\n", __FUNCTION__);
			break;
		}
	}	
	while(sf_ctrl.reg.en_flag);
}

void sf_write_data(unsigned int addr, unsigned int data, unsigned char len)
{	
	Reg_SF_Control sf_ctrl;
	Reg_SF_Cmd_Addr sf_cmd_addr;
	volatile long i=0;	
	// if(addr%16==0)
	// 	printf("\n");

	// printf(" %02x",data);
	//delay_us(10);
	sf_write_enable();
	sf_cmd_addr.reg.cmd=0x02;
	sf_cmd_addr.reg.addr = addr;
	writel(sf_cmd_addr.uint32,SF_BASE_ADDR + SF_CMD_ADDR);

	writel(data,SF_BASE_ADDR + SF_WDATA);

	sf_ctrl.reg.data_len = len;
	sf_ctrl.reg.addr_en =1;
	sf_ctrl.reg.rw = 1;	
	sf_ctrl.reg.en_flag = 1;
	if(sf_ctrl.reg.data_len > 4){
		sf_ctrl.reg.data_len=4;
	}  
	delay_us(10);
	writeb(sf_ctrl.uint8,SF_BASE_ADDR + SF_CONTROL);

	//waitting for transfer have finished
	do{
		sf_ctrl.uint8 = readb(SF_BASE_ADDR+SF_CONTROL);
		i++;
		if(i>TIME_OUT_COUNTER)
		{
			SFPRT("%s: waitting for transfer timeout\n", __FUNCTION__);
			break;
		}
		delay_us(5);
	}	
	while(sf_ctrl.reg.en_flag);
	//delay_us(10);
	//waitting sf free
	i=0;
	while(sf_read_status()&0x01)
	{
		//delay_us(10);
		i++;
		if(i>TIME_OUT_COUNTER)
		{
			SFPRT("%s: waitting for sf free(sf_read_status) timeout\n", __FUNCTION__);
			break;
		}
	}
}
void sf_write_status(unsigned short data)
{
	Reg_SF_Control sf_ctrl;
	Reg_SF_Cmd_Addr sf_cmd_addr;
	volatile long i=0;	

	sf_cmd_addr.reg.cmd=0x01;
	writel(sf_cmd_addr.uint32,SF_BASE_ADDR + SF_CMD_ADDR);

	writel(data,SF_BASE_ADDR + SF_WDATA);

	sf_ctrl.reg.data_len = 2;
	sf_ctrl.reg.addr_en =0;
	sf_ctrl.reg.rw = 1;	
	sf_ctrl.reg.en_flag = 1;
	delay_us(TRANSFER_DELAY);
	writeb(sf_ctrl.uint8,SF_BASE_ADDR + SF_CONTROL);

	//waitting for transfer have finished
	do{
		sf_ctrl.uint8 = readb(SF_BASE_ADDR+SF_CONTROL);
		i++;
		if(i>TIME_OUT_COUNTER)
		{
			SFPRT("%s: waitting for transfer timeout\n", __FUNCTION__);
			break;
		}
	}	
	while(sf_ctrl.reg.en_flag);
	
}

void sf_sector_erase(unsigned int addr)
{
	Reg_SF_Control sf_ctrl;
	Reg_SF_Cmd_Addr sf_cmd_addr;
	volatile long i=0;	

	//delay_us(10);
	sf_write_enable();
	sf_cmd_addr.reg.cmd=0x20;
	sf_cmd_addr.reg.addr = addr;
	writel(sf_cmd_addr.uint32,SF_BASE_ADDR + SF_CMD_ADDR);

	sf_ctrl.reg.data_len=0;
	sf_ctrl.reg.addr_en =1;
	sf_ctrl.reg.rw = 1;	
	sf_ctrl.reg.en_flag = 1;
	if(sf_ctrl.reg.data_len > 4){
		sf_ctrl.reg.data_len=4;
	}  
	delay_us(10);
	writeb(sf_ctrl.uint8,SF_BASE_ADDR + SF_CONTROL);

	//waitting for transfer have finished
	do{
		sf_ctrl.uint8 = readb(SF_BASE_ADDR+SF_CONTROL);
		i++;
		if(i>TIME_OUT_COUNTER)
		{
			SFPRT("%s: waitting for transfer timeout\n", __FUNCTION__);
			break;
		}
	}	
	while(sf_ctrl.reg.en_flag);

	// waitting for sf free
	delay_us(500);
	i=0;
	while(sf_read_status()&0x01)
	{
		i++;
		if(i>TIME_OUT_COUNTER*20)
		{
			SFPRT("%s: waitting for sf free(sf_read_status) timeout\n", __FUNCTION__);
			break;
		}
		delay_us(500);
	}
}

void sf_flash_test(int argc, char *argv[])
{
	int sector_num = 1;
	unsigned int data;
	unsigned int addr;
	unsigned char len;

    if(strcmp(argv[1],"-f")==0 || strcmp(argv[1],"-F")==0)
    {
		if(argc==3)
		{
			sector_num = atoi(argv[2]);
		}
        sf_sector_erase(sector_num*FP_SECTOR_SIZE);
    }
	else if(strcmp(argv[1],"-r")==0 || strcmp(argv[1],"-R")==0){
        addr = atoi(argv[2]);
        len = atoi(argv[3]);
		data=sf_read_data(addr,len);
		printf("sf read data is 0x%x\n", data);
	}
	else{
		sf_help_info();
	}
}

void sf_help_info(void)
{
	printf("  sf -w addr data [len]: write spi flash data.\n");
	printf("  sf -r addr [len]: read spi flash data.\n");
	printf("  sf -f sector_num: erase spi flash.\n");
}
/*void writel(unsigned int data,unsigned long MmioBaseAddr)
{
	*(unsigned long *)(MmioBaseAddr) = data;
}
*/
// void sf_32k_erase(unsigned int addr)
// {
// 	Reg_SF_Control sf_ctrl;
// 	Reg_SF_Cmd_Addr sf_cmd_addr;	

// 	sf_write_enable();

// 	sf_cmd_addr.reg.cmd=0x52;
// 	sf_cmd_addr.reg.addr = addr;
// 	writel(sf_cmd_addr.uint32,SF_BASE_ADDR + SF_CMD_ADDR);

// 	sf_ctrl.reg.data_len=0;
// 	sf_ctrl.reg.addr_en =1;
// 	sf_ctrl.reg.rw = 1;	
// 	sf_ctrl.reg.en_flag = 1;

// 	writeb(sf_ctrl.uint8,SF_BASE_ADDR + SF_CONTROL);

// 	//waitting for transfer have finished
// 	do{
// 		sf_ctrl.uint8 = readb(SF_BASE_ADDR+SF_CONTROL);
// 	}	
// 	while(sf_ctrl.reg.en_flag);
// 	// waitting for sf free
// 	while(sf_read_status()&0x01);
// }
// void sf_64k_erase(unsigned int addr)
// {
// 	Reg_SF_Control sf_ctrl;
// 	Reg_SF_Cmd_Addr sf_cmd_addr;	

// 	sf_write_enable();

// 	sf_cmd_addr.reg.cmd=0xD8;
// 	sf_cmd_addr.reg.addr = addr;
// 	writel(sf_cmd_addr.uint32,SF_BASE_ADDR + SF_CMD_ADDR);

// 	sf_ctrl.reg.data_len=0;
// 	sf_ctrl.reg.addr_en =1;
// 	sf_ctrl.reg.rw = 1;	
// 	sf_ctrl.reg.en_flag = 1;
 
// 	writeb(sf_ctrl.uint8,SF_BASE_ADDR + SF_CONTROL);

// 	//waitting for transfer have finished
// 	do{
// 		sf_ctrl.uint8 = readb(SF_BASE_ADDR+SF_CONTROL);
// 	}	
// 	while(sf_ctrl.reg.en_flag);
// 	// waitting for sf free
// 	while(sf_read_status()&0x01);
// }
// void sf_chip_erase(unsigned int addr)
// {
// 	Reg_SF_Control sf_ctrl;
// 	Reg_SF_Cmd_Addr sf_cmd_addr;	

// 	sf_write_enable();

// 	sf_cmd_addr.reg.cmd=0x60;
// 	sf_cmd_addr.reg.addr = addr;
// 	writel(sf_cmd_addr.uint32,SF_BASE_ADDR + SF_CMD_ADDR);

// 	sf_ctrl.reg.data_len=0;
// 	sf_ctrl.reg.addr_en =1;
// 	sf_ctrl.reg.rw = 1;	
// 	sf_ctrl.reg.en_flag = 1;
 
// 	writeb(sf_ctrl.uint8,SF_BASE_ADDR + SF_CONTROL);

// 	//waitting for transfer have finished
// 	do{
// 		sf_ctrl.uint8 = readb(SF_BASE_ADDR+SF_CONTROL);
// 	}	
// 	while(sf_ctrl.reg.en_flag);
// 	// waitting for sf free
// 	while(sf_read_status()&0x01);
// }
