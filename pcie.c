#include "def.h"

#define PAGE_SIZE 0x100000

extern VIDEO_PCI_PROP video_pci_prop;


int find_Base_Addr(configuration config)
{	
	struct pci_access * myaccess;
	struct pci_dev * mydev;
	
	unsigned int classCode;
	unsigned short PCIcmd;
	
	myaccess = pci_alloc();	
	pci_init(myaccess);	
	pci_scan_bus(myaccess);		
	
	for (mydev = myaccess->devices; mydev; mydev = mydev->next)
    {		
		pci_fill_info(mydev, PCI_FILL_IDENT | PCI_FILL_BASES | PCI_FILL_CLASS);

		if(mydev->vendor_id == config.vendorid && mydev->device_id == config.deviceid)
		{
            classCode = pci_read_long(mydev,0x08);
			if((classCode & 0xFF000000) == 0x03000000)
			{
				pci_write_byte(mydev, 0x04,0x06);
				PCIcmd = pci_read_long(mydev, 0x04);	
				//printf("PCIcmd:0x%04x\n",PCIcmd);
				if(PCIcmd&0x02)
				{
                    printf("BUS_DEV_FUNC is : %.2x:%.2x.%.1x\n",mydev->bus,mydev->dev,mydev->func);
					video_pci_prop.VenderId = mydev->vendor_id;
					video_pci_prop.DeviceId = mydev->device_id;
                    printf("PCIe Vender ID = %x, Device ID = %x\n",video_pci_prop.VenderId,video_pci_prop.DeviceId);

					video_pci_prop.MmioBase = pci_read_long(mydev, 0x10) & 0xFFFFFF00;	
                    printf("PCIE MMIOBase = %x\n",video_pci_prop.MmioBase);

					int fb = open("/dev/mem",O_RDWR | O_SYNC);				
					if(fb<0)
					{
						printf("Can't open!!\n");
					}
					
					video_pci_prop.mapped_mmioBase =(unsigned long *)mmap(0,PAGE_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,fb,video_pci_prop.MmioBase);				
                    printf("PCIE mapped MMIOBase = %x\n",(unsigned int)video_pci_prop.mapped_mmioBase);
                    if(video_pci_prop.mapped_mmioBase == 0)
					{
						printf("Can't mmap!\n");
					}
					else
					{
						return 1;
					}

				}
				else
				{
					continue;
				}

			}															
		}
	}
	return 0;
}

void read_PCIe(unsigned long mapped_base)
{
    unsigned int pcie_info_addr = 0x8070;
    unsigned int pcie_info = *(unsigned long *)(mapped_base + pcie_info_addr);
				
    printf("PCIe Speed : ");
    switch ((pcie_info & 0x000f0000)>>16)
    {
        case PCIE_SPEED_8G:
            printf("8.0GT/s\n");break;
        case PCIE_SPEED_5G:
            printf("5.0GT/s\n");break;
        case PCIE_SPEED_2_5G:
            printf("2.5GT/s\n");break;       
        default:
            break;
    }

    printf("PCIe Width : x%d\n",(pcie_info & 0x0ff00000)>>20);
}

void read_fw_version(void)
{
	unsigned int i, data;
	data = sf_read_data(0xf010,4);
	printf("Vbios Version : ");	
	for(i = 0;i<3;i++)
	{
		printf("%2.2x:",(data&0xff000000)>>24);
		data=data<<8;
	}
	printf("%2.2x \n",(data&0xff000000)>>24);
}

void read_bitwidth(unsigned long mapped_base)
{
    unsigned int i,j,data,MIU_ch[2];
    float t;
	data = readl(mapped_base+0xd000);
    i = data&0x07;
    MIU_ch[0] = (data&0x20)>>5;
    MIU_ch[1] = (data&0x800000)>>23;
    j = MIU_ch[1]*2 + MIU_ch[0];
    if(i<6)
    {
        t=1.0;
        for(int j=0;j<i;j++)
        {
            t = t*2;
        }        
        printf("Memory density : %.1f GB \n",(3-j)*t/4);
    }
    printf("MIU bit width : %d bit\n",(3-j)*64);	
}


