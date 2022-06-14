#include "def.h"

extern VIDEO_PCI_PROP video_pci_prop;

void memtest(unsigned int channel, unsigned int addr)
{
    unsigned int i = 0;
    unsigned int read_result = 0;
	unsigned int result = 0;

	read_result = readl(video_pci_prop.mapped_mmioBase + addr);
	for(i=0;i<8;i++)
	{
		result = read_result & 0xf;
		if(result == 0x1)
		{
			printf("Chip %d  byte %d  fail!\n",channel*4+i/2,i);	
		}
		else
		{
			printf("Chip %d  byte %d  pass!\n",channel*4+i/2,i);
		}
		read_result = read_result >> 4;
	}
}

void chip_mem_connection_test(int argc, char *argv[])
{
    int channel = readl(video_pci_prop.mapped_mmioBase + 0x49024);
    switch(channel)
    {
        case 0: channel = 3;break;
        case 4: channel = 2;break;
        case 6: channel = 1;break;
    }
    printf("MIU Channel Num: %d \n",channel);
    if(channel == 3)
    {
        printf("##############################  MIU0 test  ###########################\n");
        memtest(0,0xd3f4);
        printf("##############################  MIU1 test  ###########################\n");
        memtest(1,0xe3f4);
        printf("##############################  MIU2 test  ###########################\n");
        memtest(2,0xf3f4);
    }
    else if(channel == 2)
    {
        printf("##############################  MIU0 test  ###########################\n");
        memtest(0,0xd3f4);
        printf("##############################  MIU1 test  ###########################\n");
        memtest(1,0xe3f4);
    }
    else if(channel == 1)
    {
        printf("##############################  MIU0 test  ###########################\n");
        memtest(0,0xd3f4);
    }
}