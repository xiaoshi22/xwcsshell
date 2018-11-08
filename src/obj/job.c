#include "shell.h"

/* 
 * addemptyjob - dd an empty job 
 */
void addemptyjob(struct job *j){
    /* set all values to be 0 */
    j -> pid = 0;
    j -> jid = 0;
    j -> cmd[0] = '\0';
    j -> state = 0;
    j -> bg = 0;
    j -> suser = 0;
    j -> euser = 0;
    j -> ssys = 0;
    j -> esys = 0;
    j -> suseru = 0;
    j -> euseru = 0;
    j -> ssysu = 0;
    j -> esysu = 0;
    j -> sma = 0;
    j -> ema = 0;
    j -> snv = 0;
    j -> env = 0;
    j -> sniv = 0;
    j -> eniv = 0;
}

/*
 * initalize the jobs list
 */
void initjobs(void){
    for (int i = 0; i < MAXJOBS; i++){
        addemptyjob(&job_list[i]);
    }
}

/* 
 * addjob - add a given job 
 */
int addjob(pid_t pid, char* cmd, int bg, time_t s1, time_t s2, suseconds_t s3, suseconds_t s4, long int s5, long int s6, long int s7){
    if (!bg){
        fg_pid = pid; /* update the fg pid */
    }
    
    /* add this new job in the first place in our job list */
    for (int i = 0; i < MAXJOBS; i++){
        if (job_list[i].pid == 0){
            for (int j = 0; j < MAXJOBS; j++){
                if (jid_list[j] == 0){
                    job_list[i].jid = j+1;
                    jid_list[j] = pid;
                    break;
                }
            }
            job_list[i].state = 1;
            job_list[i].bg = bg;
            job_list[i].pid = pid;
            strcpy(job_list[i].cmd, cmd);
            job_list[i].suser = s1;
            job_list[i].ssys = s2;
            job_list[i].suseru = s3;
            job_list[i].ssysu = s4;
            job_list[i].sma = s5;
            job_list[i].snv = s6;
            job_list[i].sniv = s7;
            return i;
        }
    } 
    perror("Fail in the for loop for addjob.\n");
    return -1;
}

/* 
 * deletejob - delete a job with a given pid 
 */
void deletejob(pid_t pid){
    for (int i = 0; i < MAXJOBS; i++) {
        if (job_list[i].pid == pid) {
            
            //before delete the job, calculate its resource usage
            //grab the resourcee usage after process execute
            if(getrusage(RUSAGE_CHILDREN,r_usage) < 0){
                perror("get enabled usage");
            }else{
                struct timeval *end_u = &r_usage->ru_utime;
                struct timeval *end_s = &r_usage->ru_stime;
                job_list[i].euser = end_u->tv_sec;
                job_list[i].esys = end_s->tv_sec;
                job_list[i].euseru = end_u->tv_usec;
                job_list[i].esysu = end_s->tv_usec;
                job_list[i].ema = r_usage->ru_majflt;
                job_list[i].env = r_usage->ru_nvcsw;
                job_list[i].eniv = r_usage->ru_nivcsw;
            }
            //print stats
            printStats(job_list[i]);
            
            if(!job_list[i].bg){
                fg_pid = 0;
            }
            int jid = job_list[i].jid;
            addemptyjob(&job_list[i]);

            /* update the jid */
            if (jid >0 && jid<=MAXJOBS){
                jid_list[jid-1] = 0; 
            }  
            return;  
        }
    }
}

/* 
 * findjob - return the index in the job list with a given pid 
 */
int findjob(pid_t pid){
    for (int i = 0; i < MAXJOBS; i++){
        if (job_list[i].pid == pid){
            return i;
        }
    }
    return -1;
}

/* 
 * jobscmd - proceed jobs cmd 
 */
void jobscmd(void){
    for(int i = 0; i < MAXJOBS; i++){
        if (job_list[i].pid == 0) return;
        /* print jid and pid */
        printf("[%d]%6d", job_list[i].jid, job_list[i].pid);   

        /* print state */    
        if (job_list[i].state == 0){
            printf("%13s", "Default");
        }
        else if (job_list[i].state == 1){
            printf("%13s", "Runnning");
        } else if (job_list[i].state == 2){
            printf("%13s", "Stopped");
        } else if (job_list[i].state == 3){
            printf("%13s", "Terminated");
        }
        
        /* print cmd */
        if (job_list[i].cmd[0] == '\0')
            printf("\n");
        else
            printf("     %s", job_list[i].cmd);
        
    }
}

/* 
 * bgcmd - proceed bg cmd 
 */
void bgcmd(char **argv){
    pid_t pid;

    /* grep the valid pid from cmd */
    if (*argv[1] == '%'){
        int jid = atoi(argv[1]+1);
        if(jid<=0 || jid>MAXJOBS){
            perror("Wrong jid");
            return;
        }
        pid = jid_list[jid-1];
    } else {
        pid = atoi(argv[1]);
    }
    
    int index;
    if ((index = findjob(pid))== -1) {
        perror("Wrong pid");
        return;
    }
    
    /* kill that job, and modify our job list */
    kill(-pid, SIGCONT);
    job_list[index].bg = 1;
    job_list[index].state = 1;
    printf("[%d] %d %s", job_list[index].jid, pid, job_list[index].cmd);
}


/* 
 * fgcmd - proceed fg cmd 
 */
void fgcmd(char **argv){
    sigset_t prev;
    pid_t pid;
    
    /* grep the valid pid from cmd */
    if (*argv[1] == '%'){
        int jid = atoi(argv[1]+1);
        if(jid<=0 || jid>MAXJOBS){
            perror("Wrong jid");
            return;
        }
        pid = jid_list[jid-1];
    } else {
        pid = atoi(argv[1]);
    }
    
    int index;
    if ((index = findjob(pid))== -1) {
        perror("Wrong pid");
        return;
    }
    
    /* kill that job, and modify our job list */
    kill(-pid, SIGCONT);
    job_list[index].bg = 0;
    job_list[index].state = 1;
    fg_pid = pid;
    
    /* proceed this job when there is no fg job */
    while(fg_pid != 0){
        sigsuspend(&prev);
    }
    
}
