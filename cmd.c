#include "def.h"

extern VIDEO_PCI_PROP video_pci_prop;

DBG_CMD_STRUCT dbg_cmd_table[] = {
	{"?", do_dbg_help},
	{"help", do_dbg_help},
	{"mmio", do_dbg_mmio},
	{"ts", do_dbg_ts},
    {"vcore", do_dbg_voltage},
	{"sf", do_dbg_flash},
	{"prog", do_dbg_program_fw},
	{"print", do_dbg_print_info},
	{"clk", do_dbg_clk},  
	{"memtest", do_dbg_mem_test},        
};

#define DBG_CMD_TABLE_SIZE (sizeof(dbg_cmd_table) / sizeof(DBG_CMD_STRUCT))
void CToolParserCmd(void)
{
	char*   line;

	line = (char*)malloc(STRINGBUFFERSIZE);
	memset(line, 0, STRINGBUFFERSIZE);

	while(1)
	{
		EchoWait();

		CToolGetInput(line, STRINGBUFFERSIZE* sizeof(char));

		if(ProcessString(line)==0)
		{	
			break;
		}			
	}

}

int ProcessString(char* buffer) // return 0 if quit, else return 1
{
	int    bStatus;
	int    i;
	char     delim[] = " \t\n,";
	char*    cmd[MAX_PARAM_COUNT];
	unsigned char    param_count;
	DBG_CMD_STRUCT  *p_cmd_struct;

	for (i=0; i<MAX_PARAM_COUNT; i++)
		cmd [i]=NULL;
	
	cmd[0]=strtok(buffer, delim);
	if (cmd[0]==NULL)
		return 1;
	for (i=1; i<(MAX_PARAM_COUNT); i++)
	{
		cmd[i]=strtok(NULL, delim);
		if (cmd[0]==NULL)
			break;
	}

	param_count = 0;
	while (cmd[param_count]!=NULL)
	{
		param_count++;
	}

/*Just for debug
    printf("\n");
    i = 0;
    while (cmd[i]!=NULL)
    {
        printf("%s ", cmd[i]);
        i ++;
    }
    printf("\n");
*/
	for (i=0,p_cmd_struct=dbg_cmd_table; i<DBG_CMD_TABLE_SIZE; i++, p_cmd_struct++)
	{
		if (strcmp(cmd[0], p_cmd_struct->name)==0)
		{
			bStatus=p_cmd_struct->execute(cmd, param_count);
			break;
		}
	}	

return bStatus;
}

int do_dbg_mmio(char * cmd[], unsigned int param_count)
{
    unsigned int 	reg_index;
	unsigned int 	reg_data;

	if(param_count == 2)	//two parameters. register read.
	{
		reg_index = StoH(cmd[1]);
        reg_data = readl(video_pci_prop.mapped_mmioBase+reg_index);
		printf ("\nRead MMIO %x = 0x%08x\n", reg_index, reg_data);
	}
	else if(param_count == 3)	//register write
	{
		reg_index = StoH(cmd[1]);
		reg_data = StoH(cmd[2]);
        writel(reg_data, video_pci_prop.mapped_mmioBase+reg_index);
		printf("\nWrite MMIO %x = 0x%08x Done.\n", reg_index, reg_data);	
	}
	else
	{
		printf("\nParse Error!\n");
	}
	
	return TRUE;
}

int do_dbg_ts(char * cmd[], unsigned int param_count)
{
    printf("Temp : %d C\n",dout_to_temp(GetTemperature())/1000);
	return TRUE;
}

int do_dbg_voltage(char * cmd[], unsigned int param_count)
{
    printf("voltage : %d mV\n",( GetVoltage()*1000+1157200)/1869);
	return TRUE;
}

int do_dbg_print_info(char * cmd[], unsigned int param_count)
{
    sf_init(video_pci_prop.mapped_mmioBase);
    read_fw_version();
    read_PCIe(video_pci_prop.mapped_mmioBase);
    printf("DDR Version : DDR4\n");
    read_bitwidth(video_pci_prop.mapped_mmioBase);                        
    printf("Memory Clk : %d MHz\n",get_mpll());
    printf("Elite Clk : %d MHz\n",get_vepll(EPLL_REG));   
    printf("Temp : %d C\n",dout_to_temp(GetTemperature())/1000);
    printf("voltage : %d mV\n",( GetVoltage()*1000+1157200)/1869);     
}

int do_dbg_flash(char * cmd[], unsigned int param_count)
{
    //sf_flash_test(param_count,cmd);
	return TRUE;
}

int do_dbg_mem_test(char * cmd[], unsigned int param_count)
{
    chip_mem_connection_test(param_count,cmd);
	return TRUE;
}

int do_dbg_program_fw(char * cmd[], unsigned int param_count)
{
    sf_init(video_pci_prop.mapped_mmioBase);
    if(strcmp(cmd[1],"-p")==0 || strcmp(cmd[1],"-P")==0)
    {
        flash_vbios(cmd[2]);
    }
    else if(strcmp(cmd[1],"-d")==0 || strcmp(cmd[1],"-D")==0)
    {
        dump_vbios(cmd[2]);
    }
	return TRUE;
}

int do_dbg_clk(char * cmd[], unsigned int param_count)
{
    clk_prog(param_count,cmd);
	return TRUE;
}

int do_dbg_help(char * cmd[], unsigned int param_count)
{
	helpinfo();
	return TRUE;
}

int do_dbg_quit(char * cmd[], unsigned int param_count)
{
    return FALSE;
}

void helpinfo(void)
{
	printf("> ts         --- print temperature\n");
	printf("> vcore      --- print vcore\n");    
    printf("> clk        --- read/write clock\n");
    printf("> memtest    --- miu test connection\n");  
    printf("> prog       --- program or dump rom to/from flash\n");
    printf("> mmio       --- read/write register\n");  
    printf("> print      --- print all information(pcie info/mem info/vcore/clk/temp)\n");
}
