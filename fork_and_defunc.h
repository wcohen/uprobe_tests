struct symbol {
	 unsigned long   address ;
	 char            type;
	 char            name[80];
};

static const struct symbol user_symbols[] = {
	{
	 .address = -1,
	 .type = 'U',
	 .name = "atoi@@GLIBC_2.0"
	},
	{
	 .address = 0x0804a9ec,
	 .type = 'A',
	 .name = "__bss_start"
	},
	{
	 .address = 0x08048624,
	 .type = 't',
	 .name = "call_gmon_start"
	},
	{
	 .address = -1,
	 .type = 'U',
	 .name = "calloc@@GLIBC_2.0"
	},
	{
	 .address = 0x0804a9f0,
	 .type = 'b',
	 .name = "completed.5754"
	},
	{
	 .address = 0x0804a8b8,
	 .type = 'd',
	 .name = "__CTOR_END__"
	},
	{
	 .address = 0x0804a8b4,
	 .type = 'd',
	 .name = "__CTOR_LIST__"
	},
	{
	 .address = 0x0804a9e4,
	 .type = 'D',
	 .name = "__data_start"
	},
	{
	 .address = 0x0804a9e4,
	 .type = 'W',
	 .name = "data_start"
	},
	{
	 .address = 0x080496a0,
	 .type = 't',
	 .name = "__do_global_ctors_aux"
	},
	{
	 .address = 0x08048650,
	 .type = 't',
	 .name = "__do_global_dtors_aux"
	},
	{
	 .address = 0x080496f0,
	 .type = 'R',
	 .name = "__dso_handle"
	},
	{
	 .address = 0x0804a8c0,
	 .type = 'd',
	 .name = "__DTOR_END__"
	},
	{
	 .address = 0x0804a8bc,
	 .type = 'd',
	 .name = "__DTOR_LIST__"
	},
	{
	 .address = 0x0804a8c8,
	 .type = 'd',
	 .name = "_DYNAMIC"
	},
	{
	 .address = 0x0804a9ec,
	 .type = 'A',
	 .name = "_edata"
	},
	{
	 .address = 0x0804a9f8,
	 .type = 'A',
	 .name = "_end"
	},
	{
	 .address = -1,
	 .type = 'U',
	 .name = "exit@@GLIBC_2.0"
	},
	{
	 .address = 0x0804a9f4,
	 .type = 'B',
	 .name = "findme"
	},
	{
	 .address = 0x080496cc,
	 .type = 'T',
	 .name = "_fini"
	},
	{
	 .address = 0x0804a8b4,
	 .type = 'a',
	 .name = "__fini_array_end"
	},
	{
	 .address = 0x0804a8b4,
	 .type = 'a',
	 .name = "__fini_array_start"
	},
	{
	 .address = -1,
	 .type = 'U',
	 .name = "fork@@GLIBC_2.0"
	},
	{
	 .address = 0x08049387,
	 .type = 'T',
	 .name = "fork_thread"
	},
	{
	 .address = 0x080496e8,
	 .type = 'R',
	 .name = "_fp_hw"
	},
	{
	 .address = -1,
	 .type = 'U',
	 .name = "fprintf@@GLIBC_2.0"
	},
	{
	 .address = 0x08048680,
	 .type = 't',
	 .name = "frame_dummy"
	},
	{
	 .address = 0x080498b0,
	 .type = 'r',
	 .name = "__FRAME_END__"
	},
	{
	 .address = -1,
	 .type = 'U',
	 .name = "getpid@@GLIBC_2.0"
	},
	{
	 .address = 0x08048700,
	 .type = 'T',
	 .name = "gettid"
	},
	{
	 .address = 0x0804a99c,
	 .type = 'd',
	 .name = "_GLOBAL_OFFSET_TABLE_"
	},
	{
	 .address = -1,
	 .type = 'w',
	 .name = "__gmon_start__"
	},
	{
	 .address = 0x080484e4,
	 .type = 'T',
	 .name = "_init"
	},
	{
	 .address = 0x0804a8b4,
	 .type = 'a',
	 .name = "__init_array_end"
	},
	{
	 .address = 0x0804a8b4,
	 .type = 'a',
	 .name = "__init_array_start"
	},
	{
	 .address = 0x080496ec,
	 .type = 'R',
	 .name = "_IO_stdin_used"
	},
	{
	 .address = 0x0804a8c4,
	 .type = 'd',
	 .name = "__JCR_END__"
	},
	{
	 .address = 0x0804a8c4,
	 .type = 'd',
	 .name = "__JCR_LIST__"
	},
	{
	 .address = -1,
	 .type = 'w',
	 .name = "_Jv_RegisterClasses"
	},
	{
	 .address = -1,
	 .type = 'U',
	 .name = "kill@@GLIBC_2.0"
	},
	{
	 .address = 0x080486a4,
	 .type = 'T',
	 .name = "__libc_csu_fini"
	},
	{
	 .address = 0x080486ac,
	 .type = 'T',
	 .name = "__libc_csu_init"
	},
	{
	 .address = -1,
	 .type = 'U',
	 .name = "__libc_start_main@@GLIBC_2.0"
	},
	{
	 .address = 0x08049517,
	 .type = 'T',
	 .name = "main"
	},
	{
	 .address = 0x0804a9e8,
	 .type = 'd',
	 .name = "p.5752"
	},
	{
	 .address = -1,
	 .type = 'U',
	 .name = "perror@@GLIBC_2.0"
	},
	{
	 .address = 0x0804a8b4,
	 .type = 'a',
	 .name = "__preinit_array_end"
	},
	{
	 .address = 0x0804a8b4,
	 .type = 'a',
	 .name = "__preinit_array_start"
	},
	{
	 .address = -1,
	 .type = 'U',
	 .name = "printf@@GLIBC_2.0"
	},
	{
	 .address = 0x08048775,
	 .type = 'T',
	 .name = "_probe_0"
	},
	{
	 .address = 0x0804885b,
	 .type = 't',
	 .name = "probe_0"
	},
	{
	 .address = 0x080488aa,
	 .type = 'T',
	 .name = "_probe_1"
	},
	{
	 .address = 0x08048990,
	 .type = 't',
	 .name = "probe_1"
	},
	{
	 .address = 0x080489df,
	 .type = 'T',
	 .name = "_probe_2"
	},
	{
	 .address = 0x08048ac5,
	 .type = 't',
	 .name = "probe_2"
	},
	{
	 .address = 0x08048b14,
	 .type = 'T',
	 .name = "_probe_3"
	},
	{
	 .address = 0x08048bfa,
	 .type = 't',
	 .name = "probe_3"
	},
	{
	 .address = 0x08048c49,
	 .type = 'T',
	 .name = "_probe_4"
	},
	{
	 .address = 0x08048d2f,
	 .type = 't',
	 .name = "probe_4"
	},
	{
	 .address = 0x08048d7e,
	 .type = 'T',
	 .name = "_probe_5"
	},
	{
	 .address = 0x08048e64,
	 .type = 't',
	 .name = "probe_5"
	},
	{
	 .address = 0x08048eb3,
	 .type = 'T',
	 .name = "_probe_6"
	},
	{
	 .address = 0x08048f99,
	 .type = 't',
	 .name = "probe_6"
	},
	{
	 .address = 0x08048fe8,
	 .type = 'T',
	 .name = "_probe_7"
	},
	{
	 .address = 0x080490ce,
	 .type = 't',
	 .name = "probe_7"
	},
	{
	 .address = 0x0804911d,
	 .type = 'T',
	 .name = "_probe_8"
	},
	{
	 .address = 0x08049203,
	 .type = 't',
	 .name = "probe_8"
	},
	{
	 .address = 0x08049252,
	 .type = 'T',
	 .name = "_probe_9"
	},
	{
	 .address = 0x08049338,
	 .type = 't',
	 .name = "probe_9"
	},
	{
	 .address = -1,
	 .type = 'U',
	 .name = "pthread_create@@GLIBC_2.1"
	},
	{
	 .address = -1,
	 .type = 'U',
	 .name = "pthread_join@@GLIBC_2.0"
	},
	{
	 .address = -1,
	 .type = 'U',
	 .name = "puts@@GLIBC_2.0"
	},
	{
	 .address = 0x08048732,
	 .type = 'T',
	 .name = "report_fail"
	},
	{
	 .address = 0x08048714,
	 .type = 'T',
	 .name = "report_pass"
	},
	{
	 .address = 0x08048750,
	 .type = 'T',
	 .name = "report_test_fail"
	},
	{
	 .address = 0x08048600,
	 .type = 'T',
	 .name = "_start"
	},
	{
	 .address = 0x0804a9ec,
	 .type = 'B',
	 .name = "stderr@@GLIBC_2.0"
	},
	{
	 .address = -1,
	 .type = 'U',
	 .name = "syscall@@GLIBC_2.0"
	},
	{
	 .address = 0x080494f5,
	 .type = 'T',
	 .name = "wait_for_kthread"
	},
};

