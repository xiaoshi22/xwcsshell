#include "shell.h"

/* 
 * pipeline - process a pipeline 
 */
pid_t pipeline(char **argv) {
    char *argv_front[MAXARGS];

    /* parse the pipe according to its first pipe */
    parsepipe(argv, argv_front);
    
    
    /* the first process */
    int pipefd[2];
    
    if (pipe(pipefd) == -1) { /* construct a new pipe */
        ferror("pipe error");
        return -1;
    }
    pid_t pid1 = fork(); /* create a child */
    if (pid1 == -1) {
        ferror("pipe error");
        return -1;
    } else if (pid1 == 0) { /* if we are in the child */
        setpgid(0, 0);
        dup2(pipefd[1], STDOUT_FILENO); /* connect the new pipe with the stdout */
        close(pipefd[0]);
        execvp(argv_front[0], argv_front);
        perror("exec");
    }
    
    /* the middle part */
    while(parsepipe(argv, argv_front)){
        evalpipe(pipefd, argv_front);
    }
    
    /* the last part */
    pid_t pid3 = fork(); /* create a child */
    if (pid3 == -1) {
        ferror("pipe error");
        return -1;
    } else if (pid3 == 0) { /* if we are in the child */
        dup2(pipefd[0], STDIN_FILENO); /* connect the stdin with the old pipe */
        close(pipefd[1]);
        execvp(argv_front[0], argv_front);
        perror("exec");
    }
    close(pipefd[0]);
    close(pipefd[1]);
    
    /* wait for all child to be finished */
    while (wait(NULL) > 0);
    return pid1;
}

/* 
 * is_pipecmd - check whether is a pipe cmd or not
 */
int is_pipecmd(char **argv){
    int count = 0;
    while(argv[count]!=NULL){
        if (!strcmp(argv[count], "|")){
            break;
        }
        count++;
    }
    
    /* null if mno more "|" in our argv */
    if (argv[count]==NULL) return 0; 
    return 1;
}


/* 
 * parsepipe - parse a given pipe by its first '|''
 */
int parsepipe(char** argv, char** first_argv){
    for (int i = 0; first_argv[i]!=NULL; i++){
        first_argv[i] = NULL;
    }
    
    int count = 0;
    while(argv[count]!=NULL){ /* find the index when just before '|' */
        if (!strcmp(argv[count], "|")){
            break;
        }
        count++;
    }
    if (argv[count]==NULL){ /* null if mno more "|" in our argv */
        for (int i = 0; i<=count; i++){
            first_argv[i] = argv[i];
        }
        argv = NULL;
        return 0;
    } 
    
    /* construct a temp to store the second part */
    char *argv_temp[MAXARGS];
    
    /* copy tthe first cmd before "|" */ 
    for (int i = 0; i<count; i++){
        first_argv[i] = argv[i];
    }
    first_argv[count] = NULL;
    
    /* copy the right part after "|" */
    int count2 = 0;
    while(argv[count+count2+1]!=NULL){
        argv_temp[count2] = argv[count+count2+1];
        count2 ++;
    }
    argv_temp[count2] = NULL;
    for (int i = 0; i<=count2; i++){
        argv[i] = argv_temp[i];
    }
    
    return 1;
}


/* 
 * evalpipe - "recrusively" eval first part of the given pipe 
 */
int evalpipe(int* pipefd, char** argv_front){
    int pipefd_temp[2];
    /* construct a new pipe */
    if (pipe(pipefd_temp) == -1) {
        ferror("pipe error");
        return -1;
    }
    
    /* create a child */
    pid_t pid = fork();
    if (pid == -1) {
        ferror("pipe error");
        return -1;
    } else if (pid == 0) { /* if we are in the child */
        /* connect the stdin with the old pipe */
        dup2(pipefd[0], STDIN_FILENO); 
        close(pipefd[1]);
        
         /* connect the new pipe with the stdout */
        dup2(pipefd_temp[1], STDOUT_FILENO);
        close(pipefd_temp[0]);
        execvp(argv_front[0], argv_front);
        perror("exec");
    }
    
    /* close the old one in parent */
    close(pipefd[0]);
    close(pipefd[1]);
    
    /* update the current pipeid */
    pipefd[0] = pipefd_temp[0];
    pipefd[1] = pipefd_temp[1];
    
    return 1;
}