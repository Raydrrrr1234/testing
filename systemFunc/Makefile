# Makefile--
# AUTHOR:Ruikang Dai
# DATE:Sat Mar 14 11:55:21 EDT 2015
CC	= gcc
CCOPT	= -g -DDEBUG -DPRINT
#CCOPT	= -O2

command: listlib.o keeplog.o
	cc listlib.o keeplog.o -o command
liblist.o: listlib.c listlib.h
	cc -c listlib.c
libkeeplog.o: keeploglib.c
	cc -c keeploglib.c
keeplog.o: keeplog.c
	cc -static keeplog.c -lkeeplog -llist
