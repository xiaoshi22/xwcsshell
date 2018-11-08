#include "shell.h"

/* stats - This function is used for set stats swiches accroding to the 
 * command input Stats report of all commands after this command will be 
 * printed until a "stats -c" is received
 */
void stats(char **argv){
	/* if it is a -c, turn off all switches and return */
	if(!strcmp(argv[1], "-c")){
		ss->utime = 0;
		ss->stime = 0;
		ss->majflt = 0;
		ss->nvcsw = 0;
		ss->nivcsw = 0;
		/* if it is a -a, turn on all switches and return */
	}else if(!strcmp(argv[1], "-a")){
		ss->utime = 1;
		ss->stime = 1;
		ss->majflt = 1;
		ss->nvcsw = 1;
		ss->nivcsw = 1;
		/* if it is a -l, list out enabled switches */
	}else if(!strcmp(argv[1], "-l")){
		if((ss->utime == 0) && (ss->stime == 0) &&
		(ss->majflt == 0) && (ss->nvcsw == 0) && (ss->nivcsw == 0)){
			printf("No stats enabled");
		}else{
			if(ss->utime == 1){printf("user time   ");}
			if(ss->stime == 1){printf("sys time   ");}
			if(ss->majflt == 1){printf("hard page fault   ");}
			if(ss->nvcsw == 1){printf("voluntary context switches   ");}
			if(ss->nivcsw == 1){printf("involuntary context switches ");}
		}
		 printf("\n");
		/* in other cases, turn on the switches has been mentioned */ 
	}else{
		for (int i = 1; argv[i]!=NULL; i++){
			/* handle exception situations */
			/* keep previous stats not change */
			if(!((!strcmp(argv[i], "-u")) || (!strcmp(argv[i], "-s")) ||
			(!strcmp(argv[i], "-p")) || (!strcmp(argv[i], "-v")) || !strcmp(argv[i], "-i"))){
				printf("'%s' cannot be recognized.\n", argv[i]);
				break;
			}else if(!strcmp(argv[i], "-u")){
				ss->utime = 1;
			}else if(!strcmp(argv[i], "-s")){
				ss->stime = 1;
			}else if(!strcmp(argv[i], "-p")){
				ss->majflt = 1;
			}else if(!strcmp(argv[i], "-v")){
				ss->nvcsw = 1;
			}else if(!strcmp(argv[i], "-i")){
				ss->nivcsw = 1;
			}
		}
	}
}


/* Function used for print stats Report
 If the switches are on, print the report of that variable 
 Call the variables from the job's storage */
void printStats(struct job j){
    //if stats switches are on, print out informations
    if((ss->utime == 1) || (ss->stime == 1) ||
     	 (ss->majflt == 1) || (ss->nvcsw == 1) || (ss->nivcsw == 1)){
     		 printf("========Stats Report for pid = %d==========\n", j.pid);
        
     			if(ss->utime == 1){
                    long double difference = (j.euser - j.suser)*1000000 + j.euseru - j.suseru;
                    int min = 0;
                    if(((int)difference/1000000)!=0){
                        min = ((int)difference/1000000)/60;
                    }
                    printf("user time %d m %Lf s\n", min, difference/1000000);
                }
        if(ss->stime == 1){
            long double difference = (j.esys - j.ssys)*1000000 + j.esysu - j.ssysu;
            int min = 0;
            if(((int)difference/1000000)!=0){
                min = ((int)difference/1000000)/60;
            }
            printf("sys time %d m %Lf s\n", min, difference/1000000);
     			}
     			if(ss->majflt == 1){printf("hard page fault %ld\n", j.ema-j.sma);}
     			if(ss->nvcsw == 1){printf("voluntary context switches  %ld\n", j.env-j.snv);}
     			if(ss->nivcsw == 1){printf("involuntary context switches  %ld\n", j.eniv-j.sniv);}
        
    }
}

/* Function used for print stats Report for PIPELINE
 If the switches are on, print the report of that variable
 Call the variables from getrusage method directly because pipeline does not run struct job */
void printPipeStats(int pid, time_t s1, time_t s2, suseconds_t s3, suseconds_t s4, long int s5, long int s6, long int s7){
    time_t euser, esys;
    suseconds_t euseru, esysu;
    long int ema, env, eniv;
    //grab the resourcee usage after process execute
    if(getrusage(RUSAGE_CHILDREN,r_usage) < 0){
        perror("get enabled usage");
    }else{
        struct timeval *end_u = &r_usage->ru_utime;
        struct timeval *end_s = &r_usage->ru_stime;
        euser = end_u->tv_sec;
        esys = end_s->tv_sec;
        euseru = end_u->tv_usec;
        esysu = end_s->tv_usec;
        ema = r_usage->ru_majflt;
        env = r_usage->ru_nvcsw;
        eniv = r_usage->ru_nivcsw;
    }
    //if stats switches are on, print out informations
    if((ss->utime == 1) || (ss->stime == 1) ||
     	 (ss->majflt == 1) || (ss->nvcsw == 1) || (ss->nivcsw == 1)){
     		 printf("========Stats Report for pid = %d==========\n", pid);
        
     			if(ss->utime == 1){
                    long double difference = (euser - s1)*1000000 + euseru - s3;
                    int min = 0;
                    if(((int)difference/1000000)!=0){
                        min = ((int)difference/1000000)/60;
                    }
                    printf("user time %d m %Lf s\n", min, difference/1000000);
                }
        if(ss->stime == 1){
            long double difference = (esys - s2)*1000000 + esysu - s4;
            int min = 0;
            if(((int)difference/1000000)!=0){
                min = ((int)difference/1000000)/60;
            }
            printf("sys time %d m %Lf s\n", min, difference/1000000);
            
        }
        if(ss->majflt == 1){printf("hard page fault %ld\n", ema-s5);}
        if(ss->nvcsw == 1){printf("voluntary context switches  %ld\n", env-s6);}
        if(ss->nivcsw == 1){printf("involuntary context switches  %ld\n", eniv-s7);}
        
    }
    
}
