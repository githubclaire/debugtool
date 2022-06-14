#ifndef __SHARE_H__
#define __SHARE_H__

#define KEY_ESC                     (0x1B)
#define KEY_BACKSPACE               (0x08)
#define KEY_ENTER                   (0x0A)
#define KEY_BOTTOMLINE              (95)
#define KEY_SPACE                   (32)
#define KEY_UP_DOWN_PRE   (0)
#define KEY_UP                        (72)
#define KEY_DOWN                  (80)
#define KEY_LEFT                     (75)
#define KEY_RIGHT                  (77)
#define  HISTORY_STRING_NUM     10

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

typedef struct _CTOOL_HISTORY_CMD
{
    char*                chCmdHistory[HISTORY_STRING_NUM];
    DWORD              dwTotalCmd;
    DWORD              dwCurCmd;
}CTOOL_HISTORY_CMD, *PCTOOL_HISTORY_CMD;

void EchoWait(void);
void  CToolClearHisCmd(void);
void CToolGetInput(char* buffer, unsigned int size);
#endif


