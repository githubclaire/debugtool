#include "def.h"

#define PAGE_SIZE 0x100000

extern VIDEO_PCI_PROP video_pci_prop;

#ifdef __ubuntu__
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

		if(mydev->vendor_id == config.id.vendorid && mydev->device_id == config.id.deviceid)
		{
            classCode = pci_read_long(mydev,0x08);
			if((classCode & 0xFF000000) == 0x03000000)
			{
				pci_write_byte(mydev,0x04,0x06);
				PCIcmd = pci_read_long(mydev,0x04);	
				//printf("PCIcmd:0x%04x\n",PCIcmd);
				if(PCIcmd&0x02)
				{
                    printf("BUS_DEV_FUNC is : %.2x:%.2x.%.1x\n",mydev->bus,mydev->dev,mydev->func);
					video_pci_prop.VenderId = mydev->vendor_id;
					video_pci_prop.DeviceId = mydev->device_id;
                    printf("PCIe Vender ID = 0x%x, Device ID = 0x%x\n",video_pci_prop.VenderId,video_pci_prop.DeviceId);

					video_pci_prop.MmioBase = pci_read_long(mydev, 0x10) & 0xFFFFFF00;	
                    printf("PCIE MMIOBase = 0x%x\n",video_pci_prop.MmioBase);

					//if(map_to_system_memory(video_pci_prop.MmioBase, &video_pci_prop.mapped_mmioBase))
					return TRUE;
				}
				else
				{
					continue;
				}

			}															
		}
	}

	return FALSE;
}

int map_to_system_memory(unsigned long addr)
{
	int fb = open("/dev/mem",O_RDWR | O_SYNC);				
	if(fb<0)
	{
		printf("Can't open!!\n");
		return FALSE;
	}
	
	video_pci_prop.mapped_mmioBase = (unsigned long)mmap(0,PAGE_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,fb,addr);				
	printf("PCIE mapped MMIOBase = 0x%lx\n",video_pci_prop.mapped_mmioBase);
	
	if(video_pci_prop.mapped_mmioBase == 0)
	{
		printf("Can't mmap!\n");
		return FALSE;
	}

	return TRUE;
}
#endif

#ifdef __dos__
int find_Base_Addr(configuration config)
{	
	char  bus, dev;
	unsigned long MMIOBaseAddress;
	unsigned long FBBaseAddress;
	unsigned short vendorID;
	unsigned short deviceID;
	unsigned short memory_enable_bit;
	
	for(bus=0; bus<255; bus++)
	{
		for(dev=0; dev<32; dev++)
		{
			vendorID = ReadPciCfgWord(bus, dev, 0, 0x00);			
			if(vendorID != config.id.vendorid)
			{
				continue;
			}

			deviceID = ReadPciCfgWord(bus, dev, 0, 0x02);
			if(deviceID == config.id.deviceid)
			{
				memory_enable_bit = ReadPciCfgWord(bus, dev, 0, 0x4) & 0x2;
				if(memory_enable_bit!=0x02)
				{
					WritePciCfgDword(bus, dev, 0, 0x4, 0x6);
				}
				printf("BUS_DEV_FUNC is : %.2x:%.2x.0\n",bus,dev);
				video_pci_prop.DeviceId = deviceID;
				video_pci_prop.VenderId = vendorID;
				
				MMIOBaseAddress= ReadPciCfgDword(bus, dev, 0, 0x10);
				video_pci_prop.MmioBase  = MMIOBaseAddress & 0xFFFF0000;
				printf("PCIE MMIOBase = 0x%x\n",video_pci_prop.MmioBase);
				
				FBBaseAddress = ReadPciCfgDword(bus, dev, 0, 0x14);
				video_pci_prop.FBBase = FBBaseAddress & 0xFFFFFFF0;

				return TRUE;
			}
		}
	}
	return FALSE;
}
#endif

void read_PCIe(void)
{
	unsigned int pcie_width;
    unsigned int pcie_info_addr = 0x8070;
    unsigned int pcie_info = *(unsigned int *)(video_pci_prop.mapped_mmioBase + pcie_info_addr);
				
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

	pcie_width = (pcie_info & 0x0ff00000)>>20;

    printf("PCIe Width : x%d\n", pcie_width);
}

void read_fw_version(void)
{
	unsigned int i, data,data_pmp,data_pmp_len,j;
	char data_pmp_str;
	//for pmp version;
	if (video_pci_prop.VenderId == 0x6766 && video_pci_prop.DeviceId == 0x3d02)     //1020 4byte temp
	{
        printf("PMP Version : GF00");
		data_pmp = sf_read_data(0x24,4);
	    for(j=0;j<4;j++){
	       data_pmp_str = hex_to_char((data_pmp>>(j*8))&0xff);
		   printf("%c",data_pmp_str);
	    }	
	}
	if (video_pci_prop.VenderId == 0x6766 && video_pci_prop.DeviceId == 0x3d00)       //10c0  5bytes temp
	{
		printf("PMP Version : BLD");
		data_pmp_len = 5;
		for(j=0;j<data_pmp_len;j++){
		   data_pmp = sf_read_data((0x24+j),1);
	       data_pmp_str = hex_to_char(data_pmp);
		   printf("%c",data_pmp_str);
	    }	

	}
    printf("\n");
	//for viobs;
	data = sf_read_data(0xf010,4);
	printf("Vbios Version : ");	
	for(i = 0;i<3;i++)
	{
		printf("%2.2x:",(data&0xff000000)>>24);
		data=data<<8;
	}
	printf("%2.2x \n",(data&0xff000000)>>24);
}

void read_bitwidth(void)
{
    unsigned int i,j,k,data,MIU_ch[2];
    float t;
	data = readl(video_pci_prop.mapped_mmioBase+0xd000);
    i = data&0x07;
    MIU_ch[0] = (data&0x20)>>5;
    MIU_ch[1] = (data&0x800000)>>23;
    j = MIU_ch[1]*2 + MIU_ch[0];
    if(i<6)
    {
        t=1.0;
        for(k=0;k<i;k++)
        {
            t = t*2;
        }        
        printf("Memory density : %.1f GB \n",(3-j)*t/4);
    }
    printf("MIU bit width : %d bit\n",(3-j)*64);	
}


