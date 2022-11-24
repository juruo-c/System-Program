#ifndef _CLIENTINFO_
#define _CLIENTINFO_
#define USERNAME 100
#define PASSWORD 100
#define MASSAGE 400
#define MAXUSERNUM 400

typedef struct 
{
    char username[USERNAME];
    char password[PASSWORD];
}REGISTERINFO, *REGISTERINFOPTR;

typedef struct 
{
    char username[USERNAME];
    char password[PASSWORD];
}LOGININFO, *LOGININFOPTR;

typedef struct 
{
    char sendername[USERNAME];
	char receivername[USERNAME];
    char message[MASSAGE];
}MESSAGEINFO, *MESSAGEINFOPTR;

typedef struct 
{
	char username[USERNAME];
}LOGOUTINFO, *LOGOUTINFOPTR;

typedef struct 
{
	int usernum;
	int userstatus[MAXUSERNUM];
	char usernames[MAXUSERNUM][USERNAME];
}USERLIST, *USERLISTPTR;

#endif
