/************************************************************/
/*  Project   : HDCP E-Fuse Read/Write for CGU001   */
/*  Author    : CHEERYCHEN               */
/*  Date    : 12, 7, 2020              */
/*  Rev.    : 1.0.0.0                 */
/*  Compiler  : Watcom (DOS4GW)             */
/************************************************************/
typedef unsigned int       CBIOS_U32;
typedef union _REG_MM33688    //MM3688
{
    CBIOS_U32    Value;
    struct
    {
        CBIOS_U32    RESERVED_0    :26;
                CBIOS_U32    START_FLAG   :1;
                CBIOS_U32    RESERVED_1    :5;
    };
}REG_MM33688;
typedef union _REG_MM332DC    //MM32DC
{
    CBIOS_U32    Value;
    struct
    {
        CBIOS_U32    PGM_EFUSE_ADDR    :12;
                CBIOS_U32    RESERVED    :18;
                CBIOS_U32    PGM_EFUSE_FINISH    :1;
                CBIOS_U32    PGM_EFUSE_REQ    :1;       
    };
}REG_MM332DC;
typedef union _REG_MM82C0    //MM82C0
{
    CBIOS_U32    Value;
    struct
    {
                CBIOS_U32    RESERVED_0    :25;
                CBIOS_U32    EFUSE_READ_REQUEST    :1;
                CBIOS_U32    RESERVED_1    :2;
                CBIOS_U32    EFUSE_MODE    :3;
                CBIOS_U32    RESERVED_2    :1;
    };
}REG_MM82C0;
typedef union _REG_MM82B8    //MM82B8
{
    CBIOS_U32    Value;
    struct
    {
                CBIOS_U32    RESERVED_0    :16;
                CBIOS_U32    EFUSE_READ_ADDRESS    :7;
                CBIOS_U32    RESERVED_1    :9;
    };
}REG_MM82B8;
typedef union _REG_MM82BC    //MM82BC
{
    CBIOS_U32    Value;
    struct
    {
                CBIOS_U32    EFUSE_READ_DATA    :32;
    };
}REG_MM82BC;
typedef union _REG_MM32C8    //MM32C8
{
    CBIOS_U32    Value;
    struct
    {
                CBIOS_U32    DRV_EFUSE_R_DATA0   :32;
    };
}REG_MM32C8;
typedef union _REG_MM32CC    //MM32CC
{
    CBIOS_U32    Value;
    struct
    {
                CBIOS_U32    DRV_EFUSE_R_DATA1    :32;
    };
}REG_MM32CC;
typedef union _REG_MM32D0    //MM32D0
{
    CBIOS_U32    Value;
    struct
    {
                CBIOS_U32    DRV_EFUSE_R_DATA2    :32;
    };
}REG_MM32D0;
typedef union _REG_MM32D4    //MM32D4
{
    CBIOS_U32    Value;
    struct
    {
                CBIOS_U32    DRV_EFUSE_R_DATA3    :32;
    };
}REG_MM32D4;
typedef union _REG_MM32D8    //MM32D8
{
    CBIOS_U32    Value;
    struct
    {
                CBIOS_U32    DRV_EFUSE_RF    :8;
                CBIOS_U32    DRV_EFUSE_R_READ_ST    :2;
                CBIOS_U32    RESERVED    :22;
    };
}REG_MM32D8;

int efuse_test(int argc, char *argv[]);
void HdcpEfuseProgramOneBit(unsigned int bit_addr);
void HdcpEfuseProgramOneByte(unsigned int byte_addr, unsigned char indata);
void HdcpEfuseProgramOneDWord(unsigned int dword_addr, unsigned int indata);
void HdcpEfuseReadssiddDWord(unsigned int data);
void HdcpEfuseReadWCDWord(unsigned int data);
void HdcpEfuseReadLWord(unsigned int data1,unsigned int data);
int HdcpEfuseReadOneDWord(unsigned int dwaddr);
void HdcpEfuseBlankCheck();
void HdcpEfuseReadAll();
void HdcpEfuseProgramAll();















