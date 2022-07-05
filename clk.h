#ifndef _CLK_H_
#define _CLK_H_


#define BASE_CR 0x8800
#define BASE_CR_B 0x8900
#define BASE_SR 0x8600

#define VPLL_REG	BASE_CR + 0xDA
#define VPLL_LOAD	BASE_CR + 0xE0

#define EPLL_REG	BASE_CR + 0xDD
#define EPLL_LOAD	BASE_CR + 0xE2

#define reference_freq 27


typedef struct{
    //byte1
    unsigned char band			:1; //VCO Frequency range setting
    unsigned char OD			:2;	//clock1 output division ration
    unsigned char PU			:1; //power up
    unsigned char reserved1		:4;
    //byte2
    unsigned char icpsel		:3; //Charge Pump current selection
    unsigned char francen		:1; //
    unsigned char div_m_bit7	:1; // PLL integer division setting bit8
    unsigned char reserved2		:3; 
    //byte3
    unsigned char div_m			:7;//PLL integer division setting bit[6:0]
    unsigned char reserved3		:1;
		
}Reg_Pll_reg;
typedef struct 
{
	unsigned char auto_load_pll_en		:1; //auto_load_epll_en
	unsigned char soft_load_set			:1;
	unsigned char soft_load_rst			:1;
	unsigned char soft_clk				:1;
	unsigned char reserved				:4;
}Reg_Pll_load;
typedef enum 
{
	PLL_DIU_D3_PLLOK=0,
	PLL_DIU_D2_PLLOK,
	PLL_DIU_D1_PLLOK,
	PLL_DIU_D4_PLLOK,
	M2PLL_LOCK,
	M1PLL_LOCK,
	M0PLL_LOCK,
	BIU_PMP_PCLK_LOCKED,
	VPLL_LOCK,
	EPLL_LOCK
}PLL_LOCK;

int mathlog2(unsigned int val);
int get_vepll(unsigned int addr);
int get_mpll(void);
void set_epll(int fclk);
void epll_stop(void);
void vpll_stop(void);
void set_vpll(int fclk);
void clk_prog(int argc, char *argv[]);
void wait_pll_ok(PLL_LOCK pll);
void clk_help_info(void);
void clk_prog(int argc, char *argv[]);
#endif
