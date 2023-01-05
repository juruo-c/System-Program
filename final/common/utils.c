#include "utils.h"

int getLine(int fd, char* buf, int n)
{
    int end = 0; // if meet file end
    int i, res;
    char* tmp = (char*) malloc(sizeof(char));
    for (i = 0; i < n - 1; i ++ )
    {
        res = read(fd, tmp, 1);
        if (!res)
        {
            end = 1;
            break;
        }
        if (res == -1)
        {
            perror("failed to read file in getLine function");
            return -1;
        }
        buf[i] = *tmp;
        if (buf[i] == '\n') break;
    }
    buf[i] = '\0';
    free(tmp);
    tmp = NULL;

    if (i == n - 1 && buf[i - 1] != '\n')
    {
        printf("the line is so long that buf can not available\n");
        return -1;
    }

    return end;
}

int charToInt(char ch)
{
    if (ch <= 'z' && ch >= 'a') return ch - 'a';
    else if(ch == '_') return 36;
    else return ch - '0' + 26;
}

void Lock(pthread_mutex_t* m)
{
    if (pthread_mutex_lock(m))
    {
        char info[DEFAULT_INFO_LENGTH] = "[ERROR][Failed to lock mutex]";
        SysLog(info);
        pthread_exit(NULL);
    }
}

void Unlock(pthread_mutex_t* m)
{
    if (pthread_mutex_unlock(m))
    {
        char info[DEFAULT_INFO_LENGTH] = "[ERROR][Failed to unlock mutex]";
        SysLog(info);
        pthread_exit(NULL);
    }
}