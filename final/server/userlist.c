#include "userlist.h"

int maxWrongTime = DEFAULT_MAX_WRONG_TIME;
int lockTime = DEFAULT_LOCK_TIME;
int maxUserNum = DEFAULT_MAXUSER_NUMBER;

userNode* createUserNode(char* name, char* pwd)
{
    userNode* newNode = (userNode*) malloc(sizeof(userNode));
    if (newNode == NULL) return NULL;
    do 
    {
        newNode->username = (char*) malloc(sizeof(char) * (strlen(name) + 1));
        newNode->password = (char*) malloc(sizeof(char) * (strlen(pwd) + 1));
        if (newNode->username == NULL || newNode->password == NULL) break;
        strncpy(newNode->username, name, strlen(name) + 1);
        strncpy(newNode->password, pwd, strlen(pwd) + 1);

        newNode->userfifo = (char*) malloc(sizeof(char) * (strlen(userFifoPre) + strlen(name) + 8 + 1));
        if (newNode->userfifo == NULL) break;
        sprintf(newNode->userfifo, "%s/client_%s", userFifoPre, name);
        
        newNode->status = newNode->failureTime = newNode->lockLogin = 0;

        int i;
        for (i = 0; i < DEFAULT_ALPHA_NUM; i ++ )
            newNode->next[i] = NULL;

        return newNode;
    }while(0);

    if (newNode->username) free(newNode->username), newNode->username = NULL;
    if (newNode->password) free(newNode->password), newNode->password = NULL;
    if (newNode->userfifo) free(newNode->userfifo), newNode->userfifo = NULL;
    if (newNode) free(newNode), newNode = NULL;

    return NULL;
}

void destroyUserNode(userNode* node)
{
    if (node == NULL) return;
    if (node->username) free(node->username), node->username = NULL;
    if (node->password) free(node->password), node->password = NULL;
    if (node->userfifo) free(node->userfifo), node->userfifo = NULL;
    if (node) free(node), node = NULL;
}

userNode* createNode()
{
    userNode* newNode = (userNode*) malloc(sizeof(userNode));
    if (newNode == NULL) return NULL;
    newNode->username = newNode->password = newNode->userfifo = NULL;
    int i;
    for (i = 0; i < DEFAULT_ALPHA_NUM; i ++ )
        newNode->next[i] = NULL;

    return newNode;
}

userList* createUserList(int max)
{
    userList* newList = (userList*) malloc(sizeof(userList));
    if (newList == NULL) return NULL;
    do
    {
        newList->userNum = 0;
        newList->maxUserNum = max;
        newList->onlineUserNum = 0;

        newList->rt = createNode();
        if (newList->rt == NULL) break;

        if (pthread_mutex_init(&newList->mutexList, NULL) ||
            pthread_mutex_init(&newList->mutexOnline, NULL)) 
            break;

        return newList;
    }while(0);

    if (newList->rt) destroyUserNode(newList->rt), newList->rt = NULL;
    free(newList), newList = NULL;

    return NULL;
}

void destroyUserList(userList* list)
{
    if (!list) return;
    destroyHelper(list->rt);
    pthread_mutex_destroy(&list->mutexList);
    pthread_mutex_destroy(&list->mutexOnline);
    free(list);
}

void destroyHelper(userNode* u)
{
    if (!u) return;
    int i;
    for (i = 0; i < DEFAULT_ALPHA_NUM; i ++ )
        if (u->next[i])
            destroyHelper(u->next[i]);
    destroyUserNode(u);
}

int addUserNode(char* name, char* pwd, userList* list)
{   
    Lock(&list->mutexList);
    userNode* u = list->rt;
    int i, len = strlen(name);
    for (i = 0; i < len - 1; i ++ )
    {
        int v = charToInt(name[i]);
        if (!u->next[v]) 
        {
            if ((u->next[v] = createNode()) == NULL)
            {
                Unlock(&list->mutexList);
                return -1;
            }
        }
        u = u->next[v];
    }

    // last letter
    int v = charToInt(name[i]);
    userNode* q = createUserNode(name, pwd);
    if (q == NULL) 
    {
        Unlock(&list->mutexList);
        return -1;
    }
    if (u->next[v]) 
    {
        for (i = 0; i < DEFAULT_ALPHA_NUM; i ++ )
            q->next[i] = u->next[v]->next[i];
    }
    destroyUserNode(u->next[v]);
    u->next[v] = q;  
        
    list->userNum ++;
    Unlock(&list->mutexList);

    return 0;
}

userNode* findUser(char* name, userList* list)
{
    Lock(&list->mutexList);
    userNode* u = list->rt;
    int i;
    for (i = 0; name[i]; i ++ )
    {
        int v = charToInt(name[i]);
        if (!u->next[v])
        {
            Unlock(&list->mutexList);
            return NULL;
        }
        u = u->next[v];
    }
    userNode* res = (u->username == NULL) ? NULL : u;
    Unlock(&list->mutexList);
    return res;
}

int changeUserStatus(userNode* u, userList* list)
{
    if (u == NULL || list == NULL) return -1;
    
    Lock(&list->mutexList);
    u->status = (u->status ? OFFLINE : ONLINE);
    Unlock(&list->mutexList);

    Lock(&list->mutexOnline);
    list->onlineUserNum += (u->status ? 1 : -1);
    Unlock(&list->mutexOnline);

    return 0;
}

int getOnlineUserNum(userList* list)
{
    if (list == NULL) return -1;
    Lock(&list->mutexOnline);
    int num = list->onlineUserNum;
    Unlock(&list->mutexOnline);
    return num;
}

int failToLogin(userNode* u, userList* list)
{
    if (u == NULL || list == NULL) return -1;
    Lock(&list->mutexList);
    u->failureTime ++;
    if (u->failureTime == maxWrongTime)
    {
        u->lockLogin = 1; // lock login right
        u->failureTime = 0;
    }
    time(&u->lastLoginTime);
    Unlock(&list->mutexList);
    return 0;
}

int getLoginLock(userNode* u, userList* list)
{
    if (u == NULL || list == NULL) return -1;
    Lock(&list->mutexList);
    int lock = u->lockLogin;
    Unlock(&list->mutexList);
    return lock; 
}

int getUserStatus(userNode* u, userList* list)
{
    if (u == NULL || list == NULL) return -1;
    Lock(&list->mutexList);
    int status = u->status;
    Unlock(&list->mutexList);
    return status; 
}

time_t getLoginTime(userNode* u, userList* list)
{
    if (u == NULL || list == NULL) return -1;
    Lock(&list->mutexList);
    time_t t = u->lastLoginTime;
    Unlock(&list->mutexList);
    return t;
}

int unlockLogin(userNode* u, userList* list)
{
    if (u == NULL || list == NULL) return -1;
    Lock(&list->mutexList);
    u->lockLogin = 0;
    Unlock(&list->mutexList);
    return 0;
}

void sendMesToAllOnlineUser(MESSAGEINFOPTR mes, userList* list)
{
    Lock(&list->mutexList);
    sendHelper(list->rt, mes);
    Unlock(&list->mutexList);
}

void sendHelper(userNode* u, MESSAGEINFOPTR mes)
{
    if (u == NULL) return;
    if (u->username && u->status == ONLINE) 
    {
        strncpy(mes->receivername, u->username, strlen(u->username) + 1);
        int fd = openFifoForWR(commonFifos[2]);
        write(fd, mes, sizeof(MESSAGEINFO));
        close(fd);
    }
    int i;
    for (i = 0; i < DEFAULT_ALPHA_NUM; i ++ )
        sendHelper(u->next[i], mes);
}

void getUserList(USERLISTPTR info, userList* list, char* username)
{
    Lock(&list->mutexList);
    userListHelper(list->rt, info, username);
    Unlock(&list->mutexList);
}

void userListHelper(userNode* u, USERLISTPTR info, char* username)
{
    if (u == NULL) return;
    if (u->username && strcmp(u->username, username))
    {
        strncpy(info->usernames[info->usernum], u->username, strlen(u->username) + 1);
        info->userstatus[info->usernum] = u->status;
		info->usernum ++;
    }
    int i;
    for (i = 0; i < DEFAULT_ALPHA_NUM; i ++ )
        userListHelper(u->next[i], info, username);
}