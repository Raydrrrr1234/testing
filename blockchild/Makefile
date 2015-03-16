# Makefile--
# AUTHOR:Ruikang Dai
# DATE:Sun Mar 15 08:19:59 EDT 2015
blockchild: blockchild.o ./restart/restart.o
	cc blockchild.o ./restart/restart.o -o blockchild
blockchild.o: blockchild.c ./restart/restart.h
	cc -c blockchild.c ./restart/restart.h -o $@
restart.o: restart.c restart.h
	cc -c restart.c -Lrestart -o $@
