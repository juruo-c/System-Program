#ifndef _CLIENTINFO_
#define _CLIENTINFO_

#define USERNAME 50
#define PASSWORD 50
#define MESSAGE 400
#define RESPONSE 100
#define MAXUSER 200

typedef struct
{
    char username[USERNAME];
    char password[PASSWORD];
}REGISTERINFO, *REGISTERINFOPTR;

typedef REGISTERINFO LOGININFO;
typedef REGISTERINFOPTR LOGININFOPTR;

typedef struct 
{
    char sendername[USERNAME];
    char receivername[USERNAME];
    char message[MESSAGE];
    int response;
}MESSAGEINFO, *MESSAGEINFOPTR;

typedef struct 
{
    char username[USERNAME];
}LOGOUTINFO, *LOGOUTINFOPTR;

typedef struct 
{
	int usernum;
	int userstatus[MAXUSER];
	char usernames[MAXUSER][USERNAME];
}USERLIST, *USERLISTPTR;

#endif