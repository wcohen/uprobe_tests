obj-m := kthread.o kfork_thread.o kfork_and_defunc.o ksuicide.o ksuicide2.o kdie.o kexec.o kunreg.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

# Make a symbol-table include file from an a.out file.
MKSYM   = ./mksym
%.st: %
	$(MKSYM) $< > $<.st

PROGS=uthread fork_thread fork_and_defunc suicide tdie texec unreg
SYMTABS=uthread.st fork_thread.st fork_and_defunc.st suicide.st tdie.st texec.st unreg.st

default:$(SYMTABS)
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

progs: $(PROGS) $(SYMTABS)

uthread:uthread.c
	$(CC) -g -lpthread -lc -o uthread uthread.c

fork_thread:fork_thread.c
	$(CC) -g -lpthread -lc -o fork_thread fork_thread.c

fork_and_defunc:fork_and_defunc.c
	$(CC) -g -lpthread -lc -o fork_and_defunc fork_and_defunc.c

suicide:suicide.c
	$(CC) -g -lpthread -lc -o suicide suicide.c

tdie:tdie.c
	$(CC) -g -lpthread -lc -o tdie tdie.c

texec:texec.c
	$(CC) -g -lpthread -lc -o texec texec.c

unreg:unreg.c
	$(CC) -g -lpthread -lc -o unreg unreg.c


clean:
	rm -f *.mod.c *.ko *.o .*.cmd *.ko
	rm -rf .tmp_versions
	rm -rf *.st
	rm -f $(PROGS) 
	rm -f $(SYMTABS)

