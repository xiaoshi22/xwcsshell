#include "shell.h"

/*
 * Signal - wrapper for the sigaction function 
 */
handler_t *Signal(int signum, handler_t *handler) 
{
    struct sigaction action, old_action;
    
    action.sa_handler = handler;  
    sigemptyset(&action.sa_mask); /* block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* restart syscalls if possible */
    
    if (sigaction(signum, &action, &old_action) < 0){
        perror("signalaction error");
        return NULL;
    }
    
    return (old_action.sa_handler);
}

/* 
 * sigchld_handler - sigchld handler 
 */
void sigchld_handler(int s){
    int status;
    sigset_t prev_all, mask_all;
    pid_t pid;
    sigfillset(&mask_all);
    while ((pid = waitpid(-1, &status, 0)) > 0) { /* Reap a zombie child */
        sigprocmask(SIG_BLOCK, &mask_all, &prev_all); /* block all signals */
        if (WIFEXITED(status)){
            deletejob(pid); /* Delete the child from the job list */
        }
        sigprocmask(SIG_SETMASK, &prev_all, NULL); /* unblock signals */
    }
}

/* 
 * sigint_handler - sigint handler 
 */
void sigint_handler(int s){
    sigset_t prev_all, mask_all;
    
    if (!fg_pid) return;
    
    sigfillset(&mask_all);
    
    sigprocmask(SIG_BLOCK, &mask_all, &prev_all); /* block all signals */
    if (kill(-fg_pid, SIGINT) < 0)
        perror("sigint_handler kill error");
    printf("Job %d terminated by signal: Interrupt\n", fg_pid);
    deletejob(fg_pid); /* delete jobs in our job list */
    sigprocmask(SIG_SETMASK, &prev_all, NULL); /* unblock signals */
}

/* 
 * sigtstp_handler - sigtstp handler 
 */
void sigtstp_handler(int s){
    sigset_t prev_all, mask_all;
    
    if (!fg_pid) return;
    
    sigfillset(&mask_all);
    
    sigprocmask(SIG_BLOCK, &mask_all, &prev_all); /* block all signals */
    if (kill(-fg_pid, SIGINT) < 0)
        perror("sigint_handler kill error");
    printf("Job %d stopped by signal: Stopped\n", fg_pid);
    
    /* delete the corresponding job first, then add a new one with the same info but state of 2 and bg of 1 */
    int index;
    pid_t pid_temp;
    int jid_temp; 
    char cmd_temp[MAXLINE];
    
    for (int i = 0; i<MAXJOBS; i++){
        if(job_list[i].pid == fg_pid){
            index = i;
            pid_temp = job_list[i].pid;
            jid_temp = job_list[i].jid; 
            strcpy(cmd_temp, job_list[i].cmd);
            break;
        }
    }
    
     
    deletejob(fg_pid); /* delete jobs in our job list */
    
    job_list[index].pid = pid_temp;
    job_list[index].jid = jid_temp;
    jid_list[jid_temp-1] = pid_temp;
    job_list[index].state = 2;
    job_list[index].bg = 1;
    strcpy(job_list[index].cmd, cmd_temp); 
    
    
    
    sigprocmask(SIG_SETMASK, &prev_all, NULL); /* unblock signals */
}