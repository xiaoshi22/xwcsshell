# Project 1: Write a Shell

Team members: Xiaoshi Wang, Yuchen Sun

## Directory Structure
```
-xwcsshell
	-install
		-bin
			shell.exe
			myspin.c
			myspin.exe (for testing cmd relative to job. Use it by typing "./myspin 30 &", or maybe "./myspin 5".)
		-man
			xwcsshell.1
			xwcsshell.1.gz
	-src
		-include
			shell.h
		-obj
			shell.c
			pipe.c
			signal.c
			job.c
			stats.c
		-man
			bash.1
			bash.1.gz
		-docs
		Makefile
		README
```

## User Manual
	The Makefile in the top level of src directory is designed for creating the executable and proceeding other instructions in the more efficient way. The following shows commands provided by our Makefile. Notice that those commands should be executed at the src directory.

```
	make
		build .o files in LSHPATH/lsh/src/obj folder, then create the excutable in LSHPATH/lsh/install/bin

	make shell
		proceed the same instruction as make

	make clean
		clean all .o files in obj folder

	make cleanexe
		clean the executable shell.exe in bin folder

	make run
		recreate the executable, then run it
		(can get our shell run in a easier way, but do not recommand if dealing with signals. Be careful when run it! )

	make installman
		isntall the man xwcsshell.1 into the user's computer
```
