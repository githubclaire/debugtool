
#include "def.h"

FILE *in_file, *out_file, *pFile, *fw;
unsigned char HDCPKeyProg[512];
unsigned int HDCPKeyProgCount = 0;
unsigned char HDCPKeyRead[512];
unsigned int HDCPKeyReadCount = 0;
unsigned int addr_82B8 = 0x82B8;
unsigned int addr_82C0 = 0x82C0;
unsigned int addr_33688 = 0x33688;
unsigned int addr_332DC = 0x332DC;
unsigned int addr_82BC = 0x82BC;

void HdcpEfuseReadAll()
{
        unsigned int data;

        unsigned int i = 0;

        for (i = 0; i < 0x80; i++)
        {
                data = HdcpEfuseReadOneDWord(i);

                HDCPKeyRead[i * 4] = (unsigned char)(data & 0xFF);
                HDCPKeyRead[i * 4 + 1] = (unsigned char)(data >> 8 & 0xFF);
                HDCPKeyRead[i * 4 + 2] = (unsigned char)(data >> 16 & 0xFF);
                HDCPKeyRead[i * 4 + 3] = (unsigned char)(data >> 24 & 0xFF);
        }

        HDCPKeyReadCount = 512;
}
// hdcpefuseprogramall()
void HdcpEfuseProgramAll()
{
        REG_MM33688 EfuseBeginRegValue, EfuseBeginRegMask;
        REG_MM82C0 EfuseModeRegValue, EfuseModeRegMask;
        REG_MM332DC EfusePGMAddressRegValue, EfusePGMAddressRegMask;
        unsigned int i, j;
        unsigned int dwkey;
        unsigned data;

        // Set MM33688[26] = 1抌1
        EfuseBeginRegValue.Value = 0;
        EfuseBeginRegValue.START_FLAG = 1;
        EfuseBeginRegMask.Value = 0x04000000;
        WriteMMIOMask(addr_33688, EfuseBeginRegValue.Value, EfuseBeginRegMask.Value);

        // MM82C0[30:28] = 3'b011
        EfuseModeRegValue.Value = 0;
        EfuseModeRegValue.EFUSE_MODE = 3;
        EfuseModeRegMask.Value = 0x70000000;
        WriteMMIOMask(addr_82C0, EfuseModeRegValue.Value, EfuseModeRegMask.Value);

        for (i = 0; i < 0x80; i++) // 7F
        {
                dwkey = (HDCPKeyProg[i * 4 + 3] << 24) + (HDCPKeyProg[i * 4 + 2] << 16) + (HDCPKeyProg[i * 4 + 1] << 8) + HDCPKeyProg[i * 4];

                printf("%02X:%08X\n", i, dwkey);
                for (j = 0; j < 32; j++)
                {
                        if ((dwkey & (1 << j)) != 0)
                        {
                                // Set address and start program //332DC
                                EfusePGMAddressRegValue.Value = 0;
                                EfusePGMAddressRegValue.PGM_EFUSE_REQ = 1;
                                EfusePGMAddressRegValue.PGM_EFUSE_ADDR = ((j << 7) & 0x000000F80) + (i & 0x0000007F); // 0-31 is which bit;
                                EfusePGMAddressRegMask.Value = 0xFFFFFFFF;
                                WriteMMIOMask(addr_332DC, EfusePGMAddressRegValue.Value, EfusePGMAddressRegMask.Value);

                                do
                                {
                                        data = ReadMMIO(addr_332DC, S3X_DWORD); // check busy bit
                                } while ((data & 0x80000000) != 0);
                        }
                }
        }

        // Set MM332dc[30] = 1b'1
        EfusePGMAddressRegValue.Value = 0;
        EfusePGMAddressRegValue.PGM_EFUSE_FINISH = 1;
        EfusePGMAddressRegMask.Value = 0x40000000;
        WriteMMIOMask(addr_332DC, EfusePGMAddressRegValue.Value, EfusePGMAddressRegMask.Value);

        // Set MM332DC[30] = 1b'0
        EfusePGMAddressRegValue.Value = 0;
        EfusePGMAddressRegValue.PGM_EFUSE_FINISH = 0;
        EfusePGMAddressRegMask.Value = 0x40000000;
        WriteMMIOMask(addr_332DC, EfusePGMAddressRegValue.Value, EfusePGMAddressRegMask.Value);

        // MM82C0[30:28] = 3'b000 standby mode
        EfuseModeRegValue.Value = 0;
        EfuseModeRegValue.EFUSE_MODE = 0;
        EfuseModeRegMask.Value = 0x70000000;
        WriteMMIOMask(addr_82C0, EfuseModeRegValue.Value, EfuseModeRegMask.Value);

        // Set MM33688[26] = 1b'0
        EfuseBeginRegValue.Value = 0;
        EfuseBeginRegValue.START_FLAG = 0;
        EfuseBeginRegMask.Value = 0x04000000;
        WriteMMIOMask(addr_33688, EfuseBeginRegValue.Value, EfuseBeginRegMask.Value);
}
// HdcpEfuseLock
void HdcpEfuseLock()
{

        REG_MM33688 EfuseBeginRegValue, EfuseBeginRegMask;
        REG_MM82C0 EfuseModeRegValue, EfuseModeRegMask;
        REG_MM332DC EfusePGMAddressRegValue, EfusePGMAddressRegMask;
        unsigned int data;

        // Set MM33688[26] = 1b'1
        EfuseBeginRegValue.Value = 0;
        EfuseBeginRegValue.START_FLAG = 1;
        EfuseBeginRegMask.Value = 0x04000000;
        WriteMMIOMask(addr_33688, EfuseBeginRegValue.Value, EfuseBeginRegMask.Value);

        // MM82C0[30:28] = 3'b011
        EfuseModeRegValue.Value = 0;
        EfuseModeRegValue.EFUSE_MODE = 3; // 011
        EfuseModeRegMask.Value = 0x70000000;
        WriteMMIOMask(addr_82C0, EfuseModeRegValue.Value, EfuseModeRegMask.Value);

        // Set address and start program
        EfusePGMAddressRegValue.Value = 0;
        EfusePGMAddressRegValue.PGM_EFUSE_REQ = 1;
        EfusePGMAddressRegValue.PGM_EFUSE_ADDR = 0xE6D;
        EfusePGMAddressRegMask.Value = 0xFFFFFFFF;
        WriteMMIOMask(addr_332DC, EfusePGMAddressRegValue.Value, EfusePGMAddressRegMask.Value);

        do
        {
                data = ReadMMIO(addr_332DC, S3X_DWORD); // check busy bit
        } while ((data & 0x80000000) != 0);

        // Set MM332DC[30] = 1b'1
        EfusePGMAddressRegValue.Value = 0;
        EfusePGMAddressRegValue.PGM_EFUSE_FINISH = 1;
        EfusePGMAddressRegMask.Value = 0x40000000;
        WriteMMIOMask(addr_332DC, EfusePGMAddressRegValue.Value, EfusePGMAddressRegMask.Value);

        // Set MM332DC[30] = 1b'0
        EfusePGMAddressRegValue.Value = 0;
        EfusePGMAddressRegValue.PGM_EFUSE_FINISH = 0;
        EfusePGMAddressRegMask.Value = 0x40000000;
        WriteMMIOMask(addr_332DC, EfusePGMAddressRegValue.Value, EfusePGMAddressRegMask.Value);

        // MM82C0[30:28] = 3'b000 standby mode
        EfuseModeRegValue.Value = 0;
        EfuseModeRegValue.EFUSE_MODE = 0;
        EfuseModeRegMask.Value = 0x70000000;
        WriteMMIOMask(addr_82C0, EfuseModeRegValue.Value, EfuseModeRegMask.Value);

        // Set MM33688[26] = 1b'0
        EfuseBeginRegValue.Value = 0;
        EfuseBeginRegValue.START_FLAG = 0;
        EfuseBeginRegMask.Value = 0x04000000;
        WriteMMIOMask(addr_33688, EfuseBeginRegValue.Value, EfuseBeginRegMask.Value);
}
// HdcpEfuseReadOneDWORD
int HdcpEfuseReadOneDWord(unsigned int dwaddr)
{
        // REG_MM33688    EfuseFlagRegValue, EfuseFlagRegMask;

        REG_MM82B8 EfuseAddressRegValue, EfuseAddressRegMask;
        REG_MM82C0 EfuseA_ReadRegValue, EfuseA_ReadModeRegMask;
        unsigned int data;
        dwaddr &= 0x7F;
        // To read address below 0x6D, need to read lock byte 0x6D first
        if (dwaddr <= 0x6D)
        {
                // Write lock byte address
                EfuseAddressRegValue.Value = 0;
                EfuseAddressRegValue.EFUSE_READ_ADDRESS = 0x6D;
                EfuseAddressRegMask.Value = 0x007F0000;
                WriteMMIOMask(addr_82B8, EfuseAddressRegValue.Value, EfuseAddressRegMask.Value);
                // printf("addr_82B8 is 0x%8.8x\n",addr_82B8);
                // printf("write_MMIO_82B8 is 0x%8.8x\n",ReadMMIO(addr_82B8,S3X_DWORD));
                //  Send read command
                EfuseA_ReadRegValue.Value = 0;
                EfuseA_ReadRegValue.EFUSE_READ_REQUEST = 1;
                EfuseA_ReadRegValue.RESERVED_1 = 0x0 & 0x3; // 00
                EfuseA_ReadRegValue.EFUSE_MODE = 0x2 & 0x7; // 010
                EfuseA_ReadModeRegMask.Value = 0x72000000;
                // printf("EfuseA_ReadRegValue.Value is :0x%8.8x",EfuseA_ReadRegValue.Value);
                WriteMMIOMask(addr_82C0, EfuseA_ReadRegValue.Value, EfuseA_ReadModeRegMask.Value);
                // printf("addr_82C0 is 0x%8.8x\n",addr_82C0);
                // printf("write_MMIO_82C0 is 0x%8.8x\n",ReadMMIO(addr_82C0,S3X_DWORD));
        }
        // Write read efuse address
        EfuseAddressRegValue.Value = 0;
        EfuseAddressRegValue.EFUSE_READ_ADDRESS = dwaddr;
        EfuseAddressRegMask.Value = 0x007F0000;
        WriteMMIOMask(addr_82B8, EfuseAddressRegValue.Value, EfuseAddressRegMask.Value);
        // printf("addr_82B8 is 0x%8.8x\n",addr_82B8);
        // printf("write_MMIO_82B8 is 0x%8.8x\n",ReadMMIO(addr_82B8,S3X_DWORD));
        // Send read command
        EfuseA_ReadRegValue.Value = 0;
        EfuseA_ReadRegValue.EFUSE_READ_REQUEST = 1;
        EfuseA_ReadRegValue.RESERVED_1 = 0x0 & 0x3;
        EfuseA_ReadRegValue.EFUSE_MODE = 0x2 & 0x7; // 010
        EfuseA_ReadModeRegMask.Value = 0x72000000;
        WriteMMIOMask(addr_82C0, EfuseA_ReadRegValue.Value, EfuseA_ReadModeRegMask.Value);
        // printf("addr_82C0 is 0x%8.8x\n",addr_82C0);
        // printf("write_MMIO_82C0 is 0x%8.8x\n",ReadMMIO(addr_82C0,S3X_DWORD));

        // check busy bit;[25]
        do
        {
                data = ReadMMIO(addr_82C0, S3X_DWORD); // check busy bit
        } while ((data & 0x02000000) != 0);

        // read efuse data DWORD
        data = ReadMMIO(addr_82BC, S3X_DWORD);
        // printf("addr_82BC is 0x%8.8x",addr_82BC);
        return data;
}
void HdcpEfuseBlankCheck()
{
        unsigned int i = 0;
        unsigned int blank = 0xff;

        HdcpEfuseReadAll();

        for (i = 0; i < 436; i++)
        {
                if (HDCPKeyRead[i] != 0)
                {
                        blank = 0x00;
                        break;
                }
        }
        if (blank == 0)
                printf("HDCP EFuse is not Blank.\n");
        else
                printf("HDCP EFuse is Blank.\n");
        /*
         //added by cheerychen,check if iocked;
          HdcpEfuseProgramOneBit(0xDA0);  //6D BIT[0]
          if (((HDCPKeyRead[436]&0x1)==1){
           printf("HDCP EFuse lock bit is locked\n");
         }
        else{
          printf("HDCP EFuse lock bit is unlocked\n");
        }
         */
}

void HdcpEfuseProgramOneBit(unsigned int bit_addr)
{
        REG_MM33688 EfuseBeginRegValue, EfuseBeginRegMask;
        REG_MM82C0 EfuseModeRegValue, EfuseModeRegMask;
        REG_MM332DC EfusePGMAddressRegValue, EfusePGMAddressRegMask;
        unsigned int data;

        // Set MM33688[26] = 1抌1
        EfuseBeginRegValue.Value = 0;
        EfuseBeginRegValue.START_FLAG = 1;
        EfuseBeginRegMask.Value = 0x04000000;
        WriteMMIOMask(addr_33688, EfuseBeginRegValue.Value, EfuseBeginRegMask.Value);
        // printf("addr_33688 is 0x%8.8x\n",ReadMMIO(addr_33688,S3X_DWORD));

        // MM82C0[30:28] = 3'b011
        EfuseModeRegValue.Value = 0;
        EfuseModeRegValue.EFUSE_MODE = 0x3 & 0x7; // 011
        EfuseModeRegMask.Value = 0x70000000;
        WriteMMIOMask(addr_82C0, EfuseModeRegValue.Value, EfuseModeRegMask.Value);
        // printf("addr_82C0 is 0x%8.8x\n",ReadMMIO(addr_82C0,S3X_DWORD));

        // Set address and start program
        EfusePGMAddressRegValue.Value = 0;
        EfusePGMAddressRegValue.PGM_EFUSE_REQ = 1;
        EfusePGMAddressRegValue.PGM_EFUSE_ADDR = ((bit_addr << 7) & 0x000000F80) + ((bit_addr >> 5) & 0x0000007F);
        EfusePGMAddressRegMask.Value = 0xFFFFFFFF;
        WriteMMIOMask(addr_332DC, EfusePGMAddressRegValue.Value, EfusePGMAddressRegMask.Value);
        // printf("addr_332DC is 0x%8.8x\n",ReadMMIO(addr_332DC,S3X_DWORD));

        do
        {
                data = ReadMMIO(addr_332DC, S3X_DWORD); // check busy bit
        } while ((data & 0x80000000) != 0);

        // Set MM332DC[30] = 1抌1
        EfusePGMAddressRegValue.Value = 0;
        EfusePGMAddressRegValue.PGM_EFUSE_FINISH = 1;
        EfusePGMAddressRegMask.Value = 0x40000000;
        WriteMMIOMask(addr_332DC, EfusePGMAddressRegValue.Value, EfusePGMAddressRegMask.Value);
        // printf("addr_332DC is 0x%8.8x\n",ReadMMIO(addr_332DC,S3X_DWORD));

        // Set MM332DC[30] = 1抌0
        EfusePGMAddressRegValue.Value = 0;
        EfusePGMAddressRegValue.PGM_EFUSE_FINISH = 0;
        EfusePGMAddressRegMask.Value = 0x40000000;
        WriteMMIOMask(addr_332DC, EfusePGMAddressRegValue.Value, EfusePGMAddressRegMask.Value);
        // printf("addr_332DC is 0x%8.8x\n",ReadMMIO(addr_332DC,S3X_DWORD));
        // MM82C0[30:28] = 3'b000 standby mode
        EfuseModeRegValue.Value = 0;
        EfuseModeRegValue.EFUSE_MODE = 0x0 & 0x7; // 000
        EfuseModeRegMask.Value = 0x70000000;
        WriteMMIOMask(addr_82C0, EfuseModeRegValue.Value, EfuseModeRegMask.Value);
        // printf("addr_82C0 is 0x%8.8x\n",ReadMMIO(addr_82C0,S3X_DWORD));
        //  Set MM33688[26] = 1抌0
        EfuseBeginRegValue.Value = 0;
        EfuseBeginRegValue.START_FLAG = 0;
        EfuseBeginRegMask.Value = 0x04000000;
        WriteMMIOMask(addr_33688, EfuseBeginRegValue.Value, EfuseBeginRegMask.Value);
        //       printf("addr_33688 is 0x%8.8x\n",ReadMMIO(addr_33688,S3X_DWORD));
}

void HdcpEfuseProgramOneByte(unsigned int byte_addr, unsigned char indata)
{
        unsigned int dwaddr;
        unsigned int dwkey;
        unsigned int j = 0;
        REG_MM33688 EfuseBeginRegValue, EfuseBeginRegMask;
        REG_MM82C0 EfuseModeRegValue, EfuseModeRegMask;
        REG_MM332DC EfusePGMAddressRegValue, EfusePGMAddressRegMask;
        unsigned int data;

        // Set MM33688[26] = 1抌1
        EfuseBeginRegValue.Value = 0;
        EfuseBeginRegValue.START_FLAG = 1;
        EfuseBeginRegMask.Value = 0x04000000;
        WriteMMIOMask(addr_33688, EfuseBeginRegValue.Value, EfuseBeginRegMask.Value);

        // MM82C0[30:28] = 3'b011
        EfuseModeRegValue.Value = 0;
        EfuseModeRegValue.EFUSE_MODE = 3;
        EfuseModeRegMask.Value = 0x70000000;
        WriteMMIOMask(addr_82C0, EfuseModeRegValue.Value, EfuseModeRegMask.Value);

        // byte_addr:03 02 01 00 07 06 05 04

        dwaddr = (byte_addr >> 2) & 0x7F;
        dwkey = (unsigned int)indata << (8 * (byte_addr & 0x03)); // turn to 8-32 bits;indata:10100000

        for (j = 0; j < 32; j++)
        {
                if ((dwkey & (1 << j)) != 0)
                {
                        // Set address and start program
                        EfusePGMAddressRegValue.Value = 0;
                        EfusePGMAddressRegValue.PGM_EFUSE_REQ = 1;
                        EfusePGMAddressRegValue.PGM_EFUSE_ADDR = ((j << 7) & 0x000000F80) + dwaddr; // 0-31 is which bit;
                        EfusePGMAddressRegMask.Value = 0xFFFFFFFF;
                        WriteMMIOMask(addr_332DC, EfusePGMAddressRegValue.Value, EfusePGMAddressRegMask.Value);

                        do
                        {
                                data = ReadMMIO(addr_332DC, S3X_DWORD); // check busy bit
                        } while ((data & 0x80000000) != 0);
                }
        }

        // Set MM332DC[30] = 1抌1
        EfusePGMAddressRegValue.Value = 0;
        EfusePGMAddressRegValue.PGM_EFUSE_FINISH = 1;
        EfusePGMAddressRegMask.Value = 0x40000000;
        WriteMMIOMask(addr_332DC, EfusePGMAddressRegValue.Value, EfusePGMAddressRegMask.Value);

        // Set MM332DC[30] = 1抌0
        EfusePGMAddressRegValue.Value = 0;
        EfusePGMAddressRegValue.PGM_EFUSE_FINISH = 0;
        EfusePGMAddressRegMask.Value = 0x40000000;
        WriteMMIOMask(addr_332DC, EfusePGMAddressRegValue.Value, EfusePGMAddressRegMask.Value);

        // MM82C0[30:28] = 3'b000 standby mode
        EfuseModeRegValue.Value = 0;
        EfuseModeRegValue.EFUSE_MODE = 0;
        EfuseModeRegMask.Value = 0x70000000;
        WriteMMIOMask(addr_82C0, EfuseModeRegValue.Value, EfuseModeRegMask.Value);

        // Set MM33688[26] = 1抌0
        EfuseBeginRegValue.Value = 0;
        EfuseBeginRegValue.START_FLAG = 0;
        EfuseBeginRegMask.Value = 0x04000000;
        WriteMMIOMask(addr_33688, EfuseBeginRegValue.Value, EfuseBeginRegMask.Value);
}

void HdcpEfuseProgramOneDWord(unsigned int dword_addr, unsigned int indata)
{
        unsigned int j = 0;
        REG_MM33688 EfuseBeginRegValue, EfuseBeginRegMask;
        REG_MM82C0 EfuseModeRegValue, EfuseModeRegMask;
        REG_MM332DC EfusePGMAddressRegValue, EfusePGMAddressRegMask;
        unsigned int data;

        // Set MM33688[26] = 1抌1
        EfuseBeginRegValue.Value = 0;
        EfuseBeginRegValue.START_FLAG = 1;
        EfuseBeginRegMask.Value = 0x04000000;
        WriteMMIOMask(addr_33688, EfuseBeginRegValue.Value, EfuseBeginRegMask.Value);

        // MM82C0[30:28] = 3'b011
        EfuseModeRegValue.Value = 0;
        EfuseModeRegValue.EFUSE_MODE = 3;
        EfuseModeRegMask.Value = 0x70000000;
        WriteMMIOMask(addr_82C0, EfuseModeRegValue.Value, EfuseModeRegMask.Value);
        for (j = 0; j < 32; j++) // indate:0x00000000  //burn which bit,which bit is 1;
        {
                if ((indata & (1 << j)) != 0) //[j->[0,31]],which bit;
                {
                        // Set address and start program
                        EfusePGMAddressRegValue.Value = 0;
                        EfusePGMAddressRegValue.PGM_EFUSE_REQ = 1;
                        EfusePGMAddressRegValue.PGM_EFUSE_ADDR = ((j << 7) & 0x000000F80) + (dword_addr & 0x7F); // turn to 12bits;
                        EfusePGMAddressRegMask.Value = 0xFFFFFFFF;
                        WriteMMIOMask(addr_332DC, EfusePGMAddressRegValue.Value, EfusePGMAddressRegMask.Value);
                        do
                        {
                                data = ReadMMIO(addr_332DC, S3X_DWORD); // check busy bit
                        } while ((data & 0x80000000) != 0);
                }
        }
        // Set MM332DC[30] = 1抌1
        EfusePGMAddressRegValue.Value = 0;
        EfusePGMAddressRegValue.PGM_EFUSE_FINISH = 1;
        EfusePGMAddressRegMask.Value = 0x40000000;
        WriteMMIOMask(addr_332DC, EfusePGMAddressRegValue.Value, EfusePGMAddressRegMask.Value);

        // Set MM332DC[30] = 1抌0
        EfusePGMAddressRegValue.Value = 0;
        EfusePGMAddressRegValue.PGM_EFUSE_FINISH = 0;
        EfusePGMAddressRegMask.Value = 0x40000000;
        WriteMMIOMask(addr_332DC, EfusePGMAddressRegValue.Value, EfusePGMAddressRegMask.Value);

        // MM82C0[30:28] = 3'b000 standby mode
        EfuseModeRegValue.Value = 0;
        EfuseModeRegValue.EFUSE_MODE = 0;
        EfuseModeRegMask.Value = 0x70000000;
        WriteMMIOMask(addr_82C0, EfuseModeRegValue.Value, EfuseModeRegMask.Value);

        // Set MM33688[26] = 1抌0
        EfuseBeginRegValue.Value = 0;
        EfuseBeginRegValue.START_FLAG = 0;
        EfuseBeginRegMask.Value = 0x04000000;
        WriteMMIOMask(addr_33688, EfuseBeginRegValue.Value, EfuseBeginRegMask.Value);
}
// added by cheerychen 2021.9.14;
void HdcpEfuseReadssiddDWord(unsigned int data)
{
        unsigned int data_q = (data >> 12) & 0xf; // qian bit
        unsigned int data_b = (data >> 16) & 0xf; // bai bit
        unsigned int data_s = (data >> 20) & 0xf; // shi wei
        unsigned int data_g = (data >> 24) & 0xf; // ge wei
        unsigned int data_x = (data >> 28) & 0xf; // xiao shu wei
        printf("Sidd is %d%d%d%d.%dmA\n", data_q, data_b, data_s, data_g, data_x);
}
// added by cheerychen 2021.11.12
void HdcpEfuseReadWCDWord(unsigned int data)
{
        unsigned int water_id_1=(data>>4)&0xf;  //water_id
        unsigned int water_id_2=(data>>8)&0xf;
        unsigned int x=(data>>12)&0xff;
	unsigned int y=(data>>20)&0xff;
	unsigned int x_1=((x<<4)&0xff)+((x>>4)&0xff);
	unsigned int y_1=((y<<4)&0xff)+((y>>4)&0xff);
	printf("Wafer id:%x%x\n",water_id_1,water_id_2);
	printf("(X,Y)=(%x,%x)\n",x_1,y_1);
}
void HdcpEfuseReadLWord(unsigned int data1, unsigned int data) // data1:0x7a;data:0x7B
{
        unsigned int L_1 = data1 & 0x3f;
        unsigned int L_2 = (data1 >> 6) & 0x3f;
        unsigned int L_3 = (data1 >> 12) & 0x3f;
        unsigned int L_4 = (data1 >> 18) & 0x3f;
        unsigned int L_5 = (data1 >> 24) & 0x3f;
        unsigned int L_6 = ((data1 >> 30) & 0x3) + ((data & 0Xf) << 2); // BIT[30],BIT[31]+0x78bit[3]-[0]
        unsigned int L_1_C = int_to_char(L_1);
        unsigned int L_2_C = int_to_char(L_2);
        unsigned int L_3_C = int_to_char(L_3);
        unsigned int L_4_C = int_to_char(L_4);
        unsigned int L_5_C = int_to_char(L_5);
        unsigned int L_6_C = int_to_char(L_6);
        printf("Lot id:%c%c%c%c%c%c\n", L_1_C, L_2_C, L_3_C, L_4_C, L_5_C, L_6_C);
}

int int_to_char(unsigned int data)
{
        int to_char;
        if (data <= 9)
        {
                to_char = data + 48;
        }
        else
        {
                to_char = data + 55;
        }
        return to_char;
}

void Usage()
{
        printf("Usage: Read or program CHX001 HDCP E-Fuse.\n");
        printf("efuse -p -i addr       #prog 1bit at bit addr(HEX)(000 - FFF).\n");
        printf("efuse -p -b addr data  #prog 8bit at byte addr(HEX)(000 - 1FF).\n");
        printf("efuse -p -w addr data  #prog 32bit at dword addr(HEX)(00 - 7F).\n");
        printf("efuse -p -f file.bin -1.4   #prog all data from a hdcp 1.4 binary file.\n");
        printf("efuse -p -f file.bin -2.2   #prog all data from a hdcp 2.2 binary file.\n");
        printf("efuse -r -i addr       #read 1bit at bit addr(HEX)(000 - FFF).\n");
        printf("efuse -r -b addr       #read 8bit at byte addr(HEX)(000 - 1FF).\n");
        printf("efuse -r -w addr       #read 32bit at dword addr(HEX)(00 - 7F).\n");
        printf("efuse -r -s  -l        #read  efuse sidd value\n");
        printf("efuse -r -f file.bin   #read all efuse into a binary file.\n");
        printf("efuse -v -f file.bin   #verify HDCP key section with a binary file.\n");
        printf("efuse -l  #lock hdcp efuse\n");
        printf("efuse -b  #hdcp efuse blank check\n");
}

int efuse_test(int argc, char *argv[])
{
        unsigned int i, j;
        unsigned int data;
        unsigned int data1;
        unsigned int dwordaddr;
        unsigned int byteaddr;
        unsigned int bitaddr;

        if (argc == 2)
        {
                if (strcmp(argv[1], "-B") == 0 || strcmp(argv[1], "-b") == 0)
                {
                        printf("HDCP EFuse blank check start...\n");
                        HdcpEfuseBlankCheck();
                }
                else if (strcmp(argv[1], "-L") == 0 || strcmp(argv[1], "-l") == 0)
                {
                        printf("HDCP EFuse lock start...\n");
                        HdcpEfuseLock();
                        printf("HDCP EFuse lock finished.\n");
                }
                else
                {
                        printf("Please check the input parameters.\n");
                        Usage();
                        return 0;
                }
        }
        else if (argc == 4)
        {
                // Verify operation
                if (strcmp(argv[1], "-V") == 0 || strcmp(argv[1], "-v") == 0)
                {
                        if (strcmp(argv[2], "-F") == 0 || strcmp(argv[2], "-f") == 0)
                        {
                                if ((in_file = fopen(argv[3], "rb")) == NULL)
                                {
                                        printf("Cannot open key file!\n");
                                        printf("Please check the filename and path!\n");
                                        return 0;
                                }
                                printf("Verify HDCP EFuse Data with a binary file...\n");
                                HDCPKeyProgCount = fread(HDCPKeyProg, sizeof(unsigned char), 512, in_file);
                                fclose(in_file);
                                for (i = 0; i < 512; i++)
                                        HDCPKeyRead[i] = 0;
                                HdcpEfuseReadAll();
                                for (i = 0; i < 448; i++) // 448/4=112:6e+1=6f=111
                                {
                                        if (HDCPKeyRead[i] != HDCPKeyProg[i])
                                        {
                                                printf("Verify failed");
                                                return 0;
                                        }
                                }
                                printf("Verify passed.\n");
                        }
                }
                // Program operation
                if (strcmp(argv[1], "-P") == 0 || strcmp(argv[1], "-p") == 0)
                {
                        // program one bit
                        if (strcmp(argv[2], "-I") == 0 || strcmp(argv[2], "-i") == 0)
                        {
                                printf("Program E-Fuse one bit data...\n");
                                bitaddr = StoH(argv[3]);
                                HdcpEfuseProgramOneBit(bitaddr);
                                dwordaddr = bitaddr >> 5;
                                printf("bitarrr is :%x dword", dwordaddr);
                                data = HdcpEfuseReadOneDWord(dwordaddr);
                                printf("E-Fuse bit data at 0x%03X is %1X\n", bitaddr, (data & (unsigned int)(1 << (bitaddr & 0x1F))) != 0);
                        }
                        // program one bit
                        else
                        {
                                printf("Please check the input parameters.\n");
                                Usage();
                                return 0;
                        }
                }

                // Read operation
                if (strcmp(argv[1], "-R") == 0 || strcmp(argv[1], "-r") == 0)
                {
                        // read one bit
                        if (strcmp(argv[2], "-I") == 0 || strcmp(argv[2], "-i") == 0)
                        {
                                printf("Read E-Fuse one bit data...\n");
                                bitaddr = StoH(argv[3]);
                                dwordaddr = bitaddr >> 5;
                                data = HdcpEfuseReadOneDWord(dwordaddr);
                                printf("date is 0x%8.8x\n", data);
                                printf("E-Fuse bit data at 0x%03X is %1X\n", bitaddr, (data & (unsigned int)(1 << (bitaddr & 0x1F))) != 0);
                        }
                        // read one byte
                        else if (strcmp(argv[2], "-B") == 0 || strcmp(argv[2], "-b") == 0)
                        {
                                printf("Read E-Fuse one byte data...\n");
                                byteaddr = StoH(argv[3]);
                                dwordaddr = byteaddr >> 2;
                                data = HdcpEfuseReadOneDWord(dwordaddr);
                                printf("E-Fuse byte data at 0x%03X is 0x%02X\n", byteaddr, (unsigned char)(data >> (byteaddr & 0x03) * 8));
                        }
                        // read one dword
                        else if (strcmp(argv[2], "-W") == 0 || strcmp(argv[2], "-w") == 0)
                        {
                                printf("Read E-Fuse one DWORD data...\n");
                                dwordaddr = StoH(argv[3]);
                                data = HdcpEfuseReadOneDWord(dwordaddr);
                                printf("E-Fuse dword data at 0x%02X is 0x%08X\n", dwordaddr, data);
                        }
                        else if (strcmp(argv[2], "-S") == 0 || strcmp(argv[2], "-s") == 0) // added by cheerychen;add sidd read;
                        {
                                printf("Read E-Fuse SIDD data...\n");
                                // dwordaddr = StoH(argv[3]);
                                data = HdcpEfuseReadOneDWord(0x77);
                                HdcpEfuseReadssiddDWord(data);
                                data1 = HdcpEfuseReadOneDWord(0x7A);
                                data = HdcpEfuseReadOneDWord(0x7B);
                                HdcpEfuseReadLWord(data1, data);
                                HdcpEfuseReadWCDWord(data);
                                // printf("Read E-Fuse SIDD data has finished...\n");
                        }
                        // read all into a bin file
                        else if (strcmp(argv[2], "-F") == 0 || strcmp(argv[2], "-f") == 0)
                        {
                                if ((out_file = fopen(argv[3], "wb")) == NULL)
                                {
                                        printf("Cannot open key file!\n");
                                        printf("Please check the filename and path!\n");
                                        return 0;
                                }

                                printf("Read E-Fuse all data into a binary file...\n");
                                for (i = 0; i < 512; i++)
                                        HDCPKeyRead[i] = 0;

                                HdcpEfuseReadAll();
                                fwrite(HDCPKeyRead, sizeof(unsigned char), HDCPKeyReadCount, out_file);
                                fclose(out_file);

                                // print the efuse data in console window
                                for (i = 0; i < 32; i++)
                                {
                                        printf("%02X:  ", i);
                                        for (j = 0; j < 16; j++)
                                        {
                                                printf("%02X ", HDCPKeyRead[i * 16 + j]);
                                        }
                                        printf("\n");
                                }
                                printf("Read E-Fuse data into a binary file finished.\n");
                        }
                        else
                        {
                                printf("Please check the input parameters.\n");
                                Usage();
                                return 0;
                        }
                }
        }
        else if (argc == 5)
        {
                // Program operation
                if (strcmp(argv[1], "-P") == 0 || strcmp(argv[1], "-p") == 0)
                {
                        // program one bit
                        if (strcmp(argv[2], "-B") == 0 || strcmp(argv[2], "-b") == 0)
                        {
                                printf("Program E-Fuse one byte data...\n");
                                byteaddr = StoH(argv[3]);
                                HdcpEfuseProgramOneByte(byteaddr, StoH(argv[4]));
                                dwordaddr = byteaddr >> 2;
                                data = HdcpEfuseReadOneDWord(dwordaddr);
                                printf("date is 0x%8.8x\n", data);
                                printf("E-Fuse byte data at 0x%03X is 0x%02X\n", byteaddr, (unsigned char)(data >> (byteaddr & 0x03) * 8));
                        }
                        // program one bit
                        else if (strcmp(argv[2], "-W") == 0 || strcmp(argv[2], "-w") == 0)
                        {
                                printf("Program E-Fuse one byte data...\n");
                                dwordaddr = StoH(argv[3]);
                                HdcpEfuseProgramOneDWord(dwordaddr, StoH(argv[4]));

                                data = HdcpEfuseReadOneDWord(dwordaddr);
                                printf("E-Fuse dword data at 0x%02X is 0x%08X\n", dwordaddr, data);
                        }
                        else if (strcmp(argv[2], "-F") == 0 || strcmp(argv[2], "-f") == 0)
                        {
                                if ((in_file = fopen(argv[3], "rb")) == NULL)
                                {
                                        printf("Cannot open key file!\n");
                                        printf("Please check the filename and path!\n");
                                        return 0;
                                }
                                // added by cheery
                                if (strcmp(argv[4], "-2.2") == 0)
                                {
                                        int iValue[128] = {0x00};
                                        int read[7];
                                        int i = 0;
                                        int j = 0;
                                        int length = sizeof(iValue) / sizeof(int);
                                        pFile = fopen(argv[3], "rb");
                                        fread(read, 1, sizeof(read), pFile);
                                        for (i; i < 7; i++)
                                        {
                                                iValue[102 + i] = read[i];
                                        }
                                        fw = fopen("hdcpfile_key.bin", "wb");
                                        printf("2.2 hdcp.bin has been opened;\n");
                                        for (j; j < length; j++)
                                        {
                                                fwrite(iValue + j, sizeof(int), 1, fw);
                                        }
                                        fclose(fw);
                                        printf("2.2 hdcp.bin has been opened;\n");
                                        in_file = fopen("hdcpfile_key.bin", "rb");
                                }
                                printf("Program HDCP EFuse Data from a binary file...\n");
                                HDCPKeyProgCount = fread(HDCPKeyProg, sizeof(unsigned char), 512, in_file);
                                fclose(in_file);
                                HdcpEfuseProgramAll();
                                printf("Program HDCP EFuse Data from a binary file finished.\n");
                                if (remove("hdcpfile_key.bin") == 0)
                                {
                                        printf("hdcpfile_key has been deleted\n");
                                }
                                // added by cheery end
                        }
                        else
                        {
                                printf("Please check the input parameters.\n");
                                Usage();
                                return 0;
                        }
                }
                else
                {
                        printf("Please check the input parameters.\n");
                        Usage();
                        return 0;
                }
        }
        else
        {
                Usage();
        }
        // system("pause");
        return 0;
}
