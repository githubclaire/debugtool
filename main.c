#include "def.h"


VIDEO_PCI_PROP video_pci_prop;
configuration config;

static int handler(void* user, const char* section, const char* name, const char* value)
{
    configuration* pconfig = (configuration*)user;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("id", "vendorid")) 
    {
        pconfig->id.vendorid = htoi(value);
    }
    else if(MATCH("id", "deviceid"))
    {
        pconfig->id.deviceid = htoi(value);
    }
    else if(MATCH("addr", "mmiobase"))
    {
        pconfig->addr.mmiobase = htoi(value);
    }  
     else if(MATCH("i2c", "crt"))
    {
        pconfig->i2c.serialport0 = htoi(value);
    } 
    else if(MATCH("i2c", "hdmi0"))
    {
        pconfig->i2c.serialport1 = htoi(value);
    } 
    else if(MATCH("i2c", "hdmi1"))
    {
        pconfig->i2c.serialport2 = htoi(value);
    }
    else if(MATCH("i2c", "i2cdelay"))
    {
        pconfig->i2c.i2c_delay = atoi(value);
    }        
    else
    {
        return 0;  /* unknown section/name, error */
    }
    return 1;
}

void init_config_parameters(configuration* pconfig)
{
    pconfig->i2c.serialport0 = 0x89f8;
    pconfig->i2c.serialport1 = 0x88a0;
    pconfig->i2c.serialport2 = 0x88aa;
    pconfig->id.vendorid = 0x6766;
    pconfig->id.deviceid = 0x3d00;
    pconfig->i2c.i2c_delay = 400;
    pconfig->addr.mmiobase = 0xabcdef;
}

int main(int argc, char *argv[])
{
    int id_index = 16;
    int no_config_file = TRUE;
    char* config_file_name = "glendbg.ini";
    
    init_config_parameters(&config);

    if(argc == 2)
    {
        config_file_name = argv[1];
    }

    if(ini_parse(config_file_name, handler, &config)<0) 
    {
        printf("Can't load %s\n",config_file_name);
        //printf("Load chip id default : vendorid = 0x%x, deviceid = 0x%x\n", config.id.vendorid, config.id.deviceid);
    }
    else
    {
        no_config_file = FALSE;
        printf("Load chip id from file : vendorid = 0x%x, id.deviceid = 0x%x\n", config.id.vendorid, config.id.deviceid);
    }

    //loop find device id 3d00-3d0e
    do
    {      
        if(find_Base_Addr(config))
        {
            printf("Load chip id : vendorid = 0x%x, deviceid = 0x%x\n", config.id.vendorid, config.id.deviceid);
            break;
        }
        if(id_index==1)
        {
            //load mmio base from config.ini
            printf("Can't read mmio base address from pcie.\n");
            printf("Load base address from file: mmiobase = 0x%lx\n", config.addr.mmiobase);
            video_pci_prop.MmioBase = config.addr.mmiobase;
        }
        config.id.deviceid++;
    }while (id_index-- && no_config_file);

    
#ifdef __ubuntu__
    if(!map_to_system_memory(video_pci_prop.MmioBase))
    {
        return FALSE;
    }
#endif

#ifdef __dos__
    video_pci_prop.mapped_mmioBase = video_pci_prop.MmioBase;
#endif
    
    CToolParserCmd();
	return 1;
}

			
