#include "def.h"

extern VIDEO_PCI_PROP video_pci_prop;

FILE *OpenFileUser(char *szFname, char *szMode)
{
    return fopen(szFname, szMode);
}

long file_Size(char *filename)
{
	struct stat statbuf;
	stat(filename,&statbuf);
	long fileSize = statbuf.st_size;
	return fileSize;
}

int FlashRom(FILE *pStream, long dwFileSize)
{
    int i,j;
    unsigned char pBuffer[FP_SECTOR_SIZE];
    unsigned int readdata;
    unsigned long sectorNum;
    
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

        if((j*100/sectorNum)%10==0)
            printf("%d%%\n",(int)(j*100/sectorNum));
        //bWriteOk = SectorWriteSerialChip(&pChipInfo->fpInfo, pBuffer);
    }
    printf("%d%%\n",100);
    return 0;
}

void flash_vbios(char *romImageName)
{
    char *tempImage = "GFINIT.ROM";
    int is_md5_rom = 0;
    long rom_file_size;
    char *Ptr = NULL;
    char *Value_MD5 = (char *)malloc(FW_MD5_VALUE_SIZE*sizeof(char));
    FILE *file_rom = NULL;

    Ptr =(char *)malloc(MAX_VIDEO_ROM_SIZE*sizeof(char));
    if (Ptr==NULL)
    {
	    printf("ptr malloc is wrong");
        exit(GENERAL_FILE_ERROR);
    }

    printf("Check Rom Device Id:\n");
    Check_Rom_Device_Id(romImageName);

    is_md5_rom = isMD5check(romImageName);
    if (is_md5_rom)
    {
        Value_MD5 = Get_ROM_MD5_Value(romImageName);
        Generate_new_Rom_file_without_MD5(romImageName,tempImage);
        MD5_Caculate(tempImage,Ptr,Value_MD5);
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
        rom_file_size = file_Size(romImageName);
        printf("Program rom file size = %ld\n",rom_file_size);

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

int dump_vbios(char *romImageName) 
{
    unsigned char pBuffer[FP_SECTOR_SIZE];
    long i,j;
    unsigned long sectorNum;
    unsigned int dump_file_size;
    FILE *pStream;

    if ((pStream = OpenFileUser(romImageName, "wb+")) == NULL)
    {
        printf("Can't creat file %s\n",romImageName);
        return FALSE;
    } 

    //dump_file_size = sf_read_data(,);

    sectorNum = (MAX_VIDEO_ROM_SIZE / FP_SECTOR_SIZE);
    if(MAX_VIDEO_ROM_SIZE>sectorNum*FP_SECTOR_SIZE)
        sectorNum++;
    for(i=0;i<sectorNum;i++) 
    {
        for(j=0;j<FP_SECTOR_SIZE;j++)
        {
            pBuffer[j]=sf_read_data(i*FP_SECTOR_SIZE+j,1);
        }
        fwrite(pBuffer, sizeof(unsigned char), FP_SECTOR_SIZE, pStream);
        
    }
    fclose(pStream);
    printf("Dump rom from flash successfully!\n");
    return TRUE;
}  


void MD5_Caculate(char* romImage_Name2,char *Ptr,char *MD5Value)
{
    unsigned char decrypt[16]; 
    int i;
    MD5_CTX md5;

    MD5Init(&md5);
    Ptr = read_info_from_file(romImage_Name2,0x0,MAX_VIDEO_ROM_SIZE);
    //printf("\n%s",(char*)(Ptr + 0x24));
    MD5Update(&md5,Ptr,MAX_VIDEO_ROM_SIZE);
    MD5Final(&md5,decrypt);

    for(i=0;i<FW_MD5_VALUE_SIZE;i++)
    {
        //printf("decrypt:%02x\n",decrypt[i]);
	    //printf("md5:%2.2x\n",Value_MD5[i]);
        if (MD5Value[i]!=decrypt[i])
        {
            printf("MD5 Error: ROM file caculte not match MD5 !\n");
            exit(ROM_FILE_ERROR);
        }
    }
    printf("MD5 match: ROM file match MD5 !\n");
    
}

void Generate_new_Rom_file_without_MD5(char *romImageSrc,char* romImageDest)
{
    long i;
    char *pBuffer = (char*)malloc(sizeof(char));

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
    long filesize = file_Size(romImageSrc);

    for(i=0;i<filesize;i++) 
    {
        if(i>=FW_MD5_VALUE_OFFSET && i<FW_MD5_VALUE_OFFSET+FW_MD5_VALUE_SIZE)
        {
            *pBuffer = 0;
        }
        else
        {
            fread(pBuffer,sizeof(char), 1, file_src);
        }
        fwrite(pBuffer, sizeof(char), 1, file_dest);   
    }

    fclose(file_src);
    fclose(file_dest);
}

char* read_info_from_file(char *filename, unsigned int seeknum, unsigned int readsize)
{
    FILE *pfile = NULL;
    char *data = (char *)malloc(readsize*sizeof(char));

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

char* Get_ROM_MD5_Value(char* romImageName)
{
    char *Value_MD5 = malloc(FW_MD5_VALUE_SIZE*sizeof(char));
    Value_MD5 = read_info_from_file(romImageName,FW_MD5_VALUE_OFFSET,FP_MD5_SIZE);
    //printf("MD5 Value:\n");
	//for ( i =0;i<16;i++){
	//printf("%2.2x\n",  Value_MD5[i]);
	//}
    return Value_MD5;
}

void Check_Rom_Device_Id(char* romImageName)
{
    char *rom_device_id = (char *)malloc(FW_DEVICE_ID_SIZE*sizeof(char));

    rom_device_id = read_info_from_file(romImageName,FW_VENDOR_ID_OFFSET,FW_DEVICE_ID_SIZE);

    unsigned short vbios_device_id = rom_device_id[1]*256+rom_device_id[0];
    if(vbios_device_id!=video_pci_prop.DeviceId)
    {
        printf("Device ID not match ROM FILE in offset 0xF00A\n");
        printf("check device id is %4.4x\n",video_pci_prop.DeviceId);
        printf("check rom device id is %4.4x\n",vbios_device_id);
        exit(GENERAL_FILE_ERROR);
    }
    else
    {
        printf("DID match :Device ID match ROM FILE in offset 0xF00A\n");
    }
	
}


int isMD5check(char* romImageName)
{
    char *fw_header_version = (char*)malloc(sizeof(char));
    
    fw_header_version = read_info_from_file(romImageName,FW_CHECKSUM_OFFSET,FW_CHECKSUM_SIZE);
    if (*fw_header_version == 0x02)
    {
        printf("Use MD5 Check Valification!\n");
        return TRUE;
    }

	return FALSE;
}