#ifndef __PCIE_H__
#define __PCIE_H__

#include "main.h"
#define PCI_GEN_ADDR    0x8072

#define PCIE_SPEED_8G   3
#define PCIE_SPEED_5G   2
#define PCIE_SPEED_2_5G 1

typedef struct _VIDEO_PCI_PROP
{
    unsigned short	VenderId;
    unsigned short	DeviceId;

    unsigned short	BusNum;
    unsigned short	DevNum;
    unsigned short	FuncNum;

    unsigned int	MmioBase;
    unsigned int	FBBase;
    unsigned long	mapped_mmioBase;
    unsigned long	mapped_FBBase;
}VIDEO_PCI_PROP, *PVIDEO_PCI_PROP;


int find_Base_Addr(configuration config);
void read_fw_version(void);
void read_bitwidth(unsigned long mapped_base);
void read_PCIe(unsigned long mapped_base);
int map_to_system_memory(unsigned long addr);
int find_Base_Addr_Dos(configuration config);
#endif
