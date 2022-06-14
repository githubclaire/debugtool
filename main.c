#include "def.h"


VIDEO_PCI_PROP video_pci_prop;


static int handler(void* user, const char* section, const char* name, const char* value)
{
    configuration* pconfig = (configuration*)user;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("id", "vendorid")) 
    {
        pconfig->vendorid = htoi(value);
    }
    else if(MATCH("id", "deviceid"))
    {
        pconfig->deviceid = htoi(value);
    }
    else
    {
        return 0;  /* unknown section/name, error */
    }
    return 1;
}

int main(int argc, char *argv[])
{
    configuration config;


    if (ini_parse("config.ini", handler, &config) < 0) 
    {
        printf("Can't load 'config.ini'\n");
        return 1;
    }

    printf("Config loaded from 'config.ini': vendorid=%x, deviceid=%x\n", config.vendorid, config.deviceid);

    if(find_Base_Addr(config))
    {
        CToolParserCmd();
    }
    else
    {
        //load mmio base from config.ini
    }
    
	return 1;
}

			
