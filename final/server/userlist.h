#ifndef _USERLIST_
#define _USERLIST_

#include <pthread.h>
#include <string.h>
#include "../common/fifo.h"
#include "../common/utils.h"
#include "../common/clientinfo.h"
#include "log.h"

#define DEFAULT_USERNAME_LENGTH 25
#define DEFAULT_PASSWORD_LENGTH 25
#define DEFAULT_ALPHA_NUM 37 
// username can only contain little letters and numbers
#define DEFAULT_MAXUSER_NUMBER 200
#define ONLINE 1
#define OFFLINE 0
#define DEFAULT_MAX_WRONG_TIME 4
#define DEFAULT_LOCK_TIME 10

/*
    the max wrong time of user's login
    the lock time when user's login time more than maxWrongTime
*/
extern int maxWrongTime;
extern int lockTime;
extern int maxUserNum;

typedef struct userNode
{
    char* username;
    char* password;
    char* userfifo;
    int status;
    int failureTime; // the times of login failure
    time_t lastLoginTime; // the system time of last Login
    int lockLogin;
    struct userNode* next[DEFAULT_ALPHA_NUM];
}userNode;

/*
    create a new userNode with name and pwd
    return value:
        the pointer to the new Node
*/
userNode* createUserNode(char* name, char* pwd);

/*
    create a new Node with nothing
*/
userNode* createNode();

/*
    destroy a userNode
*/
userNode* destroyNode(userNode* node);

typedef struct 
{
    int userNum;
    int maxUserNum;
    int onlineUserNum;
    userNode* rt;
    pthread_mutex_t mutexList;
    pthread_mutex_t mutexOnline;
}userList; 

/*
    create a new userList with max user Number
*/
userList* createUserList(int max);

/*
    destroy a userList (with helper function)
*/
void destroyUserList(userList* u);
void destroyHelper(userNode* u);

/*
    add userNode (with name and pwd) to userList (list)
    return value:
        0: success
        -1: failure
*/
int addUserNode(char* name, char* pwd, userList* list);

/*
    find if there has user which name is the given name
    in the given user list
    return value:
        not found: NULL
        found: the specific userNode pointer
*/
userNode* findUser(char* name, userList* list);

/*
    change user's online status 
    return value:
        0: success
        -1: failure 
*/
int changeUserStatus(userNode* u, userList* list);

/*
    get the number of users who are online
*/
int getOnlineUserNum(userList* list);

/*
    after user failing to login
    return values: 
        0: success
        -1: failure
*/
int failToLogin(userNode* u, userList* list);

/*
    get login lock status
    return values:
        0: unlocked
        1: locked
*/
int getLoginLock(userNode* u, userList* list);

/* get login status */
int getUserStatus(userNode* u, userList* list);

/*
    get last login failed time
*/
time_t getLoginTime(userNode* u, userList* list);

/*
    unlock login lock
*/
int unlockLogin(userNode* u, userList* list);

/*
    send message to all online users
*/
void sendMesToAllOnlineUser(MESSAGEINFOPTR mes, userList* list);
void sendHelper(userNode* u, MESSAGEINFOPTR mes);

/*
    get all user information in the userlist except the specific user
*/
void getUserList(USERLISTPTR info, userList* list, char* username);
void userListHelper(userNode* u, USERLISTPTR info, char* username);
#endif 