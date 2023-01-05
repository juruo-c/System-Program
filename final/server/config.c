#include "config.h"

int copyFileName(char* buf, enum FIFO fifoType)
{
    int len = strlen(buf);
    if (len + 1 > DEFAULT_FIFO_LENGTH)
    {
        printf("failed to config commonFifos[%d]: name's length is too long\n", fifoType);
        return -1;
    }
    strncpy(commonFifos[fifoType], buf, len + 1);
    return 0;
}

int checkIfAllDigit(char* buf)
{
    int i;
    for (i = 0; buf[i]; i ++ )
        if (buf[i] < '0' || buf[i] > '9') 
            return 0;
    return 1;
}

int parseLine(char* line)
{
    // line contain one key and one value
    int len = strlen(line);
    int i = 0, j;
    char key[BUFSIZE],  value[BUFSIZE];
    int keyOk = 0;   // if line contains key
    int valueOk = 0; // if line contains value
    while (i < len)
    {
        if (line[i] == '#') break;
        if (line[i] == ' ' || line[i] == '\t') 
        {
            i ++;
            continue;
        }
        
        // parse a token
        j = i;
        while (j + 1 < len && line[j + 1] != ' ' && line[j + 1] != '\t')
            j ++;
        
        // has a token from i ~ j
        if (!keyOk)
        {
            strncpy(key, line + i, j - i + 1);
            key[j - i + 1] = 0;
            keyOk = 1;
        }
        else if (!valueOk)
        {
            strncpy(value, line + i, j - i + 1);
            value[j - i + 1] = 0;
            valueOk = 1;
        }
        else 
        {
            printf("there are three tokens in one line in configuration file\n");
            return -1;
        }

        i = j + 1;
    }

    if (keyOk == 0 && valueOk == 0) return 0;

    if (keyOk && valueOk == 0)
    {
        printf("there is only key but not value in one line\n");
        return -1;
    }
    // change some variable according to key and value
    enum FIFO fifoType;
    if (strcmp(key, "REG_FIFO") == 0)
    {
        fifoType = REGISTER;
        if (copyFileName(value, fifoType)) 
            return -1;
    }
    else if(strcmp(key, "LOGIN_FIFO") == 0)
    {
        fifoType = LOGIN;
        if (copyFileName(value, fifoType))
            return -1;
    }
    else if(strcmp(key, "MSG_FIFO") == 0)
    {
        fifoType = MSG;
        if (copyFileName(value, fifoType))
            return -1;
    }
    else if(strcmp(key, "LOGOUT_FIFO") == 0)
    {
        fifoType = LOGOUT;
        if (copyFileName(value, fifoType))
            return -1;
    }
    else if(strcmp(key, "USERINFO_FIFO") == 0)
    {
        fifoType = USERINFO;
        if (copyFileName(value, fifoType))
            return -1;
    }
    else if(strcmp(key, "LOG_PATH") == 0)
    {
        int len = strlen(value) + 1;
        if (len + USERNAME / 2 > DEFAULT_LOGPATH_LENGTH)
        {
            printf("failed to config logPath: the name's length is too long\n");
            return -1;
        }
        strncpy(logPath, value, len);
        sprintf(sysLogPath, "%s/%s", logPath, "sys.log");
    }
    else if(strcmp(key, "MAX_WRONG_TIME") == 0)
    {
        if (checkIfAllDigit(value) == 0)
        {
            printf("MAX_WRONG_TIME in thse configuration file is not digit\n");
            return -1;
        }
        maxWrongTime = atoi(value);
    }
    else if(strcmp(key, "LOCK_TIME") == 0)
    {
        if (checkIfAllDigit(value) == 0)
        {
            printf("LOCK_TIME in the configuration file is not digit\n");
            return -1;
        }
        lockTime = atoi(value);
    }
    else if(strcmp(key, "USERFIFO_PRE") == 0)
    {
        int len = strlen(value) + 1;
        if (len + USERNAME / 2 > DEFAULT_LOGPATH_LENGTH)
        {
            printf("failed to config userFifoPre: the name's length is too long\n");
            return -1;
        }
        strncpy(userFifoPre, value, len);
    }
    else if(strcmp(key, "MAX_USER_NUM") == 0)
    {
        if (checkIfAllDigit(value) == 0)
        {
            printf("MAX_USER_NUM in the configuration file is not digit\n");
            return -1;
        }
        maxUserNum = atoi(value);
    }
    else if(strcmp(key, "MIN_THREAD_NUM") == 0)
    {
        if (checkIfAllDigit(value) == 0)
        {
            printf("MIN_THREAD_NUM in the configuration file is not digit\n");
            return -1;
        }
        minThreadNum = atoi(value);
    }
    else if(strcmp(key, "MAX_THREAD_NUM") == 0)
    {
        if (checkIfAllDigit(value) == 0)
        {
            printf("MAX_THREAD_NUM in the configuration file is not digit\n");
            return -1;
        }
        maxThreadNum = atoi(value);
    }
    else 
    {
        printf("the token %s is wrong!", key);
        return -1;
    }

    return 0;
}

void config()
{
    int fd = open(CONFIG_FILE_PATH, O_RDONLY);
    if (fd == -1)
    {
        perror("failed to open configuration file for reading only");
        exit(1);
    }

    int res;
    char buf[BUFSIZE];
    int failed = 0; // 1: error 0: success
    while (1)
    {
        // get line from file
        res = getLine(fd, buf, BUFSIZE);
        if(res == -1)
        {
            failed = 1;
            break;
        }

        // parse line
        if (parseLine(buf))
        {
            failed = 1;
            break;
        }
        if (res == 1) break;
    }

    // check if the configuration file contains all fifos' path
    int i;
    for (i = 0; i < DEFAULT_FIFO_NUMBER; i ++ )
        if (commonFifos == NULL)
        {
            printf("fifos' paths in the configuration file are not complete\n");
            failed = 1;
            break;
        }

    // failed 
    if (failed)
        exit(1);

    // read configuration file successfully
    // puts("success");
    // for (i = 0; i < DEFAULT_FIFO_NUMBER; i ++ )
    //     printf("%s\n", commonFifos[i]);
    // printf("%s\n", logPath);
    // printf("maxWrongTime = %d\n", maxWrongTime);
    // printf("lockTime = %d\n", lockTime);
    // printf("userFifoPre = %s\n", userFifoPre);

}