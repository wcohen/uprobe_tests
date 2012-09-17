# Make a symbol-table include file from an a.out file.
%.st: %
	nm $< | ../mksym > $<.st

obj-m := probe5.o probe5ag.o probe6.o suicide.o probe5lim.o blink.o blink2.o \
	rprobe6.o pong.o pong_wa7.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

PROGS=probeme4 probeme6 probe7-thread probe8-fork probeme4s
SYMTABS=probeme6.st probeme4s.st probe7-thread.st probeme4.st probe8-fork.st

default: progs
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

progs: $(PROGS) $(SYMTABS)

probe7-thread: probe7-thread.c
	$(CC) -Wall probe7-thread.c -lpthread -o probe7-thread

add_addr:
	objdump -d probeme4 | grep '<add>:'
	objdump -d probe7-thread | grep '<add>:'
	objdump -d probe8-fork | grep '<add>:'

clean:
	rm -f *.mod.c *.ko *.o .*.cmd *~
	rm -rf .tmp_versions
	rm -f $(PROGS) probe7-thread
	rm -f $(SYMTABS)
