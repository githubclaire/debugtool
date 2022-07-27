#include "def.h"
#include "share.h"


CTOOL_HISTORY_CMD     CToolHisCmd;


char*  get_cmd_from_his(bool bUp)
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

void reset_his_cmd(void)
{
    PCTOOL_HISTORY_CMD    pHisCmd = &CToolHisCmd;
    pHisCmd->dwCurCmd = INVALID_CMD_POINTER;
}

char*  get_last_cmd(void)
{
    PCTOOL_HISTORY_CMD    pHisCmd = &CToolHisCmd;
    char*    pCmd = NULL;

    if(pHisCmd->dwTotalCmd > 0)
    {
        pCmd = pHisCmd->chCmdHistory[pHisCmd->dwTotalCmd - 1];
    }
    return pCmd;
}

void  add_cmd_to_history(char*  chCmd)
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

#ifdef __ubuntu__
//func getch definition
//linux: no func getch def
//dos: func getch def in <conio.h>
char getch(void)
{
    struct termios tmtemp,tm;
    char c;
    int fd=0;

    if(tcgetattr(fd,&tm) != 0) //get current terminal attribution, save to the struct tm
    {      
        return -1;
    }

    tmtemp=tm;
    cfmakeraw(&tmtemp);     //initial tetemp to the attribution in raw mode
    if(tcsetattr(fd,TCSANOW,&tmtemp) != 0) //set terminal attribution to raw mode
    {     
        return -1;
    }
    c=getchar();
    if(tcsetattr(fd,TCSANOW,&tm) != 0) //set terminal attrbution to original
    {      
        return 0;
    }
    return c;
}

void reg_get_comb_key(char* pch)
{
    //int comb_key = COMB_KEY_DOS_START;
    char ch = getch();
    *pch = '\0';
    
    //printf("%d",ch);
    if(ch == COMB_KEY_START)
    {
        char  ch1, ch2;
        ch1 = getch();
        ch2 = getch();
        //printf("ch1 is %d",ch1);
       // printf("ch2 is %d",ch2);

        if(ch1 == 91)
        {
            switch(ch2)
            {
            case  51:  //delete key  is 27 91 51 126
                ch1 = getc(stdin);
                *pch = REG_KEY_BACKSPACE;
                break;
            case  65:
                *pch = REG_KEY_UP;
                break;
            case 66:
                *pch = REG_KEY_DOWN;
                break;
            case 67:
                *pch = REG_KEY_RIGHT;
                break;
            case 68:
                *pch = REG_KEY_LEFT;
                break;
            default:
                break;
            }
        }
        else
        {
            *pch = REG_KEY_OTHER;
        }
    }
    else
    {
        *pch = ch;
    }
    //return  comb_key;
}
#endif

void CToolGetInput(char* buffer, int size)
{
    int  i, j;   // i is num of input char, j is current pos of cursor
    char  ch = 0;
    char*      chTemp    = NULL;
    int      bInputValid = 0;
    int      bUserInput = 0;

    j = i = 0;
    
    do{
        #ifdef __ubuntu__
        reg_get_comb_key(&ch);
        #endif
        #ifdef __dos__
        ch = getch();  
        //printf("%d",ch);   
        if(ch == COMB_KEY_DOS_START)
        {
            ch = getch();
            //printf("%d",ch);
        } 
        #endif
        //printf("%d",ch);
        if(ch != REG_KEY_ENTER)
        {
            if(ch == REG_KEY_UP)
            {
                chTemp = get_cmd_from_his(TRUE);
                //printf("1、cmd is %s\n",chTemp);
                /*
                for(int x=0;x<sizeof(chTemp);x++){
                      printf("chTemp[%d] is %d;\n",x,chTemp[x]);
                }
                */
                if(chTemp != NULL)
                {
                    bUserInput = FALSE;
                    //printf("2、cmd is %s\n",buffer);
                    /*
                    for(int y=0;y<sizeof(buffer);y++){
                      printf("buffer[%d] is %d;\n",y,buffer[y]);
                    }
                    printf("9、i is %d\n",i);
                    printf("9、j is %d\n",j);
                    */
                    if(strcmp(chTemp, buffer)!=0)
                    {
                        if(j != i)
                        {
                             printf("%s", &buffer[j]);
                        }

                        while(i > 0)
                        {
                            printf("%c", REG_KEY_BACKSPACE);
                            printf("%c", REG_KEY_SPACE);
                            printf("%c", REG_KEY_BACKSPACE);
                            //printf("i is %d\n",i);
                            i--;

                        }
                        
                        strcpy(buffer, chTemp);
                        i = strlen(chTemp);
                       // printf("8、i is %d\n",i);
                        printf("%s", chTemp);
                        fflush(stdout);
                        j = i;
                        
                    }
                    if((strcmp(chTemp, buffer)==0)&&(i==0)&&(j==0)){
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
                            printf("%c", REG_KEY_BACKSPACE);
                            printf("%c", REG_KEY_SPACE);
                            printf("%c", REG_KEY_BACKSPACE);
                            i--;
                        }
                        fflush(stdout);
                        j = i;
                        buffer[0] = '\0';
                    }
                }
            }
            else if(ch == REG_KEY_DOWN)
            {
                chTemp = get_cmd_from_his(FALSE);
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
                            printf("%c", REG_KEY_BACKSPACE);
                            printf("%c", REG_KEY_SPACE);
                            printf("%c", REG_KEY_BACKSPACE);
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
                            printf("%c", REG_KEY_BACKSPACE);
                            printf("%c", REG_KEY_SPACE);
                            printf("%c", REG_KEY_BACKSPACE);
                            i--;
                        }
                        fflush(stdout);
                        j = i;
                        buffer[0] = '\0';
                    }
                }
            }
            else if(ch == REG_KEY_LEFT)
            {
                if(j > 0)
                {
                    printf("%c", REG_KEY_BACKSPACE);
                    j--;
                }
                fflush(stdout);
            }
            else if(ch == REG_KEY_RIGHT)
            {
                if(j < i)
                {
                    printf("%c", buffer[j]);
                    j++;
                }
                fflush(stdout);
            }
            else if((ch == REG_KEY_BACKSPACE)||(ch == REG_KEY_DELETE))
            {
                if(j > 0)
                {
                    int  dwTemp = --j;
                    while(dwTemp < i - 1)
                    {

                        buffer[dwTemp] = buffer[dwTemp + 1];
                        dwTemp++;
                    }

                    buffer[--i] = '\0';
                    printf("%c", REG_KEY_BACKSPACE);
                    printf("%s", &buffer[j]);
                    printf("%c", REG_KEY_SPACE);
                    printf("%c", REG_KEY_BACKSPACE);
                    dwTemp = i - j;

                    while(dwTemp > 0)
                    {
                        printf("%c", REG_KEY_BACKSPACE);
                        dwTemp--;
                    }
                    fflush(stdout);
                }
            }
            else
            {
                int  dwTemp = i;                
                reset_his_cmd();

                while(dwTemp > j)
                {
                    buffer[dwTemp] = buffer[dwTemp - 1];
                    dwTemp--;
                }

                buffer[j] = ch;
                buffer[++i] = '\0';
                printf("%s", &buffer[j++]);
                dwTemp = i - j;

                while(dwTemp > 0)
                {
                    printf("%c", REG_KEY_BACKSPACE);
                    dwTemp--;
                }
                fflush(stdout);

                if(i >= size - 1)
                {
                    printf("Input buffer overflow!\n");
                    break;
                }

                bUserInput = TRUE;
            }  
        }        
    }while(ch != REG_KEY_ENTER);
    buffer[i] = '\0';
    printf("\n");

    bInputValid = (strlen(buffer) == 0)?FALSE:TRUE;
    chTemp = get_last_cmd();
    //printf("chtemp cmd is %s\n",chTemp);

    if(bInputValid && chTemp)
    {
        if(strcmp(chTemp, buffer) !=0)
        {
            add_cmd_to_history(buffer);
        }
    }
    else if(bInputValid && !chTemp)
    {
        add_cmd_to_history(buffer);
    }

//add total_cmd
/*
    for(i=0;i<CToolHisCmd.dwTotalCmd;i++)
    {
        printf("his cmd is %s\n", CToolHisCmd.chCmdHistory[i]);
    }
*/
    reset_his_cmd();
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

void  clear_his_cmd(void)
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





