#include <stdio.h>
#include <sys/types.h>
#include <regex.h>

#include "ukthread.h"  // must include in this order.
#include "symbols.h"


main(){
	int next;
	char *found;

	for_each_glob(next,"_probe",found){
		printf("%s  %lx\n",found,find_vaddr(found));
	}
/*

	for(next=0;(found=glob_names("probe",&next));next++){
		printf("%s  %lx\n",found,find_vaddr(found));
	}
*/
	
/*
	printf ("probe1=0x%lx\n",find_vaddr("probe_1"));
	printf ("probe2=0x%lx\n",find_vaddr("probe_2"));
	printf ("probe3=0x%lx\n",find_vaddr("probe_3"));
	printf ("__bss_start=0x%lx\n",find_vaddr("__bss_start"));
*/
}

