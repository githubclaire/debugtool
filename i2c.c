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
	SCL_WL = SCL_ENABLE | 0;		 // 8'b0001 0000; enable Serial Port 1
	SCL_WH = SCL_ENABLE | SCL_WRITE; // 8'b0001 0001; enable SP1; SPCLK1 is tri-stated
	SDA_WL = SDA_ENABLE | 0;		 // 8'b0001 0000; enable Serial Port 1
	SDA_WH = SDA_ENABLE | SDA_WRITE; // 8'b0001 0010; enable SP1; SPDAT1 is tri-stated
	SDA_R = SDA_ENABLE | SDA_WRITE;	 // 8'b0001 0010; enable SP1; SPDAT1 is tri-stated
	SDA_BIT = SDA_READ;				 // 8'b0000 1000;
	SDA_SHIFT = 3;
	SCL_BIT = SCL_READ; // 8'b0000 0100;
	SCL_SHIFT = 2;
	i2c_delay_time = config.i2c.i2c_delay;
}

void SerialPort_RegWrite(unsigned int value)
{
	if ((serial == SERIAL_CRT) || (serial == SERIAL_HDMI0) || (serial == SERIAL_HDMI1))
	{
		write8(serial, value);
	}
	else
	{
		printf("Wrong I2C serial port number 0x%4.4x\n", serial);
	}
}

unsigned int SerialPort_RegRead(void)
{
	unsigned int value = 0;
	if ((serial == SERIAL_CRT) || (serial == SERIAL_HDMI0) || (serial == SERIAL_HDMI1))
	{
		value = read8(serial);
	}
	else
	{
		printf("Wrong I2C serial port number 0x%4.4x\n", serial);
	}

	return (value);
}

void i2c_start() // Generating the I2C's START signal which is a high-to-low transition of SDA with SCL high
{

	SerialPort_RegWrite(SCL_WH | SDA_WH); // CRA0=0x13
	i2c_delay(DELAYTIME);
	SerialPort_RegWrite(SCL_WH | SDA_WL); // CRA0=0x11
	i2c_delay(DELAYTIME);
	SerialPort_RegWrite(SCL_WL | SDA_WL); // CRA0=0x10
	i2c_delay(DELAYTIME);
}

void i2c_stop()
{
	int j;

	SerialPort_RegWrite(SCL_WL | SDA_WL);
	i2c_delay(DELAYTIME);
	j = 0;
	while (j++ < RETRYTIMES)
	{
		SerialPort_RegWrite(SCL_WH | SDA_WL);
		i2c_delay(DELAYTIME);
		if ((((SerialPort_RegRead() >> SCL_SHIFT) & 0x01) && (!(SerialPort_RegRead() >> SDA_SHIFT) & 0x01)))
			break;
	}
	j = 0;
	while (j++ < RETRYTIMES)
	{
		SerialPort_RegWrite(SCL_WH | SDA_WH);
		i2c_delay(DELAYTIME);
		if ((((SerialPort_RegRead() >> SCL_SHIFT) & 0x01) && (SerialPort_RegRead() >> SDA_SHIFT) & 0x01))
			break;
	}
}

int i2c_read()
{
	int i, data = 0;
	int j;
	int maxloop = RETRYTIMES;
	for (i = 0; i < 8; i++)
	{
		SerialPort_RegWrite(SCL_WL | SDA_R);
		i2c_delay(DELAYTIME / 2);

		j = 0;
		while (j++ < maxloop)
		{
			SerialPort_RegWrite(SCL_WH | SDA_R);
			i2c_delay(DELAYTIME);
			if ((SerialPort_RegRead() >> SCL_SHIFT) & 0x01)
				break;
		}
		data = (data << 1) + (0x1 & SerialPort_RegRead() >> SDA_SHIFT);
		SerialPort_RegWrite(SCL_WL | SDA_R);
		i2c_delay(DELAYTIME / 2);
	}
	return data;
}

void i2c_write(unsigned int addr)
{
	int i, value;
	int j = 0;
	int maxloop = RETRYTIMES;

	for (i = 7; i >= 0; i--)
	{
		value = (0x1 & (addr >> i));

		if (value == 0)
		{
			SerialPort_RegWrite(SCL_WL | SDA_WL);
			i2c_delay(DELAYTIME / 2);
			j = 0;
			while (j++ < maxloop)
			{
				SerialPort_RegWrite(SCL_WH | SDA_WL);
				i2c_delay(DELAYTIME);
				if ((SerialPort_RegRead() >> SCL_SHIFT) & 0x01)
					break;
			}
			SerialPort_RegWrite(SCL_WL | SDA_WL);
			i2c_delay(DELAYTIME / 2);
		}
		else
		{
			SerialPort_RegWrite(SCL_WL | SDA_WH);
			i2c_delay(DELAYTIME / 2);
			j = 0;
			while (j++ < maxloop)
			{
				SerialPort_RegWrite(SCL_WH | SDA_WH);
				i2c_delay(DELAYTIME);
				if ((SerialPort_RegRead() >> SCL_SHIFT) & 0x01)
					break;
			}
			SerialPort_RegWrite(SCL_WL | SDA_WH);
			i2c_delay(DELAYTIME / 2);
		}
	}
}

int i2c_ack_read()
{
	int ack = 0;
	int j = 0;
	int maxloop = RETRYTIMES;

	SerialPort_RegWrite(SCL_WL | SDA_R);
	i2c_delay(DELAYTIME);
	while (j++ < maxloop)
	{
		SerialPort_RegWrite(SCL_WH | SDA_R);
		i2c_delay(DELAYTIME);
		if ((SerialPort_RegRead() >> SCL_SHIFT) & 0x01)
			break;
	}

	ack = (SDA_BIT & SerialPort_RegRead());
	SerialPort_RegWrite(SCL_WL | SDA_R);
	i2c_delay(4 * DELAYTIME);

	if (ack == 0)
		return TRUE;
	else
		return FALSE;
}

int i2c_nack_write()
{
	int j = 0;
	int maxloop = RETRYTIMES;

	SerialPort_RegWrite(SCL_WL | SDA_R);
	i2c_delay(DELAYTIME);
	while (j++ < maxloop)
	{
		SerialPort_RegWrite(SCL_WH | SDA_R);
		i2c_delay(DELAYTIME);
		if ((SerialPort_RegRead() >> SCL_SHIFT) & 0x01)
			break;
	}

	SerialPort_RegWrite(SCL_WL | SDA_R);
	i2c_delay(4 * DELAYTIME);

	return TRUE;
}

int i2c_ack_write()
{
	int j = 0;
	int maxloop = RETRYTIMES;

	SerialPort_RegWrite(SCL_WL | SDA_WL);
	i2c_delay(DELAYTIME);
	while (j++ < maxloop)
	{
		SerialPort_RegWrite(SCL_WH | SDA_WL);
		i2c_delay(DELAYTIME);
		if ((SerialPort_RegRead() >> SCL_SHIFT) & 0x01)
			break;
	}
	SerialPort_RegWrite(SCL_WL | SDA_R);
	i2c_delay(4 * DELAYTIME);

	return TRUE;
}

int i2c_read_data(unsigned char addr, unsigned char *data)
{
	// printf("reading addr: %x\n", addr);
	i2c_start(); // START

	i2c_write(slave_addr); // slave address & bit 0 = 0 is for write mode
	if (i2c_ack_read() == FALSE)
		return FALSE; // wait for ack

	i2c_write(addr & 0xff); // write addr
	if (i2c_ack_read() == FALSE)
		return FALSE; // wait for ack

	i2c_start();			   // start bit
	i2c_write(slave_addr + 1); // slave address & bit 0 = 1 is for read
	if (i2c_ack_read() == FALSE)
		return FALSE; // wait for ack

	*data = i2c_read();

	i2c_nack_write();

	i2c_stop();

	return TRUE;
}

/*
int i2c_read_data_page(unsigned char addr, unsigned int len, unsigned char *data)
{
	unsigned int i;
	// printf("reading addr: %x\n", addr);
	i2c_start(); // start bit

	i2c_write(slave_addr); // slave address & bit 0 = 0 is for write mode
	if (i2c_ack_read() == FALSE)
		return FALSE; // wait for ack

	i2c_write(addr & 0xff); // write addr
	if (i2c_ack_read() == FALSE)
		return FALSE; // wait for ack

	i2c_start();			   // start bit
	i2c_write(slave_addr + 1); // slave address & bit 0 = 1 is for read
	if (i2c_ack_read() == FALSE)
		return FALSE; // wait for ack

	// printf("I2C Read start...\n");
	for (i = 0; i < len; i++)
	{
		data[i] = i2c_read(); // add by Janice Yi, Oct. 24, 2007
		if (i == (len - 1))
			i2c_nack_write();
		else
			i2c_ack_write();
		// printf("I2C Read data index is %d\n",i);
	}

	i2c_stop();

	return TRUE;
}
*/
int i2c_read_data_page(unsigned char addr, unsigned int len, unsigned char *data)
{
	unsigned int i;
	// printf("reading addr: %x\n", addr);
	i2c_start(); // start bit
	// printf("i2c start is ok");
	i2c_write(slave_addr); // slave address & bit 0 = 0 is for write mode
	// printf("i2c write slaver_add is finish");
	for (;;)
	{
		if (i2c_ack_read() == FALSE)
		{ // add by cheerychen and to avoid fail;
			i2c_start();
			i2c_write(slave_addr);
		}
		else
		{
			break;
		}
	}
	// if(i2c_ack_read()==FALSE)
	//         return FALSE;   // wait for ack
	// printf("wait for slaver ack\n");
	i2c_write(addr & 0xff); // write addr
	if (i2c_ack_read() == FALSE)
		return FALSE;		   // wait for ack
							   // printf("wait for subaddress ack\n");
	i2c_start();			   // start bit
	i2c_write(slave_addr + 1); // slave address & bit 0 = 1 is for read
	if (i2c_ack_read() == FALSE)
		return FALSE; // wait for ack

	// printf("I2C Read start...\n");
	for (i = 0; i < len; i++)
	{
		data[i] = i2c_read(); // add by Janice Yi, Oct. 24, 2007
		if (i == (len - 1))
			i2c_nack_write();
		else
			i2c_ack_write();
		// printf("I2C Read data index is %d\n",i);
	}

	i2c_stop();

	return TRUE;
}

int i2c_write_data(unsigned char addr, unsigned char data)
{
	i2c_start();

	i2c_write(slave_addr);
	/*
	if (i2c_ack_read() == FALSE)
	{
		return FALSE;
	}
    */
   for (;;)
	{
		if (i2c_ack_read() == FALSE)
		{ // add by cheerychen and to avoid fail;
			i2c_start();
			i2c_write(slave_addr);
		}
		else
		{
			break;
		}
	}
	i2c_write(addr & 0xff);
	if (i2c_ack_read() == FALSE)
		return FALSE;

	i2c_write(data);
	if (i2c_ack_read() == FALSE)
		return FALSE;

	i2c_stop();

	return TRUE;
}
/*
int i2c_write_data_page(unsigned char addr, unsigned int len, unsigned char *data)
{
	unsigned int i;

	i2c_start();

	i2c_write(slave_addr);
	if (i2c_ack_read() == FALSE)
		return FALSE;

	i2c_write(addr & 0xff);
	if (i2c_ack_read() == FALSE)
		return FALSE;

	for (i = 0; i < len; i++)
	{
		i2c_write(data[i]);
		if (i2c_ack_read() == FALSE)
			return FALSE;
	}

	i2c_stop();

	return TRUE;
}
*/

int i2c_write_data_page(unsigned char addr, unsigned int len, unsigned char *data)
{
	unsigned int i;
	unsigned int flag = 0;
	i2c_start();
	printf("i2c start writing addr: %x\n", addr);
	i2c_write(slave_addr);
	printf("writing slaver_addr: %x\n", slave_addr);
	sleep(10);
	for (;;)
	{
		if (i2c_ack_read() == FALSE)
		{
			i2c_start();		   // add cheerychen,2020.11.14
			i2c_write(slave_addr); // nack and wait time>40ms:continue;one page write cycle is about 10ms.
			sleep(10);
			//flag++;
		}
		/*
		else if ((i2c_ack_read() == FALSE) && (flag == 4))
		{ // no ack and wait time =40ms->FALSE\ACK FAIL;;
			printf("i2c slaver address ack fail");
			return FALSE;
		}
		*/
		else if (i2c_ack_read())
		{ // ack->break loop;
			break;
		}
	}
	// return FALSE;
	printf("slaver_add ack\n");
	i2c_write(addr & 0xff);
	printf("addr ack\n");
	if (i2c_ack_read() == FALSE)
		return FALSE;
	printf("I2C write start...\n");
	printf("len is :%d\n", len);
	for (i = 0; i < len; i++)
	{

		printf("I2C write data index is %d", i);
		i2c_write(data[i]);
		// printf("I2C write data index is %d",i);
		if (i2c_ack_read() == FALSE)
		{
			printf("I2C Read data index is %d\n", i);
			return FALSE;
		}
	}
	i2c_stop();

	return TRUE;
}

unsigned int I2cReadData(unsigned char serialNo, unsigned char deviceAddr, unsigned int length, unsigned char *buffer)
{

	unsigned int i;
	memset(buffer, 0, length);

	SerialPort_Init(serialNo, deviceAddr);

	if (i2c_read_data_page(0x00, length, buffer) == FALSE)
	{
		i2c_stop();
		printf("\nERROR: I2C Read error! Can not find the Device!\n");

		return FALSE;
	}

	printf("\n");
	printf("    ");
	for (i = 0; i < 16; i++)
	{
		printf("%02x  ", i);
	}
	printf("\n");

	for (i = 0x00; i <= 0xff; i++) // read back
	{
		if ((i % 16) == 0)
		{
			printf("\n");
			printf("%02x  ", i);
		}

		printf("%02x  ", buffer[i]);
	}
	printf("\n");

	return TRUE;
}
unsigned int I2cWriteData(unsigned int length, unsigned char *data, unsigned int DeviceType)
{
	unsigned int i2cAddr;
	unsigned int x;
	unsigned char data_temp[3] = "";
	unsigned int compare[256] = {0};
	unsigned int ndata = 0x00;
	unsigned char subAddr = 0;
	unsigned int n = 0;
	unsigned int h = 0;
	unsigned int m = 0;
	unsigned int k = 0;
	unsigned int p = 0;
	unsigned int flag = 0;
	unsigned int mismatch[256] = {0};
	unsigned char data_page[8] = {0};
	// unsigned int len = 0;
	unsigned char *buffer = (unsigned char *)malloc(sizeof(unsigned char) * EDID_BUFFER_SIZE);
	switch (DeviceType)
	{
	case 0:
		i2cAddr = SERIAL_CRT;
		break;
	case 1:
		i2cAddr = SERIAL_HDMI0;
		break;
	case 2:
		i2cAddr = SERIAL_HDMI1;
		break;
	default:
		// CmdUsage();
		i2c_help_info();
		break;
	}
	// send_data
	printf("start i2c send\n");
    
	for (x = 0; x < length; x += 2)
	{
		// SerialPort_Init(i2cAddr, EDID_DEVADDR);
		data_temp[0] = data[x];
		data_temp[1] = data[x + 1];
		data_temp[2] = '\0';
		//printf("data_temp:%s", data_temp);
		ndata = StoH(data_temp);
		printf("\nI2C write value = 0x%2.2x\n", ndata);
		SerialPort_Init(i2cAddr, EDID_DEVADDR);
		if (i2c_write_data(subAddr, ndata))
		{
			//printf("I2C write ADD = 0x%2.2x\n", subAddr);
			//printf("\nI2C write value = 0x%2.2x\n", ndata);
			// i2c_stop();
		}
		else
		{
			i2c_stop();
			printf("I2C write wrong\n");
		}
		subAddr = subAddr + 1;
		printf("I2C will write ADD = 0x%2.2x\n", subAddr);
		compare[n] = ndata;
		n++;
	}

/*
	SerialPort_Init(i2cAddr, EDID_DEVADDR);
	for (x = 0; x < length; x += 2)
	{
		data_temp[0] = data[x];
		data_temp[1] = data[x + 1];
		data_temp[2] = '\0';
		printf("data_temp:%s",data_temp);
		ndata = StoH(data_temp);
		data_page[p] = ndata;
		compare[n] = ndata;
		n++;
		if ((p + 1) % 8 == 0)
		{
			if (i2c_write_data_page(subAddr, 8, data_page) == FALSE) // i2c_write_data_page(unsigned char addr, unsigned int len, unsigned char *data)
			{
				i2c_stop();
				// printf("subaddress is: 0x%x",subAddr);
				// printf("data_page[0] is:0x%x",data_page[0]);
				printf("\nERROR: I2C write error! Can not find the Device!\n");

				// return -1;  // error exit;
			}
			p = 0;
			subAddr = subAddr + 8;
		}
		else
		{
			p++;
		}
	}
  
*/
	// read data_edid;
	memset(buffer, 0, EDID_BUFFER_SIZE);
	SerialPort_Init(i2cAddr, EDID_DEVADDR);
	printf("\nserialPortNo is 0x%2.2x, device address is 0x%2.2x.\n", i2cAddr, EDID_DEVADDR);
	if (i2c_read_data_page(0x00, EDID_BUFFER_SIZE, buffer) == FALSE)
	{
		i2c_stop();
		printf("\nERROR: I2C Read error! Can not find the Device!\n");
	}
	// add check is xml and edid is right;
	for (m = 0x00; m < length/2; m++)
	{
		if (compare[m] == buffer[m])
		{
			continue;
		}
		else
		{
			flag = 1;
			mismatch[k] = m;
			k++;
		}
	}
	if (flag == 1)
	{
		printf("xml file and edid file back don't match,please check\n");
		printf("wrong index is following:");
		for (h; h < k; h++)
		{
			printf(" 0x%x ", mismatch[h]);
		}
	}
	else
	{
		printf("the data of xml file and edid file read back match.i2c write sucessfully\n");
	}

	return 0;
}

// Delays
void i2c_delay(unsigned int i)
{

	int j = 0;
	i = i * i2c_delay_time;
	for (j = 0; j < i; j++)
	{
		// mdelay(i2c_delay_time);
	}
}

int i2c_prog(int argc, char *argv[])
{
	FILE *fp = NULL;
	int i;
	unsigned char i2cIndex = 0;
	unsigned int serialPortNo = 0x88a0;
	unsigned char deviceAddr = EDID_DEVADDR;
	unsigned char subAddr = 0;
	unsigned char data;
	unsigned char edid[256];
	unsigned char buff[1000];
	unsigned char data_1[1000];
	unsigned char temp[1000];
	unsigned char xml_block[7] = "BLOCK";
	unsigned int len_buff = 0;
	unsigned int len_data = 0;
	unsigned int k = 0;
	// unsigned int i=0;
	unsigned int j = 0;
	unsigned int DeType = 1;
	i2c_delay_time = config.i2c.i2c_delay;

	if (argc >= 3)
	{
		i2cIndex = StoH(argv[1]);
		switch (i2cIndex)
		{
		case 0:
			serialPortNo = config.i2c.serialport0;
			break;
		case 1:
			serialPortNo = config.i2c.serialport1;
			break;
		case 2:
			serialPortNo = config.i2c.serialport2;
			break;
		default:
			i2c_help_info();
			break;
		}

		if (argc == 3)
		{
			deviceAddr = StoH(argv[2]);
			SerialPort_Init(serialPortNo, deviceAddr);
			printf("\nserialPortNo is 0x%4.4x, device address is 0x%2.2x.\n", serialPortNo, deviceAddr);
			memset(edid, 0, EDID_BUFFER_SIZE);
			if (i2c_read_data_page(0x00, EDID_BUFFER_SIZE, edid) == FALSE)
			{
				i2c_stop();
				printf("\nERROR: I2C Read error! Can not find the Device!\n");
			}
			else
			{
				printf("\n");
				printf("    ");
				for (i = 0; i < 16; i++)
				{
					printf("%02x  ", i);
				}
				printf("\n");

				for (i = 0x00; i < EDID_BUFFER_SIZE; i++) // read back
				{
					if ((i % 16) == 0)
					{
						printf("\n");
						printf("%02x  ", i);
					}

					printf("%02x  ", edid[i]);
				}
				printf("\n");
			}
		}
		else if (argc == 4)
		{
			deviceAddr = StoH(argv[2]);
			SerialPort_Init(serialPortNo, deviceAddr);
			printf("\nserialPortNo is 0x%4.4x, device address is 0x%2.2x.\n", serialPortNo, deviceAddr);
			memset(edid, 0, EDID_BUFFER_SIZE);
			subAddr = StoH(argv[3]);
			if (i2c_read_data(subAddr, &data))
			{
				printf("\nI2C read value = 0x%2.2x\n", data);
			}
		}
		else if (argc == 5)
		{
			if (strcmp(argv[1], "-F") == 0 || strcmp(argv[1], "-f") == 0)
			{
				if ((fp = fopen(argv[3], "rb")) == NULL) // argv[2]:xml path:"f:/c_test/980_225MHZ.xml"
				{
					printf("Cannot open xml file!\n");
					printf("Please check the filename and path!\n");
					printf("argv[3] is:%s;\n", argv[3]);
					return -1;
				}
				// analyse xml
				for (;;)
				{
					fgets(buff, 1000, (FILE *)fp);
					if (strcmp(buff, temp) == 0)
					{
						break; // stop read line
					}
					for (i = 0; i < 1000; i++)
					{
						if (buff[i] < 0)
						{
							break; // judge char
						}
						else
						{
							temp[i] = buff[i];
						}
					}
					// judge block
					if (strstr(buff, xml_block) == NULL)
					{
						continue;
					}
					else
					{
						len_buff = strlen(buff);
						// printf("buff lenth is:%d\n",len_buff);
						// date assign;
						for (j = 0; j < len_buff - 19; j++)
						{
							if (buff[j] == 32)
							{ // except ""
								continue;
							}
							else
							{
								data_1[k] = buff[j + 8]; // except block0
								k++;
							}
						}
					}
				}
				printf("xml date :%s\n", data_1);
				len_data = strlen(data_1);
				printf("xml date length is:%d\n", len_data);
				if (strcmp(argv[2], "-v") == 0 || strcmp(argv[2], "-V") == 0) // for vga;
				{
					if (len_data != 256)
					{
						printf("xml data length is not 256,please check it.\n ");
						return 0;
					}
					fclose(fp);
				}
				else
				{
					if (len_data != 512)
					{
						printf("xml data length is not 512,please check it.\n ");
						return 0;
					}
					fclose(fp);
				}
				// i2c write;
				DeType = StoH(argv[4]); // choose seial hdmi/vga/dp;  eg:vga:0/1/2
				// printf("dwtype is:%d\n",DeType);
				I2cWriteData(len_data, data_1, DeType);
				// printf("i2c write is finished\n");
			}
			else
			{
				deviceAddr = StoH(argv[2]);
				SerialPort_Init(serialPortNo, deviceAddr);
				printf("\nserialPortNo is 0x%4.4x, device address is 0x%2.2x.\n", serialPortNo, deviceAddr);
				memset(edid, 0, EDID_BUFFER_SIZE);
				subAddr = StoH(argv[3]);
				data = StoH(argv[4]);
				if (i2c_write_data(subAddr, data))
				{
					printf("\nI2C write value = 0x%2.2x\n", data);
				}

				if (i2c_read_data(subAddr, &data))
				{
					printf("\nI2C read value = 0x%2.2x\n", data);
				}
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
	return 0;
}

void i2c_help_info(void)
{
	printf("i2c 0/1/2 deviceAddr subAddr [data]: 0->CRT 1->HDMI/DP0 2->HDMI/DP1 read/write.\n");
	printf(" ex. read EDID, deviceAddr is 0xa0  cmd: i2c 0 a0\n");
	printf("i2c -f -v/-h filename 0/1/2\n");
}
