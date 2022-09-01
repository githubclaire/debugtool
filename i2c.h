#ifndef _I2C_H_
#define _I2C_H_

#define BIT0    0x01
#define BIT1    0x02
#define BIT2    0x04
#define BIT3    0x08
#define BIT4    0x10
#define BIT5    0x20
#define BIT6    0x40
#define BIT7    0x80

// Serial Port Enable
#define SPC_ENABLE   BIT4
#define SCL_ENABLE   BIT4
#define SDA_ENABLE   BIT4
#define SCL_READ     BIT2
#define SDA_READ     BIT3
#define SCL_WRITE    BIT0
#define SDA_WRITE    BIT1

#define SERIAL_CRT   	0x88A0
#define SERIAL_HDMI0	0x88AA
#define SERIAL_HDMI1	0x89F8 

#define EDID_DEVADDR    0xAA
#define EDID_BUFFER_SIZE 128*2

#define DELAYTIME  2
//#define DELAYTIME  100
//#define RETRYTIMES 10000
#define RETRYTIMES 10000

void SerialPort_Init(unsigned int serialno, unsigned char slaveaddr);
void SerialPort_RegWrite(unsigned value);
unsigned SerialPort_RegRead(void);
void i2c_start(void);
void i2c_stop(void);
int i2c_read();
void i2c_write (unsigned addr);
int i2c_ack_read(void);
int i2c_nack_write();
int i2c_ack_write();
void i2c_delay(unsigned i);
int i2c_read_data(unsigned char addr, unsigned char *data);
int i2c_read_data_page(unsigned char addr, unsigned int len, unsigned char *data);
int i2c_write_data(unsigned char addr, unsigned char data);
int i2c_write_data_page(unsigned char addr, unsigned int len, unsigned char *data);
void i2c_stop();
int i2c_prog(int argc, char *argv[]);
void i2c_help_info(void);

#endif
