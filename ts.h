#ifndef _TS_H_
#define _TS_H_

#define TS_BASE 0x8f000

#define TS_IRQ_MODE_TO_CPU		0x00
#define TS_IRQ_MODE_TO_PMP		0x01
#define TS_IRQ_MODE_TO_CPU_PMP	0x10

#define BOUNDARY_1			0x12A // 30.C
#define BOUNDARY_2			0X1F2 // 70.C
#define BOUNDARY_3			0X266 // 90.C
#define BOUNDARY_4			0X309 // 115.C
#define BOUNDARY_ZOOM_SW	0x08


typedef struct{
	unsigned int T_CB_IN_Range1			:1;
	unsigned int T_CB_IN_Range2			:1;
	unsigned int T_CB_IN_Range3			:1;
	unsigned int T_CB_IN_Range4			:1;
	unsigned int T_CB_IN_Range5			:1;
	unsigned int NORMAL_TEMP_RANGE		:1;
	unsigned int HIGH_TEMP_RANGE		:1;
    unsigned int TS_DOUT_VALID_IRQ		:1;
	unsigned int Reserved				:24;
}Reg_Ts_Irq;

typedef struct{
	unsigned int Range1_slow_Boundary	:10;
	unsigned int Range2_shigh_Boundary	:10;
	unsigned int reserved				:11;
	unsigned int Bounday_Zoom			:1;
}Reg_Ts_Range_Boundary0;
typedef struct{
	unsigned int Range3_slow_Boundary	:10;
	unsigned int Range4_shigh_Boundary	:10;
	unsigned int reserved				:11;
	unsigned int Bounday_Zoom			:1;
}Reg_Ts_Range_Boundary1;

typedef struct{
	unsigned int reserved		:10;
	unsigned int rstn			:1;
	unsigned int volMode		:1;
	unsigned int accCtrl		:5;
	unsigned int mode			:1;
	unsigned int reserved1		:6;
	unsigned int enable			:1;
	unsigned int reserved2		:7;
}Reg_Ts_Ctrl;
typedef struct{
	unsigned int Sample_Rate_Timer	:16;
	unsigned int reserved			:16;
}Reg_Ts_Sample_Rate_Ctrl;

typedef struct{
	unsigned int Ts_Dout		:10;
	unsigned int reserved		:22;
}Reg_Ts_Out_Value;

typedef struct{
	unsigned int				Irq_Mode;
	Reg_Ts_Irq					Irq_Status;
	Reg_Ts_Irq					Irq_En;
	Reg_Ts_Irq					Irq_Clr;
	Reg_Ts_Range_Boundary0		Range_Boundary_0;
	Reg_Ts_Range_Boundary1		Range_Boundary_1;
	Reg_Ts_Ctrl					Ctrl_Reg;
	Reg_Ts_Sample_Rate_Ctrl		Sample_Rate_Ctrl;
	Reg_Ts_Out_Value			Dout_Value;
	unsigned int				Dbg_Value;
}Reg_Ts;

void ts_init(void);
int GetTemperature(void);
int GetVoltage(void);
void ts_intr_handler(void);
void ts_control_handler(void);
int dout_to_temp(int val);
#endif
