#ifndef _FLASH_H_
#define _FLASH_H_

#define NORMAL_EXIT 0
#define CHECKSUM_ERROR 1
#define HARDWARE_ERROR 9
#define SYNTAX_ERROR 10
#define GENERAL_FILE_ERROR 11
#define ROM_FILE_ERROR 12
#define FLASH_ERROR 20

#define MAX_VIDEO_ROM_SIZE 0x50000 // 64k+256k
#define FP_MD5_SIZE 16	//size of flash part page (in bytes)
#define FP_PAGE_SIZE 128	//size of flash part page (in bytes)
#define FP_SECTOR_SIZE 4096	//size of flash part sector (in bytes)
#define FP_BLOCK_SIZE 32768	//size of flash part block (in bytes)

#define FW_HEADER_VERSION_OFFSET 0xF002
#define FW_CHECKSUM_OFFSET 0xF003
#define FW_VENDOR_ID_OFFSET 0xF008
#define FW_DEVICE_ID_OFFSET 0xF00A
#define FW_MD5_VALUE_OFFSET 0xF0B4
#define FW_FILE_SIZE_OFFSET 0xF0C4

#define FW_HEADER_VERSION_SIZE 1
#define FW_CHECKSUM_SIZE 1
#define FW_VENDOR_ID_SIZE 2
#define FW_DEVICE_ID_SIZE 2
#define FW_MD5_VALUE_SIZE 16
#define FW_FILE_SIZE_LEN 4

void flash_vbios(char *romImageName);
int dump_vbios(char *romImageName, unsigned int dump_file_size);
void MD5_Caculate(char* romImage_Name2,unsigned char *MD5Value);
void Generate_new_Rom_file_without_MD5(char *romImageName1,char* romImageName2);
unsigned char* Get_ROM_MD5_Value(char* romImageName);
void Check_Rom_Device_Id(char* romImageName);
int isMD5check(char* romImageName);
unsigned char* read_info_from_file(char *filename, unsigned int seeknum, unsigned int readsize);
int FlashRom(FILE *pStream, unsigned int dwFileSize);
unsigned int get_file_Size(char *filename);
#endif
