# shell.h is in include folder
IDIR =include
_DEPS = shell.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

# object files are in obj folder
ODIR=obj
_OBJ = shell.o pipe.o signal.o job.o stats.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

# set BINDIR to where we install shell
BINDIR=../install/bin/

# commands used for compile
CC = gcc
CFLAGS=-I$(IDIR)



# build .o files, then create the excutable
$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

shell: $(OBJ)
	gcc -o $@ $^ $(CFLAGS);
	mv shell $(BINDIR)


# clean all .o files in obj folder
.PHONY: clean
clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~


# clean the executable shell.exe in bin folder
.PHONY: cleanexe
cleanexe: $(BINDIR)
	cd $(BINDIR) && rm -f shell.exe


# recreate the executable, then run it
# but this fails when dealing with signals
# ex. ctrl-z may terminate the sell itself
# be careful! But nothing would be crashed.
# Notice the pwd is still under src directionary when we quit shell.
.PHONY: run
run: $(BINDIR) shell
	cd $(BINDIR) && ./shell

# isntall the man xwcsshell.1 into the user's computer
.PHONY: installman
installman: $(BINDIR)
	cd $(BINDIR) && cd man && cp xwcsshell.1.gz /usr/share/man/man1