#ifndef __SHARE_H__
#define __SHARE_H__

#define  HISTORY_STRING_NUM     10

#define   REG_KEY_ENTER      0x0D
#define   REG_KEY_BACKSPACE  0x08
#define   REG_KEY_DELETE     0x7F
#define   REG_KEY_SPACE      0x20
#define   REG_KEY_TAB        0x09
#define   COMB_KEY_START     0x1B
#define   REG_KEY_UP         0x48
#define   REG_KEY_DOWN       0x50
#define   REG_KEY_RIGHT      0x4D
#define   REG_KEY_LEFT       0x4B
#define   REG_KEY_OTHER      0x1
#define   COMB_KEY_DOS_START 0x0

#define   INVALID_CMD_POINTER   (0xFFFFFFFF)

#define LENGTHPERSTRING 128
#define NUMBEROFSTRING   5

#define  GENERALCMDNUM          50
#define  GENERALCMDLENGTH     8

typedef unsigned char UCHAR, *PUCHAR;
typedef unsigned long ULONG, *PULONG;
typedef unsigned long       DWORD, *PDWORD;

#define CTOOL_REGISTER_MEMORY_READ           0x56
#define CTOOL_REGISTER_MEMORY_WRITE          0x57
#define CTOOL_REGISTER_INVALIDINPUT          0xFF
#define CTOOL_REGISTER_BACKTOUPPERLEVEL      0xFE
#define CTOOL_REGISTER_DUMPREGISTER          0xFD
#define CTOOL_REGISTER_MMIOREGISTER          0x70
#define CTOOL_REGISTER_LOADFILE              0x60
#define CTOOL_REGISTER_SAVEFILE              0x61
#define CTOOL_REGISTER_PMUREGISTER           0x80
#define CTOOL_REGISTER_I2C_READWRITE         0x81
#define CTOOL_REGISTER_CLK_READWRITE         0x82
#define CTOOL_REGISTER_AUX_READWRITE         0x83

#define ECHOFLAGS (ECHO | ECHOE | ECHOK | ECHONL)
typedef struct _CTOOL_HISTORY_CMD
{
    char*                chCmdHistory[HISTORY_STRING_NUM];
    DWORD              dwTotalCmd;
    DWORD              dwCurCmd;
}CTOOL_HISTORY_CMD, *PCTOOL_HISTORY_CMD;

void EchoWait(void);
void  clear_his_cmd(void);
void CToolGetInput(char* buffer, int size);
#endif


