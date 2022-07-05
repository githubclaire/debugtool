#include "def.h"

unsigned int  maxVCO[2] = {1600,2400};
extern VIDEO_PCI_PROP video_pci_prop;

int mathlog2(unsigned int val)
{
	int i=0;
	while(val>1)
	{
		val>>=1;
		i++;
		if(i>8)
		{
			return -1;
		}
	}
	return i;
}

void write_evclk_to_spi_flash(unsigned int frequence)
{ 
	unsigned int j;
	unsigned char pBuffer[FP_SECTOR_SIZE];

	for(j=0;j<FP_SECTOR_SIZE;j++)
	{
		pBuffer[j]=sf_read_data(0x10000+j,1);
	}
	sf_sector_erase(0x10000);
	pBuffer[96]= frequence%256;
	pBuffer[97]= (frequence>>8)%256;
	pBuffer[98]= (frequence>>16)%256;
	pBuffer[99]= (frequence>>24)%256;    //0x10060
	pBuffer[112]= frequence%256;
	pBuffer[113]= (frequence>>8)%256;
	pBuffer[114]= (frequence>>16)%256;
	pBuffer[115]= (frequence>>24)%256;    //0x10070
	for(j=0;j<FP_SECTOR_SIZE;j+=4)
    {
        sf_write_data(0x10000 +j,*(unsigned int *)(pBuffer + j),4);
    }  
}


int get_clk(unsigned int M,unsigned int N,unsigned int R, unsigned int DIV)
{
	return (27*(M+2)>>(DIV+R));
}

int get_vepll(unsigned int addr)
{
    volatile Reg_Pll_reg *pPll_reg;
    //unlock biu register
    writeb(0x06,video_pci_prop.mapped_mmioBase+0x8608);
    writeb(0x48,video_pci_prop.mapped_mmioBase+0x8838);
    writeb(0xa5,video_pci_prop.mapped_mmioBase+0x8839);
    pPll_reg = (volatile Reg_Pll_reg*)(video_pci_prop.mapped_mmioBase+addr);
    if(*((unsigned char*)pPll_reg+0)==0 && *((unsigned char*)pPll_reg+1)==0 && *((unsigned char*)pPll_reg+2)==0)
        return 200;
    return get_clk(((pPll_reg->div_m_bit7<<7)|pPll_reg->div_m),0,0,pPll_reg->OD);
}

int get_mpll(void)
{
    unsigned int mpll_reg = readl(video_pci_prop.mapped_mmioBase+0xd130);
    unsigned int M,OD;
    //unlock biu register
    writeb(0x06,video_pci_prop.mapped_mmioBase+0x8608);
    writeb(0x48,video_pci_prop.mapped_mmioBase+0x8838);
    writeb(0xa5,video_pci_prop.mapped_mmioBase+0x8839);
    M = ((mpll_reg>>7)&0x7f)|(mpll_reg&(1<<4) ? 0x80:0);
    OD = (mpll_reg>>17)&0x3;
    return get_clk(M,0,0,OD);

}

unsigned int get_dpll(unsigned char i)
{
    unsigned int M,R,OD;
    //unlock biu register
    writeb(0x06,video_pci_prop.mapped_mmioBase+0x8608);
    writeb(0x48,video_pci_prop.mapped_mmioBase+0x8838);
    writeb(0xa5,video_pci_prop.mapped_mmioBase+0x8839);
    if(i==1)
    {
        M = *(unsigned char*)(video_pci_prop.mapped_mmioBase+BASE_SR+0x12);
        R = ((*(unsigned char*)(video_pci_prop.mapped_mmioBase+BASE_SR+0x12))>>2)&0x3;
        OD = ((*(unsigned char*)(video_pci_prop.mapped_mmioBase+BASE_CR_B+0xD0))>>6)&0x3;
    }
    else if(i==2)
    {
        M = *(unsigned char*)(video_pci_prop.mapped_mmioBase+BASE_SR+0x0E);
        R = ((*(unsigned char*)(video_pci_prop.mapped_mmioBase+BASE_SR+0x29))>>4)&0x3;
        OD = ((*(unsigned char*)(video_pci_prop.mapped_mmioBase+BASE_CR_B+0xD1))>>6)&0x3;
    }
    else if(i==3)
    {
        M = *(unsigned char*)(video_pci_prop.mapped_mmioBase+BASE_CR+0xB5);
        R = *(unsigned char*)(video_pci_prop.mapped_mmioBase+BASE_CR+0xB4)&0x3;
        OD = ((*(unsigned char*)(video_pci_prop.mapped_mmioBase+BASE_CR_B+0xD2))>>6)&0x3;
    }
    else if(i==4)
    {
        M = *(unsigned char*)(video_pci_prop.mapped_mmioBase+BASE_CR+0xD6);
        R = *(unsigned char*)(video_pci_prop.mapped_mmioBase+BASE_CR+0xD5)&0x3;
        OD = *(unsigned char*)(video_pci_prop.mapped_mmioBase+BASE_CR_B+0xD9)&0x3;
    }
    else
    {
        printf("DCLK index error\n");
        return 0;
    }
    
    if(M==0 && R==0 && OD==0)
        return 200;
    return get_clk(M,0,R,OD); 
}

void set_epll(int fclk)
{
    unsigned char bandNo=0;
    int M; //feedback divider, FN
	int OD; //output divider
	//int fclk_tar;
    volatile Reg_Pll_reg *pPll_reg;
    volatile Reg_Pll_load *pPll_load;
    pPll_reg = (volatile Reg_Pll_reg*)(video_pci_prop.mapped_mmioBase + EPLL_REG);
    pPll_load = (volatile Reg_Pll_load*)(video_pci_prop.mapped_mmioBase + EPLL_LOAD);
    if(fclk<150)
    {
        bandNo = 0;
    }
    else if(fclk<100)
    {
        printf("Error: The lowest frequency is 100MHz\n");
        return ;
    }
    else
    {
        bandNo = 1;
    }
    OD = mathlog2((unsigned int)(maxVCO[bandNo]/fclk));
    if(OD>3)
        OD=3;
    M = (fclk<<OD)/reference_freq - 2;

    writeb(0x06,video_pci_prop.mapped_mmioBase+0x8608);
    writeb(0x48,video_pci_prop.mapped_mmioBase+0x8838);
    writeb(0xa5,video_pci_prop.mapped_mmioBase+0x8839);
    //forbit pll lock reset
    writeb(0x04,video_pci_prop.mapped_mmioBase+0x8af3);
    //epll to parking pll
    
    writeb((1<<(readb(video_pci_prop.mapped_mmioBase+PCI_GEN_ADDR)-1)),video_pci_prop.mapped_mmioBase+0x80050);
    udelay(1); 
    writeb(0x10|(1<<(readb(video_pci_prop.mapped_mmioBase+PCI_GEN_ADDR)-1)),video_pci_prop.mapped_mmioBase+0x80050);
    
    pPll_load->auto_load_pll_en =1;
    pPll_load->soft_load_rst =1;
    
    pPll_reg->div_m = M&0x7f;
    pPll_reg->div_m_bit7 = (M>>7);
    pPll_reg->OD = OD;
    pPll_reg->band = bandNo;

    pPll_load->soft_load_set = 1;
    udelay(1);
    pPll_load->soft_load_set = 0;
    pPll_load->soft_load_rst = 0;
    wait_pll_ok(EPLL_LOCK);

    writeb((1<<(readb(video_pci_prop.mapped_mmioBase+PCI_GEN_ADDR)-1)),video_pci_prop.mapped_mmioBase+0x80050);
    
}

void epll_stop(void)
{
    volatile Reg_Pll_load *pPll_load;
    pPll_load = (volatile Reg_Pll_load*)(EPLL_LOAD);
    //unlock BIU register
    writeb(0x06,video_pci_prop.mapped_mmioBase+0x8608);
    writeb(0x48,video_pci_prop.mapped_mmioBase+0x8838);
    writeb(0xa5,video_pci_prop.mapped_mmioBase+0x8839);
    //forbit pll lock reset
    writeb(0x04,video_pci_prop.mapped_mmioBase+0x8af3);
    pPll_load->auto_load_pll_en =1;
    pPll_load->soft_load_rst =1;
}

void vpll_stop(void)
{
    volatile Reg_Pll_load *pPll_load;
    pPll_load = (volatile Reg_Pll_load*)(VPLL_LOAD);
    //unlock BIU register
    writeb(0x06,video_pci_prop.mapped_mmioBase+0x8608);
    writeb(0x48,video_pci_prop.mapped_mmioBase+0x8838);
    writeb(0xa5,video_pci_prop.mapped_mmioBase+0x8839);
    //forbit pll lock reset
    writeb(0x04,video_pci_prop.mapped_mmioBase+0x8af3);
    pPll_load->auto_load_pll_en =1;
    pPll_load->soft_load_rst =1;
}

void set_vpll(int fclk)
{
    unsigned char bandNo=0;
    int M; //feedback divider, FN
	int OD; //output divider
    volatile Reg_Pll_reg *pPll_reg;
    volatile Reg_Pll_load *pPll_load;

    pPll_reg = (volatile Reg_Pll_reg*)(video_pci_prop.mapped_mmioBase+VPLL_REG);
    pPll_load = (volatile Reg_Pll_load*)(video_pci_prop.mapped_mmioBase+VPLL_LOAD);
    if(fclk<150)
    {
        bandNo = 0;
    }
    else if(fclk<100)
    {
        printf("Error: The lowest frequency is 100MHz\n");
        return ;
    }
    else
    {
        bandNo = 1;
    }
    OD = mathlog2((unsigned int)(maxVCO[bandNo]/fclk));
    if(OD>3)
        OD=3;
    M = ((fclk<<OD)/reference_freq) - 2;
    //printf("M:0x%x,OD:%d,bandNO:%d\n",M,OD,bandNo);
    //start set pll config
    //unlock BIU register
    writeb(0x06,video_pci_prop.mapped_mmioBase+0x8608);
    writeb(0x48,video_pci_prop.mapped_mmioBase+0x8838);
    writeb(0xa5,video_pci_prop.mapped_mmioBase+0x8839);
    //forbit pll lock reset
    writeb(0x04,video_pci_prop.mapped_mmioBase+0x8af3);
    //epll to parking pll 
    writeb((1<<(readb(video_pci_prop.mapped_mmioBase+PCI_GEN_ADDR)-1)),video_pci_prop.mapped_mmioBase+0x80054);
    writeb((1<<(readb(video_pci_prop.mapped_mmioBase+PCI_GEN_ADDR)-1)),video_pci_prop.mapped_mmioBase+0x80058); 
    udelay(1); 
    writeb(0x10|(1<<(readb(video_pci_prop.mapped_mmioBase+PCI_GEN_ADDR)-1)),video_pci_prop.mapped_mmioBase+0x80054);
    writeb(0x10|(1<<(readb(video_pci_prop.mapped_mmioBase+PCI_GEN_ADDR)-1)),video_pci_prop.mapped_mmioBase+0x80058);
    pPll_load->auto_load_pll_en =1;
    pPll_load->soft_load_rst =1;
    
    pPll_reg->div_m = M&0x7f;
    pPll_reg->div_m_bit7 = (M>>7);
    pPll_reg->OD = OD;
    pPll_reg->band = bandNo;
    
    pPll_load->soft_load_set = 1;
    udelay(1);
    pPll_load->soft_load_set = 0;
    pPll_load->soft_load_rst = 0;
    wait_pll_ok(VPLL_LOCK);
    writeb((1<<(readb(video_pci_prop.mapped_mmioBase+PCI_GEN_ADDR)-1)),video_pci_prop.mapped_mmioBase+0x80054);
    writeb((1<<(readb(video_pci_prop.mapped_mmioBase+PCI_GEN_ADDR)-1)),video_pci_prop.mapped_mmioBase+0x80058);   
}

void wait_pll_ok(PLL_LOCK pll)
{
	unsigned int val;
	do{
		val = readl(video_pci_prop.mapped_mmioBase+0x80048);
	}while((val&(1<<pll)) !=(1<<pll));
}

void clk_prog(int argc, char *argv[])
{
	int  clk;

	if(argc == 2)
	{
		if (strcmp(argv[1], "d") == 0)
		{	
			printf("DClk1 = %d MHz\n",get_dpll(1));
            printf("DClk2 = %d MHz\n",get_dpll(2));
            printf("DClk3 = %d MHz\n",get_dpll(3));
            printf("DClk4 = %d MHz\n",get_dpll(4));
		}		
		else if (strcmp(argv[1], "e") == 0)
		{	
            printf("EClk = %d MHz\n",get_vepll(EPLL_REG));
		}
		else if (strcmp(argv[1], "v") == 0)
		{	
			printf("VClk = %d MHz\n",get_vepll(VPLL_REG));
		}	
        else if (strcmp(argv[1], "m") == 0)
		{	
            printf("Memory Clk : %d MHz\n",get_mpll());
            read_bitwidth();            
            printf("DDR Version : DDR4\n");
		}	
		else
		{
			clk_help_info();	
		}
	}
	else if(argc == 3)
	{		
		clk = atoi(argv[2]);
		if (strcmp(argv[1], "e") == 0)
		{	
            set_epll(clk*2);
            printf("EClk = %d MHz\n",get_vepll(EPLL_REG));
		}
		else if (strcmp(argv[1], "v") == 0)
		{	
            set_vpll(clk*2);
			printf("VClk = %d MHz\n",get_vepll(VPLL_REG));
		}
        else if (strcmp(argv[1], "f") == 0)
		{
            sf_init();
            set_epll(clk*2);
            printf("EClk = %d MHz\n",get_vepll(EPLL_REG));
            set_vpll(clk*2);
			printf("VClk = %d MHz\n",get_vepll(VPLL_REG));
            write_evclk_to_spi_flash(clk*10000);
        }
        else
        {
            clk_help_info();
        }
	}
    else
    {
        clk_help_info();
    }

}

void clk_help_info(void)
{
    printf("  clk d/e/v: clock d/e/v read.\n");
    printf("  clk e/v data(MHz)]: clock e/v write.\n");
    printf("  clk f data(MHz)]: clock e/v write to spi flash.\n");
}
