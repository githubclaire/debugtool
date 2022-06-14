#include "def.h"

extern VIDEO_PCI_PROP video_pci_prop;

int GetVoltage(void)
{
	unsigned int val,num=0;
	volatile Reg_Ts *pRegTs1 = (volatile Reg_Ts *)(video_pci_prop.mapped_mmioBase+TS_BASE);
    loop:
        pRegTs1->Ctrl_Reg.volMode = 1;
        udelay(500);
        pRegTs1->Ctrl_Reg.rstn = 0;
        udelay(500);
        pRegTs1->Ctrl_Reg.rstn = 1;
        udelay(500);
        pRegTs1->Irq_En.TS_DOUT_VALID_IRQ = 1;
        pRegTs1->Irq_Status.TS_DOUT_VALID_IRQ = 1;
        while(pRegTs1->Irq_Status.TS_DOUT_VALID_IRQ == 0);
        pRegTs1->Irq_Clr.TS_DOUT_VALID_IRQ = 1;
        if(pRegTs1->Ctrl_Reg.volMode == 0)
            {
                num++;
                if(num>=10)
                    {
                        perror("ERROR : GetVoltage error !!!\n Ctrl_Reg.volMode unable to write!\n");
                        return(-1157);
                    }
                goto loop;
            }
    val = pRegTs1->Dout_Value.Ts_Dout;
	return val;
}

void ts_init(void)
{	
    volatile Reg_Ts *pRegTs = (volatile Reg_Ts *)(video_pci_prop.mapped_mmioBase+TS_BASE);
	pRegTs->Ctrl_Reg.rstn = 0;
	pRegTs->Ctrl_Reg.rstn = 1;
	pRegTs->Sample_Rate_Ctrl.Sample_Rate_Timer = 0xffff;

#if 0
	pRegTs->Range_Boundary_0.Bounday_Zoom = 1;
	pRegTs->Range_Boundary_0.Range1_slow_Boundary = 0x130;
	pRegTs->Range_Boundary_0.Range2_shigh_Boundary = 0x140;
	pRegTs->Range_Boundary_1.Bounday_Zoom = 1;
	pRegTs->Range_Boundary_1.Range3_slow_Boundary = 0x150;
	pRegTs->Range_Boundary_1.Range4_shigh_Boundary = 0x155;

	//pRegTs->Irq_En.TS_DOUT_VALID_IRQ = 1;
	pRegTs->Irq_En.T_CB_IN_Range1 = 1;
	pRegTs->Irq_En.T_CB_IN_Range2 = 1;
	pRegTs->Irq_En.T_CB_IN_Range3 = 1;
	pRegTs->Irq_En.T_CB_IN_Range4 = 1;
	pRegTs->Irq_En.T_CB_IN_Range5 = 1;

	pRegTs->Irq_Mode = TS_IRQ_MODE_TO_PMP;
#endif
	pRegTs->Ctrl_Reg.mode = 1;
	pRegTs->Ctrl_Reg.enable = 1;	
}

int GetTemperature(void)
{
	volatile Reg_Ts *pRegTs = (volatile Reg_Ts *)(video_pci_prop.mapped_mmioBase+TS_BASE);
	pRegTs->Ctrl_Reg.volMode = 0;
	pRegTs->Ctrl_Reg.rstn = 0;
	pRegTs->Ctrl_Reg.rstn = 1;
	pRegTs->Irq_En.TS_DOUT_VALID_IRQ = 1;
	pRegTs->Irq_Status.TS_DOUT_VALID_IRQ = 1;
	while(pRegTs->Irq_Status.TS_DOUT_VALID_IRQ == 0);
	
	pRegTs->Irq_Clr.TS_DOUT_VALID_IRQ = 1;
	return pRegTs->Dout_Value.Ts_Dout;
	
}
int dout_to_temp(int val)
{
	int a,b,c,d;
	int temp;

	a =-9*val*val/100;
	b = 2754*val/10;
	temp = a+b-44596;
	return temp;
}