#include "def.h"

unsigned char Value_MD5[20] = {0};
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
    bool bWriteOk= true;
    unsigned char pBuffer[FP_SECTOR_SIZE];
    unsigned int readdata;
    unsigned long sectorNum;
    
    int erase_pmp_index=0;
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
            printf("%d%%\n",(j*100/sectorNum));
        //bWriteOk = SectorWriteSerialChip(&pChipInfo->fpInfo, pBuffer);
    }
    printf("%d%%\n",100);
    return 0;
}

int flash_vbios(char *romImageName)
{
    unsigned char romImage_Name2[128];
    long fileSize;
    FILE *pStream = NULL;
    char *Ptr =NULL;
    unsigned int checksum_flag=0;
    Ptr =(char *)malloc(MAX_VIDEO_ROM_SIZE*sizeof(char));
    if (Ptr==NULL)
    {
	 printf("ptr malloc is wrong");
        exit(GENERAL_FILE_ERROR);
    }

    if ((pStream = OpenFileUser(romImageName, "rb+")) != NULL)
	{
		fileSize = file_Size(romImageName);
		printf("open file success\n");
		printf("file size = %ld\n",fileSize);
		printf("check device id:\n");
		Check_Did(romImageName);
        //1:find firmware MD5 for 0XF0B4;pmpbuff is MD5;
        //check 0Xf003==0X02;0X02 is for MD5;
        printf("check MD5:\n");
        checksum_flag=checksum(romImageName);   
        if (checksum_flag==1)
		{
            FM_md5_Info(romImageName);
            //2:change  MD5 =>0 in init file rom;
            strcpy(romImage_Name2,"GFINIT.ROM");
            Change_Rom(romImageName,romImage_Name2);
            //3:MD5 caculate;
            MD5_Caculate(romImage_Name2,Ptr);
    	}
		else
		{
       		printf("use checksum verification\n");
    	}
		if(FlashRom(pStream,fileSize)==0)
        {
            printf("Flash successfully!\n");
        }
        else
        {
            printf("Flash failed\n");
        }
	}
	else
	{
		printf("open file failed! \n");
	}

	fclose(pStream);

    return 0;
}

int dump_vbios(char *romImageName) 
{

    unsigned char pBuffer[FP_SECTOR_SIZE];
    long i,j;
    unsigned long sectorNum;
    FILE *pStream;

    if ((pStream = OpenFileUser(romImageName, "wb+")) == NULL)
    {
        printf("Can't creat file %s\n",romImageName);
        return 0;
    } 

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
}  



int MD5_Caculate(char* romImage_Name2,char *Ptr)
{
       long fileSize;
	 FILE *pStream_M = NULL;
	 unsigned char *pmpBuf_checksum[1];
	 *pmpBuf_checksum = NULL;
	 unsigned char decrypt[16]; 
	 int i;
	 MD5_CTX md5;
        MD5Init(&md5); 
	 //fileSize = GetFileSizeUser(romImageName);
       if ((pStream_M = OpenFileUser(romImage_Name2, "rb")) == NULL)
      {
        printf("Can't open rom %s\n",romImage_Name2);
        exit(ROM_FILE_ERROR);
      }
    fseek(pStream_M , 0x0, SEEK_SET); 
    fread(Ptr,sizeof(unsigned char), MAX_VIDEO_ROM_SIZE, pStream_M );
    //printf("\n%s",(char*)(Ptr + 0x24));
    MD5Update(&md5,Ptr,MAX_VIDEO_ROM_SIZE);
    MD5Final(&md5,decrypt);
    fclose(pStream_M);
    for(i=0;i<16;i++)
    {
        //printf("decrypt:%02x\n",decrypt[i]);
	//printf("md5:%2.2x\n",Value_MD5[i]);
	 if (Value_MD5[i]!=decrypt[i]){
           printf("MD5 Error: ROM file caculte not match MD5 !\n");
	     exit(ROM_FILE_ERROR);
        }
    }
    printf("MD5 match: ROM file match MD5 !\n");
    
}

int Change_Rom(char *romImageName1,char* romImageName2)
{
    unsigned char pBuffer[FP_SECTOR_SIZE];
    unsigned char pBuffer_md5[16]={0};
   // unsigned char pBuffer_all[MAX_VIDEO_ROM_SIZE];
    long i,j;
    unsigned long sectorNum;
    FILE *pStream_w;
    FILE *pStream_2;

    if ((pStream_w = OpenFileUser(romImageName2, "wb+")) == NULL)
    {
        printf("Can't creat file %s\n",romImageName2);
        exit(GENERAL_FILE_ERROR);
    } 
    if ((pStream_2 = OpenFileUser(romImageName1, "rb")) == NULL)
    {
        printf("Can't creat file %s\n",romImageName1);
        exit(GENERAL_FILE_ERROR);
    } 

    sectorNum = (MAX_VIDEO_ROM_SIZE / FP_SECTOR_SIZE);
    if(MAX_VIDEO_ROM_SIZE>sectorNum*FP_SECTOR_SIZE)
        sectorNum++;
    for(i=0;i<sectorNum;i++) 
    {
        fseek(pStream_2, i*FP_SECTOR_SIZE, SEEK_SET); 
	  fread(pBuffer,sizeof(unsigned char), FP_SECTOR_SIZE, pStream_2);
	  /*
        for(j=0;j<FP_SECTOR_SIZE;j++)
        {
            //pBuffer[j]=sf_read_data(i*FP_SECTOR_SIZE+j,1);
             
            
		strcpy(pBuffer_all,pBuffer);
        }
        */
        fseek(pStream_w, i*FP_SECTOR_SIZE, SEEK_SET);
        fwrite(pBuffer, sizeof(unsigned char), FP_SECTOR_SIZE, pStream_w);   
    }
    fseek(pStream_w, 0XF0B4, SEEK_SET); 
    fwrite(pBuffer_md5, sizeof(unsigned char), 16, pStream_w);   
    fclose(pStream_2);
    fclose(pStream_w);
}

int FM_md5_Info(char* romImageName)
{
	//int i;
	FILE *pStream_MD5INIF = NULL;
	unsigned char *pmpBuf[FP_MD5_SIZE];
	*pmpBuf = NULL;
	 if ((pStream_MD5INIF = OpenFileUser(romImageName, "rb")) == NULL)
      {
        printf("Can't open rom %s\n",romImageName);
        exit(ROM_FILE_ERROR);
       }
	fseek(pStream_MD5INIF, 0XF0B4, SEEK_SET); 
       fread(pmpBuf,sizeof(unsigned char), FP_MD5_SIZE, pStream_MD5INIF);
	strcpy(Value_MD5, pmpBuf);
	//printf("MD5 Value:\n");
	//for ( i =0;i<16;i++){
	    //printf("%2.2x\n",  Value_MD5[i]);
	//}
	fclose(pStream_MD5INIF);
}

int Check_Did(char* romImageName)
{
       long fileSize;
	 FILE *pStream = NULL;
 	 unsigned char *pmpBuf_checkDid[2];
	 *pmpBuf_checkDid = NULL;
	 fileSize = file_Size(romImageName);
       if ((pStream = OpenFileUser(romImageName, "rb")) == NULL)
      {
          printf("Can't open rom %s\n",romImageName);
          exit(ROM_FILE_ERROR);
       }
	fseek(pStream, 0XF00a, SEEK_SET); 
	fread(pmpBuf_checkDid,sizeof(unsigned char), 2, pStream);
       if(pmpBuf_checkDid[0]!=video_pci_prop.DeviceId)
	 {
            printf("Device ID not match ROM FILE in offset 0xf00a\n");
            printf("check device id is %4.4x\n",video_pci_prop.DeviceId);
            printf("check rom device id is %8.8x\n",pmpBuf_checkDid[0]);
            exit(GENERAL_FILE_ERROR);
	 }
	 else
	 {
            printf("DID match :Device ID match ROM FILE in offset 0xf00a\n");
        }
	
	fclose(pStream);
}


int checksum(char* romImageName)
{
       long fileSize;
	 FILE *pStream = NULL;
	 unsigned char *pmpBuf_checksum[1];
	 *pmpBuf_checksum = NULL;
	 char buildTime[1];
	 unsigned int checksum_flag=0;
	 fileSize = file_Size(romImageName);
       if ((pStream = OpenFileUser(romImageName, "rb")) == NULL)
      {
        printf("Can't open rom %s\n",romImageName);
        exit(ROM_FILE_ERROR);
       }
	fseek(pStream, 0XF002, SEEK_SET); 
       fread(pmpBuf_checksum,sizeof(unsigned char), 1, pStream);
	strcpy(buildTime, pmpBuf_checksum);
       if ( buildTime[0]!= 0x02)
	 {
		printf("NO md5!");
		
		//exit(GENERAL_FILE_ERROR);
	 }
	 else
	 {
            checksum_flag=1;
	 }
	fclose(pStream);
	return checksum_flag;
}