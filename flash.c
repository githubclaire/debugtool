#include "def.h"

extern VIDEO_PCI_PROP video_pci_prop;

FILE *OpenFileUser(char *szFname, char *szMode)
{
    return fopen(szFname, szMode);
}

unsigned int get_file_Size(char *filename)
{
    unsigned int fileSize=0;
    
#ifdef __ubuntu__
	struct stat statbuf;
	stat(filename,&statbuf);
#endif

#ifdef __dos__
   	struct _stat statbuf;
	_stat(filename,&statbuf);
#endif
    fileSize = statbuf.st_size;
	
	return fileSize;
}

int FlashRom(FILE *pStream, unsigned int dwFileSize)
{
    int i,j;
    unsigned char pBuffer[FP_SECTOR_SIZE];
    unsigned int readdata;
    unsigned int sectorNum;
    
    fseek(pStream, 0, SEEK_SET);
    //  base on file size, round up to an integer of sector num  
    sectorNum = (dwFileSize / FP_SECTOR_SIZE);
    if(dwFileSize>sectorNum*FP_SECTOR_SIZE)
        sectorNum++;
    
    //write hex file to rom
    //printf("sector offset:%d ,file size:%d,sectorNum:%d\n",pChipInfo->fpInfo.dwSector,dwFileSize,sectorNum);
    for(j=0; j<sectorNum; j++)
    {
		
        // erase rom
        sf_sector_erase(j*FP_SECTOR_SIZE);   
  
        //flash rom         
        // read a sector from file
        if (fread(pBuffer, sizeof(unsigned char), FP_SECTOR_SIZE, pStream)
            != FP_SECTOR_SIZE && (j<(dwFileSize / FP_SECTOR_SIZE))) {
                
                printf("\nVideo BIOS image File reading failure\n");
                break;
        }
        //write
        for(i=0;i<FP_SECTOR_SIZE;i+=4)
        {
            sf_write_data(j*FP_SECTOR_SIZE +i,*(unsigned int *)(pBuffer + i),4);
        }   
        //read out and verify
        for(i=0;i<FP_SECTOR_SIZE;i+=4)
        {
            readdata = sf_read_data(j*FP_SECTOR_SIZE +i,4);
            if(readdata!=*(unsigned int *)(pBuffer + i))
            {
                printf("Write Data error: addr:%08x, write data:%08x, read back:%08x\n",j*FP_SECTOR_SIZE +i,*(unsigned int *)(pBuffer + i),readdata);
                return -1;
            }
            //sf_write_data(j*FP_SECTOR_SIZE +i,*(unsigned int *)(pBuffer + i),4);
        }

        if(((j+1)*100/sectorNum)%20==0)
            printf("%d%%\n",(int)((j+1)*100/sectorNum));
        //bWriteOk = SectorWriteSerialChip(&pChipInfo->fpInfo, pBuffer);
    }
    printf("%d%%\n",100);
    return 0;
}

void flash_vbios(char *romImageName)
{
    char *tempImage = "GFINIT.ROM";
    int is_md5_rom = 0;
    unsigned int rom_file_size;
    unsigned char *Value_MD5 = NULL;
    FILE *file_rom = NULL;

    //memset(Value_MD5,0,FW_MD5_VALUE_SIZE);
    
    Check_Rom_Device_Id(romImageName);

    is_md5_rom = isMD5check(romImageName);
    if (is_md5_rom)
    {
        Value_MD5 = Get_ROM_MD5_Value(romImageName);
        Generate_new_Rom_file_without_MD5(romImageName,tempImage);
        MD5_Caculate(tempImage,Value_MD5);
        if(remove(tempImage) == 0)
            printf("Removed %s.\n", tempImage);
        else
            printf("Remove temp bin error\n");
    }
    else
    {
        printf("Use Checksum Verification!\n");
    }

    if ((file_rom = OpenFileUser(romImageName, "rb")) == NULL)
    {
        printf("Can't open rom %s\n",romImageName);
        exit(ROM_FILE_ERROR);
    }
    else
    {
        rom_file_size = get_file_Size(romImageName);
        printf("Program rom file size = %d\n",rom_file_size);

        if(FlashRom(file_rom,rom_file_size)==0)
        {
            printf("Flash Successfully!\n");
        }
        else
        {
            printf("Flash Failed\n");
        }
    }
    fclose(file_rom);

}

int dump_vbios(char *romImageName, unsigned int dump_file_size) 
{
    unsigned char pBuffer[FP_SECTOR_SIZE];
    unsigned int i,j;
    unsigned int sectorNum;
    //unsigned int dump_file_size;
    FILE *pStream;

    memset(pBuffer,0,FP_SECTOR_SIZE);
    if ((pStream = OpenFileUser(romImageName, "wb+")) == NULL)
    {
        printf("Can't creat file %s\n",romImageName);
        return FALSE;
    } 
 
    sectorNum = (dump_file_size / FP_SECTOR_SIZE);
    if(dump_file_size > sectorNum*FP_SECTOR_SIZE)
        sectorNum++;
    for(i=0;i<sectorNum;i++) 
    {
        for(j=0;j<FP_SECTOR_SIZE;j++)
        {
            pBuffer[j]=sf_read_data(i*FP_SECTOR_SIZE+j,1);
        }
        fwrite(pBuffer, sizeof(unsigned char), FP_SECTOR_SIZE, pStream);
        
        if(((i+1)*100/sectorNum)%20==0)
            printf("%d%%\n",(int)((i+1)*100/sectorNum));
    }
    fclose(pStream);
    printf("Dump rom from flash successfully!\n");
    return TRUE;
}  


void MD5_Caculate(char* romImage_Name2,unsigned char *MD5Value)
{
    int i;
    MD5_CTX md5;
    unsigned char decrypt[16];
    unsigned int noMD5filesize = get_file_Size(romImage_Name2);
    unsigned char* ptr = (unsigned char *)malloc(noMD5filesize*sizeof(unsigned char));
    if(ptr==NULL)
    {
        printf("ptr malloc is wrong");
        exit(GENERAL_FILE_ERROR);
    }
    
    MD5Init(&md5);
    ptr = read_info_from_file(romImage_Name2,0x0,noMD5filesize);
    MD5Update(&md5,ptr,noMD5filesize);
    MD5Final(&md5,decrypt);

    printf("Caculate MD5 Value: ");
	for ( i =0;i<FW_MD5_VALUE_SIZE;i++)
    {
	    printf("%2.2x ",  decrypt[i]);
	}
    printf("\n");

    for(i=0;i<FW_MD5_VALUE_SIZE;i++)
    {
        if (MD5Value[i]!=decrypt[i])
        {
            printf("MD5 Error: ROM file caculte not match MD5 !\n");
            exit(ROM_FILE_ERROR);
        }
    }
    printf("MD5 match: ROM file match MD5 !\n");

    free(ptr);
    ptr=NULL;
}

void Generate_new_Rom_file_without_MD5(char *romImageSrc,char* romImageDest)
{
    unsigned int filesize;
    unsigned int i;
    unsigned char *pBuffer = (unsigned char*)malloc(sizeof(unsigned char));

    FILE *file_dest;
    FILE *file_src;

    if ((file_dest = OpenFileUser(romImageDest, "wb+")) == NULL)
    {
        printf("Can't creat dest file %s\n",romImageDest);
        exit(GENERAL_FILE_ERROR);
    } 
    if ((file_src = OpenFileUser(romImageSrc, "rb")) == NULL)
    {
        printf("Can't open source file %s\n",romImageSrc);
        exit(GENERAL_FILE_ERROR);
    } 
    filesize = get_file_Size(romImageSrc);

    for(i=0;i<filesize;i++) 
    {
        fread(pBuffer,sizeof(unsigned char), 1, file_src);
        if(i>=FW_MD5_VALUE_OFFSET && i<FW_MD5_VALUE_OFFSET+FW_MD5_VALUE_SIZE)
        {
            *pBuffer = 0;
        }
        fwrite(pBuffer, sizeof(unsigned char), 1, file_dest);   
    }

    fclose(file_src);
    fclose(file_dest);
}

unsigned char* read_info_from_file(char *filename, unsigned int seeknum, unsigned int readsize)
{
    FILE *pfile = NULL;
    unsigned char *data = (unsigned char *)malloc(readsize*sizeof(unsigned char));

    if ((pfile = OpenFileUser(filename, "rb")) == NULL)
    {
        printf("Can't open rom %s\n",filename);
        exit(ROM_FILE_ERROR);
    }
    fseek(pfile, seeknum, SEEK_SET); 
    fread(data,sizeof(*data), readsize, pfile);
    fclose(pfile);

    return data;
}

unsigned char* Get_ROM_MD5_Value(char* romImageName)
{
    int i;
    unsigned char *Value_MD5 = NULL;
    Value_MD5 = read_info_from_file(romImageName,FW_MD5_VALUE_OFFSET,FP_MD5_SIZE);

    printf("ROM MD5 Value: ");
	for ( i =0;i<FW_MD5_VALUE_SIZE;i++)
    {
	    printf("%2.2x ",  Value_MD5[i]);
	}
    printf("\n");

    return Value_MD5;
}

void Check_Rom_Device_Id(char* romImageName)
{
    unsigned short vbios_device_id;
    unsigned char *rom_device_id = NULL;

    rom_device_id = read_info_from_file(romImageName,FW_DEVICE_ID_OFFSET,FW_DEVICE_ID_SIZE);

    printf("Check Rom Device Id:\n");
    vbios_device_id = rom_device_id[1]*256+rom_device_id[0];
    if(vbios_device_id!=video_pci_prop.DeviceId)
    {
        printf("Device ID not match ROM FILE in offset 0x%x\n",FW_DEVICE_ID_OFFSET);
        printf("Check device id is 0x%4.4x\n",video_pci_prop.DeviceId);
        printf("Check rom device id is 0x%4.4x\n",vbios_device_id);
        exit(GENERAL_FILE_ERROR);
    }
    else
    {
        printf("Device ID match ROM FILE in offset 0x%x\n",FW_DEVICE_ID_OFFSET);
    }
	
}


int isMD5check(char* romImageName)
{
    unsigned char *fw_header_version = NULL;
    
    fw_header_version = read_info_from_file(romImageName,FW_HEADER_VERSION_OFFSET,FW_HEADER_VERSION_SIZE);
    printf("Header version is 0x%x\n", *fw_header_version);

    if (*fw_header_version == 0x02)
    {
        printf("Use MD5 Check Valification!\n");
        return TRUE;
    }

	return FALSE;
}

