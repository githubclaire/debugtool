#ifndef _FS_H_
#define _FS_H_

#define FS0_BASE 0x8D000
#define FS1_BASE 0x8C000

#define FS0_GPIO	I2C1_SDA
#define FS1_GPIO	GPIO21

typedef struct{
	unsigned int fs_en			:1;
	unsigned int fs_clr			:1;
	unsigned int reserved		:30;
}Reg_Fs_Ctrl;
typedef struct{
	unsigned int time_out		:1;
	unsigned int complete 		:1;
	unsigned int cpu			:1;
	unsigned int pmp			:1;
	unsigned int reserved		:28;
}Reg_FS_Int;
typedef struct{
	unsigned int time_out		:1;
	unsigned int complete 		:1;
	unsigned int reserved		:30;
}Reg_FS_Int_Status_Clr;

typedef struct{
	Reg_Fs_Ctrl ctrl;
	Reg_FS_Int status;
	Reg_FS_Int_Status_Clr status_clr;
	Reg_FS_Int int_en;
	Reg_FS_Int int_clr;
	unsigned int timeout_cnt_set;
	unsigned int timeout_cnt_val;
	unsigned int period_cnt_val; // CntData0
	unsigned int high_level_cnt_val; //CntData1
}Reg_Fs;

//void fs_init(void);
unsigned int get_fs_speed(unsigned char index);
//void fs0_intr_handler(void);
//void fs1_intr_handler(void);

#endif
