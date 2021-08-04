# fire1024 (c) 2014
# makefile program sf1
#
CC = gcc
#-Wall correct code test
CFLAGS =-lconfig -std=gnu99
all:
	$(CC) $(CFLAGS)  sf1.c -o sf1
	chmod 700 sf1
	killall -9 sf1 | rm -rf /var/run/sf1.pid
clean:
	rm sf1

