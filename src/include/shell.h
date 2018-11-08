/* $begin shell.h */
#ifndef __SHELL_H__
#define __SHELL_H__


#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXLINE   1024    /* max line num */
#define MAXARGS   128     /* max args num */
#define MAXJOBS 16        /* max jobs num */
#define MAXPROCESSES 16   /* max processes num */

/* job struct */
typedef struct job
{
    pid_t pid;			    /* its pid */
    int jid; 			      /* its jid */
    char cmd[MAXLINE];	/* the command line when excuted */
    int state; 			    /* 0 for default, 1 for running, 2 for stopped, 3 for terminated */
    int bg;				      /* whether runs backgroud or not */
    time_t suser;    /* start second of user time tracker */
    time_t euser;    /* end second of user time tracker */
    time_t ssys;    /* start second of system time tracker */
    time_t esys;    /* end second of system time tracker */
    suseconds_t suseru;   /* start microsecond of user time tracker */
    suseconds_t euseru;   /* end microsecond of user time tracker */
    suseconds_t ssysu;   /* start microsecond of system time tracker */
    suseconds_t esysu;   /* end microsecond of system time tracker */
    long int sma;   /* start number of hard page error */
    long int ema;   /* end number of hard page error */
    long int snv;   /* start number of voluntary context switches */
    long int env;   /* end number of voluntary context switches */
    long int sniv;  /* start number of involuntary context switches */
    long int eniv;  /* end number of involuntary context switches */
} job;

/* swiches for the stats function
 * when each variable is 0, stats is not being tracked
 * when variable is 1, certain statistic information is tracked and printed
 */
struct stats_switch{
	int utime;   /* user running time */
	int stime;   /* systerm/ kernal running time */
	int majflt;  /* number of hard page fault */
	int nvcsw;   /* number of voluntary context exchange */
	int nivcsw;  /* number of involuntary context exchange */
}*ss;


int argc;                     /* Number of args */
struct job job_list[MAXJOBS]; /* the job list to keeping track on all processing jobs */
pid_t jid_list[MAXJOBS];      /* to track which of the 16 num is aviabliable, and keep the corresponding pid */
pid_t fg_pid;                 /* the only one fg process, o if no fg process */

struct rusage *r_usage; /* build resource usage tracker */




/* Function prototypes */
/* handle basic parse and eval for builtin_cmd and not builtin_cmd */
void eval(char *cmdline);			    	    /* eval cmd */
int parseline(char *buf, char **argv);  /* parse cmd */
int builtin_command(char **argv);		    /* proceed builtin_cmd */

/* work on the case when cmd is a pipe */
int pipeline(char **argv);						          /* process a pipeline */
int is_pipecmd(char **arrgv);					          /* check whether is a pipe cmd or not*/
int parsepipe(char** argv, char** first_argv);	/* parse a given pipe by its first '|*/
int evalpipe(int* pipefd1, char** argv_front); 	/* "recrusively" eval first part of the given pipe */

/* Deal with struct job, job_list, fg_pid, and serveral relative methods */
/* Deal with struct job, job_list, fg_pid, and serveral relative methods */
int addjob(pid_t pid, char* cmd, int bg, time_t s1, time_t s2, suseconds_t s3, suseconds_t s4, long int s5, long int s6, long int s7);	/* add a given job */
void addemptyjob(struct job *j);			    /* add an empty job */
void deletejob(pid_t pid);					      /* delete a job with a given pid */
int findjob(pid_t pid);						        /* return the index in the job list with a given pid */
void initjobs(void);						          /* initalize our job list */

void jobscmd(void);			  /* proceed jobs cmd */
void bgcmd(char **argv);	/* proceed bg cmd */
void fgcmd(char **argv);	/* proceed fg cmd */

/* Handler chld, int, stp signals for processes running in our shell */
typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);
void sigchld_handler(int s);	/* sigchld handler */
void sigint_handler(int s);		/* sigint handler */
void sigtstp_handler(int s);	/* sigtstp handler */

/* stats function */
void stats(char **argv);
void printStats(struct job j); /*print stats for jobs*/
void printPipeStats(int pid, time_t s1, time_t s2, suseconds_t s3, suseconds_t s4, long int s5, long int s6, long int s7); /*print stats for pipes*/


#endif /* __SHELL_H__ */
/* $end shell.h */
