#include "daemon.h"

int createDaemon()
{
    int pid;
    int i;

    // ignore terminal I/O signal, stop signal
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);

    // fork son process and stop parent process
    pid = fork();
    if (pid > 0) 
        exit(0);
    else if (pid < 0)
        return -1;
    
    // set up a new process group, let son process be the new group leader
    setsid();

    // fork a new son process, make sure this process is not the leader 
    pid = fork();
    if (pid > 0)
        exit(0);
    else if(pid < 0) 
        return -1;
    
    // close all file descriptor from parent
    for (i = 0; i < NOFILE; close(i ++));
    
    // change working folder
    chdir("/");

    // set the file creation mask to 0
    umask(0);

    return 0;
}