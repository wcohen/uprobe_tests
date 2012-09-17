#!/bin/bash 

if [ $# -lt 3 ]
then
	echo "**Error** - Insufficient parameters"
	echo "Usage: $0 <dir name> <testprg> <module-name> <no of args to testprg> <args to testprg> <args to module>"
	exit 1;
fi

cd $1
shift

kmod=$2
uprog=$1

shift
shift

fgrep -qw $kmod /proc/modules && ! sbin/rmmod $kmod  2> /dev/null
if [ $? == 0 ];then
	echo "**Error** - Module $kmod did not unload"
	exit 1
fi

num=$1
shift

# start user prog
./$uprog ${@:1:$num} &
PID=$!
echo pid is $PID
sleep 1;
shift $num

# Load kernel module
/sbin/insmod $kmod.ko pid=$PID $* && fgrep -qw $kmod /proc/modules
if [ $? != 0 ];then
	echo "**Error** - Cannot insert module $kmod"
	echo "Refer /var/log/messages"
	kill -s SIGCONT $PID
	kill -s SIGKILL $PID
	exit 1
fi

kill -s SIGCONT $PID && wait

#cat /sys/kernel/debug/uprobes_test/$kmod > /tmp/$kmod
#cat /tmp/$kmod

# unload the module
#/sbin/rmmod $kmod && ! fgrep -qw $kmod /proc/modules
#if [ $? != 0 ];then
#	echo "**Error** - Module $kmod did not unload"
#	exit 1
#fi

exit 0
