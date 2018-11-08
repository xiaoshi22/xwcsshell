#include "shell.h"

int main()
{
    char cmdline[MAXLINE]; /* Command line */

    /* initialize structs */
    ss = (struct stats_switch*)malloc(sizeof(struct stats_switch));
    r_usage = (struct rusage*)malloc(sizeof(struct rusage));

    /* initialize the jobs list */
    Signal (SIGCHLD, sigchld_handler);
    Signal (SIGINT, sigint_handler);
    Signal (SIGTSTP, sigtstp_handler);


    /* initialize the jobs list */
    initjobs();

    /* initialize the jid list */
    for (int i = 0; i < MAXJOBS; i++){
        jid_list[i] = 0;
    }

    fg_pid = 0;

    /* set lshprompt as an envirn variable */
    setenv("lshprompt", "lsh>", 1);


    while (1) {
        /* Read */
        printf("%s ", getenv("lshprompt"));
        fflush(stdout);
        fgets(cmdline, MAXLINE, stdin);
        if (feof(stdin))
            exit(0);
        /* Evaluate */
        eval(cmdline);
    }
    exit(0);
}


/* 
 * eval - Evaluate a command line 
 */
void eval(char *cmdline){
    char *argv[MAXARGS]; /* Argument list execve() */
    char buf[MAXLINE];   /* Holds modified command line */
    int bg;              /* Should the job run in bg or fg? */
    pid_t pid;           /* Process id */
    sigset_t mask_chld, prev; /* set for block signal while proceeding */
    int index;          /* index in job_list */

    /* parse cmdline */
    strcpy(buf, cmdline);
    bg = parseline(buf, argv);

    if (argv[0] == NULL){
        return;   /* Ignore empty lines */
    }


    if (!builtin_command(argv)) { //if is not biniltin_command
        
        sigemptyset(&mask_chld);
        sigaddset(&mask_chld, SIGCHLD);
        sigprocmask(SIG_BLOCK, &mask_chld, NULL); /* Block SIGCHLD */
        
        time_t s1, s2;
        suseconds_t s3, s4;
        long int s5, s6, s7;
        //grab the resourcee usage before process execute
        if(getrusage(RUSAGE_CHILDREN,r_usage) < 0){
            perror("get start usage");
        }else{
            struct timeval *start_u = &r_usage->ru_utime;
            struct timeval *start_s = &r_usage->ru_stime;
            s1 = start_u->tv_sec;
            s2 = start_s->tv_sec;
            s3 = start_u->tv_usec;
            s4 = start_s->tv_usec;
            s5 = r_usage->ru_majflt;
            s6 = r_usage->ru_nvcsw;
            s7 = r_usage->ru_nivcsw;
        }
        
        if (is_pipecmd(argv)){
            pid = pipeline(argv);
            printPipeStats(pid,s1,s2,s3,s4,s5,s6,s7);
        } else {
            if ((pid = fork()) == 0) {   /* Child runs user job */
                setpgid(0, 0);
                sigprocmask(SIG_UNBLOCK, &mask_chld, NULL); /* Unblock SIGCHLD */
                if (execvp(argv[0], argv) < 0) {
                    printf("%s: Command error.\n", argv[0]);
                    exit(0);
                }
            }
            
            sigprocmask(SIG_BLOCK, &mask_chld, NULL); /* Block SIGCHLD */
            if ((index = addjob(pid, cmdline, bg, s1, s2, s3, s4, s5, s6, s7))<0){
                perror("addjob in eval error");
            } /* Add the child to the job list */
            sigprocmask(SIG_UNBLOCK, &mask_chld, NULL);  /* Unblock SIGCHLD */
            /* Parent waits for foreground job to terminate */
            if (!bg) {
                while(fg_pid != 0){
                    // sigsuspend(&prev);
                    sleep(0);
                }
            }else{
                printf("[%d] %d %s", ++index, pid, cmdline);
            }
        }
    }
    return;
}




/* 
 * parseline - Parse the command line and build the argv array 
 */
int parseline(char *buf, char **argv)
{
    char *delim;         /* Points to first space delimiter */
    int bg;              /* Background job or not */

    buf[strlen(buf)-1] = ' ';       /* Replace trailing '\n' with space */
    while (*buf && (*buf == ' '))   /* Ignore leading spaces */
        buf++;

    /* Build the argv list */
    argc = 0;
    while ((delim = strchr(buf, ' '))) {
        argv[argc++] = buf;
        *delim = '\0';
        buf = delim + 1;
        while (*buf && (*buf == ' ')) /* Ignore spaces */
            buf++;
    }
    argv[argc] = NULL;


    if (argc == 0){
        return 1;/* Ignore blank line */
    }

    /* modify input for ECHO method
    when input has the first character as $, it means it is the name of an
    variable, replace the variable name with its value */
    for(int i = 1; i < argc; i++){
        if(((strchr(argv[i],'$'))!=NULL)&&(*argv[i]=='$')){
            char n[MAXARGS];
            memset(n,0,MAXARGS*sizeof(char));
            /*set variable name without $*/
            strncpy(n, argv[i]+1, strlen(argv[i])-1);
            argv[i] = "\0";
            argv[i] = getenv(n);
        }
    }

    /* Should the job run in the background? */
    bg = 0;
    if ((argv[argc-1] != NULL)&&((bg = (*argv[argc-1] == '&')) != 0)){
        argv[--argc] = NULL;
    }else{
    }

    return bg;
}


/*
 * builtin_command - proceed builtin_cmd
 * If a command is built-in, run it and return 1. Otherwise, return 0
 */
int builtin_command(char **argv)
{
    /* quit command */
    if (!strcmp(argv[0], "quit")){
        exit(0);
    }

    /* function for set or remove environmental variables */
    if((strchr(argv[0],'='))!=NULL){
        char *p;
        p = strchr (argv[0],'=');
        char name[MAXARGS];
        memset(name,0,MAXARGS*sizeof(char));
        strncpy(name, argv[0],p-argv[0]);
        char value[MAXARGS];
        memset(value,0,MAXARGS*sizeof(char));
        if((strlen(argv[0])-strlen(name))==1){

            if (!strcmp(name, "lshprompt")){
                setenv("lshprompt", "lsh>", 1);
                return 1;
            }

            if(unsetenv(name) < 0){
                perror("remove environmental variable failed");
             }
        } else{
            strncpy(value, argv[0]+(p-argv[0])+1,strlen(argv[0])-strlen(name));
            if(setenv(name, value, 1) < 0){
                perror("set environmental variable failed");
            }
        }
        return 1;
    }

    /* jobs command */
    if(!strcmp(argv[0], "jobs")){
        jobscmd();
        return 1;
    }

    /* bg command */
    if(!strcmp(argv[0], "bg")){
        bgcmd(argv);
        return 1;
    }

    /* fg command */
    if(!strcmp(argv[0], "fg")){
        fgcmd(argv);
        return 1;
    }

    /* stats command */
    if(!strcmp(argv[0], "stats")){
        stats(argv);
        return 1;
    }

    /* Ignore singleton & */
    if (!strcmp(argv[0], "&")){
        return 1;
    }
    return 0;                     /* Not a builtin command */
}
