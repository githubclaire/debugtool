#include "def.h"
#include "share.h"


CTOOL_HISTORY_CMD     CToolHisCmd;


char*  CToolGetCmdFromHis(bool bUp)
{
    PCTOOL_HISTORY_CMD    pHisCmd = &CToolHisCmd;
    char*    pCmd = 0;
    if(pHisCmd->dwTotalCmd && bUp)
    {
        
        if(pHisCmd->dwCurCmd == INVALID_CMD_POINTER)
        {
            pHisCmd->dwCurCmd  = pHisCmd->dwTotalCmd - 1;
        }
        else if(pHisCmd->dwCurCmd > 0)
        {
            pHisCmd->dwCurCmd -= 1;
        }
        pCmd = pHisCmd->chCmdHistory[pHisCmd->dwCurCmd];
    }
    else if(pHisCmd->dwTotalCmd && !bUp)
    {
        if((pHisCmd->dwCurCmd < pHisCmd->dwTotalCmd - 1)
            && (pHisCmd->dwCurCmd != INVALID_CMD_POINTER))
        {
            pHisCmd->dwCurCmd += 1;
            pCmd = pHisCmd->chCmdHistory[pHisCmd->dwCurCmd];
        }
        else
        {
            pHisCmd->dwCurCmd = INVALID_CMD_POINTER;
        }
    }
    return pCmd;
}

void CToolResetHisCmdCur(void)
{
    PCTOOL_HISTORY_CMD    pHisCmd = &CToolHisCmd;
    pHisCmd->dwCurCmd = INVALID_CMD_POINTER;
}

char*  CToolGetLastCmd(void)
{
    PCTOOL_HISTORY_CMD    pHisCmd = &CToolHisCmd;
    char*    pCmd = NULL;

    if(pHisCmd->dwTotalCmd > 0)
    {
        pCmd = pHisCmd->chCmdHistory[pHisCmd->dwTotalCmd - 1];
    }
    return pCmd;
}


void  CToolAddCmdToHistory(char*  chCmd)
{
    PCTOOL_HISTORY_CMD    pHisCmd = &CToolHisCmd;
    if(pHisCmd->dwTotalCmd < HISTORY_STRING_NUM)
    {
        pHisCmd->chCmdHistory[pHisCmd->dwTotalCmd] = (char*)malloc(STRINGBUFFERSIZE * sizeof(char));
        if(pHisCmd->chCmdHistory[pHisCmd->dwTotalCmd] == NULL)
        {
            printf("Failed to allocate memory!\n");
            return;
        }
        memset(pHisCmd->chCmdHistory[pHisCmd->dwTotalCmd], 0, STRINGBUFFERSIZE* sizeof(char));
        strcpy(pHisCmd->chCmdHistory[pHisCmd->dwTotalCmd], chCmd);
        pHisCmd->dwTotalCmd += 1;
    }
    else
    {
        DWORD  i = 0;
        char*  chTemp = pHisCmd->chCmdHistory[0];
        for(i = 0; i < HISTORY_STRING_NUM - 1; i++)
        {
            pHisCmd->chCmdHistory[i] = pHisCmd->chCmdHistory[i+1];
        }
        strcpy(chTemp, chCmd);
        pHisCmd->chCmdHistory[HISTORY_STRING_NUM - 1] = chTemp;
    }
}

void CToolGetInput(char* buffer, unsigned int size)
{
    DWORD i, j;
    char ch = 0;
    //char*    chLastCmd = NULL;
    char*      chTemp    = NULL;
    bool      bInputValid = FALSE;
    bool      bUserInput = FALSE;

    j = i = 0;
    do{
        ch = getchar();
        if((ch != KEY_ENTER) && (ch != KEY_BACKSPACE))
        {
            if(ch == KEY_UP_DOWN_PRE)
            {
                mdelay(300);
                continue;
            }
            if(ch == KEY_UP)
            {
                chTemp = CToolGetCmdFromHis(TRUE);
                if(chTemp != NULL)
                {
                    bUserInput = FALSE;
                    if(strcmp(chTemp, buffer)!=0)
                    {
                        if(j != i)
                        {
                            printf("%s", &buffer[j]);
                        }
                        while(i > 0)
                        {
                            printf("%c", KEY_BACKSPACE);
                            printf("%c", KEY_SPACE);
                            printf("%c", KEY_BACKSPACE);
                            i--;
                        }
                        strcpy(buffer, chTemp);
                        i = strlen(chTemp);
                        printf("%s", chTemp);
                        fflush(stdout);
                        j = i;
                    }
                }
            }
            else if(ch == KEY_DOWN)
            {
                chTemp = CToolGetCmdFromHis(FALSE);
                if(chTemp != NULL)
                {
                    bUserInput = FALSE;
                    if(strcmp(chTemp, buffer)!=0)
                    {
                        if(j != i)
                        {
                            printf("%s", &buffer[j]);
                        }
                        while(i > 0)
                        {
                            printf("%c", KEY_BACKSPACE);
                            printf("%c", KEY_SPACE);
                            printf("%c", KEY_BACKSPACE);
                            i--;
                        }
                        strcpy(buffer, chTemp);
                        i = strlen(chTemp);
                        printf("%s", chTemp);
                        fflush(stdout);
                        j = i;
                    }
                }
                else
                {
                    if(!bUserInput)
                    {
                        if(j != i)
                        {
                            printf("%s", &buffer[j]);
                        }
                        while(i > 0)
                        {
                            printf("%c", KEY_BACKSPACE);
                            printf("%c", KEY_SPACE);
                            printf("%c", KEY_BACKSPACE);
                            i--;
                        }
                        fflush(stdout);
                        j = i;
                        buffer[0] = '\0';
                    }
                }
            }
            else if(ch == KEY_LEFT)
            {
                if(j > 0)
                {
                    printf("%c", KEY_BACKSPACE);
                    j--;
                }
                fflush(stdout);
            }
            else if(ch == KEY_RIGHT)
            {
                if(j < i)
                {
                    printf("%c", buffer[j]);
                    j++;
                }
                fflush(stdout);
            }
            else
            {
                DWORD  dwTemp = i;                
                CToolResetHisCmdCur();

                while(dwTemp > j)
                {
                    buffer[dwTemp] = buffer[dwTemp - 1];
                    dwTemp--;
                }
                buffer[j] = ch;
                buffer[++i] = '\0';
                j++;
                //printf("%s", &buffer[j++]);
                dwTemp = i - j;
                while(dwTemp > 0)
                {
                    printf("%c", KEY_BACKSPACE);
                    dwTemp--;
                }
                fflush(stdout);

                if(i > size - 1)
                {
                    printf("Input buffer overflow!\n");
                }
                bUserInput = TRUE;
            }
        }
        else if(ch == KEY_BACKSPACE)
        {
            if(j > 0)
            {
                DWORD  dwTemp = --j;
                while(dwTemp < i - 1)
                {
                    buffer[dwTemp] = buffer[dwTemp + 1];
                    dwTemp++;
                }
                buffer[--i] = '\0';
                printf("%c", KEY_BACKSPACE);
                printf("%s", &buffer[j]);
                printf("%c", KEY_SPACE);
                printf("%c", KEY_BACKSPACE);
                dwTemp = i - j;
                while(dwTemp > 0)
                {
                    printf("%c", KEY_BACKSPACE);
                    dwTemp--;
                }
                fflush(stdout);
            }
        }

    }while(ch != KEY_ENTER);
    buffer[i] = '\0';
    printf("\n");

    bInputValid = (strlen(buffer) == 0)?FALSE:TRUE;
    chTemp = CToolGetLastCmd();
    
    if(bInputValid && chTemp)
    {
        if(strcmp(chTemp, buffer) !=0)
        {
            CToolAddCmdToHistory(buffer);
        }
    }
    else if(bInputValid && !chTemp)
    {
        CToolAddCmdToHistory(buffer);
    }
    
    CToolResetHisCmdCur();

    return;
}

DWORD  CToolGetLastBit(DWORD  value)
{
    DWORD  result = 0;
    if(value)
    {
        result = value & ~(value -1);
    }
    return result;
}

void CToolWriteBufferUlong(volatile PULONG RegisterPort, ULONG Value)
{
    ULONG        ulAlignedMask = sizeof(ULONG) - 1;

    if((ULONG)RegisterPort & ulAlignedMask)
    {
        printf("The port address is not aligned!\n");
    }
	
    *RegisterPort = Value;
    return;
}

void EchoWait(void)
{
    printf(">");
    fflush(stdout);
}

void  CToolClearHisCmd(void)
{
    PCTOOL_HISTORY_CMD    pHisCmd = &CToolHisCmd;
    if(!pHisCmd->dwTotalCmd)
    {
        DWORD   i = 0;
        for(i = 0; i < pHisCmd->dwTotalCmd; i++)
        {
            free(pHisCmd->chCmdHistory[i]);
            pHisCmd->chCmdHistory[i] = NULL;
        }
        pHisCmd->dwTotalCmd = 0;
        pHisCmd->dwCurCmd = INVALID_CMD_POINTER;
    }
}

							 