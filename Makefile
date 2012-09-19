
default:
	( cd uprobes.w && make -f Makefile default)
#	( cd uprobes.existing && make -f Makefile default)

check: default
	runtest --tool uprobes

clean:
	(cd uprobes.w && make -f Makefile clean)
	(cd uprobes.existing && make -f Makefile clean)
