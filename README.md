# xwcsshell

# Project 1: Write a Shell
## Purpose

The purpose of this project is to improve your C and system programming skills, and increase your understanding of the Unix programming environment, in particular shells.  You will create, monitor, and report on processes in Linux.  You will also create pipelines, and develop a better understanding of stdin, stdout, stderr, PATH, and environment variables.



## Resources

Suggested reading before beginning to work on this process:

* Computer Systems Textbook, Chapter 8 is essential reading.

* Unix Shell, Wikipedia

* Pipeline, Wikipedia

* How Linux pipes work under the hood, Brandon Wamboldt

* man pages for getrusage, setenv, and other man pages as necessary.  If a man page does not answer your question directly, check out the SEE ALSO Section for hints about related functions/commands/data structures.

## Team Organization

For this assignment, you and up to one teammate, are to implement a mini shell for Linux.  If you work in a pair for this assignment, you are to practice pair programming.  One person sits at the keyboard typing the code, and the other person watches for errors, thinks about the design, and makes suggestions.  To do well in this course, every student needs to understand the programming projects in detail.  If you divide and conquer, you will miss important details that you are supposed to learn by doing.

## What to Hand In

You are to hand in a tar file for your completed project.  The directory structure for your project should be:

* /teamNameShell
* /teamNameShell/src  -- your source subtree which can have additional subdirectories, of course
* /teamNameShell/src/man -- the source for your man page in the right man page subfolder (e.g. man1).  This is for the man page that you develop.
* /teamNameShell/src/docs -- a report on project effort
* /teamNameShell/src/Makefile -- the Makefile for building your executable
* /teamNameShell/src/README -- instructions for using the Makefile to build your system including an explanation of any build options
* /teamNameShell/install -- your running code subtree
* /teamNameShell/install/bin -- where your shell executable goes
* /teamNameShell/install/man -- where your installed man page for the shell goes (in the right man page subfolder).  This is for the man page that people display.

The tar file should include a Makefile in the top level src directory for creating the executable.  The top level src directory should have a README file that explains how to build your project and any special options that can be set when building.  There should be a man subdirectory that supplies a man page for your project (Use one of the existing man pages as a template.  They are in /usr/share/man/man1 for commands).  There should also be a docs subdirectory containing a report document that describes who worked on the project, obstacles you overcame, and major things that you learned.  You should also include a section on enhancements you think would be good for your project.

## Requirments

1. The minishell must be written in C.
2. The shell should be called lsh for Lafayette shell.
3. The shell should print a prompt lsh> by default, or the string contained in the lshprompt environment variable if one exists.  
4. The shell inherits environment variables from the process that starts it.  It should be possible to set environment variables by saying  variableName=value. It should be possible to remove variables by saying variableName= (note the absence of blanks around the =), and echo $PATH should print the value of the PATH environment variable.  These features work in the default shell on the system.  Try them out.  Reproduce them in your mini shell.  Also note that setting and deleting an environment variable is a built-in shell command while echo is a utility program (try "whereis echo" in your default shell to see this).
5. Any process created by the shell to execute a command must inherit all the shells current environment variables.
6. Your shell supports pipes.  That is a command like  "ls -l | more"  sends the output of the ls command to the input of more.  This pipeline allows you to page through a directory listing.  (Supporting i/o redirection is not required).
7. Your shell supports the requirements of Homework Problem 8.26 in the Computer Systems textbook, and is allowed to use Figure 8.23 and its related figures as a starting framework.  That is, you can copy and extend this code.  DO NOT COPY OTHER CODE FROM THE INTERNET OR ELSEWHERE.  This would be a violation of academic honesty.  Write your own code.
8. When running a program in a process you create, you must follow the Unix convention for the arguments to main.  The first argument is always the command the user entered.  Every subsequent argument is separated by one or more blanks from the other arguments.  For example, ls -lr /  has three arguments.  They are "ls", "-lr" and "/".  Try executing a standard linux utility, like ls, to check that your argument passing is working.
9. An additional feature of your shell is the ability to enable process statistics on each external command executed.  The built-in command stats enables this.  Stats has the following flags;
  * -u   the cpu time spent in user mode
  * -s   the cpu time spent in system/kernel mode
  * -p   the hard page faults
  * -v   the voluntary context switches
  * -i    the involuntary context switches
  * -a   all of the above
  * -l   a list of stats that are enabled
  * -c   to clear all the enabled stats (in this case your shell runs external commands like the default shell)
  * It should be possible to enter multiple flags after one dash, e.g. -us instead of -u -s, but a series of single flags should also work.
  * When multiple statistics are returned, they should always be printed in the same order.

Example command sequence:

```
lsh> stats -l
No stats enabled
lsh> stats -u -s
lsh> ls -lr / > /tmp/listing
user: 0m0.000s   sys:0m0.004s
```

## Important Notes, Prohibitions and Advice 

I will update this list with various tips as students ask me questions.

1. You cannot use the system() function to do this.  This prohibition is for your own good  If you do system() to create your processes, you will be unable to complete all the requirements.
2. Write error messages and debugging code out to stderr rather than stdout.  stderr is not buffered, so the error message will appear immediately.
3. Here's how bg and fg work.  When you create a process without an & at the end, it runs in the foreground.  During the foreground process, no shell prompt displays, any commands you type are held until after this foreground process completes.  If you type ^Z in the shell, the foreground process and any children of that process (any descendents at all) are stopped by your shell.  The user can then use the bg command to resume that process in the background.  Or, the user can use the fg command to resume that process in the foreground.   If there are jobs running in the background, the user can display them with the jobs command.  The user can then use fg to make one of the background jobs a foreground job (and the background job does not need to be stopped in order to do this).  If you want to stop a background job with ^Z, you first need to make it a foreground job.  Use the default shell to experiment with bg, fg, jobs, ^Z to get a good understanding of what you are implementing.
4. stats is a command that acts like a switch in your shell.  For example, if you set 

stats -u -s

Then every command that is not built-in, that is run after the stats are set, will print usage and system statistics after it completes.  The commands will do that until stats are cleared with the stats -c command.

```
lsh> stats -u -s 
lsh> cat * | wc
12696   59669  456985
user    0m0.004s  sys     0m0.000s
lsh> ls -lr / > /tmp/listing
user: 0m0.000s   sys:0m0.004s 
lsh> stats -c
lsh> cat * | wc
12696   59669  456985
lsh> rm /tmp/listing
lsh> ls -lr / > /tmp/listing 
lsh>
```
