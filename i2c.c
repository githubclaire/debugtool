#include "def.h"

extern configuration config;

unsigned char SCL_WL = 0;    
unsigned char SCL_WH = 0;    
unsigned char SDA_WL = 0;    
unsigned char SDA_WH = 0; 
unsigned char SDA_BIT = 0;
unsigned char SDA_SHIFT = 0;
unsigned char SCL_BIT = 0;
unsigned char SCL_SHIFT = 0;
unsigned char SDA_R = 0;

unsigned char slave_addr;
unsigned int serial;
unsigned int i2c_delay_time;

void SerialPort_Init(unsigned int serialno, unsigned char slaveaddr)
{
	serial = serialno;	
	slave_addr = slaveaddr;
	SCL_WL = SCL_ENABLE|0;				//8'b0001 0000; enable Serial Port 1
	SCL_WH = SCL_ENABLE|SCL_WRITE;	//8'b0001 0001; enable SP1; SPCLK1 is tri-stated
	SDA_WL = SDA_ENABLE|0;				//8'b0001 0000; enable Serial Port 1
	SDA_WH = SDA_ENABLE|SDA_WRITE;	//8'b0001 0010; enable SP1; SPDAT1 is tri-stated
	SDA_R = SDA_ENABLE|SDA_WRITE;	//8'b0001 0010; enable SP1; SPDAT1 is tri-stated
	SDA_BIT = SDA_READ;					//8'b0000 1000;
	SDA_SHIFT = 3;
	SCL_BIT = SCL_READ;					//8'b0000 0100;
	SCL_SHIFT = 2;
	i2c_delay_time = config.i2c.i2c_delay;
}

void SerialPort_RegWrite(unsigned int value)
{
	if((serial == SERIAL_CRT) || (serial == SERIAL_HDMI0) || (serial == SERIAL_HDMI1))
    {
        write8(serial,value);
    }
	else 
	{
		printf("Wrong I2C serial port number 0x%4.4x\n", serial);		
	}
}

unsigned int SerialPort_RegRead(void)
{
	unsigned int value = 0;
	if((serial == SERIAL_CRT) || (serial == SERIAL_HDMI0) || (serial == SERIAL_HDMI1))
    {
        value = read8(serial);
    }
	else 
	{
		printf("Wrong I2C serial port number 0x%4.4x\n", serial);		
	}

  	return(value);
}


void i2c_start()	//Generating the I2C's START signal which is a high-to-low transition of SDA with SCL high
{

        SerialPort_RegWrite(SCL_WH|SDA_WH); 	//CRA0=0x13     
        i2c_delay(DELAYTIME);
        SerialPort_RegWrite(SCL_WH|SDA_WL); 	//CRA0=0x11        
        i2c_delay(DELAYTIME);
        SerialPort_RegWrite(SCL_WL|SDA_WL); 		//CRA0=0x10      
        i2c_delay(DELAYTIME);

}

void i2c_stop()
{
	int j;

	SerialPort_RegWrite(SCL_WL|SDA_WL); 
	i2c_delay(DELAYTIME);
	j = 0;
	while(j++ < RETRYTIMES)
	{
		SerialPort_RegWrite(SCL_WH|SDA_WL); 
		i2c_delay(DELAYTIME);
		if((((SerialPort_RegRead()>>SCL_SHIFT)&0x01)&&(!(SerialPort_RegRead()>>SDA_SHIFT)&0x01)))
			break;
	}        
	j = 0;
	while(j++ < RETRYTIMES)
	{
		SerialPort_RegWrite(SCL_WH|SDA_WH); 
		i2c_delay(DELAYTIME);
		if((((SerialPort_RegRead()>>SCL_SHIFT)&0x01)&&(SerialPort_RegRead()>>SDA_SHIFT)&0x01))
			break;
	}                
}

int i2c_read()
{
	int i, data=0;
	int j;
	int maxloop = RETRYTIMES;
	for (i=0; i<8; i++)
	{
		SerialPort_RegWrite (SCL_WL|SDA_R); 
		i2c_delay(DELAYTIME/2);

		j = 0;
		while(j++ < maxloop)
		{
			SerialPort_RegWrite (SCL_WH|SDA_R);
			i2c_delay(DELAYTIME);
			if((SerialPort_RegRead()>>SCL_SHIFT)&0x01)
				break;
		}
		data = (data<<1) + (0x1 & SerialPort_RegRead()>>SDA_SHIFT);
		SerialPort_RegWrite (SCL_WL|SDA_R); 
		i2c_delay(DELAYTIME/2);
	}
	return data;
}

void i2c_write(unsigned int addr)
{
	int i, value;
	int j = 0;
	int maxloop = RETRYTIMES;     

	for (i=7; i>=0; i--)
	{
		value= (0x1 & (addr>>i));

		if (value ==0)
		{
			SerialPort_RegWrite(SCL_WL|SDA_WL); 
			i2c_delay(DELAYTIME/2);
			j = 0;
			while(j++ < maxloop)
			{
				SerialPort_RegWrite(SCL_WH|SDA_WL); 
				i2c_delay(DELAYTIME);
				if((SerialPort_RegRead()>>SCL_SHIFT)&0x01)
					break;
			}      
			SerialPort_RegWrite(SCL_WL|SDA_WL);
			i2c_delay(DELAYTIME/2);
		}
		else 
		{
			SerialPort_RegWrite(SCL_WL|SDA_WH); 
			i2c_delay(DELAYTIME/2);
			j = 0;
			while(j++ < maxloop)
			{
				SerialPort_RegWrite(SCL_WH|SDA_WH);
				i2c_delay(DELAYTIME);
				if((SerialPort_RegRead()>>SCL_SHIFT)&0x01)
					break;
			}                  
			SerialPort_RegWrite(SCL_WL|SDA_WH); 
			i2c_delay(DELAYTIME/2);
		}
	}
}

int i2c_ack_read()
{
	int ack = 0;
	int j = 0;
	int maxloop = RETRYTIMES;

	SerialPort_RegWrite(SCL_WL|SDA_R); 
	i2c_delay(DELAYTIME);
	while(j++ < maxloop)
	{
		SerialPort_RegWrite(SCL_WH|SDA_R); 
		i2c_delay(DELAYTIME);
		if((SerialPort_RegRead()>>SCL_SHIFT)&0x01)
			break;
	}        

	ack = (SDA_BIT & SerialPort_RegRead()); 
	SerialPort_RegWrite(SCL_WL|SDA_R); 
	i2c_delay(4*DELAYTIME);

	if (ack==0) 
		return TRUE;
	else 
		return FALSE;
}

int i2c_nack_write()
{
	int j = 0;
	int maxloop = RETRYTIMES;    

	SerialPort_RegWrite(SCL_WL|SDA_R); 
	i2c_delay(DELAYTIME);
	while(j++ < maxloop)
	{
		SerialPort_RegWrite(SCL_WH|SDA_R); 
		i2c_delay(DELAYTIME);
		if((SerialPort_RegRead()>>SCL_SHIFT)&0x01)
			break;
	}           
	 
	SerialPort_RegWrite(SCL_WL|SDA_R); 
	i2c_delay(4*DELAYTIME);

	return TRUE;
}

int i2c_ack_write()
{
	int j = 0;
	int maxloop = RETRYTIMES;    

	SerialPort_RegWrite(SCL_WL|SDA_WL); 
	i2c_delay(DELAYTIME);
	while(j ++ < maxloop)
	{
		SerialPort_RegWrite(SCL_WH|SDA_WL); 
		i2c_delay(DELAYTIME);
		if ((SerialPort_RegRead() >> SCL_SHIFT) & 0x01)
			break;
	}
	SerialPort_RegWrite(SCL_WL|SDA_R); 
	i2c_delay(4*DELAYTIME);

	return TRUE;
}

int i2c_read_data(unsigned char addr, unsigned char *data)
{
	//printf("reading addr: %x\n", addr);
	i2c_start();        // START

	i2c_write(slave_addr);    // slave address & bit 0 = 0 is for write mode
	if(i2c_ack_read()==FALSE) 
		return FALSE;   // wait for ack

	i2c_write(addr&0xff);    // write addr
	if(i2c_ack_read()==FALSE) 
		return FALSE;   // wait for ack

	i2c_start();        // start bit
	i2c_write(slave_addr+1);    // slave address & bit 0 = 1 is for read
	if(i2c_ack_read()==FALSE) 
		return FALSE;   // wait for ack

	*data = i2c_read();

	i2c_nack_write();

	i2c_stop();
	
	return TRUE;
}

int i2c_read_data_page(unsigned char addr, unsigned int len, unsigned char *data)
{
	unsigned int i;
	//printf("reading addr: %x\n", addr);
	i2c_start();        // start bit

	i2c_write(slave_addr);    // slave address & bit 0 = 0 is for write mode
	if(i2c_ack_read()==FALSE) 
		return FALSE;   // wait for ack

	i2c_write(addr&0xff);    // write addr
	if(i2c_ack_read()==FALSE) 
		return FALSE;   // wait for ack

	i2c_start();        // start bit
	i2c_write(slave_addr+1);    // slave address & bit 0 = 1 is for read
	if(i2c_ack_read()==FALSE) 
		return FALSE;   // wait for ack

		
	//printf("I2C Read start...\n");	
 	for (i=0; i<len; i++)
 	{
		data[i] = i2c_read(); 	//add by Janice Yi, Oct. 24, 2007
		if(i==(len-1))
			i2c_nack_write();
		else
			i2c_ack_write();
		//printf("I2C Read data index is %d\n",i);
	}

	i2c_stop();
	
	return TRUE;
}

int i2c_write_data(unsigned char addr, unsigned char data)
{
	i2c_start();
	
	i2c_write(slave_addr);
	if(i2c_ack_read()==FALSE) 
	{
		return FALSE;
	}
	
	i2c_write(addr&0xff);
	if(i2c_ack_read()==FALSE) 
		return FALSE;
	
	i2c_write(data);
	if(i2c_ack_read()==FALSE) 
		return FALSE;
	
	i2c_stop();
	
	return TRUE;
}

int i2c_write_data_page(unsigned char addr, unsigned int len, unsigned char *data)
{
	unsigned int i;
	
	i2c_start();
	
	i2c_write(slave_addr);
	if(i2c_ack_read()==FALSE) 
		return FALSE;
	
	i2c_write(addr&0xff);
	if(i2c_ack_read()==FALSE) 
		return FALSE;
		
	for(i=0;i<len;i++)
	{
		i2c_write(data[i]);
		if(i2c_ack_read()==FALSE) 
			return FALSE;
	}	
	
	i2c_stop();
	
	return TRUE;
}

//Delays
void i2c_delay(unsigned int i)
{

	int j=0;
    i=i*i2c_delay_time;
    for(j=0;j<i;j++)
    {
        //mdelay(i2c_delay_time);
    }

}

void i2c_prog(int argc, char *argv[])
{
    int i;
	unsigned char i2cIndex = 0;
	unsigned int serialPortNo = 0x88a0;	
	unsigned char deviceAddr = EDID_DEVADDR;
	unsigned char subAddr = 0;
	unsigned char data;
	unsigned char edid[256];
	i2c_delay_time = config.i2c.i2c_delay;
	
    if(argc >= 3)
    {
        i2cIndex = StoH(argv[1]);
        switch(i2cIndex)
        {
            case 0: serialPortNo = config.i2c.serialport0;break;
            case 1: serialPortNo = config.i2c.serialport1;break;
            case 2: serialPortNo = config.i2c.serialport2;break;			
            default: i2c_help_info();break;
        }

        deviceAddr = StoH(argv[2]);
        SerialPort_Init(serialPortNo, deviceAddr);			
        printf("\nserialPortNo is 0x%4.4x, device address is 0x%2.2x.\n", serialPortNo, deviceAddr);
		memset(edid,0,EDID_BUFFER_SIZE);
        
        if(argc == 3)
        {
			if (i2c_read_data_page(0x00, EDID_BUFFER_SIZE, edid) == FALSE)
			{
				i2c_stop();
				printf("\nERROR: I2C Read error! Can not find the Device!\n");				
			}
			else
			{
				printf("\n");
				printf("    ");
				for (i = 0; i < 16; i ++)
				{
					printf ("%02x  ", i);
				}
				printf("\n");

				for (i = 0x00; i < EDID_BUFFER_SIZE; i++)	//read back
				{
					if ((i % 16) == 0)
					{
						printf("\n");
						printf ("%02x  ", i);
					}

					printf("%02x  ", edid[i]);
				
				}
				printf("\n");
			}								
        }
        else if(argc == 4)
        {
            subAddr = StoH(argv[3]);				
            if(i2c_read_data(subAddr, &data))
            {
                printf("\nI2C read value = 0x%2.2x\n", data);
            }
        }	
        else if(argc == 5)
        {
            subAddr = StoH(argv[3]);				
            data = StoH(argv[4]);							
            if(i2c_write_data(subAddr, data))
            {
                printf("\nI2C write value = 0x%2.2x\n", data);
            }

            if(i2c_read_data(subAddr, &data))
            {
                printf("\nI2C read value = 0x%2.2x\n", data);
            }			
        }
        else
        {
            i2c_help_info();
        }
    }
	else
	{
        i2c_help_info();
	}
}

void i2c_help_info(void)
{
    printf("	i2c 0/1/2 deviceAddr subAddr [data]: 0->CRT 1->HDMI/DP0 2->HDMI/DP1 read/write.\n");
	printf(" ex. read EDID, deviceAddr is 0xa0  cmd: i2c 0 a0");
}

